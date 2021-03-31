// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.

#include "polyscope/render/opengl/shaders/cuboid_shaders.h"

namespace polyscope {
namespace render {
namespace backend_openGL3_glfw {


// clang-format off

const ShaderStageSpecification FLEX_CUBOID_VERT_SHADER = {

    ShaderStageType::Vertex,

    // uniforms
    {
        {"u_modelView", DataType::Matrix44Float},
    }, 

    // attributes
    {
        {"a_position_tail", DataType::Vector3Float},
        {"a_position_tip", DataType::Vector3Float},
        {"a_up_direction", DataType::Vector3Float},
    },

    {}, // textures

    // source
    R"(
        in vec3 a_position_tail;
        in vec3 a_position_tip;
        in vec3 a_up_direction;
        uniform mat4 u_modelView;

        out vec4 position_tip;
        out vec3 up_direction;
        
        void main()
        {
            gl_Position = u_modelView * vec4(a_position_tail,1.0);
            position_tip = u_modelView * vec4(a_position_tip, 1.0);
            up_direction = mat3(u_modelView) * a_up_direction;
        }
    )"
};



const ShaderStageSpecification FLEX_CUBOID_GEOM_SHADER = {
    
    ShaderStageType::Geometry,
    
    // uniforms
    {
        {"u_projMatrix", DataType::Matrix44Float},
        {"u_wn", DataType::Float},
        {"u_wb", DataType::Float},
    }, 

    // attributes
    {
    },

    {}, // textures

    // source
R"(
    ${ GLSL_VERSION }$

    layout(points) in;
    layout(triangle_strip, max_vertices=24) out;
    in vec4 position_tip[];
    in vec3 up_direction[];
    uniform mat4 u_projMatrix;
    uniform mat4 u_modelView;
    uniform float u_wn;
    uniform float u_wb;
    out vec3 tipView;
    out vec3 tailView;
    out vec3 cameraNormal;

    ${ GEOM_DECLARATIONS }$

    void main() {

        // Build an orthogonal basis
        vec3 tailViewVal = gl_in[0].gl_Position.xyz / gl_in[0].gl_Position.w;
        vec3 tipViewVal = position_tip[0].xyz / position_tip[0].w;
        vec3 cylDir = normalize(tipViewVal - tailViewVal);
        vec3 basisY = normalize(up_direction[0] - dot(up_direction[0], cylDir) * cylDir);
        vec3 basisX = cross(basisY, cylDir);

        // Compute corners of cube
        vec4 tailProj = u_projMatrix * gl_in[0].gl_Position;
        vec4 tipProj = u_projMatrix * position_tip[0];
        vec4 dx = u_projMatrix * vec4(basisX * u_wb, 0.);
        vec4 dy = u_projMatrix * vec4(basisY * u_wn, 0.);

        vec4 p1 = tailProj - dx - dy;
        vec4 p2 = tailProj + dx - dy;
        vec4 p3 = tailProj - dx + dy;
        vec4 p4 = tailProj + dx + dy;
        vec4 p5 = tipProj - dx - dy;
        vec4 p6 = tipProj + dx - dy;
        vec4 p7 = tipProj - dx + dy;
        vec4 p8 = tipProj + dx + dy;
        
        // Other data to emit   
        cameraNormal = dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p7; EmitVertex(); 
        cameraNormal = dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p8; EmitVertex(); 
        cameraNormal = dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p3; EmitVertex(); 
        cameraNormal = dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p4; EmitVertex();
        EndPrimitive();

        cameraNormal = -dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p6; EmitVertex();
        cameraNormal = -dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p2; EmitVertex(); 
        cameraNormal = -dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p8; EmitVertex(); 
        cameraNormal = -dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p4; EmitVertex();
        EndPrimitive();

        cameraNormal = dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p7; EmitVertex(); 
        cameraNormal = dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p3; EmitVertex(); 
        cameraNormal = dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p5; EmitVertex(); 
        cameraNormal = dx.xyz / u_wb; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p1; EmitVertex();
        EndPrimitive();

        cameraNormal = -dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p1; EmitVertex();
        cameraNormal = -dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p2; EmitVertex(); 
        cameraNormal = -dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p5; EmitVertex(); 
        cameraNormal = -dy.xyz / u_wn; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p6; EmitVertex();
        EndPrimitive(); 

        cameraNormal = -cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p1; EmitVertex();
        cameraNormal = -cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p2; EmitVertex(); 
        cameraNormal = -cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p3; EmitVertex(); 
        cameraNormal = -cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p4; EmitVertex();
        EndPrimitive(); 

        cameraNormal = cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p5; EmitVertex();
        cameraNormal = cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p6; EmitVertex(); 
        cameraNormal = cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p7; EmitVertex(); 
        cameraNormal = cylDir; tailView = tailViewVal; tipView = tipViewVal; gl_Position = p8; EmitVertex();
        EndPrimitive(); 
    }
)"
};

const ShaderStageSpecification FLEX_CUBOID_FRAG_SHADER = {
    
    ShaderStageType::Fragment,
    
    // uniforms
    {
        {"u_projMatrix", DataType::Matrix44Float},
        {"u_invProjMatrix", DataType::Matrix44Float},
        {"u_viewport", DataType::Vector4Float},
        {"u_wn", DataType::Float},
        {"u_wb", DataType::Float},
    }, 

    { }, // attributes
    
    // textures 
    {
    },
 
    // source
R"(
        ${ GLSL_VERSION }$
        uniform mat4 u_projMatrix;
        uniform mat4 u_invProjMatrix;
        uniform vec4 u_viewport;
        uniform float u_wb;
        in vec3 tailView;
        in vec3 tipView;
        in vec3 cameraNormal;
        layout(location = 0) out vec4 outputF;

        float LARGE_FLOAT();
        vec3 fragmentViewPosition(vec4 viewport, vec2 depthRange, mat4 invProjMat, vec4 fragCoord);
        bool rayCylinderIntersection(vec3 rayStart, vec3 rayDir, vec3 cylTail, vec3 cylTip, float cylRad, out float tHit, out vec3 pHit, out vec3 nHit);
        float fragDepthFromView(mat4 projMat, vec2 depthRange, vec3 viewPoint);

        ${ FRAG_DECLARATIONS }$

        void main()
        {
           // Build a ray corresponding to this fragment
           vec2 depthRange = vec2(gl_DepthRange.near, gl_DepthRange.far);
           vec3 viewRay = fragmentViewPosition(u_viewport, depthRange, u_invProjMatrix, gl_FragCoord);

           // Raycast to the cylinder  
           float tHit;
           vec3 pHit;
           vec3 nHit;
           rayCylinderIntersection(vec3(0., 0., 0), viewRay, tailView, tipView, u_wb, tHit, pHit, nHit); 
           if(tHit >= LARGE_FLOAT()) {
              discard;
           }
           // float depth = fragDepthFromView(u_projMatrix, depthRange, pHit);
           // ${ GLOBAL_FRAGMENT_FILTER }$
           
           // // Set depth (expensive!)
           // gl_FragDepth = depth;
          
           // Shading
           ${ GENERATE_SHADE_VALUE }$
           ${ GENERATE_SHADE_COLOR }$
           // Lighting
           vec3 shadeNormal = nHit;
           ${ GENERATE_LIT_COLOR }$
           // Set alpha
           float alphaOut = 1.0;
           ${ GENERATE_ALPHA }$
           // Write output
           outputF = vec4(litColor, alphaOut);

        //    // Set depth (expensive!)
        //    float depth = fragDepthFromView(u_projMatrix, depthRange, pHit);
        //    gl_FragDepth = depth;
        }
)"
};


