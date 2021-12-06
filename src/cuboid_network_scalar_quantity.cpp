// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network_scalar_quantity.h"

#include "polyscope/file_helpers.h"
#include "polyscope/polyscope.h"

#include "imgui.h"

using std::cout;
using std::endl;

namespace polyscope {

CuboidNetworkScalarQuantity::CuboidNetworkScalarQuantity(std::string name, CuboidNetwork& network_,
                                                         std::string definedOn_, const std::vector<double>& values_,
                                                         DataType dataType_)
    : CuboidNetworkQuantity(name, network_, true), ScalarQuantity(*this, values_, dataType_), definedOn(definedOn_) {}

void CuboidNetworkScalarQuantity::draw() {
  if (!isEnabled()) return;

  if (edgeProgram == nullptr) {
    createProgram();
  }

  // Set uniforms
  parent.setStructureUniforms(*edgeProgram);

  parent.setCuboidNetworkEdgeUniforms(*edgeProgram);

  setScalarUniforms(*edgeProgram);

  edgeProgram->draw();
}


void CuboidNetworkScalarQuantity::buildCustomUI() {
  ImGui::SameLine();

  // == Options popup
  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("OptionsPopup");
  }
  if (ImGui::BeginPopup("OptionsPopup")) {

    buildScalarOptionsUI();

    ImGui::EndPopup();
  }
}

void CuboidNetworkScalarQuantity::refresh() {
  edgeProgram.reset();
  Quantity::refresh();
}

std::string CuboidNetworkScalarQuantity::niceName() { return name + " (" + definedOn + " scalar)"; }

// ========================================================
// ==========             Node Scalar            ==========
// ========================================================

CuboidNetworkNodeScalarQuantity::CuboidNetworkNodeScalarQuantity(std::string name, const std::vector<double>& values_,
                                                                 CuboidNetwork& network_, DataType dataType_)
    : CuboidNetworkScalarQuantity(name, network_, "node", values_, dataType_)

{}

void CuboidNetworkNodeScalarQuantity::createProgram() {
  // Create the program to draw this quantity
  edgeProgram = render::engine->requestShader("CUBOID", addScalarRules(parent.addCuboidNetworkEdgeRules({"CUBOID_PROPAGATE_BLEND_VALUE"})));

  // Fill geometry buffers
  parent.fillEdgeGeometryBuffers(*edgeProgram);


  { // Fill edge color buffers
    std::vector<double> valueTail(parent.nEdges());
    std::vector<double> valueTip(parent.nEdges());
    for (size_t iE = 0; iE < parent.nEdges(); iE++) {
      auto& edge = parent.edges[iE];
      size_t eTail = std::get<0>(edge);
      size_t eTip = std::get<1>(edge);
      valueTail[iE] = values[eTail];
      valueTip[iE] = values[eTip];
    }

    edgeProgram->setAttribute("a_value_tail", valueTail);
    edgeProgram->setAttribute("a_value_tip", valueTip);
  }

  edgeProgram->setTextureFromColormap("t_colormap", cMap.get());
  render::engine->setMaterial(*edgeProgram, parent.getMaterial());
}


void CuboidNetworkNodeScalarQuantity::buildNodeInfoGUI(size_t nInd) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();
  ImGui::Text("%g", values[nInd]);
  ImGui::NextColumn();
}


// ========================================================
// ==========            Edge Scalar             ==========
// ========================================================

CuboidNetworkEdgeScalarQuantity::CuboidNetworkEdgeScalarQuantity(std::string name, const std::vector<double>& values_,
                                                                 CuboidNetwork& network_, DataType dataType_)
    : CuboidNetworkScalarQuantity(name, network_, "edge", values_, dataType_)

{}

void CuboidNetworkEdgeScalarQuantity::createProgram() {
  // Create the program to draw this quantity
  edgeProgram = render::engine->requestShader("CUBOID", addScalarRules({"CUBOID_PROPAGATE_VALUE"}));
  // Fill geometry buffers
  parent.fillEdgeGeometryBuffers(*edgeProgram);

  { // Fill node color buffers
    // Compute an average color at each node
    std::vector<double> averageValueNode(parent.nNodes(), 0.);
    for (size_t iE = 0; iE < parent.nEdges(); iE++) {
      auto& edge = parent.edges[iE];
      size_t eTail = std::get<0>(edge);
      size_t eTip = std::get<1>(edge);
      averageValueNode[eTail] += values[iE];
      averageValueNode[eTip] += values[iE];
    }

    for (size_t iN = 0; iN < parent.nNodes(); iN++) {
      averageValueNode[iN] /= parent.nodeDegrees[iN];
    }
  }

  { // Fill edge color buffers
    edgeProgram->setAttribute("a_value", values);
  }

  edgeProgram->setTextureFromColormap("t_colormap", cMap.get());
  render::engine->setMaterial(*edgeProgram, parent.getMaterial());
}


void CuboidNetworkEdgeScalarQuantity::buildEdgeInfoGUI(size_t eInd) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();
  ImGui::Text("%g", values[eInd]);
  ImGui::NextColumn();
}


} // namespace polyscope
