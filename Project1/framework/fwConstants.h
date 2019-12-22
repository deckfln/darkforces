#pragma once

// list of possible meshes
enum {
	FW_MESH_NORMAL,
	FW_MESH_INSTANCED,
	FW_MESH_SKINNED
};

// list of object types sorted in the renderer
enum {
	FW_RENDER_OPAQ,
	FW_RENDER_OPAQ_PARTICLES,
	FW_RENDER_TRANSPARENT
};

#define FW_LIGHT1 8
#define FW_DIRECTIONAL_LIGHT FW_LIGHT1 | 131072
#define FW_POINT_LIGHT FW_LIGHT1 | 262144
#define FW_SPOT_LIGHT FW_LIGHT1 | 524288
