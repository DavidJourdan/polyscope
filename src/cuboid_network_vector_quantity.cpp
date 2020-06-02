// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#include "polyscope/cuboid_network_vector_quantity.h"

#include "polyscope/file_helpers.h"
#include "polyscope/polyscope.h"
#include "polyscope/render/shaders.h"

#include "imgui.h"

#include <complex>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

namespace polyscope {

CuboidNetworkVectorQuantity::CuboidNetworkVectorQuantity(std::string name, CuboidNetwork& network_,
                                                         VectorType vectorType_)
    : CuboidNetworkQuantity(name, network_), vectorType(vectorType_),
      vectorLengthMult(uniquePrefix() + "#vectorLengthMult",
                       vectorType == VectorType::AMBIENT ? absoluteValue(1.0) : relativeValue(0.02)),
      vectorRadius(uniquePrefix() + "#vectorRadius", relativeValue(0.0025)),
      vectorColor(uniquePrefix() + "#vectorColor", getNextUniqueColor()),
      material(uniquePrefix() + "#material", "clay") {}

void CuboidNetworkVectorQuantity::prepareVectorMapper() {

  // Create a mapper (default mapper is identity)
  if (vectorType == VectorType::AMBIENT) {
    mapper.setMinMax(vectors);
  } else {
    mapper = AffineRemapper<glm::vec3>(vectors, DataType::MAGNITUDE);
  }
}

void CuboidNetworkVectorQuantity::draw() {
  if (!isEnabled()) return;

  if (program == nullptr) prepareProgram();

  // Set uniforms
  parent.setTransformUniforms(*program);

  program->setUniform("u_radius", vectorRadius.get().asAbsolute());
  program->setUniform("u_baseColor", vectorColor.get());

  if (vectorType == VectorType::AMBIENT) {
    program->setUniform("u_lengthMult", 1.0);
  } else {
    program->setUniform("u_lengthMult", vectorLengthMult.get().asAbsolute());
  }

  glm::mat4 P = view::getCameraPerspectiveMatrix();
  glm::mat4 Pinv = glm::inverse(P);
  program->setUniform("u_invProjMatrix", glm::value_ptr(Pinv));
  program->setUniform("u_viewport", render::engine->getCurrentViewport());

  program->draw();
}

void CuboidNetworkVectorQuantity::prepareProgram() {

  program = render::engine->generateShaderProgram(
      {render::PASSTHRU_VECTOR_VERT_SHADER, render::VECTOR_GEOM_SHADER, render::VECTOR_FRAG_SHADER}, DrawMode::Points);

  // Fill buffers
  std::vector<glm::vec3> mappedVectors;
  for (glm::vec3& v : vectors) {
    mappedVectors.push_back(mapper.map(v));
  }

  program->setAttribute("a_vector", mappedVectors);
  program->setAttribute("a_position", vectorRoots);

  render::engine->setMaterial(*program, getMaterial());
}

void CuboidNetworkVectorQuantity::buildCustomUI() {
  ImGui::SameLine();
  if (ImGui::ColorEdit3("Color", &vectorColor.get()[0], ImGuiColorEditFlags_NoInputs)) setVectorColor(getVectorColor());
  ImGui::SameLine();


  // === Options popup
  if (ImGui::Button("Options")) {
    ImGui::OpenPopup("OptionsPopup");
  }
  if (ImGui::BeginPopup("OptionsPopup")) {
    if (ImGui::MenuItem("Write to file")) writeToFile();
    ImGui::EndPopup();
  }


  // Only get to set length for non-ambient vectors
  if (vectorType != VectorType::AMBIENT) {
    if (ImGui::SliderFloat("Length", vectorLengthMult.get().getValuePtr(), 0.0, .1, "%.5f", 3.)) {
      vectorLengthMult.manuallyChanged();
      requestRedraw();
    }
  }

  if (ImGui::SliderFloat("Radius", vectorRadius.get().getValuePtr(), 0.0, .1, "%.5f", 3.)) {
    vectorRadius.manuallyChanged();
    requestRedraw();
  }

  { // Draw max and min magnitude
    ImGui::TextUnformatted(mapper.printBounds().c_str());
  }

  drawSubUI();
}

void CuboidNetworkVectorQuantity::drawSubUI() {}

void CuboidNetworkVectorQuantity::writeToFile(std::string filename) {

  if (filename == "") {
    filename = promptForFilename();
    if (filename == "") {
      return;
    }
  }

  if (options::verbosity > 0) {
    cout << "Writing curve network vector quantity " << name << " to file " << filename << endl;
  }

  std::ofstream outFile(filename);
  outFile << "#Vectors written by polyscope from Cuboid Network Vector Quantity " << name << endl;
  outFile << "#displayradius " << vectorRadius.get().asAbsolute() << endl;
  outFile << "#displaylength " << vectorLengthMult.get().asAbsolute() << endl;


  for (size_t i = 0; i < vectors.size(); i++) {
    if (glm::length(vectors[i]) > 0) {
      outFile << vectorRoots[i] << " " << vectors[i] << endl;
    }
  }

  outFile.close();
}

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorLengthScale(double newLength, bool isRelative) {
  vectorLengthMult = ScaledValue<double>(newLength, isRelative);
  requestRedraw();
  return this;
}
double CuboidNetworkVectorQuantity::getVectorLengthScale() { return vectorLengthMult.get().asAbsolute(); }

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorRadius(double val, bool isRelative) {
  vectorRadius = ScaledValue<double>(val, isRelative);
  requestRedraw();
  return this;
}
double CuboidNetworkVectorQuantity::getVectorRadius() { return vectorRadius.get().asAbsolute(); }

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setVectorColor(glm::vec3 color) {
  vectorColor = color;
  requestRedraw();
  return this;
}
glm::vec3 CuboidNetworkVectorQuantity::getVectorColor() { return vectorColor.get(); }

CuboidNetworkVectorQuantity* CuboidNetworkVectorQuantity::setMaterial(std::string m) {
  material = m;
  if (program) render::engine->setMaterial(*program, getMaterial());
  requestRedraw();
  return this;
}
std::string CuboidNetworkVectorQuantity::getMaterial() { return material.get(); }


std::string CuboidNetworkEdgeVectorQuantity::niceName() { return name + " (edge vector)"; }

// ========================================================
// ==========             Node Vector            ==========
// ========================================================

CuboidNetworkNodeVectorQuantity::CuboidNetworkNodeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_,
                                                                 CuboidNetwork& network_, VectorType vectorType_)

    : CuboidNetworkVectorQuantity(name, network_, vectorType_), vectorField(vectors_) {

  size_t i = 0;
  vectorRoots = parent.nodes;
  vectors = vectorField;

  prepareVectorMapper();
}

