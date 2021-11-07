#include "dfSign.h"

#include <imgui.h>

#include "../dfBitmap.h"
#include "../dfMesh.h"
#include "../dfSector.h"
#include "../../config.h"
#include "../dfComponent.h"

DarkForces::Component::Sign::Sign(dfMesh* mesh, dfSector* sector, dfWall* wall, float z, float z1):
	gaComponent(DF_COMPONENT_SIGN),
	m_mesh(mesh),
	m_position(0),
	m_normal(0)
{
	buildGeometry(sector, wall, z, z1);
}

/**
 *
 */
void DarkForces::Component::Sign::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::TRIGGER:
		setStatus(1);
		break;
	case DarkForces::Message::DONE:
		setStatus(0);
		break;
	}
}

/**
 * display the component in the debugger
 */
void DarkForces::Component::Sign::debugGUIinline(void)
{
	if (ImGui::TreeNode("Sign")) {
		ImGui::Text("Status: %d", m_status);
		ImGui::TreePop();
	}
}

/**
 * change the display of the sign
 */
void DarkForces::Component::Sign::setStatus(int status)
{
	if (status == m_status) {
		return;
	}

	m_status = status;

	dfBitmapImage* image = m_bitmap->getImage(status);	// extract the target image
	float textureID = (float)image->m_textureID;				// index of the image in the atlas map

	// update the vertices with the new textureID
	for (int i = m_start; i < m_start + m_size; i++) {
		m_mesh->updateTexture(i, textureID);
	}

	// push the changes
	m_mesh->updateGeometryTextures(m_start, m_size);
}

/**
 *
 */
void DarkForces::Component::Sign::buildGeometry(dfSector* sector, dfWall* wall, float z, float z1)
{
	// resize the opengl buffers
	int p = m_mesh->resize(6);

	m_start = p;
	m_size = 6;

	float x = sector->m_vertices[wall->m_left].x,
		y = sector->m_vertices[wall->m_left].y,
		x1 = sector->m_vertices[wall->m_right].x,
		y1 = sector->m_vertices[wall->m_right].y;

	float bitmapID = wall->m_tex[DFWALL_TEXTURE_SIGN].r;
	m_bitmap = m_mesh->bitmaps()[(int)bitmapID];

	dfBitmapImage* image = m_bitmap->getImage();

	float length = sqrt(pow(x - x1, 2) + pow(y - y1, 2));
	float xpixel = (float)image->m_width;
	float ypixel = (float)image->m_height;

	glm::vec2 segment = glm::normalize(glm::vec2(x1 - x, y1 - y));
	glm::vec2 start = sector->m_vertices[wall->m_left] + segment * (wall->m_tex[DFWALL_TEXTURE_SIGN].g - wall->m_tex[DFWALL_TEXTURE_MID].g);
	glm::vec2 end = sector->m_vertices[wall->m_left] + segment * (wall->m_tex[DFWALL_TEXTURE_SIGN].g - wall->m_tex[DFWALL_TEXTURE_MID].g + xpixel / 8.0f);

	// translate by the mesh position
	const glm::vec3& translate = m_mesh->position() * 10.0f;

	start.x -= translate.x;
	start.y -= translate.z;

	end.x -= translate.x;
	end.y -= translate.z;

	// wall normals
	m_normal = glm::normalize(glm::vec3(-segment.y, segment.x, 0));	//  and (dy, -dx).

	// create a copy of the wall and shrink to the size and position of the sign
	// ratio of texture pixel vs world position = 64 pixels for 8 clicks => 8x1
	glm::vec3 sign_p = glm::vec3(
		start.x,
		start.y,
		//z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b + wall->m_tex[DFWALL_TEXTURE_MID].b)
		z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b)
	);
	glm::vec3 sign_p1 = glm::vec3(
		end.x,
		end.y,
		// z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b + wall->m_tex[DFWALL_TEXTURE_MID].b) + ypixel / 8.0f
		z - (wall->m_tex[DFWALL_TEXTURE_SIGN].b) + ypixel / 8.0f
	);

	// move the the wall along the normal
	sign_p += m_normal / 10.0f;
	sign_p1 += m_normal / 10.0f;

	// Handle request to flip the texture
	bool flipTexture = wall->flag1(dfWallFlag::FLIP_TEXTURE_HORIZONTALLY);

	glm::vec2 offset(0, 0);
	glm::vec2 size(1, 1);

	m_mesh->updateRectangle(p,
		sign_p,
		sign_p1,
		offset,
		size,
		image->m_textureID,
		sector->m_ambient);

	// position of the sign is the center of the sign. move to gl space
	m_position = (glm::vec3(sign_p.x, sign_p.z, sign_p.y) + glm::vec3(sign_p1.x, sign_p1.z, sign_p1.y)) / 20.0f;
	m_normal = glm::vec3(m_normal.x, m_normal.z, m_normal.y) / 10.0f;

	float dx = (sign_p1.x - sign_p.x) / 20.0f;
	float dy = (sign_p1.z - sign_p.z) / 20.0f;
	float dz = (sign_p1.y - sign_p.y) / 20.0f;

	m_localAABB.set(-dx, -dy, -dz, dx, dy, dz);

	// check if it is needed to resize the geometry
	m_mesh->resizeGeometry();
}
