#pragma once

#include <vector>
#include <tuple>
#include <map>
#include <set>

#include "queue.h"

struct Edge {
    const unsigned end;
    int flow;
    unsigned residualCapacity;

    explicit Edge(unsigned _end, unsigned _weight);
    Edge(const Edge&);
    Edge& operator=(const Edge&);
};

struct Vertex {
    unsigned height;
    int excess;
    std::vector<Edge> adjancedTo;
    std::vector<unsigned> adjancedFrom;

    Vertex();
};

std::vector<unsigned> bfs(
    const std::vector<Vertex>& ver,
    const unsigned s,
    const bool backEdges = false
);

bool push(
    std::vector<Vertex>& ver,
    const unsigned u,
    Edge& e,
    const bool isInitial = false
);

void relabel(std::vector<Vertex>& ver, const unsigned u);

void initializePreflow(
    std::vector<Vertex>& ver,
    const unsigned s,
    const unsigned t,
    Queue<unsigned>& q
);


void globalRelabeling(
    std::vector<Vertex>& ver,
    const unsigned s,
    const unsigned t,
    unsigned& counter,
    bool isInitial = false
);

unsigned pushRelabelMaxFlow(
    const size_t n,
    const size_t m,
    std::map<std::pair<unsigned, unsigned>, unsigned>& edges,
    const unsigned s,
    const unsigned t,
    std::set<unsigned>& obj,
    std::set<unsigned>& bg,
    std::set<std::pair<unsigned, unsigned>>& cut,
    std::map<std::pair<unsigned, unsigned>, unsigned>& edgesToSave,
    const unsigned GRparam,
    std::vector<unsigned>& counters
);