// == Rules

const ShaderReplacementRule CUBOID_PROPAGATE_VALUE (
    /* rule name */ "CUBOID_PROPAGATE_VALUE",
    { /* replacement sources */
      {"VERT_DECLARATIONS", R"(
          in float a_value;
          out float a_valueToGeom;
        )"},
      {"VERT_ASSIGNMENTS", R"(
          a_valueToGeom = a_value;
        )"},
      {"GEOM_DECLARATIONS", R"(
          in float a_valueToGeom[];
          out float a_valueToFrag;
        )"},
      {"GEOM_PER_EMIT", R"(
          a_valueToFrag = a_valueToGeom[0]; 
        )"},
      {"FRAG_DECLARATIONS", R"(
          in float a_valueToFrag;
        )"},
      {"GENERATE_SHADE_VALUE", R"(
          float shadeValue = a_valueToFrag;
        )"},
    },
    /* uniforms */ {},
    /* attributes */ {
      {"a_value", DataType::Float},
    },
    /* textures */ {}
);

// like propagate value, but takes two values at tip and tail and linearly interpolates
const ShaderReplacementRule CUBOID_PROPAGATE_BLEND_VALUE (
    /* rule name */ "CUBOID_PROPAGATE_BLEND_VALUE",
    { /* replacement sources */
      {"VERT_DECLARATIONS", R"(
          in float a_value_tail;
          in float a_value_tip;
          out float a_valueTailToGeom;
          out float a_valueTipToGeom;
        )"},
      {"VERT_ASSIGNMENTS", R"(
          a_valueTailToGeom = a_value_tail;
          a_valueTipToGeom = a_value_tip;
        )"},
      {"GEOM_DECLARATIONS", R"(
          in float a_valueTailToGeom[];
          in float a_valueTipToGeom[];
          out float a_valueTailToFrag;
          out float a_valueTipToFrag;
        )"},
      {"GEOM_PER_EMIT", R"(
          a_valueTailToFrag = a_valueTailToGeom[0]; 
          a_valueTipToFrag = a_valueTipToGeom[0]; 
        )"},
      {"FRAG_DECLARATIONS", R"(
          in float a_valueTailToFrag;
          in float a_valueTipToFrag;
          float length2(vec3 x);
        )"},
      {"GENERATE_SHADE_VALUE", R"(
          float tEdge = dot(pHit - tailView, tipView - tailView) / length2(tipView - tailView);
          float shadeValue = mix(a_valueTailToFrag, a_valueTipToFrag, tEdge);
        )"},
    },
    /* uniforms */ {},
    /* attributes */ {
      {"a_value_tail", DataType::Float},
      {"a_value_tip", DataType::Float},
    },
    /* textures */ {}
);

