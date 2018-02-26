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
	/* Plane */	PhysicsScene::plane2Plane,	PhysicsScene::sphere2Plane,		PhysicsScene::box2Plane,
	/* Sphere */PhysicsScene::plane2Sphere, PhysicsScene::sphere2Sphere,	PhysicsScene::box2Sphere,
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
					seperateCollision(object1, object2);
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

	if (plane == nullptr && box == nullptr)
	{
		return CollisionData(false);
	}


	glm::vec2 collisionNormal = plane->getNormal();

	glm::vec2 boxToPlaneBtmLeft = box->min();
	glm::vec2 boxToPlaneTopLeft = { box->min().x, box->max().y };
	glm::vec2 boxToPlaneTopRight = box->max();
	glm::vec2 boxToPlaneBtmRight = { box->max().x, box->min().y };

	//float intersectionMin = glm::distance(box->getPosition(), box->min()) * 0.5 - boxToPlaneBtmLeft;
	//float intersectionMax = glm::distance(box->getPosition(), box->max()) * 0.5 - boxToPlaneTopRight;

	std::vector<float> closestPosArry;

	closestPosArry.push_back(glm::dot(boxToPlaneTopLeft - collisionNormal * plane->getDistance(), collisionNormal));
	closestPosArry.push_back(glm::dot(boxToPlaneTopRight - collisionNormal * plane->getDistance(), collisionNormal));
	closestPosArry.push_back(glm::dot(boxToPlaneBtmLeft - collisionNormal * plane->getDistance(), collisionNormal));
	closestPosArry.push_back(glm::dot(boxToPlaneBtmRight - collisionNormal * plane->getDistance(), collisionNormal));

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

	if (closestPosArry[vertIndex] < 0) 
	{
		return CollisionData(-closestPosArry[vertIndex], collisionNormal, true);
	}

	return false;
}



CollisionData PhysicsScene::sphere2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	return plane2Sphere(object2, object1);
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

		float radiusCombined = sphere1->getRadius() + sphere2->getRadius();
		radiusCombined *= radiusCombined;

		//calculate the normal
		glm::vec2 normal = glm::normalize(sphere2->getPosition() - sphere1->getPosition());
		

		float xSquared = sphere2->getPosition().x - sphere1->getPosition().x;
		xSquared *= xSquared;
		float ySquared = sphere2->getPosition().y - sphere1->getPosition().y;
		ySquared *= ySquared;

		// if distance is less than the combined radius of 
		// both spheres, then a collision occurred so set the

		if ((xSquared + ySquared) < radiusCombined)
		{
			glm::vec2 vecBetween = sphere1->getPosition() - sphere2->getPosition();
			float overlap = (sphere1->getRadius() + sphere2->getRadius()) - glm::length(vecBetween);
			return CollisionData(overlap, normal, true);
		}
	
	}
	return false;
}

CollisionData PhysicsScene::sphere2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Sphere *sphere;
	Box *box;

	if (dynamic_cast<Sphere*>(object1) != nullptr)
	{
		sphere = (Sphere*)object1;
		box = (Box*)object2;
	}
	else
	{
		sphere = (Sphere*)object2;
		box = (Box*)object1;
	}

	glm::vec2 vecBetween = sphere->getPosition() - box->getPosition();

	glm::vec2 offset = {0,0};

	//clamp offset
	offset.x = glm::dot(vecBetween, { 1,0 });
	offset.y = glm::dot(vecBetween, { 0,1 });

	if (glm::abs(offset.x) > box->getSize().x)
	{
		offset.x = (box->getSize().x) * ((offset.x > 0) ? 1.0 : -1.0f);
	}
	if (glm::abs(offset.y) > box->getSize().y)
	{
		offset.y = (box->getSize().y) * ((offset.y > 0) ? 1.0 : -1.0f);
	}

	offset += box->getPosition();

	glm::vec2 vecBetweenClamp = offset - sphere->getPosition();

	if (glm::length(vecBetweenClamp) < sphere->getRadius())
	{
		float overlap = glm::length(vecBetweenClamp) - sphere->getRadius();
		glm::vec2 normal = glm::normalize(vecBetweenClamp);

		return CollisionData(overlap, normal, true);
	}

	return CollisionData(false);
}

CollisionData PhysicsScene::box2Box(PhysicsObject * object1, PhysicsObject * object2)
{
	Box *box1 = dynamic_cast<Box*>(object1);
	Box *box2 = dynamic_cast<Box*>(object2);
	
	float offsetFaceDis = 0.0f;

	if (box1 != nullptr && box2 != nullptr) 
	{
		if (box1->min().x < box2->max().x
			&& box1->max().x > box2->min().x
			&& box1->min().y < box2->max().y
			&& box1->max().y > box2->min().y)
		{
			glm::vec2 xOverlap1 = { box1->max().x - box2->min().x, 0.0f };
			glm::vec2 xOverlap2 = { box1->min().x - box2->max().x, 0.0f };

			glm::vec2 yOverlap1 = { 0.0f , box1->max().y - box2->min().y };
			glm::vec2 yOverlap2 = { 0.0f ,box1->min().y - box2->max().y };


			float lowestDist = abs(xOverlap1.x);
			glm::vec2 lowestVec = xOverlap1;



			if (abs(xOverlap2.x) < lowestDist)
			{
				lowestDist = abs(xOverlap2.x);
				lowestVec = xOverlap2;
			}

			if (abs(yOverlap1.y) < lowestDist)
			{
				lowestDist = abs(yOverlap1.y);
				lowestVec = yOverlap1;
			}

			if (abs(yOverlap2.y) < lowestDist)
			{
				lowestDist = abs(yOverlap2.y);
				lowestVec = yOverlap2;
			}

			return CollisionData(lowestDist, glm::normalize(lowestVec), true);

		}
	}
	return false;
}

