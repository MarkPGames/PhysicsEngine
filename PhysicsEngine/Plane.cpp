#include "Plane.h"
#include <Gizmos.h>"
#include "Rigidbody.h"

Plane::Plane()
{

}

Plane::Plane(const glm::vec2& normal, float distance, glm::vec4 colour) : PhysicsObject(ShapeType::PLANE) , m_normal(glm::normalize(normal)), m_distanceToOrigin(distance), m_colour(colour)
{
	//m_distanceToOrigin = 0;
	//m_normal = glm::vec2(0, 1);
}


Plane::~Plane()
{
}

void Plane::makeGizmo() const
{
	float lineSegmentLength = 300;
	glm::vec2 centerPoint = m_normal * m_distanceToOrigin;
	// easy to rotate normal through 90 degrees around z
	glm::vec2 parallel(m_normal.y, -m_normal.x);
	glm::vec2 start = centerPoint + (parallel * lineSegmentLength);
	glm::vec2 end = centerPoint - (parallel * lineSegmentLength);
	aie::Gizmos::add2DLine(start, end, m_colour);
}
