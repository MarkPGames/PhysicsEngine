#pragma once
#include "PhysicsScene.h"
#include "Application.h"
#include "Renderer2D.h"

class Sphere;
class Box;

class PhysicsEngineApp : public aie::Application {
public:

	PhysicsEngineApp();
	virtual ~PhysicsEngineApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void setupContinousDemo(glm::vec2 startPos, float inclination, float speed, float gravity);

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
	PhysicsScene* m_physicsScene;
	Sphere* m_sphere1;
	Box* m_box1;

};