CollisionData PhysicsScene::box2Plane(PhysicsObject * object1, PhysicsObject * object2)
{
	return plane2Box(object2, object1);
}

CollisionData PhysicsScene::box2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	CollisionData collData = sphere2Box(object2, object1);
	collData.normal *= -1.0f;
	return collData;
}

CollisionData PhysicsScene::plane2Sphere(PhysicsObject * object1, PhysicsObject * object2)
{
	Sphere *sphere = dynamic_cast<Sphere*>(object1);
	Plane *plane = dynamic_cast<Plane*>(object2);

	//if we are successful then test for collision
	if (sphere != nullptr && plane != nullptr)
	{
		glm::vec2 collisionNormal = plane->getNormal();
		float sphereToPlane = glm::dot(sphere->getPosition(), plane->getNormal()) - plane->getDistance();

		//// if we are behind plane then we flip the normal
		//if (sphereToPlane < 0) {
		//	collisionNormal *= -1;
		//	sphereToPlane *= -1;
		//}

		float intersection = sphere->getRadius() - sphereToPlane;
		if (intersection > 0)
		{
			return CollisionData(intersection, collisionNormal, true);
		}
	}
	return CollisionData(false);
}

void PhysicsScene::resolveCollision(PhysicsObject * object1, PhysicsObject* object2)
{
	Rigidbody* rb1 = dynamic_cast<Rigidbody*>(object1);
	Rigidbody* rb2  = dynamic_cast<Rigidbody*>(object2);

	glm::vec2 normal;
	glm::vec2 relativeVelocity;
	float elasticity = 1.0f;
	float j;

	if (rb1 == nullptr)
	{
		rb2 = (Rigidbody*)object2;
		normal = collisionData.normal;

		glm::vec2 relativeVelocity = rb2->getVelocity();

		float invMass = 1.0f / rb2->getMass();
		float top = (-(1 + elasticity) * glm::dot((relativeVelocity), normal));
		j = top / invMass;

		glm::vec2 force = normal * j;

		rb2->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
	else if (rb2 == nullptr)
	{
		rb1 = (Rigidbody*)object1;


		normal = collisionData.normal;
		glm::vec2 relativeVelocity = rb1->getVelocity();

		float invMass = 1.0f / rb1->getMass();
		float top = (-(1 + elasticity) * glm::dot((relativeVelocity), normal));
		j = top / invMass;

		glm::vec2 force = normal * j;

		rb1->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
	else
	{
		normal = glm::normalize(rb2->getPosition() - rb1->getPosition());
		relativeVelocity = rb2->getVelocity() - rb1->getVelocity();

		float j = glm::dot(-(1 + elasticity) * (relativeVelocity), normal)
			/ glm::dot(normal, normal * ((1 / rb1->getMass()) + (1 / rb2->getMass())));

		glm::vec2 force = normal * j;

		rb1->applyForce(-force, Rigidbody::ForceMode::IMPULSE);
		rb2->applyForce(force, Rigidbody::ForceMode::IMPULSE);
	}
}

void PhysicsScene::seperateCollision(PhysicsObject * object1, PhysicsObject * object2)
{
	Rigidbody* rb1 = dynamic_cast<Rigidbody*>(object1);
	Rigidbody * rb2 = dynamic_cast<Rigidbody*>(object2);

	float ratio1 = 0.0f;
	float ratio2 = 0.0f;

	float totalMass = 0.0f;

	//assert(collisionData.overlap > 0);

	if (rb1 == nullptr)
	{
		ratio2 = 1.0f;
		totalMass = rb2->getMass();
		ratio2 = rb2->getMass() / totalMass;
		rb2->setPosition(rb2->getPosition() + ratio2 * (collisionData.overlap) * collisionData.normal);
	}
	if (rb2 == nullptr)
	{
		ratio1 = 1.0f;
		totalMass = rb1->getMass();
		ratio1 = rb1->getMass() / totalMass;
		rb1->setPosition(rb1->getPosition() + ratio1 * collisionData.overlap * collisionData.normal);
	}
	if (rb1 != nullptr && rb2 != nullptr)
	{
		totalMass = rb1->getMass() + rb2->getMass();
		ratio1 = 1 - rb1->getMass() / totalMass;
		ratio2 = 1 - rb2->getMass() / totalMass;

		rb1->setPosition(rb1->getPosition() - ratio1 * collisionData.overlap * collisionData.normal);
		rb2->setPosition(rb2->getPosition() + ratio2 * collisionData.overlap * collisionData.normal);
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