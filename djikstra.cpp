
/*

Djikstra Routing (C++) ver 1.0

Author: Yongtae Kim
Email: KimYongtae@Live.com

Instruction:

- Complie (tested on gcc 7.2)
>g++ main.cpp -std=c++17 -o main
(Requires C++ compiler supporting standard 17)

- Run
command line argument: [name of the graph file] [root node] [destination node]
example: ./main g1.txt A E


*/

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
	int cost;
	std::string addr;
	std::string tracker;

public:
	Node(std::string address, int theCost)
		: addr(address), cost(theCost) {};

	Node(std::string address, int theCost, std::string &theTrace)
		: addr(address), cost(theCost), tracker(theTrace) {};

	// accessors
	int getCost() { return cost; }
	std::string getAddr() { return addr; }
	std::string getTracker() { return tracker; }
};


// Defines compare rule for the priority_queue. Lowest cost goes to top.
class myCompare_Lowest {
public:
	bool operator()(const std::shared_ptr<Node> lhs, const std::shared_ptr<Node> rhs) const
	{
		return lhs->getCost() > rhs->getCost();
	}
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

	// Main container for the entire graph.
	std::unordered_map<std::string, std::vector<std::shared_ptr<Node>>> nodeMap;

	// Reads graph file
	// Each line must contains path and cost. eg) [0] A, [1] C, [2] 4
	std::string line;
	std::ifstream file_G(argv[1]);

	// Extracting the graph data from the file.
	if (file_G.is_open())
	{
		// Creates new nodeMapList to be mapped and stored to the nodeMap.
		std::vector<std::shared_ptr<Node>> nodeMapList;

		while (std::getline(file_G, line))
		{
			std::vector <std::string> tmp = split(line, " ");

			// If it has NO mapping for the key tmp[0].
			if (nodeMap.count(tmp[0]) == 0) 
			{
				// Clear the containor and build with the new key.
				nodeMapList.clear();
				cout << "Creating List for " << tmp[0] << endl;

				// Put the List to the Map with the key tmp[0].
				nodeMap.insert_or_assign(tmp[0], nodeMapList);

				// Add path to the List.
				auto path = std::make_shared<Node>(tmp[1], std::stoi(tmp[2]));
				nodeMapList.push_back(path);
				nodeMap.insert_or_assign(tmp[0], nodeMapList);
			}

			// If it HAS mapping for the key.
			else if (nodeMap.count(tmp[0]) != 0)
			{
				// Add path to the List.
				auto path = std::make_shared<Node>(tmp[1], std::stoi(tmp[2]));
				nodeMapList.push_back(path);

				// Put the List to the Map with the key tmp[0].
				nodeMap.insert_or_assign(tmp[0], nodeMapList);
			}
		}
		file_G.close();
	}

	else {
        cout << "Can't read the file!";
        return 0;
    }


	// Print out the list of Nodes just constructed.
	cout << "\nGraph Constructed! \n(Node1 - Node2 : Cost)\n" << endl;
	for (std::pair<std::string, std::vector<std::shared_ptr<Node>>> element : nodeMap)
	{
		for (auto i : element.second)
		{
			cout << element.first << "-" << i->getAddr() << ":" << i->getCost() << "  ";
		}
		cout << endl;
	}

	// Defines root and destination Node.
	std::string root = argv[2];
	std::string destination = argv[3];

	// for storing shortest path.
	std::unordered_map<std::string, std::shared_ptr<Node>> shortestDistances;

	// Djikstra's algorithm using the PriorityQueue with the comparison class.
	std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, myCompare_Lowest> nodePQ;

	// Initialize the current node, cost and routing path information.
	auto startNode = std::make_shared<Node>(root, 0, root); // Initiate the tracker with root.
	nodePQ.push(startNode);

	while (!nodePQ.empty()) {

		// Retrieve the current node information at the head of the queue.
		auto currentNode = nodePQ.top();
		nodePQ.pop();

		// If the top(Node with lowest cost) is inside the shortestDistance Map,
		// update the Node and push into the priority_queue.
		if (shortestDistances.find(currentNode->getAddr()) == shortestDistances.end())
		{
			int currentCost = currentNode->getCost();
			std::string nextHop = currentNode->getAddr();

			// Push the current shortest into the priority_queue.
			shortestDistances.insert_or_assign(nextHop, currentNode);

			std::vector<std::shared_ptr<Node>> next = nodeMap[nextHop];

			// Ordering with the priority Queue
			for (auto i : next) {

				// update the total cost of current path.
				int updatedCost = i->getCost() + currentCost;

				// Save the routing path information to the current node.
				std::string updatedTracker = currentNode->getTracker() + " -> " + i->getAddr();

				// Create new Node with updated information.
				auto updatedNode = std::make_shared<Node>(i->getAddr(), updatedCost, updatedTracker);

				// Push into the priority_queue.
				nodePQ.push(updatedNode);

				updatedTracker.clear();
			}
		}
	}

	// Extracts a Next-hop Node.
	auto nexthop = split(shortestDistances[destination]->getTracker(), " -> ");

	// Print out the result.
	cout << "\n\nResult:\n\n";
	cout << "Dest   Cost   Next-Hop" << endl;
	cout << "----   ----   --------" << endl;
	cout << "  " << destination << "     ";
	cout << shortestDistances[destination]->getCost() << "       ";
	cout << nexthop[1];
	cout << "\n\n";
	cout << "     Path Used  " << endl;
	cout << "   -------------" << endl;
	cout << "     " << shortestDistances[destination]->getTracker() << endl;
	cout << "\nAuthor: Yongtae Kim\n\n";
}
