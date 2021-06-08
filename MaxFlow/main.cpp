#include "maxflow.h"

#include <string>
#include <map>
#include <tuple>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono>
#include <filesystem>

using std::map;
using std::set;
using std::pair;
using std::tuple;
using std::vector;
using std::string;
using std::ifstream;
using std::get;
using std::endl;
using std::make_pair;
using std::cout;
using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

namespace fs = std::filesystem;

pair<
	pair<unsigned, unsigned>,
	map<pair<unsigned, unsigned>, unsigned>
> getDataFromFile(const string& path) {
	ifstream file(path);
	if (!file) throw "Cannot open file!";

	pair<
		pair<size_t, size_t>,
		map<pair<unsigned, unsigned>, unsigned>
	> graph;
	size_t n, m;
	
	file >> n >> m;
	graph.first.first = n;
	graph.first.second = m;

	for (size_t k = 0; k < m; ++k) {
		unsigned i, j, weight;
		file >> i >> j >> weight;
		graph.second[make_pair(i - 1, j - 1)] = weight;
	}

	file.close();
	return graph;
}

int main() {
	/*pair<
		pair<size_t, size_t>,
		map<pair<unsigned, unsigned>, unsigned>
	> graph;
	graph = make_pair(
		make_pair(3u, 2u),
		map<pair<unsigned, unsigned>, unsigned>()
	);
	graph.second[make_pair(0u, 1u)] = 4u;
	graph.second[make_pair(1u, 2u)] = 3u;*/

	

	string path = "tests";
	int counter = 0;
	for (const auto& entry : fs::directory_iterator(path)) {
		if (counter == 28) {
				auto graph = getDataFromFile(entry.path().string());
				set<unsigned> obj, bg;
				set<pair<unsigned, unsigned>> cut;
				map<pair<unsigned, unsigned>, unsigned> toSave;
				unsigned answer = 0u;
				long long duration = 0ll;
				vector<unsigned> ms = {
					graph.first.second / 5, 
					graph.first.second,
					graph.first.second * 5,
					1000000000
				};

				for (const auto m: ms) {
					vector<unsigned> counters(4, 0u);
					unsigned s{ 0u }, t{ graph.first.first - 1u };
					auto begin = steady_clock::now();
					answer = pushRelabelMaxFlow(
						graph.first.first,
						graph.first.second,
						graph.second,
						s,
						t,
						obj,
						bg,
						cut,
						toSave,
						m,
						counters);
					auto end = steady_clock::now();

					duration = duration_cast<milliseconds>(end - begin).count();

					if (duration >= 1000ll) {
						cout << "Parameter m = " << m << endl;
						for (size_t i = 0; i < counters.size(); ++i) {
							cout << "Counter[" << i << "] shows: " << counters[i] << endl;
						}
						cout << "Duration = " << duration / 1000 << "." << duration % 1000 << " sec." << endl << endl;
					}
				}
				cout << entry.path().string() << "\t" << answer << endl << endl << endl;
		}
		++counter;
	}

	return 0;
}