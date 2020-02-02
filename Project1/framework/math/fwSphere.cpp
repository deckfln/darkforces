#include "fwSphere.h"

fwSphere::fwSphere()
{

}

fwSphere::fwSphere(float radius)
{
	m_radius = radius;
}

fwSphere::fwSphere(glm::vec3 &center, float radius)
{
	m_center = center;
	m_radius = radius;
}

void fwSphere::applyMatrix4From(glm::mat4 &matrix, fwSphere *source)
{
	m_center = glm::vec3(matrix * glm::vec4(source->m_center, 1.0));

	float scaleXSq = matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2];
	float scaleYSq = matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2];
	float scaleZSq = matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2];

	float maxScaleOnAxis = sqrt(fmax(fmax(scaleXSq, scaleYSq), scaleZSq));

	m_radius = source->m_radius * maxScaleOnAxis;
}

void fwSphere::applyMatrix4(glm::mat4& matrix)
{
	m_center = glm::vec3(matrix * glm::vec4(m_center, 1.0));

	float scaleXSq = matrix[0][0] * matrix[0][0] + matrix[0][1] * matrix[0][1] + matrix[0][2] * matrix[0][2];
	float scaleYSq = matrix[1][0] * matrix[1][0] + matrix[1][1] * matrix[1][1] + matrix[1][2] * matrix[1][2];
	float scaleZSq = matrix[2][0] * matrix[2][0] + matrix[2][1] * matrix[2][1] + matrix[2][2] * matrix[2][2];

	float maxScaleOnAxis = sqrt(fmax(fmax(scaleXSq, scaleYSq), scaleZSq));

	m_radius = m_radius * maxScaleOnAxis;
}

fwSphere::~fwSphere()
{

}