const ShaderReplacementRule CUBOID_PROPAGATE_COLOR (
    /* rule name */ "CUBOID_PROPAGATE_COLOR",
    { /* replacement sources */
      {"VERT_DECLARATIONS", R"(
          in vec3 a_color;
          out vec3 a_colorToGeom;
        )"},
      {"VERT_ASSIGNMENTS", R"(
          a_colorToGeom = a_color;
        )"},
      {"GEOM_DECLARATIONS", R"(
          in vec3 a_colorToGeom[];
          out vec3 a_colorToFrag;
        )"},
      {"GEOM_PER_EMIT", R"(
          a_colorToFrag = a_colorToGeom[0]; 
        )"},
      {"FRAG_DECLARATIONS", R"(
          in vec3 a_colorToFrag;
        )"},
      {"GENERATE_SHADE_VALUE", R"(
          vec3 shadeColor = a_colorToFrag;
        )"},
    },
    /* uniforms */ {},
    /* attributes */ {
      {"a_color", DataType::Vector3Float},
    },
    /* textures */ {}
);

// like propagate color, but takes two values at tip and taail and linearly interpolates
const ShaderReplacementRule CUBOID_PROPAGATE_BLEND_COLOR (
    /* rule name */ "CUBOID_PROPAGATE_BLEND_COLOR",
    { /* replacement sources */
      {"VERT_DECLARATIONS", R"(
          in vec3 a_color_tail;
          in vec3 a_color_tip;
          out vec3 a_colorTailToGeom;
          out vec3 a_colorTipToGeom;
        )"},
      {"VERT_ASSIGNMENTS", R"(
          a_colorTailToGeom = a_color_tail;
          a_colorTipToGeom = a_color_tip;
        )"},
      {"GEOM_DECLARATIONS", R"(
          in vec3 a_colorTailToGeom[];
          in vec3 a_colorTipToGeom[];
          out vec3 a_colorTailToFrag;
          out vec3 a_colorTipToFrag;
        )"},
      {"GEOM_PER_EMIT", R"(
          a_colorTailToFrag = a_colorTailToGeom[0]; 
          a_colorTipToFrag = a_colorTipToGeom[0]; 
        )"},
      {"FRAG_DECLARATIONS", R"(
          in vec3 a_colorTailToFrag;
          in vec3 a_colorTipToFrag;
          float length2(vec3 x);
        )"},
      {"GENERATE_SHADE_VALUE", R"(
          float tEdge = dot(pHit - tailView, tipView - tailView) / length2(tipView - tailView);
          vec3 shadeColor = mix(a_colorTailToFrag, a_colorTipToFrag, tEdge);
        )"},
    },
    /* uniforms */ {},
    /* attributes */ {
      {"a_color_tail", DataType::Vector3Float},
      {"a_color_tip", DataType::Vector3Float},
    },
    /* textures */ {}
);

