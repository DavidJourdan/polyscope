// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/cuboid_network.h"
#include "polyscope/ribbon_artist.h"

namespace polyscope {

// ==== Common base class

// Represents a vector field associated with a curve network
class CuboidNetworkVectorQuantity : public CuboidNetworkQuantity {
public:
  CuboidNetworkVectorQuantity(std::string name, CuboidNetwork& network_, VectorType vectorType_ = VectorType::STANDARD);


  virtual void draw() override;
  virtual void buildCustomUI() override;

  // Allow children to append to the UI
  virtual void drawSubUI();

  // === Members
  const VectorType vectorType;
  std::vector<glm::vec3> vectorRoots;
  std::vector<glm::vec3> vectors;

  // === Option accessors

  //  The vectors will be scaled such that the longest vector is this long
  CuboidNetworkVectorQuantity* setVectorLengthScale(double newLength, bool isRelative = true);
  double getVectorLengthScale();

  // The radius of the vectors
  CuboidNetworkVectorQuantity* setVectorRadius(double val, bool isRelative = true);
  double getVectorRadius();

  // The color of the vectors
  CuboidNetworkVectorQuantity* setVectorColor(glm::vec3 color);
  glm::vec3 getVectorColor();

  // Material
  CuboidNetworkVectorQuantity* setMaterial(std::string name);
  std::string getMaterial();

  void writeToFile(std::string filename = "");

protected:
  // === Visualization options
  PersistentValue<ScaledValue<float>> vectorLengthMult;
  PersistentValue<ScaledValue<float>> vectorRadius;
  PersistentValue<glm::vec3> vectorColor;
  PersistentValue<std::string> material;

  // The map that takes values to [0,1] for drawing
  AffineRemapper<glm::vec3> mapper;

  // GL things
  void prepareProgram();
  std::shared_ptr<render::ShaderProgram> program;

  // Set up the mapper for vectors
  void prepareVectorMapper();
};


// ==== R3 vectors at nodes

class CuboidNetworkNodeVectorQuantity : public CuboidNetworkVectorQuantity {
public:
  CuboidNetworkNodeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_, CuboidNetwork& network_,
                                  VectorType vectorType_ = VectorType::STANDARD);

  std::vector<glm::vec3> vectorField;

  virtual std::string niceName() override;
  virtual void buildNodeInfoGUI(size_t vInd) override;
  virtual void geometryChanged() override;
};


// ==== R3 vectors at edges

class CuboidNetworkEdgeVectorQuantity : public CuboidNetworkVectorQuantity {
public:
  CuboidNetworkEdgeVectorQuantity(std::string name, std::vector<glm::vec3> vectors_, CuboidNetwork& network_,
                                  VectorType vectorType_ = VectorType::STANDARD);

  std::vector<glm::vec3> vectorField;

  virtual std::string niceName() override;
  virtual void buildEdgeInfoGUI(size_t fInd) override;
  virtual void geometryChanged() override;
};


} // namespace polyscope
