// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

#include "polyscope/affine_remapper.h"
#include "polyscope/color_management.h"
#include "polyscope/cuboid_network_quantity.h"
#include "polyscope/polyscope.h"
#include "polyscope/render/engine.h"
#include "polyscope/standardize_data_array.h"
#include "polyscope/structure.h"

#include "polyscope/cuboid_network_color_quantity.h"
#include "polyscope/cuboid_network_scalar_quantity.h"
#include "polyscope/cuboid_network_vector_quantity.h"

#include <vector>

namespace polyscope {

// Forward declare cuboid network
class CuboidNetwork;

// Forward declare quantity types
class CuboidNetworkNodeScalarQuantity;
class CuboidNetworkEdgeScalarQuantity;
class CuboidNetworkNodeColorQuantity;
class CuboidNetworkEdgeColorQuantity;
class CuboidNetworkNodeVectorQuantity;
class CuboidNetworkEdgeVectorQuantity;


template <> // Specialize the quantity type
struct QuantityTypeHelper<CuboidNetwork> {
  typedef CuboidNetworkQuantity type;
};

class CuboidNetwork : public QuantityStructure<CuboidNetwork> {
public:
  // === Member functions ===

  // Construct a new cuboid network structure
  CuboidNetwork(std::string name, std::vector<glm::vec3> nodes, std::vector<std::array<size_t, 2>> edges,
                std::vector<glm::vec3> orientations);

  // === Overloads

  // Build the imgui display
  virtual void buildCustomUI() override;
  virtual void buildCustomOptionsUI() override;
  virtual void buildPickUI(size_t localPickID) override;

  // Render the the structure on screen
  virtual void draw() override;

  // Render for picking
  virtual void drawPick() override;

  // A characteristic length for the structure
  virtual double lengthScale() override;

  // Axis-aligned bounding box for the structure
  virtual std::tuple<glm::vec3, glm::vec3> boundingBox() override;
  virtual std::string typeName() override;

  // === Quantities

  // Scalars
  template <class T>
  CuboidNetworkNodeScalarQuantity* addNodeScalarQuantity(std::string name, const T& values,
                                                         DataType type = DataType::STANDARD);
  template <class T>
  CuboidNetworkEdgeScalarQuantity* addEdgeScalarQuantity(std::string name, const T& values,
                                                         DataType type = DataType::STANDARD);

  // Colors
  template <class T>
  CuboidNetworkNodeColorQuantity* addNodeColorQuantity(std::string name, const T& values);
  template <class T>
  CuboidNetworkEdgeColorQuantity* addEdgeColorQuantity(std::string name, const T& values);

  // Vectors
  template <class T>
  CuboidNetworkNodeVectorQuantity* addNodeVectorQuantity(std::string name, const T& vectors,
                                                         VectorType vectorType = VectorType::STANDARD);
  template <class T>
  CuboidNetworkEdgeVectorQuantity* addEdgeVectorQuantity(std::string name, const T& vectors,
                                                         VectorType vectorType = VectorType::STANDARD);
  template <class T>
  CuboidNetworkNodeVectorQuantity* addNodeVectorQuantity2D(std::string name, const T& vectors,
                                                           VectorType vectorType = VectorType::STANDARD);
  template <class T>
  CuboidNetworkEdgeVectorQuantity* addEdgeVectorQuantity2D(std::string name, const T& vectors,
                                                           VectorType vectorType = VectorType::STANDARD);


  // === Members and utilities

  // The nodes that make up this cuboid network
  std::vector<glm::vec3> nodes;
  std::vector<glm::vec3> upDir;
  std::vector<size_t> nodeDegrees; // populated on construction
  size_t nNodes() const { return nodes.size(); }

  std::vector<std::array<size_t, 2>> edges;
  size_t nEdges() const { return edges.size(); }


  // Misc data
  static const std::string structureTypeName;

