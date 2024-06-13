#include "ray_trace.h"

using namespace std;

map_loader map;

int main()
{
    Vector r_start = { 1107, 169, -148 };
    Vector r_end = { 842,440,-193 }; //should be invisible
    Vector r_end_2 = { 1131, 404 , -198 };//should be visible
    
    map.load_map("mirage");

    int cnt = 0;

    while (true) {
        auto time_begin = std::chrono::steady_clock::now();

        if (map.is_visible(r_start, r_end)) {
            cout << "INVISIBLE" << endl;
            auto i_end = std::chrono::steady_clock::now();
            cout << "TimeCost" << std::chrono::duration<double, std::milli>(i_end - time_begin).count() << "ms" << endl;
        }
        else {
            auto time_end = std::chrono::steady_clock::now();
            cnt++;
            cout << "Count:" << cnt << " TimeCost" << std::chrono::duration<double, std::milli>(time_end - time_begin).count() << "ms" << endl;
            break;
        }
    }

    map.unload();

    system("pause");
}
