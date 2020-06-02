// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network_scalar_quantity.h"

#include "polyscope/file_helpers.h"
#include "polyscope/polyscope.h"
#include "polyscope/render/shaders.h"

#include "imgui.h"

using std::cout;
using std::endl;

namespace polyscope {

CuboidNetworkScalarQuantity::CuboidNetworkScalarQuantity(std::string name, CuboidNetwork& network_,
                                                         std::string definedOn_, DataType dataType_)
    : CuboidNetworkQuantity(name, network_, true), dataType(dataType_),
      cMap(uniquePrefix() + name + "#cmap", defaultColorMap(dataType)), definedOn(definedOn_) {}

void CuboidNetworkScalarQuantity::draw() {
  if (!isEnabled()) return;

  if (edgeProgram == nullptr) {
    createProgram();
  }

  // Set uniforms
  parent.setTransformUniforms(*edgeProgram);

  parent.setCuboidNetworkEdgeUniforms(*edgeProgram);

  setProgramUniforms(*edgeProgram);

  edgeProgram->draw();
}


// Update range uniforms
void CuboidNetworkScalarQuantity::setProgramUniforms(render::ShaderProgram& program) {
  program.setUniform("u_rangeLow", vizRange.first);
  program.setUniform("u_rangeHigh", vizRange.second);
}

CuboidNetworkScalarQuantity* CuboidNetworkScalarQuantity::resetMapRange() {
  switch (dataType) {
  case DataType::STANDARD:
    vizRange = dataRange;
    break;
  case DataType::SYMMETRIC: {
    double absRange = std::max(std::abs(dataRange.first), std::abs(dataRange.second));
    vizRange = std::make_pair(-absRange, absRange);
  } break;
  case DataType::MAGNITUDE:
    vizRange = std::make_pair(0., dataRange.second);
    break;
  }

  requestRedraw();
  return this;
}

void CuboidNetworkScalarQuantity::buildCustomUI() {
  ImGui::SameLine();

  // == Options popup
  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("OptionsPopup");
  }
  if (ImGui::BeginPopup("OptionsPopup")) {

    if (ImGui::MenuItem("Reset colormap range")) resetMapRange();

    ImGui::EndPopup();
  }

  if (render::buildColormapSelector(cMap.get())) {
    edgeProgram.reset();
    setColorMap(getColorMap());
  }

  // Draw the histogram of values
  hist.colormapRange = vizRange;
  hist.buildUI();

  // Data range
  // Note: %g specifies are generally nicer than %e, but here we don't acutally have a choice. ImGui (for somewhat
  // valid reasons) links the resolution of the slider to the decimal width of the formatted number. When %g formats a
  // number with few decimal places, sliders can break. There is no way to set a minimum number of decimal places with
  // %g, unfortunately.
  {
    switch (dataType) {
    case DataType::STANDARD:
      ImGui::DragFloatRange2("", &vizRange.first, &vizRange.second, (dataRange.second - dataRange.first) / 100.,
                             dataRange.first, dataRange.second, "Min: %.3e", "Max: %.3e");
      break;
    case DataType::SYMMETRIC: {
      float absRange = std::max(std::abs(dataRange.first), std::abs(dataRange.second));
      ImGui::DragFloatRange2("##range_symmetric", &vizRange.first, &vizRange.second, absRange / 100., -absRange,
                             absRange, "Min: %.3e", "Max: %.3e");
    } break;
    case DataType::MAGNITUDE: {
      ImGui::DragFloatRange2("##range_mag", &vizRange.first, &vizRange.second, vizRange.second / 100., 0.0,
                             dataRange.second, "Min: %.3e", "Max: %.3e");
    } break;
    }
  }
}

void CuboidNetworkScalarQuantity::geometryChanged() { edgeProgram.reset(); }

CuboidNetworkScalarQuantity* CuboidNetworkScalarQuantity::setColorMap(std::string name) {
  cMap = name;
  hist.updateColormap(cMap.get());
  requestRedraw();
  return this;
}
std::string CuboidNetworkScalarQuantity::getColorMap() { return cMap.get(); }

CuboidNetworkScalarQuantity* CuboidNetworkScalarQuantity::setMapRange(std::pair<double, double> val) {
  vizRange = val;
  requestRedraw();
  return this;
}
std::pair<double, double> CuboidNetworkScalarQuantity::getMapRange() { return vizRange; }

std::string CuboidNetworkScalarQuantity::niceName() { return name + " (" + definedOn + " scalar)"; }

// ========================================================
// ==========             Node Scalar            ==========
// ========================================================

CuboidNetworkNodeScalarQuantity::CuboidNetworkNodeScalarQuantity(std::string name, std::vector<double> values_,
                                                                 CuboidNetwork& network_, DataType dataType_)
    : CuboidNetworkScalarQuantity(name, network_, "node", dataType_), values(std::move(values_))

{
  hist.updateColormap(cMap.get());
  hist.buildHistogram(values);

  dataRange = robustMinMax(values, 1e-5);
  resetMapRange();
}

void CuboidNetworkNodeScalarQuantity::createProgram() {
  // Create the program to draw this quantity
  edgeProgram = render::engine->generateShaderProgram(
      // {render::CYLINDER_BLEND_VALUE_VERT_SHADER, render::CYLINDER_BLEND_VALUE_GEOM_SHADER,
      //  render::CYLINDER_VALUE_FRAG_SHADER},
      {render::CUBOID_BLEND_VALUE_VERT_SHADER, render::CUBOID_BLEND_VALUE_GEOM_SHADER,
       render::CUBOID_VALUE_FRAG_SHADER}, // or maybe CUBOID_BLEND_VALUE_FRAG_SHADER ?
      DrawMode::Points);


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

CuboidNetworkEdgeScalarQuantity::CuboidNetworkEdgeScalarQuantity(std::string name, std::vector<double> values_,
                                                                 CuboidNetwork& network_, DataType dataType_)
    : CuboidNetworkScalarQuantity(name, network_, "edge", dataType_), values(std::move(values_))

{
  hist.updateColormap(cMap.get());
  hist.buildHistogram(values);

  dataRange = robustMinMax(values, 1e-5);
  resetMapRange();
}

void CuboidNetworkEdgeScalarQuantity::createProgram() {
  // Create the program to draw this quantity
  edgeProgram = render::engine->generateShaderProgram(
      // {render::CYLINDER_VALUE_VERT_SHADER, render::CYLINDER_VALUE_GEOM_SHADER, render::CYLINDER_VALUE_FRAG_SHADER},
      {render::CUBOID_VALUE_VERT_SHADER, render::CUBOID_VALUE_GEOM_SHADER, render::CUBOID_VALUE_FRAG_SHADER},
      DrawMode::Points);
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
