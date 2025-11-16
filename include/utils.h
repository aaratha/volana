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

enum class OscType : int { Sine = 0, Saw = 1, Square = 2, Triangle = 3 };

// ---------------- Node base ----------------

class GraphNode;

struct Node {
  virtual ~Node() = default;
  virtual void update() = 0;  // pure virtual
  GraphNode *owner = nullptr; // automatically set when added to graph

  std::atomic<float> out{0.0f};
  bool audioOut = false; // whether this node contributes to final audio
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
