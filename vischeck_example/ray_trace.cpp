#include "ray_trace.h"
#include "handle.h"

using namespace std;

map_loader map;

bool IsKeyDown(int vk)
{
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

int main()
{
    Memory csgo = Memory();
    csgo.open("cs2.exe");

    uint64_t client_base = csgo.get_client_base();

    printf("client.dll: 0x%lx \n", client_base);

    Vector r_start;
    Vector r_end;
    
    map.load_map("inferno");

    int cnt = 0;

    while (true) {

        uint64_t LocalPawn = csgo.Read<uint64_t>(client_base + OFFSET_LOCAL_PAWN);
        if (IsKeyDown(VK_F6)) {
            r_end = csgo.Read<Vector>(LocalPawn + OFFSET_POS);
            r_end.z += 73;
        }
        else {
            r_start = csgo.Read<Vector>(LocalPawn + OFFSET_POS);
            r_start.z += 73;
        }
        
        auto time_begin = std::chrono::steady_clock::now();

        if (map.is_visible(r_start, r_end)) {
            cout << "INVISIBLE" << endl;
            auto i_end = std::chrono::steady_clock::now();
            //cout << "TimeCost" << 1000/std::chrono::duration<double, std::milli>(i_end - time_begin).count() << "fps" << endl;
        }
        else {
            auto time_end = std::chrono::steady_clock::now();
            cnt++;
            cout << "Count:" << cnt << " TimeCost" << 1000/std::chrono::duration<double, std::milli>(time_end - time_begin).count() << "fps" << endl;
        }
    }

    map.unload();

    system("pause");
}
