#include "PhysicsEngineApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "Sphere.h"
#include "Box.h"
#include <Gizmos.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <cmath>

#define M_PI  3.14159265358979323846


PhysicsEngineApp::PhysicsEngineApp() {

}

PhysicsEngineApp::~PhysicsEngineApp() {

}

bool PhysicsEngineApp::startup() {
	
	aie::Gizmos::create(255U, 255U, 65535U, 65535U);

	m_2dRenderer = new aie::Renderer2D();

	

	// TODO: remember to change this when redistributing a build!
	// the following path would be used instead: "./font/consolas.ttf"
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(glm::vec2(0, -10));
	m_physicsScene->setTimeStep(0.5f);

	float radius = 1.0f;
	float speed = 30;
	glm::vec2 startPos(-40, 0);
	float inclination = (float)M_PI / 4.0f;
	float inclinationInRadians = 0.78539816339;

	m_physicsScene->addActor(new Sphere(startPos, glm::vec2( (cos(inclinationInRadians) , sin(inclinationInRadians)) * speed ), 1, radius, glm::vec4(1, 0, 0, 1)));

	//m_sphere1 = new Sphere(glm::vec2{ 0, 0 }, glm::vec2{ 0,0 }, 10, 5, glm::vec4{ 1,0,0,1 });
	//m_box1 = new Box(glm::vec2{ 20, 0 }, glm::vec2{ 0,0 }, 10, glm::vec2{ 5,5 }, glm::vec4{ 0,1,0,1 });
	//
	//m_physicsScene->addActor(m_sphere1);
	//m_physicsScene->addActor(m_box1); 

	setupContinousDemo(glm::vec2(-40, 0), 45.0f, 20.0f, -10.0f);

	return true;
}

void PhysicsEngineApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
	
}

void PhysicsEngineApp::update(float deltaTime) {

	//aie::Gizmos::clear();

	// input example
	aie::Input* input = aie::Input::getInstance();

	m_physicsScene->update(deltaTime); 
	m_physicsScene->updateGizmos();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void PhysicsEngineApp::draw() {

	// wipe the screen to the background colour
	clearScreen();
	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	
	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	float aspectRatio = (float)getWindowWidth() / getWindowHeight();
	aie::Gizmos::draw2D(glm::ortho<float>(-100, 100, -100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));

	// done drawing sprites
	m_2dRenderer->end();
}

void PhysicsEngineApp::setupContinousDemo(glm::vec2 startPos, float inclination, float speed, float gravity)
{
	float t = 0;
	float tStep = 0.5f;
	float radius = 1.0f;
	int segments = 12;
	glm::vec4 colour = glm::vec4(1, 1, 0, 1);

	while (t <= 5)
	{
		// calculate the x, y position of the projectile at time t
		glm::vec2 currPosition;

		currPosition.x = startPos.x + (speed * t);
		currPosition.y = startPos.y + (speed * t) + (0.5f * (gravity * (t*t)));
			

		aie::Gizmos::add2DCircle(glm::vec2(currPosition.x, currPosition.y), radius, segments, colour);
		t += tStep;
	}
}