#include <vector>
#include <queue>
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;


// Typedefs for better code readability.
// A proper graph abstract data type would be better. 
typedef vector<vector<pair<int, int>>> Graph;
typedef vector<vector<pair<int, int>>> Tree;
typedef vector<int> Path;

// Dijkstra implementation
Path dijkstra(Graph graph, int start, int finish);

// Generates minimum spanning tree of wieghted undirected graph
Tree MST(Graph graph);

// Minimum spanning tree depth-first search. 
// The default parameters are used for recursion, and should not be passed from outside the function.
Path MSTDFS(Tree graph, int start, int finish, int node = -1, int prev = -1, Path path = {});

int main() {

	// Hardcoded graph. Ideally this should be loaded from a text file similar to the sample data
	Graph graph = {
		
		/* node     List of connected {node, weight} edges */
		
		/* 0 */		{{1, 10}, {3, 12}, {6, 11}, {7,  4}},
		/* 1 */		{{0, 10}, {3,  8}, {7, 20}},
		/* 2 */		{{3, 17}, {4,  8}, {6, 13}, {7, 10}},
		/* 3 */		{{0, 12}, {1,  8}, {2, 17}, {5, 16}, {6, 24}, {7, 14}},
		/* 4 */		{{2,  8}, {5,  8}, {6, 11}, {7,  5}},
		/* 5 */		{{3, 16}, {4,  8}, {6, 18}, {7, 21}},
		/* 6 */		{{0, 11}, {2, 13}, {3, 24}, {4, 11}, {5, 18}, {7, 30}},
		/* 7 */		{{0,  4}, {1, 20}, {2, 10}, {3, 14}, {4,  5}, {5, 21}, {6, 30}}
	};

	const int count = 100000;


	// Run Dijkstra
	milliseconds startMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	for (int i = 0; i < count; i++) {
		dijkstra(graph, rand() % 8, rand() % 8);
	}
	milliseconds finishMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	milliseconds duration = finishMS - startMS;

	cout << "Dijkstra: " << duration.count() << "ms" << endl;


	// Run MSTDFS
	// Graph must first be converted to MST
	startMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	Tree mst = MST(graph);
	for (int i = 0; i < count; i++) {
		MSTDFS(mst, rand() % 8, rand() % 8);
	}
	finishMS = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	duration = finishMS - startMS;

	cout << "MSTDFS: " << duration.count() << "ms" << endl;


	// Exit
	system("pause>nul");
	return 0;
}

Graph MST(Graph graph) {

	// Struct representing candidate edge/node
	struct node {
		int index;  // Node index
		int weight; // Edge weight
		int from;   // Preceding node index
	};

	// Struct used to define priority queue sorting by weight
	struct compare {
		constexpr bool operator()(
			node const& a,
			node const& b)
			const noexcept
		{
			return a.weight > b.weight;
		}
	};

	// Priority queue of canditate edges. O(1) top access. O(log n) insertion
	priority_queue<node, vector<node>, compare> edges;

	// Vector of size V to represent whether nodes are visited. O(1) visited querying.
	vector<bool> visited = vector<bool>(graph.size(), false);

	// Generate MST using Prim's algorithm
	Graph mst = vector<vector<pair<int, int>>>(graph.size(), vector<pair<int, int>>());

	int currentNode = 0; // Pick arbitrary starting node

	// Attempt to add V-1 edges. Less is not connected. More is not a tree.
	for (int i = 0; i < graph.size() - 1; i++) {

		visited[currentNode] = true;

		// For each unvisited node connected to the current node, add it to the priority queue.
		for (auto edge : graph[currentNode]) {
			if (!visited[edge.first]) {
				edges.push({ edge.first, edge.second, currentNode });
			}
		}

		// Select the minimum edge from the priority queue
		while (true) {

			// If no valid edges, break
			if (edges.empty()) {
				cout << "No tree found" << endl;
				return {};
			}

			// If min node is already visited, discard and continue
			if (visited[edges.top().index]) {
				edges.pop();
				continue;
			}

			// Otherwise, select current node and add edge to the tree.
			currentNode = edges.top().index;
			mst[edges.top().from].push_back(make_pair(edges.top().index, edges.top().weight));
			mst[edges.top().index].push_back(make_pair(edges.top().from, edges.top().weight));

			edges.pop(); // Remove edge from priority queue
			break;
		}

	}
	return mst;
}

Path MSTDFS(Tree graph, int start, int finish, int node, int prev, Path path) {

	// If node == -1, this is the top recursion level, so we cout and return the result here.
	if (node == -1) {
		Path result = MSTDFS(graph, start, finish, start, -1, {});
		//for (int i = 0; i < result.size(); i++) {
		//	cout << result[i] << (i == result.size() - 1 ? "\n" : " -> ");
		//}
		return result;
	}

	// Add self to the path
	path.push_back(node);

	// For each edge connected to the current node
	for (auto edge : graph[node]) {

		// Discard edge to previous node. This avoids double backing on self.
		if (edge.first == prev) {
			continue;
		}
		// Check for finish node
		if (edge.first == finish) {
			path.push_back(edge.first);
			return path;
		}
		// Otherwise, recurse!
		Path branch = MSTDFS(graph, start, finish, edge.first, node, path);

		// If the target node was on the branch, return the solution back up the chain
		if (!branch.empty()) {
			return branch;
		}
	}
	// Otherwise, dud branch.
	return {};
}

Path dijkstra(Graph graph, int start, int finish) {
	
	// Struct representing candidate edge/node
	struct node {
		int index;  // Node index
		int weight; // Edge weight
		int from;   // Preceding node index
	};
	// Struct used to define priority queue sorting by weight
	struct compare {
		constexpr bool operator()(
			node const& a,
			node const& b)
			const noexcept
		{
			return a.weight > b.weight;
		}
	};

	// Priority queue of canditate edges. O(1) top access. O(log n) insertion
	priority_queue<node, vector<node>, compare> unvisited;

	// Vector of size V to represent whether nodes are visited, and by which node.
	// Given, node i, visited[i] represents the node which last updated i's distance.
	// If visited[i] = -1, then i is unvisited.
	enum { UNVISITED = -1 };
	vector<int> visited = vector<int>(graph.size(), UNVISITED);

	// Assign initital current node/dist
	int currentNode = start;
	int currentDist = 0;

	while (true) {

		// For each unvisited node connected to the current node, add connecting edge to the priority queue.
		for (auto edge : graph[currentNode]) {
			if (visited[edge.first] == UNVISITED) {
				unvisited.push({ edge.first, edge.second + currentDist, currentNode }); 
			}
		}

		// Select next cheapest unvisited node.
		while (true) {

			// If no valid edges, break
			if (unvisited.empty()) {
				cout << "No solution found" << endl;
				return {};
			}
			// If cheapest node is already visited, discard and continue
			if (visited[unvisited.top().index] != UNVISITED) {
				unvisited.pop();
				continue;
			}
			// Else, select current node
			currentNode = unvisited.top().index;
			currentDist = unvisited.top().weight;
			visited[currentNode] = unvisited.top().from;
			unvisited.pop();
			break;
		}

		// Check for completion
		if (currentNode == finish) {
			
			// Rebuild path by backtracking from current node, until reached start node.
			Path path;
			while (currentNode != start) {
				path.insert(path.begin(), currentNode);
				currentNode = visited[currentNode];
			}
			path.insert(path.begin(), currentNode);

			// cout and return path
			//for (int i = 0; i < path.size(); i++) {
			//	cout << path[i] << (i == path.size() - 1 ? "\n" : " -> ");
			//}
			return path;
		}
	}
}
