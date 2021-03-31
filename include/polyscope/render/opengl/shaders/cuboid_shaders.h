#pragma once

#include "polyscope/render/opengl/gl_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {

// High level pipeline
extern const ShaderStageSpecification FLEX_CUBOID_VERT_SHADER;
extern const ShaderStageSpecification FLEX_CUBOID_GEOM_SHADER;
extern const ShaderStageSpecification FLEX_CUBOID_FRAG_SHADER;

// Rules specific to cuboids
extern const ShaderReplacementRule CUBOID_PROPAGATE_VALUE;
extern const ShaderReplacementRule CUBOID_PROPAGATE_BLEND_VALUE;
extern const ShaderReplacementRule CUBOID_PROPAGATE_COLOR;
extern const ShaderReplacementRule CUBOID_PROPAGATE_BLEND_COLOR;
extern const ShaderReplacementRule CUBOID_PROPAGATE_PICK;


} // namespace backend_openGL3_glfw
} // namespace render
} // namespace polyscope
