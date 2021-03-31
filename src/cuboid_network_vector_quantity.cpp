// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network_vector_quantity.h"

#include "polyscope/file_helpers.h"
#include "polyscope/polyscope.h"

#include "imgui.h"

#include <complex>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

namespace polyscope {

CuboidNetworkVectorQuantity::CuboidNetworkVectorQuantity(std::string name, CuboidNetwork& network_,
                                                         VectorType vectorType_)
    : CuboidNetworkQuantity(name, network_), vectorType(vectorType_) {}

void CuboidNetworkVectorQuantity::prepareVectorArtist() {
  vectorArtist.reset(new VectorArtist(parent, name + "#vectorartist", vectorRoots, vectors, vectorType));
}

void CuboidNetworkVectorQuantity::draw() {
  if (!isEnabled()) return;
  vectorArtist->draw();
}

void CuboidNetworkVectorQuantity::buildCustomUI() {
  ImGui::SameLine();
  vectorArtist->buildParametersUI();
  drawSubUI();
}

void CuboidNetworkVectorQuantity::drawSubUI() {}

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorLengthScale(double newLength, bool isRelative) {
  vectorArtist->setVectorLengthScale(newLength, isRelative);
  return this;
}
double CuboidNetworkVectorQuantity::getVectorLengthScale() { return vectorArtist->getVectorLengthScale(); }
CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorRadius(double val, bool isRelative) {
  vectorArtist->setVectorRadius(val, isRelative);
  return this;
}
double CuboidNetworkVectorQuantity::getVectorRadius() { return vectorArtist->getVectorRadius(); }
CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorColor(glm::vec3 color) {
  vectorArtist->setVectorColor(color);
  return this;
}
glm::vec3 CuboidNetworkVectorQuantity::getVectorColor() { return vectorArtist->getVectorColor(); }

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setMaterial(std::string m) {
  vectorArtist->setMaterial(m);
  return this;
}
std::string CuboidNetworkVectorQuantity::getMaterial() { return vectorArtist->getMaterial(); }


std::string CuboidNetworkEdgeVectorQuantity::niceName() { return name + " (edge vector)"; }

// ========================================================
// ==========             Node Vector            ==========
// ========================================================

CuboidNetworkNodeVectorQuantity::CuboidNetworkNodeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_,
                                                                 CuboidNetwork& network_, VectorType vectorType_)

    : CuboidNetworkVectorQuantity(name, network_, vectorType_) {
  vectors = vectors_;
  refresh();
}

void CuboidNetworkNodeVectorQuantity::refresh() {
  size_t i = 0;
  vectorRoots = parent.nodes;

  prepareVectorArtist();
  Quantity::refresh();
}

void CuboidNetworkNodeVectorQuantity::buildNodeInfoGUI(size_t iV) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  std::stringstream buffer;
  buffer << vectors[iV];
  ImGui::TextUnformatted(buffer.str().c_str());

  ImGui::NextColumn();
  ImGui::NextColumn();
  ImGui::Text("magnitude: %g", glm::length(vectors[iV]));
  ImGui::NextColumn();
}

std::string CuboidNetworkNodeVectorQuantity::niceName() { return name + " (node vector)"; }

// ========================================================
// ==========            Edge Vector             ==========
// ========================================================

CuboidNetworkEdgeVectorQuantity::CuboidNetworkEdgeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_,
                                                                 CuboidNetwork& network_, VectorType vectorType_)
    : CuboidNetworkVectorQuantity(name, network_, vectorType_) {
  vectors = vectors_;
  refresh();
}

void CuboidNetworkEdgeVectorQuantity::refresh() {
  // Copy the vectors
  vectorRoots.resize(parent.nEdges());

  for (size_t iE = 0; iE < parent.nEdges(); iE++) {
    auto& edge = parent.edges[iE];
    size_t eTail = std::get<0>(edge);
    size_t eTip = std::get<1>(edge);

    vectorRoots[iE] = 0.5f * (parent.nodes[eTail] + parent.nodes[eTip]);
  }

  prepareVectorArtist();
  Quantity::refresh();
}

void CuboidNetworkEdgeVectorQuantity::buildEdgeInfoGUI(size_t iF) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  std::stringstream buffer;
  buffer << vectors[iF];
  ImGui::TextUnformatted(buffer.str().c_str());

  ImGui::NextColumn();
  ImGui::NextColumn();
  ImGui::Text("magnitude: %g", glm::length(vectors[iF]));
  ImGui::NextColumn();
}

} // namespace polyscope
