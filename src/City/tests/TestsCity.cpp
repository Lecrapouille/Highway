#include "../../Common/FileSystem.hpp"

#define protected public
#define private public
#include "../CityGenerator.hpp"
#undef protected
#undef private

int main()
{
    CityGenerator::PriorityQueue q;
    std::vector<CityGenerator::Road> roads{
        CityGenerator::Road({0.0_m, 0.0_m}, {400.0_m, 0.0_m}, 0u, true),
        CityGenerator::Road({0.0_m, 0.0_m}, {0.0_m, 0.0_m}, 3u, true),
        CityGenerator::Road({0.0_m, 0.0_m}, {0.0_m, 0.0_m}, 82u, true),
        CityGenerator::Road({0.0_m, 0.0_m}, {-400.0_m, 0.0_m}, 0u, true),
        CityGenerator::Road({0.0_m, 0.0_m}, {0.0_m, 0.0_m}, 5u, true),
        CityGenerator::Road({0.0_m, 0.0_m}, {0.0_m, 0.0_m}, 32u, true)
    };

    for (auto& it: roads)
        q.push(&it);

    while (!q.empty())
    {
        std::cout << q.top()->to.x << ": " << q.top()->priority << std::endl;
        q.pop();
    }
    return 0;
}
