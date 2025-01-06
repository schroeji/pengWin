#include "ray_trace.h"
BoundingBox calculateBoundingBox(const std::vector<Triangle> &triangles) {
  BoundingBox box;
  box.min = box.max = triangles[0].p1;
  for (const auto &tri : triangles) {
    for (const auto &p : {tri.p1, tri.p2, tri.p3}) {
      box.min.x = std::min(box.min.x, p.x);
      box.min.y = std::min(box.min.y, p.y);
      box.min.z = std::min(box.min.z, p.z);
      box.max.x = std::max(box.max.x, p.x);
      box.max.y = std::max(box.max.y, p.y);
      box.max.z = std::max(box.max.z, p.z);
    }
  }
  return box;
}

KDNode *buildKDTree(std::vector<Triangle> &triangles, int depth) {
  if (triangles.empty())
    return nullptr;

  KDNode *node = new KDNode();
  node->bbox = calculateBoundingBox(triangles);
  node->axis = depth % 3; // 分割轴是根据深度选择的

  if (triangles.size() <= 3) {
    node->triangle = triangles;
    return node;
  }

  auto comparator = [axis = node->axis](const Triangle &a, const Triangle &b) {
    // 比较函数使用 node->axis 来获取当前的分割轴
    float a_center, b_center;
    switch (axis) {
    case 0:
      a_center = (a.p1.x + a.p2.x + a.p3.x) / 3;
      b_center = (b.p1.x + b.p2.x + b.p3.x) / 3;
      break;
    case 1:
      a_center = (a.p1.y + a.p2.y + a.p3.y) / 3;
      b_center = (b.p1.y + b.p2.y + b.p3.y) / 3;
      break;
    case 2:
      a_center = (a.p1.z + a.p2.z + a.p3.z) / 3;
      b_center = (b.p1.z + b.p2.z + b.p3.z) / 3;
      break;
    }
    return a_center < b_center;
  };

  std::nth_element(triangles.begin(), triangles.begin() + triangles.size() / 2,
                   triangles.end(), comparator);

  std::vector<Triangle> left_triangles(
      triangles.begin(), triangles.begin() + triangles.size() / 2);
  std::vector<Triangle> right_triangles(
      triangles.begin() + triangles.size() / 2, triangles.end());

  node->left = buildKDTree(left_triangles, depth + 1);
  node->right = buildKDTree(right_triangles, depth + 1);

  return node;
}

bool rayIntersectsKDTree(KDNode *node, const Vector &ray_origin,
                         const Vector &ray_end) {
  std::cout << "Node: " << node << std::endl;
  printVec("Ray origin: ", ray_origin);
  printVec("Ray end: ", ray_end);
  if (node == nullptr)
    return false;

  if (!node->bbox.intersect(ray_origin, ray_end)) {
    std::cout << "No bbox intersect" << std::endl;
    return false;
  }

  if (node->triangle.size() > 0) {
    std::cout << "Triangles > 0" << std::endl;
    bool hit = false;
    for (const auto &tri : node->triangle) {
      if (tri.intersect(ray_origin, ray_end)) {
        hit = true;
        break;
      }
    }
    return hit;
  }

  bool hit_left = rayIntersectsKDTree(node->left, ray_origin, ray_end);
  bool hit_right = rayIntersectsKDTree(node->right, ray_origin, ray_end);

  return hit_left || hit_right;
}
