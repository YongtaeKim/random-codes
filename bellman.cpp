
/*

Bellman-Ford Algorithm (C++) ver 1.0

Author: Yongtae Kim
Email: KimYongtae@Live.com

Instruction:

- Complie (tested on GCC 7.2, Visual Studio 17, LLVM 9.0)
>g++ main.cpp -o main

- Run
command line argument: [name of the graph file]
example: ./main graph1.txt


*/

//#include "stdafx.h"

#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>


using std::cout;
using std::endl;

// Node class for this algorithm.
struct Node {

private:
	int v_Weight;
	int v_Node1;
	int v_Node2;

public:
	// accessors
	int getWeight() { return v_Weight; }
	int getNode1() { return v_Node1; }
	int getNode2() { return v_Node2; }

	// mutators
	void setWeight(int &&newWeight) { v_Weight = newWeight; }
	void setNode1(int &&newNode1) { v_Node1 = newNode1; }
	void setNode2(int &&newNode2) { v_Node2 = newNode2; }
};

// implements text split function to std::string.
std::vector<std::string> split(std::string str, std::string token) {
	std::vector<std::string> result;
	while (str.size()) {
		int index = str.find(token);
		if (index != std::string::npos) {
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)result.push_back(str);
		}
		else {
			result.push_back(str);
			str = "";
		}
	}
	return result;
}



int main(int argc, char *argv[])
{
	// Graph containing edges.
	std::vector<std::shared_ptr<Node>> edges;

	const size_t infinity = 99999;

	// Reads graph file
	// x-y:w where x-y represents the node-node, and w is the edge weight.
	std::string line;
	std::ifstream file_G(argv[1]);

	// Used for counting nodes.
	std::string counter;
	size_t numNodes = 0;

	// Stores shortest-path information.
	std::vector<int> distance;

	// Extracting the graph data from the file.
	if (file_G.is_open())
	{
		size_t idx = 0;

		while (std::getline(file_G, line))
		{
			// Node-Node:Weight
			std::vector <std::string> tmp = split(line, ":");
			// Node-Node
			std::vector <std::string> node = split(tmp[0], "-");

			// Node to be added to the edges
			auto tempNode = std::make_shared<Node>();

			// Builds Nodes with the data from the graph file.
			tempNode->setWeight(stoi(tmp[1]));
			tempNode->setNode1(stoi(node[0]));
			tempNode->setNode2(stoi(node[1]));

			// Adds the Node to the edges container.
			edges.push_back(tempNode);
			idx++;

			 // Prints the graph built.
			 cout << "Creating Edge " << idx << " -> Parent " << node[0] 
				 << " Child " << node[1] << ", Weight " << tmp[1] << endl;

			 // Counts the number of unique Nodes in the Graph
			 // (It's for convinience but adds additional time complexity)
			 if (counter.find(node[0]) == std::string::npos) { counter.append("/" + node[0]); }
			 if (counter.find(node[1]) == std::string::npos) { counter.append("/" + node[1]); }
		}
		// Number of unique Nodes in the Graph
		numNodes = std::count(counter.begin(), counter.end(), '/');

		cout << "\nProcessed! \nNumber of Edges: " << edges.size() 
			<< "\nNumber of Nodes: " << numNodes << endl;

		file_G.close();
	}

	else {
        cout << "Can't read the file!";
        return 0;
    }

	// Step 1: Initialize Graph
	// At the beginning , all nodes have a weight of infinity
	for (size_t i = 0; i < numNodes; i++)
	{
		distance.push_back(infinity);
	}

	// Distance from Source to Source is obviously zero.
	distance[0] = 0;

	// Step 2: relax edges repeatedly
	for (size_t i = 1; i < numNodes; i++)
	{
		for (auto k : edges) {
			// Set the distances with the Source and Distance values from each Nodes.
			int u = k->getNode1();
			int v = k->getNode2();
			int weight = k->getWeight();

			// Then for all edges, if the distance to the destination can be shortened by taking the edge,
			// the distance is updated to the new lower value.
			// At each iteration that the edges are scanned, the algorithm finds all shortest
			// paths of at most length.
			if ((distance.at(u) != infinity) && (distance.at(u) + weight < distance.at(v))) {
				distance.at(v) = distance.at(u) + weight;
			}
		}
	}

	// Step 3: check for negative-weight cycles
	// A final scan of all the edges is performed.
	for (auto i : edges)
	{
		int u = i->getNode1();
		int v = i->getNode2();
		int weight = i->getWeight();

		// This can only occur if at least one negative cycle exists in the graph.
		if ((distance.at(u) != infinity) && (distance.at(u) + weight < distance.at(v))) {
			cout << "Graph contains negative-weight cycle!!" << endl;
		}
	}

	// Print the result.
	cout << "\n-------- SOLUTION --------\n" << endl;
	cout << "Node   Distance from Source" << endl;
	for (size_t i = 0; i < numNodes; i++)
	{
		cout << " " << i << "\t\t" << distance.at(i) << endl;
	}
	cout << "\n\nAuthor: Yongtae Kim\n\n" << endl;

}

