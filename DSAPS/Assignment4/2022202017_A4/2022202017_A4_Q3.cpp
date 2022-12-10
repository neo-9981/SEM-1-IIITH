#include <climits>
#include <iostream>
#include <math.h>
#include <utility>
#include <vector>
using namespace std;

struct treeNode {
  long int weight;              // used to hold the current weight
  int node;                     // used to hold the node
  treeNode() {}                 // default constructor
  treeNode(long int s, int i) { // parameterised constructor
    weight = s;
    node = i;
  }
};

class priorityQ {

public:
  treeNode *pqArray;
  int currSize;

  priorityQ() {
    currSize = 0;
    pqArray = new treeNode[100000];
  }

  void push(treeNode e) {
    currSize++;
    pqArray[currSize - 1] = e;

    long long ind = currSize - 1;
    while (true) {
      int parent = (ind - 1) / 2;
      if (parent < 0 || parent == ind ||
          pqArray[parent].weight <= pqArray[ind].weight) {
        break;
      }

      struct treeNode temp = pqArray[parent];
      pqArray[parent] = pqArray[ind];
      pqArray[ind] = temp;

      ind = parent;
    }
  }

  int size() { return currSize; }

  treeNode top() { return pqArray[0]; }
  void pop() {
    pqArray[0] = pqArray[currSize - 1];
    currSize--;
    heapify(0);
  }
  void heapify(int ind) {
    long long leftChild = 2 * ind + 1;
    long long rightChild = 2 * ind + 2;

    int maxIndex = ind;

    if (leftChild < currSize) {
      if (pqArray[leftChild].weight < pqArray[maxIndex].weight) {
        maxIndex = leftChild;
      }
    }
    if (rightChild < currSize) {
      if (pqArray[rightChild].weight < pqArray[maxIndex].weight) {
        maxIndex = rightChild;
      }
    }

    if (maxIndex != ind) {
      struct treeNode temp;
      temp.weight = pqArray[ind].weight;
      temp.node = pqArray[ind].node;

      pqArray[ind].weight = pqArray[maxIndex].weight;
      pqArray[ind].node = pqArray[maxIndex].node;

      pqArray[maxIndex].weight = temp.weight;
      pqArray[maxIndex].node = temp.node;

      heapify(maxIndex);
    }
  }
};

vector<int> Dijikstra(const vector<vector<pair<int, int>>> &Graph,
                      int initialPoliceStation) {
  int n = Graph.size();
  vector<int> distance(n, INT_MAX);
  distance[initialPoliceStation] = 0;

  vector<int> visited(n, false);
  // mh of (distance, node) pairs prioritized by smallest distance
  priorityQ mh;
  // push into priorityQ
  treeNode n1;
  n1.node = initialPoliceStation;
  n1.weight = 0;
  mh.push(n1);
  while (mh.size() != 0) {
    int u = mh.top().node;
    mh.pop();
    if (visited[u])
      continue; // not first extraction so ignore it
    visited[u] = true;
    for (auto edge : Graph[u]) {
      int v = edge.first, l = edge.second;
      if (distance[u] + l < distance[v]) {
        distance[v] = distance[u] + l;
        mh.push({distance[v], v});
      }
    }
  }
  return distance;
}

int main() {

  int n, m, k;
  cin >> n >> m >> k;

  vector<vector<pair<int, int>>> Graph;
  Graph.resize(n);

  for (int i = 0; i < m; i++) {
    int u, v, w;
    cin >> u >> v >> w;
    Graph[u - 1].push_back({v - 1, w});
    Graph[v - 1].push_back({u - 1, w});
  }

  int initialPoliceStation;
  cin >> initialPoliceStation;
  initialPoliceStation--;
  vector<int> policeStations;
  for (int i = 1; i < k; i++) {
    int var;
    cin >> var;
    policeStations.push_back(var - 1);
  }

  for (int i = 0; i < k - 1; i++) {
    Graph[initialPoliceStation].push_back({policeStations[i], 0});
    Graph[policeStations[i]].push_back({initialPoliceStation, 0});
  }

  vector<int> result;
  result = Dijikstra(Graph, initialPoliceStation);

  int size = result.size();
  for (int i = 0; i < size; i++) {
    if (result[i] == INT_MAX) {
      cout << -1 << " ";
    } else {
      cout << result[i] << " ";
    }
  }

  return 0;
}