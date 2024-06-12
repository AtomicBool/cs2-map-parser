#include "kv3-parser.hpp"
#include <fstream>
#include <stdlib.h>
#include <chrono>

using namespace std;

const char *file_name = "mirage.vphys";
const char *export_file_name = "mirage.tri";

// credits tni & learn_more (pasted from www.unknowncheats.me/forum/3868338-post34.html)
#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define get_byte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

template <typename Ty>
vector<Ty> bytes_to_vec(const string& bytes)
{
    const auto num_bytes = bytes.size() / 3;
    const auto num_elements = num_bytes / sizeof(Ty);

    vector<Ty> vec;
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

template <typename Ty>
static string vec_to_bytes(const vector<Ty>& vec)
{
    string bytes;
    for (const auto& element : vec)
    {
        const uint8_t* p1 = reinterpret_cast<const uint8_t*>(&element);
        for (size_t i = 0; i < sizeof(Ty); ++i)
        {
            stringstream ss;
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*(p1 + i));
            bytes += ss.str();
            bytes += ' '; // Adding spaces
        }
    }
    return bytes;
}

typedef struct Vector3 {
    float x, y, z;
};

typedef struct Triangle {
    Vector3 p1, p2, p3;
};

typedef struct Edge {
    uint8_t next, twin, origin, face;
};

c_kv3_parser parser;
vector<Triangle> triangles;

int main()
{
    ifstream in(file_name, ios::in);
    istreambuf_iterator<char> beg(in), end;
    string strdata(beg, end);
    in.close();

    parser.parse(strdata);

    string().swap(strdata);

    int index = 0;
    int count_hulls = 0;
    int count_meshes = 0;

    //check hulls 
    while (true) {
        string index_str = to_string(index);
        string collision_index_str = parser.get_value("m_parts[0].m_rnShape.m_hulls[" + index_str + "].m_nCollisionAttributeIndex");
        if (collision_index_str != "") {
            int collision_index = atoi(collision_index_str.c_str());
            if (collision_index == 0) {
                vector<float> vertex_processed = bytes_to_vec<float>(parser.get_value("m_parts[0].m_rnShape.m_hulls[" + index_str + "].m_Hull.m_Vertices"));

                vector<Vector3> vertices;
                for (int i = 0; i < vertex_processed.size(); i += 3) {
                    vertices.push_back({ vertex_processed[i], vertex_processed[i + 1], vertex_processed[i + 2] });
                }
                vector<float>().swap(vertex_processed);

                vector<uint8_t> faces_processed = bytes_to_vec<uint8_t>(parser.get_value("m_parts[0].m_rnShape.m_hulls[" + index_str + "].m_Hull.m_Faces"));

                vector<uint8_t> edges_tmp = bytes_to_vec<uint8_t>(parser.get_value("m_parts[0].m_rnShape.m_hulls[" + index_str + "].m_Hull.m_Edges"));
                vector<Edge> edges_processed;
                for (int i = 0; i < edges_tmp.size(); i += 4) {
                    edges_processed.push_back({ edges_tmp[i], edges_tmp[i + 1], edges_tmp[i + 2], edges_tmp[i + 3] });
                }
                vector<uint8_t>().swap(edges_tmp);

                for (auto edge_index : faces_processed) {
                    triangles.push_back(
                        {
                            vertices[edges_processed[edge_index].origin],
                            vertices[edges_processed[edges_processed[edge_index].next].origin],
                            vertices[edges_processed[edges_processed[edge_index].twin].origin],
                        });
                }
                vector<uint8_t>().swap(faces_processed);
                vector<Edge>().swap(edges_processed);
                vector<Vector3>().swap(vertices);

                count_hulls++;
            }
        }
        else {
            cout << endl << "Hulls: " << index << " (Total)" << endl;
            cout << endl << "Founded " << count_hulls << " hulls with tag 0" << endl;
            break;
        }
        index++;
    }

    //reset index and check meshes
    index = 0;
    while (true) {
        string index_str = to_string(index);
        string collision_index_str = parser.get_value("m_parts[0].m_rnShape.m_meshes[" + index_str + "].m_nCollisionAttributeIndex");
        if (collision_index_str != "") {
            int collision_index = atoi(collision_index_str.c_str());
            if (collision_index == 0) {
                vector<int> triangle_processed = bytes_to_vec<int>(parser.get_value("m_parts[0].m_rnShape.m_meshes.[" + index_str + "].m_Mesh.m_Triangles"));
                vector<float> vertex_processed = bytes_to_vec<float>(parser.get_value("m_parts[0].m_rnShape.m_meshes.[" + index_str + "].m_Mesh.m_Vertices"));

                vector<Vector3> vertices;
                for (int i = 0; i < vertex_processed.size(); i += 3) {
                    vertices.push_back({ vertex_processed[i], vertex_processed[i + 1], vertex_processed[i + 2] });
                }
                vector<float>().swap(vertex_processed);

                for (int i = 0; i < triangle_processed.size(); i += 3) {
                    triangles.push_back({ vertices[triangle_processed[i]], vertices[triangle_processed[i + 1]], vertices[triangle_processed[i + 2]] });
                }

                vector<int>().swap(triangle_processed);
                vector<Vector3>().swap(vertices);

                count_meshes++;
            }
        }
        else {
            cout << endl << "Meshes: " << index << " (Total)" << endl;
            cout << endl << "Founded " << count_meshes << " meshes with tag 0" << endl;
            break;
        }
        index++;
    }

    parser.~c_kv3_parser();

    ofstream out(export_file_name, ios::out);
    out << vec_to_bytes<Triangle>(triangles);

    system("pause");
}