#pragma once
#include <glm/glm.hpp>

enum ShapeType 
{
	PLANE = 0,
	SPHERE,
	BOX,
	SHAPE_COUNT
};

class PhysicsObject
{
protected:
	PhysicsObject(ShapeType a_shapeID) : m_shapeID(a_shapeID) {}
	PhysicsObject();

public:
	virtual void fixedUpdate(glm::vec2 gravity, float timeStep) = 0;
	virtual void debug() = 0;
	virtual void makeGizmo() const = 0;
	virtual void resetPosition() {};

	int getShapeID() { return m_shapeID; };

protected: 
	 ShapeType m_shapeID;
};
