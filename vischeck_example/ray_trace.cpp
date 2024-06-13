#include "ray_trace.h"

using namespace std;

vector<Triangle> triangles;

static void load_map(string map_name, vector<Triangle> &vec_triangles) {
    ifstream in( map_name + ".tri", ios::in);
    istreambuf_iterator<char> beg(in), end;
    string strdata(beg, end);
    vec_triangles = bytes_to_vec<Triangle>(strdata);
    string().swap(strdata);
    in.close();
}

int main()
{
    Vector r_start = { 1107, 169, -148 };
    Vector r_end = { 842,440,-193 }; //should be invisible
    Vector r_end_2 = { 1131, 404 , -198 };//should be visible

    load_map("mirage", triangles);
    int cnt = 0;

    KDNode* kd_tree = buildKDTree(triangles);

    vector<Triangle>().swap(triangles);

    while (true) {
        auto begin = std::chrono::steady_clock::now();

        if (rayIntersectsKDTree(kd_tree, r_start, r_end)) {
            cout << "INVISIBLE" << endl;
            auto i_end = std::chrono::steady_clock::now();
            cout << std::chrono::duration<double, std::milli>(i_end - begin).count() << "ms" << endl;
        }
        else {
            auto o_end = std::chrono::steady_clock::now();
            cnt++;
            cout << "Count:" << cnt << " TimeCost" << std::chrono::duration<double, std::milli>(o_end - begin).count() << "ms" << endl;
        }
    }
}

