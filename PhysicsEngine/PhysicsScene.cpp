#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "algorithm"
#include <cassert>
#include "Sphere.h"
#include "Plane.h"
#include "Box.h"
#include "Rigidbody.h"
#include <iostream>
#include <cmath>

PhysicsScene::PhysicsScene() : m_timeStep(0.01f), m_gravity(glm::vec2(0, 0))
{
	m_gravity = glm::vec2(0, -9.8f);
	collisionData = CollisionData();
}


PhysicsScene::~PhysicsScene()
{
	for (auto& actor : m_actors)
	{
		delete actor;
	}
}

void PhysicsScene::update(float dt)
{
	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;

	while (accumulatedTime >= m_timeStep)
	{
		for (auto pActor : m_actors)
		{
			pActor->fixedUpdate(m_gravity, m_timeStep);
		}

		accumulatedTime -= m_timeStep;
	}
	checkForCollisions();
}

void PhysicsScene::updateGizmos()
{
	for (auto pActor : m_actors) {
		pActor->makeGizmo();
	}
}

// function pointer array for doing our collisions
typedef CollisionData(*fn)(PhysicsObject*, PhysicsObject*);

static fn collisionFunctionArray[] =
{
	//              Plane							 Sphere							 Box
	/* Plane */	PhysicsScene::plane2Plane,	PhysicsScene::sphere2Plane,		PhysicsScene::plane2Box,
	/* Sphere */PhysicsScene::sphere2Plane, PhysicsScene::sphere2Sphere,	PhysicsScene::sphere2Box,
	/* Box */	PhysicsScene::plane2Box,	PhysicsScene::sphere2Box,		PhysicsScene::box2Box,
};

void PhysicsScene::checkForCollisions()
{
	int actorCount = m_actors.size();

	//need to check for collisions against all objects except this one.
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsObject* object1 = m_actors[outer];
			PhysicsObject* object2 = m_actors[inner];
			int shapeId1 = object1->getShapeID();
			int shapeId2 = object2->getShapeID();

			// using function pointers
			int functionIdx = (shapeId1 * SHAPE_COUNT) + shapeId2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIdx];
			if (collisionFunctionPtr != nullptr)
			{
				// did a collision occur?
				collisionData = collisionFunctionPtr(object1, object2);
				if (collisionData.collision == true)
				{
					resolveCollision(object1, object2);
				}
			}


		}
	}
}

CollisionData PhysicsScene::plane2Plane(PhysicsObject* object1, PhysicsObject* object2)
{
	return false;
}

CollisionData PhysicsScene::plane2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Plane *plane = dynamic_cast<Plane*>(object2);
	Box *box = dynamic_cast<Box*>(object1);

	glm::vec2 collisionNormal = plane->getNormal();

	glm::vec2 boxToPlaneBtmLeft = box->min();
	glm::vec2 boxToPlaneTopLeft = { box->min().x, box->max().y };
	glm::vec2 boxToPlaneTopRight = box->max();
	glm::vec2 boxToPlaneBtmRight = { box->min().x, box->max().y };

	//float intersectionMin = glm::distance(box->getPosition(), box->min()) * 0.5 - boxToPlaneBtmLeft;
	//float intersectionMax = glm::distance(box->getPosition(), box->max()) * 0.5 - boxToPlaneTopRight;

	std::vector<float> closestPosArry;

	closestPosArry.push_back( glm::dot(boxToPlaneTopLeft, (plane->getNormal() - plane->getDistance())));
	closestPosArry.push_back(glm::dot(boxToPlaneTopRight, (plane->getNormal() - plane->getDistance())));
	closestPosArry.push_back(glm::dot(boxToPlaneBtmLeft, (plane->getNormal() - plane->getDistance())));
	closestPosArry.push_back(glm::dot(boxToPlaneBtmRight, (plane->getNormal() - plane->getDistance())));

	float closestPoint = closestPosArry[0];
	int vertIndex = 0;

	for (int i = 1; i < 3; i++)
	{
		if (closestPosArry[i] < closestPoint)
		{
			closestPoint = closestPosArry[i];
			vertIndex = i;
		}
	}



	if (closestPosArry[vertIndex] < 0) {
		return CollisionData(true);
	}

	return false;
}



CollisionData PhysicsScene::sphere2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	Sphere *sphere = dynamic_cast<Sphere*>(object1);
	Plane *plane = dynamic_cast<Plane*>(object2);

	//if we are successful then test for collision
	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		// if we are behind plane then we flip the normal
		if (sphereToPlane < 0) {
			collisionNormal *= -1;
			sphereToPlane *= -1;
		}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0) 
		{
			return CollisionData(intersection, collisionNormal, true);
		}
	}
	return CollisionData(false);
}

