#include "fwCameraPanoramic.h"

#include <string>

fwCameraPanoramic::fwCameraPanoramic(float aspect, float _near, float _far) :
    m_far(_far)
{
    m_projection = glm::perspective(glm::radians(90.0f), aspect, _near, _far);
}

void fwCameraPanoramic::update(void)
{
    // build a transformation matrix per face of the shadow cube
    m_transforms[0] = m_projection * glm::lookAt(position(), position() + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    m_transforms[1] = m_projection * glm::lookAt(position(), position() + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
    m_transforms[2] = m_projection * glm::lookAt(position(), position() + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
    m_transforms[3] = m_projection * glm::lookAt(position(), position() + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
    m_transforms[4] = m_projection * glm::lookAt(position(), position() + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
    m_transforms[5] = m_projection * glm::lookAt(position(), position() + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

    updateWorldMatrix(nullptr, false);
    m_projScreenMatrix = m_projection * glm::inverse(m_worldMatrix);

    m_frustum.setFromMatrix(m_projScreenMatrix);
}

void fwCameraPanoramic::set_uniform(std::string name, std::string attr, glProgram* program)
{
    if (attr == "far_plane") {
        program->set_uniform(name, m_far);
    }
    else {
        fwCamera::set_uniform(name, attr, program);
    }
}

void fwCameraPanoramic::set_uniforms(glProgram* program)
{
    fwCamera::set_uniforms(program);

    //todo: if I use an array (explicit by the size), why do I have to search for tranforms[0]
    program->set_uniform("transforms[0]", m_transforms, 6);
    program->set_uniform("far_plane", m_far);
}

fwCameraPanoramic::~fwCameraPanoramic()
{

}