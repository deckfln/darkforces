#pragma once
#include <iostream>
#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include "fwTransforms.h"

#include "../Reference.h"

using namespace Framework;

class alSource;
class alSound;

class fwObject3D : public Reference
{
	float debug = 0;
	alSource* m_source = nullptr;					// sound source, creation deferred to the first time a sound is played

	glm::mat4 m_modelMatrix;						// model space matrix

	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_scale = glm::vec3(1);
	glm::vec3 m_rotation = glm::vec3(0);
	glm::quat m_quaternion = glm::quat(0.0, 0.0, 0.0, 0.0);

	glm::mat4 m_saveModelMatrix = glm::mat4(0);			// backup data
	glm::mat4 m_saveWorldMatrix = glm::mat4(0);			
	glm::mat4 m_saveInverseWorldMatrix = glm::mat4(0);	
	glm::vec3 m_savePosition = glm::vec3(0);
	glm::vec3 m_saveScale = glm::vec3(1);
	glm::vec3 m_saverotation = glm::vec3(0);
	glm::quat m_saveQuaternion = glm::quat(0.0, 0.0, 0.0, 0.0);

	bool m_updated = true;							// if the matrix components have been update (or are new)

protected:
	std::string m_name;
	int classID = 0;

	glm::mat4 m_worldMatrix = glm::mat4(0);			// world space matrix (including children)
	glm::mat4 m_inverseWorldMatrix = glm::mat4(0);	// world space matrix (including children)

	fwObject3D* m_parent = nullptr;
	std::list <fwObject3D *> m_children;

	bool m_castShadow = false;
	bool m_receiveShadow = false;

	bool updated(void) { return m_updated; };
	void updated(bool b) { m_updated = b; };

public:
	fwObject3D();
	fwObject3D(const glm::vec3& position);

	void pushTransformations(void);						// push the current transformations
	void popTransformations(void);						// pop the transformations

	bool is_class(int classID);
	fwObject3D &set_name(const std::string& _name);
	const std::string& name(void) { return m_name;	};

	fwObject3D& rotate(const glm::vec3 &rotation);
	fwObject3D& rotate(const glm::quat& quaternion);
	fwObject3D& rotateBy(const glm::vec3& delta);		// rotate BY a delta vector
	fwObject3D& rotateBy(const glm::vec3* pDelta);		// rotate BY a delta vector
	fwObject3D& rotate(glm::vec3 *rotation);
	fwObject3D& rotate(glm::quat const *quaternion);
	fwObject3D& translate(const glm::vec3 &vector);
	fwObject3D& translate(float x, float y, float z);
	fwObject3D& translate(glm::vec3 *vector);
	fwObject3D& moveBy(const glm::vec3& delta);			// move the position BY a delta vector
	fwObject3D& moveBy(const glm::vec3* pDelta);		// move the position BY a delta vector
	fwObject3D &set_scale(const glm::vec3 &_scale);
	fwObject3D &set_scale(float _scale);
	fwObject3D& transform(const fwTransforms& transforms);
	fwObject3D& transform(fwTransforms *pTransforms);

	const glm::vec3& get_scale(void);
	const glm::vec3& get_position(void);
	const glm::vec3& position(void) { return m_position; };
	const glm::quat& quaternion(void) { return m_quaternion; };

	float distanceTo(fwObject3D* other);				// distance between the 2 objects
	float distanceTo(const glm::vec3& p);				// distance from the entity position to the point

	const std::list <fwObject3D *> &get_children(void);
	bool hasChild(fwObject3D* child);


	void updateWorldMatrix(fwObject3D *parent, bool force = false);

	bool castShadow(void) { return m_castShadow; }
	virtual bool castShadow(bool s) { m_castShadow = s; return s; }

	bool receiveShadow(void) { return m_receiveShadow; }
	bool receiveShadow(bool s) { m_receiveShadow = s; return s; }

	const glm::mat4& worldMatrix(void) { 
		updateWorldMatrix(nullptr);
		return m_worldMatrix; 
	}
	const glm::mat4& inverseWorldMatrix(void) { 
		return m_inverseWorldMatrix; 
	}
	glm::mat4* pWorldMatrix(void) { 
		updateWorldMatrix(nullptr);
		return &m_worldMatrix; 
	}
	glm::mat4* pInverseWorldMatrix(void) { 
		return &m_inverseWorldMatrix; 
	}

	void worldMatrix(const glm::mat4& worldMatrix,
		const glm::mat4& inverseWorldMatrix
	);
	void worldMatrix(const glm::mat4& worldMatrix);
	void worldMatrix(glm::mat4* pWorldMatrix,
		glm::mat4* pInverseWorldMatrix
	);
	void worldMatrix(glm::mat4* pWorldMatrix);

	fwObject3D &addChild(fwObject3D * obj);			// add an object to the scene
	void removeChild(fwObject3D* obj);				// remove an object from the scene

	float sqDistanceTo(fwObject3D *to);

	// audio API
	bool play(alSound* sound);
	void stop(alSound* sound);

	~fwObject3D();
};