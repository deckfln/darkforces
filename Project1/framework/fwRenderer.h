#pragma once

#include <list>
#include <vector>

#include "../glEngine/glProgram.h"
#include "fwCamera.h"
#include "fwScene.h"

class fwRenderer
{
protected:
	glColorMap* m_colorMap = nullptr;

	std::map <std::string, glProgram*> m_programs;
	std::map <int, fwMaterial*> m_materials;
	void getAllChildren(fwObject3D* root, std::vector<std::list <fwMesh*>>& meshes);
	void parseChildren(fwObject3D* root, std::list <fwMesh*> meshes[], fwCamera* camera);
	bool drawShadows(fwCamera* camera, fwScene* scene);
	virtual void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, std::string &defines) {};

	void preProcessLights(fwScene* scene, std::map <std::string, std::list <fwLight*>>& lightsByType, std::string& defines, std::string& codeLights);
	void parseShaders(std::list <fwMesh*>& meshes,
		std::string& defines,
		std::string codeLights,
		bool withShadow,
		std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial
	);
	void drawMeshes(
		fwCamera *camera,
		std::list <fwMesh*>& meshes,
		std::string& defines,
		std::string& codeLights,
		std::map <std::string, std::list <fwLight*>>& lightsByType,
		bool withShadow
	);
	void drawTransparentMeshes(
		fwCamera* camera,
		std::list <fwMesh*>& meshes,
		std::string& defines,
		std::string& codeLights,
		std::map <std::string, std::list <fwLight*>>& lightsByType,
		bool withShadow
	);
	static void sortMeshes(std::list<fwMesh*>& meshes, glm::vec3 CameraPosition);

public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	void start(void);
	void stop(void);
	glm::vec2 size(void);

	~fwRenderer();
};