// Copyright 2017-2019, Nicholas Sharp and the Polyscope contributors. http://polyscope.run.
#pragma once

namespace polyscope {


// Shorthand to add a curve network to polyscope
template <class P, class E, class Z>
CuboidNetwork* registerCuboidNetwork(std::string name, const P& nodes, const E& edges, const Z& orientation) {
  CuboidNetwork* s = new CuboidNetwork(name, standardizeVectorArray<glm::vec3, 3>(nodes),
                                       standardizeVectorArray<std::array<size_t, 2>, 2>(edges),
                                       standardizeVectorArray<glm::vec3, 3>(orientation));
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}
template <class P, class E>
CuboidNetwork* registerCuboidNetwork2D(std::string name, const P& nodes, const E& edges) {
  std::vector<glm::vec3> points3D(standardizeVectorArray<glm::vec3, 2>(nodes));
  for (auto& v : points3D) {
    v.z = 0.;
  }
  std::vector<glm::vec3> orientation(edges.size(), {0, 0, 1});
  CuboidNetwork* s =
      new CuboidNetwork(name, points3D, standardizeVectorArray<std::array<size_t, 2>, 2>(edges), orientation);
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}


// Shorthand to add curve from a line of points
template <class P, class Z>
CuboidNetwork* registerCuboidNetworkLine(std::string name, const P& nodes, const Z& orientation) {

  std::vector<std::array<size_t, 2>> edges;
  size_t N = adaptorF_size(nodes);
  for (size_t iE = 1; iE < N; iE++) {
    edges.push_back({iE - 1, iE});
  }

  CuboidNetwork* s = new CuboidNetwork(name, standardizeVectorArray<glm::vec3, 3>(nodes), edges,
                                       standardizeVectorArray<glm::vec3, 3>(orientation));
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}
template <class P>
CuboidNetwork* registerCuboidNetworkLine2D(std::string name, const P& nodes) {

  std::vector<std::array<size_t, 2>> edges;
  for (size_t iE = 1; iE < nodes.size(); iE++) {
    edges.push_back({iE - 1, iE});
  }
  std::vector<glm::vec3> points3D(standardizeVectorArray<glm::vec3, 2>(nodes));
  for (auto& v : points3D) {
    v.z = 0.;
  }

  std::vector<glm::vec3> orientation(edges.size(), {0, 0, 1});
  CuboidNetwork* s = new CuboidNetwork(name, points3D, edges, orientation);
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}

// Shorthand to add curve from a loop of points
template <class P, class Z>
CuboidNetwork* registerCuboidNetworkLoop(std::string name, const P& nodes, const Z& orientation) {

  std::vector<std::array<size_t, 2>> edges;
  size_t N = adaptorF_size(nodes);
  for (size_t iE = 0; iE < N; iE++) {
    edges.push_back({iE, (iE + 1) % nodes.size()});
  }

  CuboidNetwork* s = new CuboidNetwork(name, standardizeVectorArray<glm::vec3, 3>(nodes), edges,
                                       standardizeVectorArray<glm::vec3, 3>(orientation));
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}
template <class P>
CuboidNetwork* registerCuboidNetworkLoop2D(std::string name, const P& nodes) {

  std::vector<std::array<size_t, 2>> edges;
  size_t N = adaptorF_size(nodes);
  for (size_t iE = 0; iE < N; iE++) {
    edges.push_back({iE, (iE + 1) % nodes.size()});
  }
  std::vector<glm::vec3> points3D(standardizeVectorArray<glm::vec3, 2>(nodes));
  for (auto& v : points3D) {
    v.z = 0.;
  }

  std::vector<glm::vec3> orientation(edges.size(), {0, 0, 1});
  CuboidNetwork* s = new CuboidNetwork(name, points3D, edges, orientation);
  bool success = registerStructure(s);
  if (!success) {
    safeDelete(s);
  }
  return s;
}


template <class V>
void CuboidNetwork::updateNodePositions(const V& newPositions) {
  nodes = standardizeVectorArray<glm::vec3, 3>(newPositions);
  geometryChanged();
}


template <class V>
void CuboidNetwork::updateNodePositions2D(const V& newPositions2D) {
  std::vector<glm::vec3> positions3D = standardizeVectorArray<glm::vec3, 2>(newPositions2D);
  for (glm::vec3& v : positions3D) {
    v.z = 0.;
  }

  // Call the main version
  updateNodePositions(positions3D);
}

template <class V>
void CuboidNetwork::updateOrientations(const V& newOrientations) {
  upDir = standardizeVectorArray<glm::vec3, 3>(newOrientations);

  edgeProgram.reset();
  nodePickProgram.reset();
  edgePickProgram.reset();

  for (auto& q : quantities) {
    q.second->geometryChanged();
  }
}

// Shorthand to get a curve network from polyscope
inline CuboidNetwork* getCuboidNetwork(std::string name) {
  return dynamic_cast<CuboidNetwork*>(getStructure(CuboidNetwork::structureTypeName, name));
}
inline bool hasCuboidNetwork(std::string name) { return hasStructure(CuboidNetwork::structureTypeName, name); }
inline void removeCuboidNetwork(std::string name, bool errorIfAbsent) {
  removeStructure(CuboidNetwork::structureTypeName, name, errorIfAbsent);
}

// =====================================================
// ============== Quantities
// =====================================================

template <class T>
CuboidNetworkNodeColorQuantity* CuboidNetwork::addNodeColorQuantity(std::string name, const T& colors) {
  validateSize(colors, nNodes(), "curve network node color quantity " + name);
  return addNodeColorQuantityImpl(name, standardizeVectorArray<glm::vec3, 3>(colors));
}

template <class T>
CuboidNetworkEdgeColorQuantity* CuboidNetwork::addEdgeColorQuantity(std::string name, const T& colors) {
  validateSize(colors, nEdges(), "curve network edge color quantity " + name);
  return addEdgeColorQuantityImpl(name, standardizeVectorArray<glm::vec3, 3>(colors));
}


template <class T>
CuboidNetworkNodeScalarQuantity* CuboidNetwork::addNodeScalarQuantity(std::string name, const T& data, DataType type) {
  validateSize(data, nNodes(), "curve network node scalar quantity " + name);
  return addNodeScalarQuantityImpl(name, standardizeArray<double, T>(data), type);
}

template <class T>
CuboidNetworkEdgeScalarQuantity* CuboidNetwork::addEdgeScalarQuantity(std::string name, const T& data, DataType type) {
  validateSize(data, nEdges(), "curve network edge scalar quantity " + name);
  return addEdgeScalarQuantityImpl(name, standardizeArray<double, T>(data), type);
}


template <class T>
CuboidNetworkNodeVectorQuantity* CuboidNetwork::addNodeVectorQuantity(std::string name, const T& vectors,
                                                                      VectorType vectorType) {
  validateSize(vectors, nNodes(), "curve network node vector quantity " + name);
  return addNodeVectorQuantityImpl(name, standardizeVectorArray<glm::vec3, 3>(vectors), vectorType);
}

template <class T>
CuboidNetworkNodeVectorQuantity* CuboidNetwork::addNodeVectorQuantity2D(std::string name, const T& vectors,
                                                                        VectorType vectorType) {
  validateSize(vectors, nNodes(), "curve network node vector quantity " + name);

  std::vector<glm::vec3> vectors3D(standardizeVectorArray<glm::vec3, 2>(vectors));
  for (auto& v : vectors3D) {
    v.z = 0.;
  }
  return addNodeVectorQuantityImpl(name, vectors3D, vectorType);
}


template <class T>
CuboidNetworkEdgeVectorQuantity* CuboidNetwork::addEdgeVectorQuantity(std::string name, const T& vectors,
                                                                      VectorType vectorType) {
  validateSize(vectors, nEdges(), "curve network edge vector quantity " + name);
  return addEdgeVectorQuantityImpl(name, standardizeVectorArray<glm::vec3, 3>(vectors), vectorType);
}

template <class T>
CuboidNetworkEdgeVectorQuantity* CuboidNetwork::addEdgeVectorQuantity2D(std::string name, const T& vectors,
                                                                        VectorType vectorType) {
  validateSize(vectors, nEdges(), "curve network edge vector quantity " + name);

  std::vector<glm::vec3> vectors3D(standardizeVectorArray<glm::vec3, 2>(vectors));
  for (auto& v : vectors3D) {
    v.z = 0.;
  }
  return addEdgeVectorQuantityImpl(name, vectors3D, vectorType);
}


} // namespace polyscope
