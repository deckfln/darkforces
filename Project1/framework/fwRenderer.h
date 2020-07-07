#pragma once

#include <list>
#include <vector>
#include <map>
#include <string>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class glColorMap;
class glProgram;
class glTexture;
class fwMaterial;
class fwObject3D;
class fwMesh;
class fwScene;
class fwLight;
class fwCamera;

class fwRenderer
{
protected:
	glColorMap* m_colorMap = nullptr;
	std::string m_customLightning = "";
	std::map<std::string, bool> m_customDefines;

	std::map <std::string, glProgram*> m_programs;
	std::map <int, fwMaterial*> m_materials;
	void getAllChildren(fwObject3D* root, std::vector<std::list <fwMesh*>>& meshes);
	void parseChildren(fwObject3D* root, std::list <fwMesh*> meshes[], fwCamera* camera);
	bool drawShadows(fwCamera* camera, fwScene* scene);
	virtual void drawMesh(fwCamera* camera, fwMesh* mesh, glProgram* program, const std::string &defines) {};

	void preProcessLights(fwScene* scene, 
		std::map <std::string, std::list <fwLight*>>& lightsByType, 
		std::string& defines, 
		std::string& codeLights
	);
	void parseShaders(const std::list <fwMesh*>& meshes,
		const std::string& defines,
		const std::string& codeLights,
		bool withShadow,
		std::map<std::string, std::map<int, std::list <fwMesh*>>>& meshPerMaterial
	);
	void drawMeshes(
		fwCamera *camera,
		const std::list <fwMesh*>& meshes,
		const std::string& defines,
		const std::string& codeLights,
		const std::map <std::string, std::list <fwLight*>>& lightsByType,
		bool withShadow
	);
	void drawTransparentMeshes(
		fwCamera* camera,
		const std::list <fwMesh*>& meshes,
		const std::string& defines,
		const std::string& codeLights,
		const std::map <std::string, std::list <fwLight*>>& lightsByType,
		bool withShadow
	);
	static void sortMeshes(std::list<fwMesh*>& meshes, const glm::vec3& CameraPosition);

public:
	fwRenderer();

	virtual glTexture* draw(fwCamera* camera, fwScene* scene) { return nullptr; };
	void start(void);
	void stop(void);
	glm::vec2 size(void);
	void customLight(const std::string& shader) { m_customLightning = shader; };
	void customDefine(const std::string& define, bool defined) { m_customDefines[define] = defined; };

	~fwRenderer();
};