  // Small utilities
  void setCuboidNetworkNodeUniforms(render::ShaderProgram& p);
  void setCuboidNetworkEdgeUniforms(render::ShaderProgram& p);
  void fillEdgeGeometryBuffers(render::ShaderProgram& program);
  void fillNodeGeometryBuffers(render::ShaderProgram& program);


  // === Mutate
  template <class V>
  void updateNodePositions(const V& newPositions);
  template <class V>
  void updateOrientations(const V& newPositions);
  template <class V>
  void updateNodePositions2D(const V& newPositions);

  // === Get/set visualization parameters

  // set the base color of the points
  CuboidNetwork* setColor(glm::vec3 newVal);
  glm::vec3 getColor();

  // set the radius of the points
  CuboidNetwork* setWidth(float newVal, bool isRelative = true);
  float getWidth();
  CuboidNetwork* setHeight(float newVal, bool isRelative = true);
  float getHeight();

  // Material
  CuboidNetwork* setMaterial(std::string name);
  std::string getMaterial();

private:
  // === Visualization parameters
  PersistentValue<glm::vec3> color;
  PersistentValue<ScaledValue<float>> w_n;
  PersistentValue<ScaledValue<float>> w_b;
  PersistentValue<std::string> material;


  // Drawing related things
  // if nullptr, prepare() (resp. preparePick()) needs to be called
  std::shared_ptr<render::ShaderProgram> edgeProgram;
  std::shared_ptr<render::ShaderProgram> edgePickProgram;
  std::shared_ptr<render::ShaderProgram> nodePickProgram;

  // === Helpers

  // Do setup work related to drawing, including allocating openGL data
  void prepare();
  void preparePick();

  void geometryChanged();

  // Pick helpers
  void buildNodePickUI(size_t nodeInd);
  void buildEdgePickUI(size_t edgeInd);

  // === Quantity adder implementations
  // clang-format off
  CuboidNetworkNodeScalarQuantity* addNodeScalarQuantityImpl(std::string name, const std::vector<double>& data, DataType type);
  CuboidNetworkEdgeScalarQuantity* addEdgeScalarQuantityImpl(std::string name, const std::vector<double>& data, DataType type);
  CuboidNetworkNodeColorQuantity* addNodeColorQuantityImpl(std::string name, const std::vector<glm::vec3>& colors);
  CuboidNetworkEdgeColorQuantity* addEdgeColorQuantityImpl(std::string name, const std::vector<glm::vec3>& colors);
  CuboidNetworkNodeVectorQuantity* addNodeVectorQuantityImpl(std::string name, const std::vector<glm::vec3>& vectors, VectorType vectorType);
  CuboidNetworkEdgeVectorQuantity* addEdgeVectorQuantityImpl(std::string name, const std::vector<glm::vec3>& vectors, VectorType vectorType);
  // clang-format on
};


// Shorthand to add a cuboid network to polyscope
template <class P, class E, class Z>
CuboidNetwork* registerCuboidNetwork(std::string name, const P& points, const E& edges, const Z& orientation);
template <class P, class E>
CuboidNetwork* registerCuboidNetwork2D(std::string name, const P& points, const E& edges);


// Shorthand to add a cuboid network, automatically constructing the connectivity of a line
template <class P, class Z>
CuboidNetwork* registerCuboidNetworkLine(std::string name, const P& points, const Z& orientation);
template <class P>
CuboidNetwork* registerCuboidNetworkLine2D(std::string name, const P& points);


// Shorthand to add a cuboid network, automatically constructing the connectivity of a loop
template <class P, class Z>
CuboidNetwork* registerCuboidNetworkLoop(std::string name, const P& points, const Z& orientation);
template <class P>
CuboidNetwork* registerCuboidNetworkLoop2D(std::string name, const P& points);

// Shorthand to get a cuboid network from polyscope
inline CuboidNetwork* getCuboidNetwork(std::string name = "");
inline bool hasCuboidNetwork(std::string name = "");
inline void removeCuboidNetwork(std::string name = "", bool errorIfAbsent = true);

} // namespace polyscope

#include "polyscope/cuboid_network.ipp"
