#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <set>
#include <iostream>
#include <chrono>

#include "maxflow.h"
#include "Queue.h"

using std::vector;
using std::pair;
using std::map;
using std::set;
using std::min;
using std::get;
using std::find;
using std::min_element;
using std::remove;
using std::make_pair;
using std::endl;
using std::cout;
using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

template<class T>
inline void eraseRemove(vector<T>& from, const T& el) {
    from.erase(remove(from.begin(), from.end(), el), from.end());
}

template<class T>
inline bool contains(const vector<T>& arr, const T& el) {
    return find(arr.begin(), arr.end(), el) != arr.end();
}

Edge::Edge(unsigned _end, unsigned _weight) :
    end{ _end },
    flow{ 0 },
    residualCapacity{ _weight }
{}

Edge::Edge(const Edge& e) :
    end{ e.end },
    flow{ e.flow },
    residualCapacity{ e.residualCapacity }
{}

Edge& Edge::operator=(const Edge& e) {
    Edge newEdge{e};
    return newEdge;
}

Vertex::Vertex() :
    height{ 0u },
    excess{ 0 },
    adjancedTo{ vector<Edge>{} },
    adjancedFrom{ vector<unsigned>{} }
{}

vector<unsigned> bfs(
    const vector<Vertex>& ver,
    const unsigned s,
    const bool backEdges
) {
    vector<unsigned> distances(ver.size(), UINT_MAX);
    distances[s] = 0u;
    Queue<unsigned> q;
    q.push(s);
    while (!q.isEmpty()) {
        unsigned u{ q.pop() };
        if (backEdges) {
            for (const unsigned v : ver[u].adjancedFrom) {
                if (distances[v] == UINT_MAX) {
                    q.push(v);
                    distances[v] = distances[u] + 1u;
                }
            }
        }
        else {
            for (const auto& edge : ver[u].adjancedTo) {
                unsigned v{ edge.end };
                if (distances[v] == UINT_MAX) {
                    q.push(v);
                    distances[v] = distances[u] + 1u;
                }
            }
        }
    }
    return distances;
}

inline bool push(
    vector<Vertex>& ver,
    const unsigned u,
    Edge& e,
    const bool isInitial
) {
    unsigned v{ e.end };
    bool isSaturated{ false };
    int flow{ int(e.residualCapacity) };
    if (!isInitial) {
        flow = min(ver[u].excess, flow);
        if (flow == e.residualCapacity) {
            isSaturated = true;
        }
    }

    if (!contains(ver[u].adjancedFrom, v)) {
        ver[v].adjancedTo.push_back(Edge(u, 0u));
        ver[u].adjancedFrom.push_back(v);
    }
    auto& rev_e = *find_if(ver[v].adjancedTo.begin(), ver[v].adjancedTo.end(),
        [&u](const auto& e) { return e.end == u; });

    ver[u].excess = ver[u].excess - flow;
    ver[v].excess = ver[v].excess + flow;
    e.flow = e.flow + flow;
    e.residualCapacity = e.residualCapacity - flow;
    rev_e.flow = rev_e.flow - flow;
    rev_e.residualCapacity = rev_e.residualCapacity + flow;

    return isSaturated;
}

inline void relabel(vector<Vertex>& ver, const unsigned u) {
    ver[u].height = 1u + ver[(*min_element(
        ver[u].adjancedTo.begin(),
        ver[u].adjancedTo.end(),
        [&ver](const auto& first, const auto& smallest) {
            return ver[first.end].height < ver[smallest.end].height;
        }
    )).end].height;
}

void initializePreflow(
    vector<Vertex>& ver,
    const unsigned s,
    const unsigned t,
    Queue<unsigned>& q
) {
    for (auto& edge : ver[s].adjancedTo) {
        push(ver, s, edge, true);
        eraseRemove(ver[edge.end].adjancedFrom, s);
        if (edge.end != t and edge.end != s) {
            q.push(edge.end);
        }
    }

    ver[s].adjancedTo.clear();
    ver[s].height = ver.size();
}


void globalRelabeling(
    vector<Vertex>& ver,
    const unsigned s,
    const unsigned t,
    unsigned& pushRelabelCounter,
    bool isInitial
) {
    auto distances{ bfs(ver, t, true) };
    vector<bool> visited(ver.size(), false);
    bool secondBfs{ false };
    for (size_t u = 0u; u < ver.size(); ++u) {
        if (distances[u] != UINT_MAX) {
            visited[u] = true;
            if (distances[u] > ver[u].height) {
                if (!isInitial) pushRelabelCounter += (distances[u] - ver[u].height);
                ver[u].height = distances[u];
            }
        }
        else if (u != s) {
            secondBfs = true;
        }
    }
    if (secondBfs) {
        distances = bfs(ver, s, true);
        for (size_t u = 0u; u < ver.size(); ++u) {
            if (distances[u] != UINT_MAX && !visited[u]) {
                if (distances[u] > ver[u].height) {
                    if (!isInitial) pushRelabelCounter += (distances[u] - ver[u].height);
                    ver[u].height = distances[u];
                }
                visited[u] = true;
            }
        }
    }
}

