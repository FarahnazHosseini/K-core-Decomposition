#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <omp.h>

using namespace std;

class graph {
public:
    vector< vector <int> > levels;
    int totalEdge;
    int totalNode;
    vector<int> edges;
    vector<int> degree;
    vector<int> NodeIndex;
    vector<int> core;
    int maxDegree;
    int minDegree;
    void kCore();
    void setGraph(int, int);
    friend void readFile(graph& g, string fileName);
    friend void printGraph(graph g);
    void setLevels();
    void printLevels();
    void printCores();
    void printNodeIndex();

};

//...........................
void graph::setGraph(int totalNode, int totalEdge) {
    this->totalNode = totalNode;
    this->totalEdge = totalEdge;
    edges.resize(totalNode + totalEdge + 1, -1);
    degree.resize(totalNode, 0);
    NodeIndex.resize(totalNode, 0);
    core.resize(totalNode, -1);
}

//...........................
void graph::setLevels() {
    levels.resize(totalNode);

    // Parallelizing loop to distribute work across threads
    #pragma omp parallel for
    for (int i = 0; i < totalNode; i++) {
        int d = degree[i];
        if (d != -1) {
            #pragma omp critical  // Prevents concurrent writes to levels[d]
            {
                levels[d].push_back(i);
                NodeIndex[i] = levels[d].size() - 1;
            }
        }
    }
}

//........................
void graph::printLevels() {
    for (int i = 0; i <= maxDegree; i++) {
        cout << "level " << i << ": ";
        for (int j : levels[i])
            cout << j << " , ";
        cout << endl;
    }
}

//...........................
int findMin(vector<int> items) {
    int min = items.size();
    for (int i = 0; i < items.size(); i++) {
        if (min > items[i] && items[i] != -1) {
            min = items[i];
        }
    }
    return min;
}
//...........................
int findMax(vector<int> items) {
    int max = items[0];
    for (int i = 0; i < items.size(); i++) {
        if (max < items[i] && items[i] != -1) {
            max = items[i];
        }
    }
    return max;
}
//...........................
void graph::printNodeIndex() {
    cout << "Index of Nodes:\n";
    for (int j = 0; j < totalNode; j++) {
        cout << "node " << j << " :" << NodeIndex[j] << "\n";
    }
    cout << endl;
}

//...........................
void graph::printCores() {
    cout << "Cores of Nodes:\n";
    for (int j = 0; j < totalNode; j++) {
        cout << "node " << j << " :" << core[j] << "\n";
    }
    cout << "\n#################################\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
}

//...........................
void graph::kCore() {
    vector<int> newDegree = degree;
    int l = minDegree, currentNode, i, neighbour, levelNeighbour, from, n;

    while (l <= maxDegree) {
        i = 0;
        while (i < levels[l].size()) {
            if (levels[l][i] != -1) {
                currentNode = levels[l][i];
                levels[l][i] = -1;
                newDegree[currentNode] = -1;
                core[currentNode] = l;
                from = edges[currentNode];

                // Parallelizing the neighbor updates
                #pragma omp parallel for private(neighbour, levelNeighbour, from, n)
                for (int j = 0; j < degree[currentNode]; j++) {
                    neighbour = edges[j + from];
                    if (newDegree[neighbour] != -1) {
                        levelNeighbour = newDegree[neighbour];
                        n = NodeIndex[neighbour];

                        if (levelNeighbour > l) {
                            #pragma omp critical  // Ensures safe modification of shared data
                            {
                                levels[levelNeighbour][n] = -1;
                                levels[levelNeighbour - 1].push_back(neighbour);
                                NodeIndex[neighbour] = levels[levelNeighbour - 1].size() - 1;
                            }
                            newDegree[neighbour]--;
                        }
                    }
                }
            }
            i++;
        }
        l++;
    }
}
//...........................
void printDegrees(vector<int>ndegree) {

    cout << ", minDegree=" << findMin(ndegree) << endl;
    cout << "\n__________________________________\n" << "Degrees of Nodes: " << endl;
    for (int i = 0; i < ndegree.size(); i++)
        cout << "Degree " << i << " :" << ndegree[i] << "\t";
    cout << endl;
    cout << "\n__________________________________\n";
}
//............................
void printGraph(graph g) {
    cout << endl;
    int from, to;
    for (int i = 0;i < g.totalNode;i++) {
        from = g.edges[i];
        cout << i << ": ";
        if (g.degree[i]>0) {
            for (int j = 0;j < g.degree[i];j++) {
                cout << g.edges[j+from] << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
    printDegrees(g.degree);
}
//...........................

void readFile(graph& g, string fileName) {
    ifstream p(fileName);
    if (!p.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        exit(1);
    }

    int totalNode, totalEdge;
    p >> totalNode >> totalEdge;
    g.setGraph(totalNode, totalEdge);

    int src, rowIndex = 0, dest, lastSrc = -1;
    int colIndex = totalNode;

    while (p >> src >> dest) {
        if (dest == -1) {
            rowIndex++;
            lastSrc = src;
            g.degree[src] = -1;
            continue;
        } else if (src != lastSrc) {
            g.edges[rowIndex] = colIndex;
            rowIndex++;
            lastSrc = src;
        }
        g.edges[colIndex] = dest;
        colIndex++;
        g.degree[src]++;
    }

    g.maxDegree = *max_element(g.degree.begin(), g.degree.end());
    g.minDegree = *min_element(g.degree.begin(), g.degree.end());

    p.close();
}

//............................
int main() {
    graph g;
    string fileName = "network6.txt";
    readFile(g, fileName);

    g.setLevels();
    clock_t start = clock();

    g.kCore();

    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "\n" << "cpu_time_used  " << cpu_time_used << "\n";

    return 0;
}
