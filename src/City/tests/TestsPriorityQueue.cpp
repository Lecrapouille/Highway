#include <queue>
#include <iostream>
#include <vector>

class Road
{
public:

    Road(size_t p)
        : priority(p)
    {}

    size_t priority;
};

class Compare
{
public:

    bool operator() (Road const* road1, Road const* road2)
    {
        return road1->priority > road2->priority;
    }
};

using PriorityQueue = std::priority_queue<Road*, std::vector<Road*>, Compare>;

int main()
{
    PriorityQueue q;
    std::vector<Road> roads{
        Road(42), Road(0), Road(3), Road(82), Road(5), Road(32)
    };

    for (auto& it: roads)
        q.push(&it);

    while (!q.empty())
    {
        std::cout << q.top()->priority << std::endl;
        q.pop();
    }
    return 0;
}
