#pragma once

#include "polyscope/render/engine.h"

namespace polyscope {
namespace render {

// == Texture draw shaders
extern const ShaderStageSpecification TEXTURE_DRAW_VERT_SHADER;
extern const ShaderStageSpecification SPHEREBG_DRAW_VERT_SHADER;
extern const ShaderStageSpecification PLAIN_TEXTURE_DRAW_FRAG_SHADER;
extern const ShaderStageSpecification DOT3_TEXTURE_DRAW_FRAG_SHADER;
extern const ShaderStageSpecification MAP3_TEXTURE_DRAW_FRAG_SHADER;
extern const ShaderStageSpecification SPHEREBG_DRAW_FRAG_SHADER;

// == Lighting shaders
extern const ShaderStageSpecification MAP_LIGHT_FRAG_SHADER;
extern const ShaderStageSpecification SPLIT_SPECULAR_PRECOMPUTE_FRAG_SHADER;

// == Histogram shaders
extern const ShaderStageSpecification HISTOGRAM_VERT_SHADER;
extern const ShaderStageSpecification HISTOGRAM_FRAG_SHADER;

// == Ground plane shaders
extern const ShaderStageSpecification GROUND_PLANE_VERT_SHADER;
extern const ShaderStageSpecification GROUND_PLANE_FRAG_SHADER;

// == Surface mesh shaders

extern const ShaderStageSpecification PLAIN_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification PLAIN_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification SURFACE_WIREFRAME_VERT_SHADER;
extern const ShaderStageSpecification SURFACE_WIREFRAME_FRAG_SHADER;
extern const ShaderStageSpecification PICK_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification PICK_SURFACE_FRAG_SHADER;

extern const ShaderStageSpecification VERTCOLOR3_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification VERTCOLOR3_SURFACE_FRAG_SHADER;

extern const ShaderStageSpecification VERTCOLOR_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification VERTCOLOR_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification HALFEDGECOLOR_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification HALFEDGECOLOR_SURFACE_FRAG_SHADER;

extern const ShaderStageSpecification VERT_DIST_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification VERT_DIST_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification PARAM_SURFACE_VERT_SHADER;
extern const ShaderStageSpecification PARAM_CHECKER_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification PARAM_GRID_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification PARAM_LOCAL_RAD_SURFACE_FRAG_SHADER;
extern const ShaderStageSpecification PARAM_LOCAL_CHECKER_SURFACE_FRAG_SHADER;

// == Vector shaders
extern const ShaderStageSpecification PASSTHRU_VECTOR_VERT_SHADER;
extern const ShaderStageSpecification VECTOR_GEOM_SHADER;
extern const ShaderStageSpecification VECTOR_FRAG_SHADER;

// == Ribbon shaders
extern const ShaderStageSpecification RIBBON_VERT_SHADER;
extern const ShaderStageSpecification RIBBON_GEOM_SHADER;
extern const ShaderStageSpecification RIBBON_FRAG_SHADER;

// == Sphere shaders
extern const ShaderStageSpecification SPHERE_VERT_SHADER;
extern const ShaderStageSpecification SPHERE_VALUE_VERT_SHADER;
extern const ShaderStageSpecification SPHERE_COLOR_VERT_SHADER;
extern const ShaderStageSpecification SPHERE_BILLBOARD_GEOM_SHADER;
extern const ShaderStageSpecification SPHERE_VALUE_BILLBOARD_GEOM_SHADER;
extern const ShaderStageSpecification SPHERE_COLOR_BILLBOARD_GEOM_SHADER;
extern const ShaderStageSpecification SPHERE_BILLBOARD_FRAG_SHADER;
extern const ShaderStageSpecification SPHERE_VALUE_BILLBOARD_FRAG_SHADER;
extern const ShaderStageSpecification SPHERE_COLOR_BILLBOARD_FRAG_SHADER;
extern const ShaderStageSpecification SPHERE_COLOR_PLAIN_BILLBOARD_FRAG_SHADER;

// == Cylinder shaders
extern const ShaderStageSpecification PASSTHRU_CYLINDER_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_VALUE_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_COLOR_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_VALUE_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_COLOR_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_PICK_VERT_SHADER;
extern const ShaderStageSpecification CYLINDER_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_VALUE_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_COLOR_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_VALUE_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_COLOR_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_PICK_GEOM_SHADER;
extern const ShaderStageSpecification CYLINDER_FRAG_SHADER;
extern const ShaderStageSpecification CYLINDER_VALUE_FRAG_SHADER;
extern const ShaderStageSpecification CYLINDER_COLOR_FRAG_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_VALUE_FRAG_SHADER;
extern const ShaderStageSpecification CYLINDER_BLEND_COLOR_FRAG_SHADER;
extern const ShaderStageSpecification CYLINDER_PICK_FRAG_SHADER;

// == Cuboid shaders
extern const ShaderStageSpecification PASSTHRU_CUBOID_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_VALUE_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_COLOR_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_VALUE_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_COLOR_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_PICK_VERT_SHADER;
extern const ShaderStageSpecification CUBOID_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_VALUE_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_COLOR_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_VALUE_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_COLOR_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_PICK_GEOM_SHADER;
extern const ShaderStageSpecification CUBOID_FRAG_SHADER;
extern const ShaderStageSpecification CUBOID_VALUE_FRAG_SHADER;
extern const ShaderStageSpecification CUBOID_COLOR_FRAG_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_VALUE_FRAG_SHADER;
extern const ShaderStageSpecification CUBOID_BLEND_COLOR_FRAG_SHADER;
extern const ShaderStageSpecification CUBOID_PICK_FRAG_SHADER;

} // namespace render
} // namespace polyscope
