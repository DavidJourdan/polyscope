// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network_color_quantity.h"

#include "polyscope/polyscope.h"
#include "polyscope/render/shaders.h"

#include "imgui.h"

namespace polyscope {

CuboidNetworkColorQuantity::CuboidNetworkColorQuantity(std::string name, CuboidNetwork& network_,
                                                       std::string definedOn_)
    : CuboidNetworkQuantity(name, network_, true), definedOn(definedOn_) {}

void CuboidNetworkColorQuantity::draw() {
  if (!isEnabled()) return;

  if (edgeProgram == nullptr) {
    createProgram();
  }

  // Set uniforms
  parent.setTransformUniforms(*edgeProgram);

  parent.setCuboidNetworkEdgeUniforms(*edgeProgram);

  edgeProgram->draw();
}

// ========================================================
// ==========             Node Color             ==========
// ========================================================

CuboidNetworkNodeColorQuantity::CuboidNetworkNodeColorQuantity(std::string name, std::vector<glm::vec3> values_,
                                                               CuboidNetwork& network_)
    : CuboidNetworkColorQuantity(name, network_, "node"), values(std::move(values_))

{}

void CuboidNetworkNodeColorQuantity::createProgram() {
  // Create the program to draw this quantity
  edgeProgram = render::engine->generateShaderProgram(
      {render::CUBOID_BLEND_COLOR_VERT_SHADER, render::CUBOID_BLEND_COLOR_GEOM_SHADER,
       render::CUBOID_COLOR_FRAG_SHADER}, // or maybe CUBOID_BLEND_COLOR_FRAG_SHADER ?
      DrawMode::Points);

  // Fill geometry buffers
  parent.fillEdgeGeometryBuffers(*edgeProgram);

  { // Fill edge color buffers
    std::vector<glm::vec3> colorTail(parent.nEdges());
    std::vector<glm::vec3> colorTip(parent.nEdges());
    for (size_t iE = 0; iE < parent.nEdges(); iE++) {
      auto& edge = parent.edges[iE];
      size_t eTail = std::get<0>(edge);
      size_t eTip = std::get<1>(edge);
      colorTail[iE] = values[eTail];
      colorTip[iE] = values[eTip];
    }

    edgeProgram->setAttribute("a_color_tail", colorTail);
    edgeProgram->setAttribute("a_color_tip", colorTip);
  }

  render::engine->setMaterial(*edgeProgram, parent.getMaterial());
}


void CuboidNetworkNodeColorQuantity::buildNodeInfoGUI(size_t vInd) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  glm::vec3 tempColor = values[vInd];
  ImGui::ColorEdit3("", &tempColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);
  ImGui::SameLine();
  std::string colorStr = to_string_short(tempColor);
  ImGui::TextUnformatted(colorStr.c_str());
  ImGui::NextColumn();
}

std::string CuboidNetworkColorQuantity::niceName() { return name + " (" + definedOn + " color)"; }

void CuboidNetworkColorQuantity::geometryChanged() { edgeProgram.reset(); }

// ========================================================
// ==========            Edge Color              ==========
// ========================================================

CuboidNetworkEdgeColorQuantity::CuboidNetworkEdgeColorQuantity(std::string name, std::vector<glm::vec3> values_,
                                                               CuboidNetwork& network_)
    : CuboidNetworkColorQuantity(name, network_, "edge"), values(std::move(values_))

{}

void CuboidNetworkEdgeColorQuantity::createProgram() {
  edgeProgram = render::engine->generateShaderProgram(
      {render::CUBOID_COLOR_VERT_SHADER, render::CUBOID_COLOR_GEOM_SHADER, render::CUBOID_COLOR_FRAG_SHADER},
      DrawMode::Points);

  // Fill geometry buffers
  parent.fillEdgeGeometryBuffers(*edgeProgram);

  { // Fill node color buffers

    // Compute an average color at each node
    std::vector<glm::vec3> averageColorNode(parent.nNodes(), glm::vec3{0., 0., 0.});
    for (size_t iE = 0; iE < parent.nEdges(); iE++) {
      auto& edge = parent.edges[iE];
      size_t eTail = std::get<0>(edge);
      size_t eTip = std::get<1>(edge);
      averageColorNode[eTail] += values[iE];
      averageColorNode[eTip] += values[iE];
    }

    for (size_t iN = 0; iN < parent.nNodes(); iN++) {
      averageColorNode[iN] /= parent.nodeDegrees[iN];
    }
  }

  { // Fill edge color buffers
    edgeProgram->setAttribute("a_color", values);
  }

  render::engine->setMaterial(*edgeProgram, parent.getMaterial());
}


void CuboidNetworkEdgeColorQuantity::buildEdgeInfoGUI(size_t eInd) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  glm::vec3 tempColor = values[eInd];
  ImGui::ColorEdit3("", &tempColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoPicker);
  ImGui::SameLine();
  std::stringstream buffer;
  buffer << values[eInd];
  ImGui::TextUnformatted(buffer.str().c_str());
  ImGui::NextColumn();
}

} // namespace polyscope