// data for picking
const ShaderReplacementRule CUBOID_PROPAGATE_PICK (
    /* rule name */ "CUBOID_PROPAGATE_PICK",
    { /* replacement sources */
      {"VERT_DECLARATIONS", R"(
          in vec3 a_color_tail;
          in vec3 a_color_tip;
          in vec3 a_color_edge;
          out vec3 a_colorTailToGeom;
          out vec3 a_colorTipToGeom;
          out vec3 a_colorEdgeToGeom;
        )"},
      {"VERT_ASSIGNMENTS", R"(
          a_colorTailToGeom = a_color_tail;
          a_colorTipToGeom = a_color_tip;
          a_colorEdgeToGeom = a_color_edge;
        )"},
      {"GEOM_DECLARATIONS", R"(
          in vec3 a_colorTailToGeom[];
          in vec3 a_colorTipToGeom[];
          in vec3 a_colorEdgeToGeom[];
          out vec3 a_colorTailToFrag;
          out vec3 a_colorTipToFrag;
          out vec3 a_colorEdgeToFrag;
        )"},
      {"GEOM_PER_EMIT", R"(
          a_colorTailToFrag = a_colorTailToGeom[0]; 
          a_colorTipToFrag = a_colorTipToGeom[0]; 
          a_colorEdgeToFrag = a_colorEdgeToGeom[0]; 
        )"},
      {"FRAG_DECLARATIONS", R"(
          in vec3 a_colorTailToFrag;
          in vec3 a_colorTipToFrag;
          in vec3 a_colorEdgeToFrag;
          float length2(vec3 x);
        )"},
      {"GENERATE_SHADE_VALUE", R"(
          float tEdge = dot(pHit - tailView, tipView - tailView) / length2(tipView - tailView);
          float endWidth = 0.2;
          vec3 shadeColor;
          if(tEdge < endWidth) {
            shadeColor = a_colorTailToFrag;
          } else if (tEdge < (1.0f - endWidth)) {
            shadeColor = a_colorEdgeToFrag;
          } else {
            shadeColor = a_colorTipToFrag;
          }
        )"},
    },
    /* uniforms */ {},
    /* attributes */ {
      {"a_color_tail", DataType::Vector3Float},
      {"a_color_tip", DataType::Vector3Float},
      {"a_color_edge", DataType::Vector3Float},
    },
    /* textures */ {}
);


// clang-format on

} // namespace backend_openGL3_glfw
} // namespace render
} // namespace polyscope
