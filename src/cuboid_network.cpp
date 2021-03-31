// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network.h"

#include "polyscope/pick.h"
#include "polyscope/polyscope.h"
#include "polyscope/render/engine.h"

#include "imgui.h"

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

namespace polyscope {

// Initialize statics
const std::string CuboidNetwork::structureTypeName = "Cuboid Network";

// Constructor
CuboidNetwork::CuboidNetwork(std::string name, std::vector<glm::vec3> nodes_, std::vector<std::array<size_t, 2>> edges_,
                             std::vector<glm::vec3> upDir_)
    : QuantityStructure<CuboidNetwork>(name, typeName()), nodes(std::move(nodes_)), edges(std::move(edges_)),
      color(uniquePrefix() + "#color", getNextUniqueColor()), w_n(uniquePrefix() + "#thickness", relativeValue(0.005)),
      material(uniquePrefix() + "#material", "clay"), w_b(uniquePrefix() + "#width", relativeValue(0.1)),
      upDir(std::move(upDir_)) {

  nodeDegrees = std::vector<size_t>(nNodes(), 0);

  size_t maxInd = nodes.size();
  for (size_t iE = 0; iE < edges.size(); iE++) {
    auto edge = edges[iE];
    size_t nA = std::get<0>(edge);
    size_t nB = std::get<1>(edge);

    // Make sure there are no out of bounds indices
    if (nA >= maxInd || nB >= maxInd) {
      polyscope::error("CuboidNetwork [" + name + "] edge " + std::to_string(iE) + " has bad node indices { " +
                       std::to_string(nA) + " , " + std::to_string(nB) + " } but there are " + std::to_string(maxInd) +
                       " nodes.");
    }

    // Increment degree
    nodeDegrees[nA]++;
    nodeDegrees[nB]++;
  }
}


// Helper to set uniforms
void CuboidNetwork::setCuboidNetworkNodeUniforms(render::ShaderProgram& p) {
  glm::mat4 P = view::getCameraPerspectiveMatrix();
  glm::mat4 Pinv = glm::inverse(P);
  p.setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
  p.setUniform("u_viewport", render::engine->getCurrentViewport());
  p.setUniform("u_pointRadius", getWidth());
  // p.setUniform("u_wn", getHeight());
  // p.setUniform("u_wb", getWidth());
}

void CuboidNetwork::setCuboidNetworkEdgeUniforms(render::ShaderProgram& p) {
  glm::mat4 P = view::getCameraPerspectiveMatrix();
  glm::mat4 Pinv = glm::inverse(P);
  p.setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
  p.setUniform("u_viewport", render::engine->getCurrentViewport());
  p.setUniform("u_wn", getHeight());
  p.setUniform("u_wb", getWidth());
}

void CuboidNetwork::draw() {
  if (!isEnabled()) {
    return;
  }

  // If there is no dominant quantity, then this class is responsible for drawing points
  if (dominantQuantity == nullptr) {

    // Ensure we have prepared buffers
    if (edgeProgram == nullptr) {
      prepare();
    }

    // Set program uniforms
    setTransformUniforms(*edgeProgram);

    setCuboidNetworkEdgeUniforms(*edgeProgram);

    edgeProgram->setUniform("u_baseColor", getColor());

    // Draw the actual curve network
    edgeProgram->draw();
  }

  // Draw the quantities
  for (auto& x : quantities) {
    x.second->draw();
  }
}

void CuboidNetwork::drawPick() {
  if (!isEnabled()) {
    return;
  }

  // Ensure we have prepared buffers
  if (edgePickProgram == nullptr || nodePickProgram == nullptr) {
    preparePick();
  }

  // Set uniforms
  setTransformUniforms(*edgePickProgram);
  setTransformUniforms(*nodePickProgram);

  setCuboidNetworkEdgeUniforms(*edgePickProgram);
  setCuboidNetworkNodeUniforms(*nodePickProgram);

  edgePickProgram->draw();
  nodePickProgram->draw();
}

void CuboidNetwork::prepare() {
  if (dominantQuantity != nullptr) {
    return;
  }

  // If no quantity is coloring the network, draw with a default color
  edgeProgram = render::engine->requestShader("CUBOID", {"SHADE_BASECOLOR"});

  render::engine->setMaterial(*edgeProgram, getMaterial());

  // Fill out the geometry data for the programs
  fillEdgeGeometryBuffers(*edgeProgram);
}

void CuboidNetwork::preparePick() {

  // Pick index layout (local indices):
  //   |     --- nodes ---     |      --- edges ---      |
  //   ^                       ^
  //   0                    nNodes()

  // Request pick indices
  size_t totalPickElements = nNodes() + nEdges();
  size_t pickStart = pick::requestPickBufferRange(this, totalPickElements);

  { // Set up node picking program
    nodePickProgram = render::engine->requestShader("RAYCAST_SPHERE", {"SPHERE_PROPAGATE_COLOR"},
                                                    render::ShaderReplacementDefaults::Pick);

    // Fill color buffer with packed point indices
    std::vector<glm::vec3> pickColors;
    pickColors.reserve(nNodes());
    for (size_t i = pickStart; i < pickStart + nNodes(); i++) {
      glm::vec3 val = pick::indToVec(i);
      pickColors.push_back(pick::indToVec(i));
    }


    // Store data in buffers
    nodePickProgram->setAttribute("a_color", pickColors);

    fillNodeGeometryBuffers(*nodePickProgram);
  }

  { // Set up edge picking program
    edgePickProgram = render::engine->requestShader("RAYCAST_CYLINDER", {"CYLINDER_PROPAGATE_PICK"},
                                                    render::ShaderReplacementDefaults::Pick);

    // Fill color buffer with packed node/edge indices
    std::vector<glm::vec3> edgePickTail(nEdges());
    std::vector<glm::vec3> edgePickTip(nEdges());
    std::vector<glm::vec3> edgePickEdge(nEdges());

    // Fill posiiton and pick index buffers
    for (size_t iE = 0; iE < nEdges(); iE++) {
      auto& edge = edges[iE];
      size_t eTail = std::get<0>(edge);
      size_t eTip = std::get<1>(edge);

      glm::vec3 colorValTail = pick::indToVec(pickStart + eTail);
      glm::vec3 colorValTip = pick::indToVec(pickStart + eTip);
      glm::vec3 colorValEdge = pick::indToVec(pickStart + nNodes() + iE);
      edgePickTail[iE] = colorValTail;
      edgePickTip[iE] = colorValTip;
      edgePickEdge[iE] = colorValEdge;
    }
    edgePickProgram->setAttribute("a_color_tail", edgePickTail);
    edgePickProgram->setAttribute("a_color_tip", edgePickTip);
    edgePickProgram->setAttribute("a_color_edge", edgePickEdge);

    fillEdgeGeometryBuffers(*edgePickProgram);
  }
}

void CuboidNetwork::fillNodeGeometryBuffers(render::ShaderProgram& program) {
  program.setAttribute("a_position", nodes);
}

void CuboidNetwork::fillEdgeGeometryBuffers(render::ShaderProgram& program) {

  // Positions at either end of edges
  std::vector<glm::vec3> posTail(nEdges());
  std::vector<glm::vec3> posTip(nEdges());
  for (size_t iE = 0; iE < nEdges(); iE++) {
    auto& edge = edges[iE];
    size_t eTail = std::get<0>(edge);
    size_t eTip = std::get<1>(edge);
    posTail[iE] = nodes[eTail];
    posTip[iE] = nodes[eTip];
  }
  program.setAttribute("a_position_tail", posTail);
  program.setAttribute("a_position_tip", posTip);
  program.setAttribute("a_up_direction", upDir);
}

void CuboidNetwork::refresh() {
  edgeProgram.reset();
  nodePickProgram.reset();
  edgePickProgram.reset();

  for (auto& q : quantities) {
    q.second->geometryChanged();
  }
  requestRedraw();
  QuantityStructure<CuboidNetwork>::refresh(); // call base class version, which refreshes quantities
}

void CuboidNetwork::geometryChanged() { refresh(); }

void CuboidNetwork::buildPickUI(size_t localPickID) {

  if (localPickID < nNodes()) {
    buildNodePickUI(localPickID);
  } else if (localPickID < nNodes() + nEdges()) {
    buildEdgePickUI(localPickID - nNodes());
  } else {
    error("Bad pick index in curve network");
  }
}

void CuboidNetwork::buildNodePickUI(size_t nodeInd) {

  ImGui::TextUnformatted(("node #" + std::to_string(nodeInd) + "  ").c_str());
  ImGui::SameLine();
  ImGui::TextUnformatted(to_string(nodes[nodeInd]).c_str());

  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Indent(20.);

  // Build GUI to show the quantities
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3);
  for (auto& x : quantities) {
    x.second->buildNodeInfoGUI(nodeInd);
  }

