// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/cuboid_network.h"
#include "polyscope/histogram.h"
#include "polyscope/render/color_maps.h"

namespace polyscope {

class CuboidNetworkScalarQuantity : public CuboidNetworkQuantity {
public:
  CuboidNetworkScalarQuantity(std::string name, CuboidNetwork& network_, std::string definedOn, DataType dataType);

  virtual void draw() override;
  virtual void buildCustomUI() override;
  virtual std::string niceName() override;
  virtual void geometryChanged() override;

  // === Members
  const DataType dataType;

  // === Get/set visualization parameters

  // The color map
  CuboidNetworkScalarQuantity* setColorMap(std::string name);
  std::string getColorMap();

  // Data limits mapped in to colormap
  CuboidNetworkScalarQuantity* setMapRange(std::pair<double, double> val);
  std::pair<double, double> getMapRange();
  CuboidNetworkScalarQuantity* resetMapRange(); // reset to full range

protected:
  // === Visualization parameters

  // Affine data maps and limits
  std::pair<float, float> vizRange;
  std::pair<double, double> dataRange;
  Histogram hist;

  // UI internals
  PersistentValue<std::string> cMap;
  const std::string definedOn;
  std::shared_ptr<render::ShaderProgram> edgeProgram;

  // Helpers
  virtual void createProgram() = 0;
  void setProgramUniforms(render::ShaderProgram& program);
};

// ========================================================
// ==========             Node Scalar            ==========
// ========================================================

class CuboidNetworkNodeScalarQuantity : public CuboidNetworkScalarQuantity {
public:
  CuboidNetworkNodeScalarQuantity(std::string name, std::vector<double> values_, CuboidNetwork& network_,
                                  DataType dataType_ = DataType::STANDARD);

  virtual void createProgram() override;

  void buildNodeInfoGUI(size_t nInd) override;

  // === Members
  std::vector<double> values;
};


// ========================================================
// ==========            Edge Scalar             ==========
// ========================================================

class CuboidNetworkEdgeScalarQuantity : public CuboidNetworkScalarQuantity {
public:
  CuboidNetworkEdgeScalarQuantity(std::string name, std::vector<double> values_, CuboidNetwork& network_,
                                  DataType dataType_ = DataType::STANDARD);

  virtual void createProgram() override;

  void buildEdgeInfoGUI(size_t edgeInd) override;


  // === Members
  std::vector<double> values;
};


} // namespace polyscope
