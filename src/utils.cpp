#include "utils.h"

GraphNode::GraphNode(std::unique_ptr<Node> data_) : data(std::move(data_)) {}

GraphNode *Graph::addNode(std::unique_ptr<Node> data) {
  nodes.push_back(std::make_unique<GraphNode>(std::move(data)));
  return nodes.back().get();
}

void Graph::addEdge(GraphNode *parent, GraphNode *child) {
  parent->children.push_back(child);
  child->parents.push_back(parent);
}

std::vector<GraphNode *> Graph::sort() {
  std::unordered_map<GraphNode *, int> inDegree;
  for (auto &n : nodes) {
    inDegree[n.get()] = n->parents.size();
  }

  std::queue<GraphNode *> q;
  for (auto &[node, deg] : inDegree) {
    if (deg == 0)
      q.push(node);
  }

  std::vector<GraphNode *> sorted;
  while (!q.empty()) {
    auto n = q.front();
    q.pop();
    sorted.push_back(n);

    for (auto *child : n->children) {
      inDegree[child]--;
      if (inDegree[child] == 0)
        q.push(child);
    }
  }

  if (sorted.size() != nodes.size()) {
    throw std::runtime_error("Graph has cycles!");
  }

  return sorted;
}

void Graph::traverse(const std::function<void(Node *)> &func) {
  for (auto &n : nodes) {
    func(n->data.get());
  }
}

const std::vector<std::unique_ptr<GraphNode>> &Graph::getNodes() const {
  return nodes;
}
