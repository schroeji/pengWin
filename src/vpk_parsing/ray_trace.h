#ifndef RAY_TRACE_H
#define RAY_TRACE_H
#include "typedefs.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

// credits tni & learn_more (www.unknowncheats.me/forum/3868338-post34.html)
#define INRANGE(x, a, b) (x >= a && x <= b)
#define getBits(x)                                                             \
  (INRANGE(x, '0', '9') ? (x - '0') : ((x & (~0x20)) - 'A' + 0xa))
#define get_byte(x) (getBits(x[0]) << 4 | getBits(x[1]))

struct BoundingBox {
  Vector min, max;

  bool intersect(const Vector &ray_origin,
                 const Vector &ray_end) const { // Slabs method
    Vector dir = ray_end - ray_origin;
    dir = dir.Normalize(); // 确保方向向量是单位向量

    float t1 = (min.x - ray_origin.x) / dir.x;
    float t2 = (max.x - ray_origin.x) / dir.x;
    float t3 = (min.y - ray_origin.y) / dir.y;
    float t4 = (max.y - ray_origin.y) / dir.y;
    float t5 = (min.z - ray_origin.z) / dir.z;
    float t6 = (max.z - ray_origin.z) / dir.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)),
                          std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)),
                          std::max(t5, t6));

    // 如果 tmax < 0，光线与盒子相交在光线的反方向上，所以不相交
    if (tmax < 0) {
      return false;
    }

    // 如果 tmin > tmax，光线不会穿过盒子，所以不相交
    if (tmin > tmax) {
      return false;
    }

    return true;
  }
};

struct Triangle {
  Vector p1, p2, p3;

  bool intersect(Vector ray_origin, Vector ray_end) const {
    const float EPSILON = 0.0000001f;
    Vector edge1, edge2, h, s, q;
    float a, f, u, v, t;
    edge1 = p2 - p1;
    edge2 = p3 - p1;
    h = CrossProduct(ray_end - ray_origin, edge2);
    a = edge1.Dot(h);

    if (a > -EPSILON && a < EPSILON)
      return false; // 光线与三角形平行，不相交

    f = 1.0 / a;
    s = ray_origin - p1;
    u = f * s.Dot(h);

    if (u < 0.0 || u > 1.0)
      return false;

    q = CrossProduct(s, edge1);
    v = f * (ray_end - ray_origin).Dot(q);

    if (v < 0.0 || u + v > 1.0)
      return false;

    // 计算 t 来找到交点
    t = f * edge2.Dot(q);

    if (t > EPSILON && t < 1.0) // 确保 t 在 0 和 1 之间，表示交点在线段上
      return true;

    return false; // 这意味着光线与三角形不相交或者在三角形的边界上
  }
};

struct KDNode {
  BoundingBox bbox;
  std::vector<Triangle> triangle;
  KDNode *left, *right = nullptr;
  int axis;

  void deleteKDTree(KDNode *node) {
    if (node == nullptr)
      return;

    // 递归地删除子节点
    deleteKDTree(node->left);
    deleteKDTree(node->right);

    // 删除当前节点
    delete node;
  }
};

bool rayIntersectsKDTree(KDNode *node, const Vector &ray_origin,
                         const Vector &ray_end);

BoundingBox calculateBoundingBox(const std::vector<Triangle> &triangles);

KDNode *buildKDTree(std::vector<Triangle> &triangles, int depth = 0);

class MapLoader {
public:
  std::vector<Triangle> triangles;
  KDNode *kd_tree;

  void unload() { kd_tree->deleteKDTree(kd_tree); }

  void load_map(std::string map_name) {
    auto begin = std::chrono::steady_clock::now();

    std::ifstream in(map_name + ".tri", std::ios::in | std::ios::binary);

    in.seekg(0, std::ios::end);
    std::streamsize fileSize = in.tellg();
    in.seekg(0, std::ios::beg);

    std::size_t num_elements = fileSize / sizeof(Triangle);
    triangles.resize(num_elements);
    if (!in.read(reinterpret_cast<char *>(triangles.data()), fileSize)) {
      throw std::runtime_error("Failed to read file: " + map_name + ".tri");
    }
    in.close();

    kd_tree = buildKDTree(triangles);
    std::vector<Triangle>().swap(triangles);

    auto i_end = std::chrono::steady_clock::now();
    std::cout
        << "[MAP] Loaded {" << map_name << "} "
        << std::chrono::duration<double, std::milli>(i_end - begin).count()
        << "ms" << std::endl;
  }

  bool is_visible(Vector ray_origin, Vector ray_end) {
    return !rayIntersectsKDTree(kd_tree, ray_origin, ray_end);
  }
};
#endif