  ImGui::Indent(-20.);
}

void CuboidNetwork::buildEdgePickUI(size_t edgeInd) {
  ImGui::TextUnformatted(("edge #" + std::to_string(edgeInd) + "  ").c_str());
  ImGui::SameLine();
  size_t n0 = std::get<0>(edges[edgeInd]);
  size_t n1 = std::get<1>(edges[edgeInd]);
  ImGui::TextUnformatted(("  " + std::to_string(n0) + " -- " + std::to_string(n1)).c_str());

  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Spacing();
  ImGui::Indent(20.);

  // Build GUI to show the quantities
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() / 3);
  for (auto& x : quantities) {
    x.second->buildEdgeInfoGUI(edgeInd);
  }

  ImGui::Indent(-20.);
}


void CuboidNetwork::buildCustomUI() {
  ImGui::Text("nodes: %lld  edges: %lld", static_cast<long long int>(nNodes()), static_cast<long long int>(nEdges()));
  if (ImGui::ColorEdit3("Color", &color.get()[0], ImGuiColorEditFlags_NoInputs)) {
    setColor(getColor());
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(100);
  if (ImGui::SliderFloat("Normal width", w_n.get().getValuePtr(), 0.0, .1, "%.5f", 3.)) {
    w_n.manuallyChanged();
    requestRedraw();
  }
  if (ImGui::SliderFloat("Binormal width", w_b.get().getValuePtr(), 0.0, .1, "%.5f", 3.)) {
    w_b.manuallyChanged();
    requestRedraw();
  }
  ImGui::PopItemWidth();
}

void CuboidNetwork::buildCustomOptionsUI() {
  if (render::buildMaterialOptionsGui(material.get())) {
    material.manuallyChanged();
    setMaterial(material.get()); // trigger the other updates that happen on set()
  }
}

double CuboidNetwork::lengthScale() {
  // TODO cache

  // Measure length scale as twice the radius from the center of the bounding box
  auto bound = boundingBox();
  glm::vec3 center = 0.5f * (std::get<0>(bound) + std::get<1>(bound));

  double lengthScale = 0.0;
  for (glm::vec3& rawP : nodes) {
    glm::vec3 p = glm::vec3(objectTransform.get() * glm::vec4(rawP, 1.0));
    lengthScale = std::max(lengthScale, (double)glm::length2(p - center));
  }

  return 2 * std::sqrt(lengthScale);
}

std::tuple<glm::vec3, glm::vec3> CuboidNetwork::boundingBox() {

  glm::vec3 min = glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();
  glm::vec3 max = -glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();

  for (glm::vec3& rawP : nodes) {
    glm::vec3 p = glm::vec3(objectTransform * glm::vec4(rawP, 1.0));
    min = componentwiseMin(min, p);
    max = componentwiseMax(max, p);
  }

  return std::make_tuple(min, max);
}

CuboidNetwork* CuboidNetwork::setColor(glm::vec3 newVal) {
  color = newVal;
  polyscope::requestRedraw();
  return this;
}
glm::vec3 CuboidNetwork::getColor() { return color.get(); }

CuboidNetwork* CuboidNetwork::setWidth(float newVal, bool isRelative) {
  w_b = ScaledValue<float>(newVal, isRelative);
  polyscope::requestRedraw();
  return this;
}
float CuboidNetwork::getWidth() { return w_b.get().asAbsolute(); }

CuboidNetwork* CuboidNetwork::setHeight(float newVal, bool isRelative) {
  w_n = ScaledValue<float>(newVal, isRelative);
  polyscope::requestRedraw();
  return this;
}
float CuboidNetwork::getHeight() { return w_n.get().asAbsolute(); }

CuboidNetwork* CuboidNetwork::setMaterial(std::string m) {
  material = m;
  geometryChanged(); // (serves the purpose of re-initializing everything, though this is a bit overkill)
  requestRedraw();
  return this;
}
std::string CuboidNetwork::getMaterial() { return material.get(); }

std::string CuboidNetwork::typeName() { return structureTypeName; }

// === Quantities

CuboidNetworkQuantity::CuboidNetworkQuantity(std::string name_, CuboidNetwork& CuboidNetwork_, bool dominates_)
    : Quantity<CuboidNetwork>(name_, CuboidNetwork_, dominates_) {}


void CuboidNetworkQuantity::buildNodeInfoGUI(size_t nodeInd) {}
void CuboidNetworkQuantity::buildEdgeInfoGUI(size_t edgeInd) {}

// === Quantity adders


CuboidNetworkNodeColorQuantity* CuboidNetwork::addNodeColorQuantityImpl(std::string name,
                                                                        const std::vector<glm::vec3>& colors) {
  CuboidNetworkNodeColorQuantity* q = new CuboidNetworkNodeColorQuantity(name, colors, *this);
  addQuantity(q);
  return q;
}

CuboidNetworkEdgeColorQuantity* CuboidNetwork::addEdgeColorQuantityImpl(std::string name,
                                                                        const std::vector<glm::vec3>& colors) {
  CuboidNetworkEdgeColorQuantity* q = new CuboidNetworkEdgeColorQuantity(name, colors, *this);
  addQuantity(q);
  return q;
}


CuboidNetworkNodeScalarQuantity*
CuboidNetwork::addNodeScalarQuantityImpl(std::string name, const std::vector<double>& data, DataType type) {
  CuboidNetworkNodeScalarQuantity* q = new CuboidNetworkNodeScalarQuantity(name, data, *this, type);
  addQuantity(q);
  return q;
}

CuboidNetworkEdgeScalarQuantity*
CuboidNetwork::addEdgeScalarQuantityImpl(std::string name, const std::vector<double>& data, DataType type) {
  CuboidNetworkEdgeScalarQuantity* q = new CuboidNetworkEdgeScalarQuantity(name, data, *this, type);
  addQuantity(q);
  return q;
}

CuboidNetworkNodeVectorQuantity* CuboidNetwork::addNodeVectorQuantityImpl(std::string name,
                                                                          const std::vector<glm::vec3>& vectors,
                                                                          VectorType vectorType) {
  CuboidNetworkNodeVectorQuantity* q = new CuboidNetworkNodeVectorQuantity(name, vectors, *this, vectorType);
  addQuantity(q);
  return q;
}

CuboidNetworkEdgeVectorQuantity* CuboidNetwork::addEdgeVectorQuantityImpl(std::string name,
                                                                          const std::vector<glm::vec3>& vectors,
                                                                          VectorType vectorType) {
  CuboidNetworkEdgeVectorQuantity* q = new CuboidNetworkEdgeVectorQuantity(name, vectors, *this, vectorType);
  addQuantity(q);
  return q;
}


} // namespace polyscope
