// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/cuboid_network.h"
#include "polyscope/histogram.h"
#include "polyscope/render/color_maps.h"
#include "polyscope/scalar_quantity.h"

namespace polyscope {

class CuboidNetworkScalarQuantity : public CuboidNetworkQuantity, public ScalarQuantity<CuboidNetworkScalarQuantity> {
public:
  CuboidNetworkScalarQuantity(std::string name, CuboidNetwork& network_, std::string definedOn,
                              const std::vector<double>& values, DataType dataType);

  virtual void draw() override;
  virtual void buildCustomUI() override;
  virtual std::string niceName() override;
  virtual void refresh() override;

protected:
  // UI internals
  const std::string definedOn;
  std::shared_ptr<render::ShaderProgram> edgeProgram;

  // Helpers
  virtual void createProgram() = 0;
};

// ========================================================
// ==========             Node Scalar            ==========
// ========================================================

class CuboidNetworkNodeScalarQuantity : public CuboidNetworkScalarQuantity {
public:
  CuboidNetworkNodeScalarQuantity(std::string name, const std::vector<double>& values_, CuboidNetwork& network_,
                                  DataType dataType_ = DataType::STANDARD);

  virtual void createProgram() override;

  void buildNodeInfoGUI(size_t nInd) override;
};


// ========================================================
// ==========            Edge Scalar             ==========
// ========================================================

class CuboidNetworkEdgeScalarQuantity : public CuboidNetworkScalarQuantity {
public:
  CuboidNetworkEdgeScalarQuantity(std::string name, const std::vector<double>& values_, CuboidNetwork& network_,
                                  DataType dataType_ = DataType::STANDARD);

  virtual void createProgram() override;

  void buildEdgeInfoGUI(size_t edgeInd) override;
};


} // namespace polyscope
