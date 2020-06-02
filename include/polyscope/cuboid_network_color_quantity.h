// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/cuboid_network.h"

namespace polyscope {

// forward declaration
class CuboidNetworkMeshQuantity;
class CuboidNetwork;

class CuboidNetworkColorQuantity : public CuboidNetworkQuantity {
public:
  CuboidNetworkColorQuantity(std::string name, CuboidNetwork& network_, std::string definedOn);

  virtual void draw() override;
  virtual std::string niceName() override;

  virtual void geometryChanged() override;

protected:
  // UI internals
  const std::string definedOn;
  std::shared_ptr<render::ShaderProgram> edgeProgram;

  // Helpers
  virtual void createProgram() = 0;
};

// ========================================================
// ==========           Node Color             ==========
// ========================================================

class CuboidNetworkNodeColorQuantity : public CuboidNetworkColorQuantity {
public:
  CuboidNetworkNodeColorQuantity(std::string name, std::vector<glm::vec3> values_, CuboidNetwork& network_);

  virtual void createProgram() override;

  void buildNodeInfoGUI(size_t vInd) override;

  // === Members
  std::vector<glm::vec3> values;
};

// ========================================================
// ==========             Edge Color             ==========
// ========================================================

class CuboidNetworkEdgeColorQuantity : public CuboidNetworkColorQuantity {
public:
  CuboidNetworkEdgeColorQuantity(std::string name, std::vector<glm::vec3> values_, CuboidNetwork& network_);

  virtual void createProgram() override;

  void buildEdgeInfoGUI(size_t eInd) override;

  // === Members
  std::vector<glm::vec3> values;
};

} // namespace polyscope