void printGraph(const vector<Vertex>& ver) {
    cout << "\n\n";
    for (size_t i = 0u; i < ver.size(); ++i) {
        cout << "i=" << i << " h=" << ver[i].height << " e=" << ver[i].excess << " adj_to=" << "[ ";
        for (const auto& e : ver[i].adjancedTo) {
            cout << "{v=" << e.end << ",f=" << e.flow << ",c=" << e.residualCapacity << "} ";
        }
        cout << "] [ ";
        for (const auto& v : ver[i].adjancedFrom) {
            cout << v << " ";
        }
        cout << "]\n";
    }
    cout << "\n\n";
}

void printQueue(const Queue<unsigned>& q, const int pushRelabelCounter) {
    cout << "i=" << pushRelabelCounter << " q=[ ";
    for (size_t i = 0; i < q.size(); ++i) {
        cout << q[i] << " ";
    }
    cout << "]" << endl;
}

unsigned pushRelabelMaxFlow(
    const size_t n,
    const size_t m,
    map<pair<unsigned, unsigned>, unsigned>& edges,
    const unsigned s,
    const unsigned t,
    set<unsigned>& obj,
    set<unsigned>& bg,
    set<pair<unsigned, unsigned>>& cut,
    map<pair<unsigned, unsigned>, unsigned>& edgesToSave,
    const unsigned GRparam,
    vector<unsigned>& counters
) {
    vector<Vertex> ver(n, Vertex());
    Queue<unsigned> q;

    for (const auto& [edge, weight] : edges) {
        ver[edge.first].adjancedTo.push_back(
            Edge(edge.second, weight)
        );
        ver[edge.second].adjancedFrom.push_back(edge.first);
    }

    unsigned GRCounter = 0u;
    bool needToDoGR = true;
    unsigned pushRelabelCounter = 0u;

    initializePreflow(ver, s, t, q);
    globalRelabeling(ver, s, t, GRCounter, true);
    ++counters[2];

    // printGraph(ver);

    while (!q.isEmpty()) {
        // printGraph(ver);
        // printQueue(q, pushRelabelCounter);
        if (needToDoGR && pushRelabelCounter >= GRparam) {
            pushRelabelCounter = 0u;
            GRCounter = 0u;
            ++counters[2];
            globalRelabeling(ver, s, t, GRCounter);
            if (GRCounter == 0) {
                --counters[2];
                needToDoGR = false;
            }
            else {
                counters[3] += GRCounter;
            }
        }

        auto u{ q.pop() };
        set<unsigned> toDel;

        for (auto& e : ver[u].adjancedTo) {
            auto v{ e.end };
            if (ver[u].height == ver[v].height + 1u) {
                if (needToDoGR) ++pushRelabelCounter;
                ++counters[0];
                if (push(ver, u, e)) {
                    toDel.insert(v);
                }
                if (v != s && v != t && !q.contains(v)) {
                    q.push(v);
                }
                if (ver[u].excess == 0) {
                    break;
                }
            }
        }
        if (!toDel.empty()) {
            for (const auto& e : ver[u].adjancedTo) {
                if (toDel.count(e.end)) {
                    eraseRemove(ver[e.end].adjancedFrom, u);
                }
            }
            vector<Edge> tempArr;
            for (const auto& e : ver[u].adjancedTo) {
                if (toDel.count(e.end) == 0) {
                    tempArr.push_back(e);
                }
            }
            ver[u].adjancedTo.clear();
            ver[u].adjancedTo = tempArr;
        }
        if (ver[u].excess > 0) {
            if (needToDoGR) ++pushRelabelCounter;
            ++counters[1];
            relabel(ver, u);
            q.push(u);
        }
    }

    auto distances{ bfs(ver, s) };
    for (size_t i = 0u; i < ver.size(); ++i) {
        if (distances[i] == UINT_MAX) {
            bg.insert(i);
        }
        else {
            obj.insert(i);
        }
    }

    for (const auto& [edge, weight] : edges) {
        if (obj.count(edge.first) && bg.count(edge.second)) {
            cut.insert(edge);
        }
    }

    for (size_t u = 0u; u < ver.size(); ++u) {
        for (const auto& edge : ver[u].adjancedTo) {
            auto v{ edge.end };
            auto c{ edge.residualCapacity };
            edgesToSave[make_pair(u, v)] = c;
        }
    }
    
    //printGraph(ver);

    return ver[t].excess;
}