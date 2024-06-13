#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include "Vector3D.hpp"

// credits tni & learn_more (www.unknowncheats.me/forum/3868338-post34.html)
#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define get_byte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

typedef struct Triangle {
    Vector3D p1, p2, p3;
};

struct BoundingBox {
    Vector3D min, max;

    bool intersect(const Vector3D& ray_origin, const Vector3D& ray_end) const { //Slabs method
        Vector3D dir = ray_end.Subtract(ray_origin);
        dir = dir.Normalize(); // ȷ�����������ǵ�λ����

        float t1 = (min.x - ray_origin.x) / dir.x;
        float t2 = (max.x - ray_origin.x) / dir.x;
        float t3 = (min.y - ray_origin.y) / dir.y;
        float t4 = (max.y - ray_origin.y) / dir.y;
        float t5 = (min.z - ray_origin.z) / dir.z;
        float t6 = (max.z - ray_origin.z) / dir.z;

        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

        // ��� tmax < 0������������ཻ�ڹ��ߵķ������ϣ����Բ��ཻ
        if (tmax < 0) {
            return false;
        }

        // ��� tmin > tmax�����߲��ᴩ�����ӣ����Բ��ཻ
        if (tmin > tmax) {
            return false;
        }

        return true;
    }
};

struct KDNode {
    BoundingBox bbox;
    std::vector<Triangle> triangle;
    KDNode* left, * right;
    int axis;
};

bool ray_intersects_triangle(Vector3D p1, Vector3D p2, Vector3D p3, Vector3D ray_origin, Vector3D ray_end) {
    const float EPSILON = 0.0000001;
    Vector3D edge1, edge2, h, s, q;
    float a, f, u, v, t;
    edge1 = p2.Subtract(p1);
    edge2 = p3.Subtract(p1);
    h = CrossProduct(ray_end.Subtract(ray_origin), edge2);
    a = edge1.DotProduct(h);

    if (a > -EPSILON && a < EPSILON)
        return false;    // ������������ƽ�У����ཻ

    f = 1.0 / a;
    s = ray_origin.Subtract(p1);
    u = f * s.DotProduct(h);

    if (u < 0.0 || u > 1.0)
        return false;

    q = CrossProduct(s, edge1);
    v = f * (ray_end.Subtract(ray_origin)).DotProduct(q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    // ���� t ���ҵ�����
    t = f * edge2.DotProduct(q);

    if (t > EPSILON && t < 1.0) // ȷ�� t �� 0 �� 1 ֮�䣬��ʾ�������߶���
        return true;

    return false; // ����ζ�Ź����������β��ཻ�����������εı߽���
}

bool rayIntersectsKDTree(KDNode* node, const Vector3D& ray_origin, const Vector3D& ray_end) {
    if (node == nullptr) return false;

    Vector3D ray_direction = ray_end.Subtract(ray_origin);
    if (!node->bbox.intersect(ray_origin, ray_direction)) {
        return false;
    }

    if (node->triangle.size() > 0) {
        bool hit = false;
        for (const auto& tri : node->triangle) {
            if (ray_intersects_triangle(tri.p1, tri.p2, tri.p3, ray_origin, ray_end)) {
                hit = true;
            }
        }
        return hit;
    }

    bool hit_left = rayIntersectsKDTree(node->left, ray_origin, ray_end);
    bool hit_right = rayIntersectsKDTree(node->right, ray_origin, ray_end);

    return hit_left || hit_right;
}

BoundingBox calculateBoundingBox(const std::vector<Triangle>& triangles) {
    BoundingBox box;
    // ��ʼ��Ϊ��һ�������εĵ�һ����
    box.min = box.max = triangles[0].p1;
    for (const auto& tri : triangles) {
        for (const auto& p : { tri.p1, tri.p2, tri.p3 }) {
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

KDNode* buildKDTree(std::vector<Triangle>& triangles, int depth = 0) {
    if (triangles.empty()) return nullptr;

    KDNode* node = new KDNode();
    node->bbox = calculateBoundingBox(triangles);
    node->axis = depth % 3; // �ָ����Ǹ������ѡ���

    if (triangles.size() <= 3) {
        node->triangle = triangles;
        return node;
    }

    auto comparator = [axis = node->axis](const Triangle& a, const Triangle& b) {
        // �ȽϺ���ʹ�� node->axis ����ȡ��ǰ�ķָ���
        switch (axis) {
        case 0: return a.p1.x < b.p1.x; // x��
        case 1: return a.p1.y < b.p1.y; // y��
        case 2: return a.p1.z < b.p1.z; // z��
        default: return false; // ��ֹδ������Ϊ
        }
    };

    std::nth_element(triangles.begin(), triangles.begin() + triangles.size() / 2, triangles.end(), comparator);

    std::vector<Triangle> left_triangles(triangles.begin(), triangles.begin() + triangles.size() / 2);
    std::vector<Triangle> right_triangles(triangles.begin() + triangles.size() / 2, triangles.end());

    node->left = buildKDTree(left_triangles, depth + 1);
    node->right = buildKDTree(right_triangles, depth + 1);

    return node;
}

template <typename Ty>
std::vector<Ty> bytes_to_vec(const std::string& bytes)
{
    const auto num_bytes = bytes.size() / 3;
    const auto num_elements = num_bytes / sizeof(Ty);

    std::vector<Ty> vec;
    vec.resize(num_elements + 1);

    const char* p1 = bytes.c_str();
    uint8_t* p2 = reinterpret_cast<uint8_t*>(vec.data());
    while (*p1 != '\0')
    {
        if (*p1 == ' ')
        {
            ++p1;
        }
        else
        {
            *p2++ = get_byte(p1);
            p1 += 2;
        }
    }

    return vec;
}