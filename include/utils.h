#pragma once
#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

#define DEVICE_SAMPLE_RATE 44800.0f
#define DEVICE_FORMAT ma_format_f32
#define DEVICE_CHANNELS 2

// ---------------- Node base ----------------
struct Node {
  virtual ~Node() = default;
  virtual void update() = 0; // pure virtual
};

// ---------------- GraphNode ----------------
class GraphNode {
public:
  std::unique_ptr<Node> data;
  std::vector<GraphNode *> children;
  std::vector<GraphNode *> parents;

  explicit GraphNode(std::unique_ptr<Node> data_);
};

// ---------------- Graph ----------------
class Graph {
private:
  std::vector<std::unique_ptr<GraphNode>> nodes;

public:
  Graph() = default;

  // Add a node and return a raw pointer to it
  GraphNode *addNode(std::unique_ptr<Node> data);

  // Add a parent → child edge
  void addEdge(GraphNode *parent, GraphNode *child);

  // Topological sort using Kahn's algorithm
  std::vector<GraphNode *> sort();

  // Traverse all nodes and call func on their data

  void traverse(const std::function<void(Node *)> &func);

  const std::vector<std::unique_ptr<GraphNode>> &getNodes() const;
};