CollisionData PhysicsScene::sphere2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	//try to cast objects to sphere and sphere
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1);
	Sphere *sphere2 = dynamic_cast<Sphere*>(object2);

	assert((sphere1 != nullptr && sphere2 != nullptr));
	//if we are successful then test for collision
	if (sphere1 != nullptr && sphere2 != nullptr) {
		//this is where the collision detection happens
		//you need code which sets the velocity of the two spheres to zero

		//if they are overlapping
		// get distance between 2 spheres
		float distBetween = 0;
		distBetween = glm::distance(sphere2->getPosition(), sphere1->getPosition());

		//calculate the normal
		glm::vec2 normal = glm::normalize(sphere2->getPosition() - sphere1->getPosition());

		// if distance is less than the combined radius of 
		// both spheres, then a collision occurred so set the
		if (distBetween < (sphere1->getRadius() + sphere2->getRadius()))
		{
			return CollisionData(distBetween, normal, true);
		}
	
	}
	return false;
}

CollisionData PhysicsScene::sphere2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Sphere *sphere = dynamic_cast<Sphere*>(object1);
	Box *box = dynamic_cast<Box*>(object2);

	if (box != nullptr && sphere != nullptr)
	{
		if (box->min().x < sphere->getPosition().x + sphere->getRadius()
			&& box->max().x > sphere->getPosition().x - sphere->getRadius()
			&& box->min().y < sphere->getPosition().y + sphere->getRadius()
			&& box->max().y > sphere->getPosition().y - sphere->getRadius())
		{
			return CollisionData(true);
		}
	}
	return false;
}

//bool PhysicsScene::box2Plane(PhysicsObject * object1, PhysicsObject * object2)
//{
//	return false;
//}

//bool PhysicsScene::box2Sphere(PhysicsObject * object1, PhysicsObject * object2)
//{
//
//	return false;
//}

CollisionData PhysicsScene::box2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Box *box1 = dynamic_cast<Box*>(object1);
	Box *box2 = dynamic_cast<Box*>(object2);

	if (box1 != nullptr && box2 != nullptr) 
	{
		if (box1->min().x < box2->max().x
			&& box1->max().x > box2->min().x
			&& box1->min().y < box2->max().y
			&& box1->max().y > box2->min().y)
		{
			return CollisionData(true);
		}
	}
	return false;
}

void PhysicsScene::resolveCollision(PhysicsObject * object1, PhysicsObject* object2)
{
	Rigidbody* rb1;
	Rigidbody* rb2;

	glm::vec2 normal;
	glm::vec2 relativeVelocity;
	float elasticity = 1;
	float j;

	if (dynamic_cast<Rigidbody*>(object1) == nullptr)
	{
		rb2 = (Rigidbody*)object2;
		normal = collisionData.normal;
		glm::vec2 relativeVelocity = rb2->getVelocity();

		elasticity = 1.0f;

		float invMass = 1.0f / rb2->getMass();
		float top = (-(1 + elasticity) * glm::dot((relativeVelocity), normal));
		j = top / invMass;

		glm::vec2 force = normal * j;

		rb2->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
	else if (dynamic_cast<Rigidbody*>(object2) == nullptr)
	{
		rb1 = (Rigidbody*)object1;

		normal = collisionData.normal;
		glm::vec2 relativeVelocity = rb1->getVelocity();

		elasticity = 1.0f;

		float invMass = 1.0f / rb1->getMass();
		float top = (-(1 + elasticity) * glm::dot((relativeVelocity), normal));
		j = top / invMass;

		glm::vec2 force = normal * j;

		rb1->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
	else
	{
		rb1 = (Rigidbody*)object1;
		rb2 = (Rigidbody*)object2;

		normal = glm::normalize(rb2->getPosition() - rb1->getPosition());
		relativeVelocity = rb2->getVelocity() - rb1->getVelocity();

		elasticity = 1;
		float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal)
			/ glm::dot(normal, normal * ((1 / rb1->getMass()) + (1 / rb2->getMass())));

		glm::vec2 force = normal * j;

		rb1->applyForce(force, Rigidbody::ForceMode::IMPULSE);
		rb2->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
}

void PhysicsScene::addActor(PhysicsObject* actor)
{
	assert(actor != nullptr);
	assert(std::find(std::begin(m_actors), std::end(m_actors), actor) == m_actors.end());
	m_actors.push_back(actor);
}

void PhysicsScene::removeActor(PhysicsObject* actor)
{
	std::remove(std::begin(m_actors), std::end(m_actors), actor);
}