void CuboidNetworkNodeVectorQuantity::buildNodeInfoGUI(size_t iV) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  std::stringstream buffer;
  buffer << vectorField[iV];
  ImGui::TextUnformatted(buffer.str().c_str());

  ImGui::NextColumn();
  ImGui::NextColumn();
  ImGui::Text("magnitude: %g", glm::length(vectorField[iV]));
  ImGui::NextColumn();
}

std::string CuboidNetworkNodeVectorQuantity::niceName() { return name + " (node vector)"; }

void CuboidNetworkNodeVectorQuantity::geometryChanged() {
  vectorRoots = parent.nodes;
  program.reset();
}

// ========================================================
// ==========            Edge Vector             ==========
// ========================================================

CuboidNetworkEdgeVectorQuantity::CuboidNetworkEdgeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_,
                                                                 CuboidNetwork& network_, VectorType vectorType_)
    : CuboidNetworkVectorQuantity(name, network_, vectorType_), vectorField(vectors_) {

  // Copy the vectors
  vectors = vectorField;
  vectorRoots.resize(parent.nEdges());

  for (size_t iE = 0; iE < parent.nEdges(); iE++) {
    auto& edge = parent.edges[iE];
    size_t eTail = std::get<0>(edge);
    size_t eTip = std::get<1>(edge);

    vectorRoots[iE] = 0.5f * (parent.nodes[eTail] + parent.nodes[eTip]);
  }

  prepareVectorMapper();
}

void CuboidNetworkEdgeVectorQuantity::buildEdgeInfoGUI(size_t iF) {
  ImGui::TextUnformatted(name.c_str());
  ImGui::NextColumn();

  std::stringstream buffer;
  buffer << vectorField[iF];
  ImGui::TextUnformatted(buffer.str().c_str());

  ImGui::NextColumn();
  ImGui::NextColumn();
  ImGui::Text("magnitude: %g", glm::length(vectorField[iF]));
  ImGui::NextColumn();
}

void CuboidNetworkEdgeVectorQuantity::geometryChanged() {
  vectorRoots.resize(parent.nEdges());
  for (size_t iE = 0; iE < parent.nEdges(); iE++) {
    auto& edge = parent.edges[iE];
    size_t eTail = std::get<0>(edge);
    size_t eTip = std::get<1>(edge);

    vectorRoots[iE] = 0.5f * (parent.nodes[eTail] + parent.nodes[eTip]);
  }

  program.reset();
}

} // namespace polyscope
