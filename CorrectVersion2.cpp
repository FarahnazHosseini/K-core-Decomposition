#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
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
    NodeIndex.resize(totalNode, 0);//
    core.resize(totalNode, -1);
}
//...........................
void graph::setLevels() {
    int d;
    levels.resize(totalNode);//should be resized to max degree 
    for (int i = 0; i < totalNode; i++) {
        d = degree[i];
        if(d!=-1){
            levels[d].push_back(i);
            NodeIndex[i]=levels[d].size()-1;
        }
    }
    //printNodeIndex();
}
//........................
void graph::printLevels() {
    for (int i = 0;i <= maxDegree;i++) {
        cout << "level  " << i << ":";
        for (int j = 0;j < levels[i].size();j++)
            cout << levels[i][j] << " , ";
        cout << endl;
    }
}
//.........................
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
    cout<<endl;
    //cout << "\n#################################\n" << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
}
//...........................
void graph::printCores() {
    cout << "Cores of Nodes:\n";
    for (int j = 0; j < totalNode; j++) {
        cout << "node " << j << " :" << core[j] << "\n";
    }
    cout << "\n#################################\n" << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
}
//...........................
void graph::kCore() {
    vector<int> newDegree = degree;
    int l= minDegree, currentNode, i, neighbour, levelNeighbour, from, n;
    while (l <= maxDegree) {
            i = 0;
            while (i < levels[l].size()) {
                if (levels[l][i] != -1) {
                    currentNode = levels[l][i];
                    levels[l][i] = -1;
                    newDegree[currentNode] = -1;
                    core[currentNode] = l;
                    from = edges[currentNode];
                    for (int j = 0;j < degree[currentNode];j++) {
                        neighbour = edges[j + from];
                        if (newDegree[neighbour] != -1) {
                            levelNeighbour = newDegree[neighbour];
                            //for (n = 0;n < levels[levelNeighbour].size();n++)//
                              //  if (levels[levelNeighbour][n] == neighbour)//
                                //    break;//
                            n=NodeIndex[neighbour];
                            if (levelNeighbour > l) {
                                levels[levelNeighbour][n] = -1;
                                levels[levelNeighbour - 1].push_back(neighbour);//
                                NodeIndex[neighbour]=levels[levelNeighbour - 1].size()-1;
                                newDegree[neighbour]--;
                            }
                        }
                        //printCores();
                        //printNodeIndex();
                    }//for
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
    fstream p;
    string s_src, s_dest, totalEdge, totalNode;
    p.open(fileName);
    p >> totalNode >> totalEdge;
    g.setGraph(stoi(totalNode), stoi(totalEdge));
    int src, rowIndex = 0, dest, lastSrc = -1;
    int colIndex = stoi(totalNode);
    while (!p.eof()) {
        p >> s_src >> s_dest;
        src = stoi(s_src);
        dest = stoi(s_dest);
        if (dest == -1) {
            rowIndex++;
            lastSrc = src;
            g.degree[src] = -1;
            continue;
        }
        else if (src != lastSrc) {
            g.edges[rowIndex] = colIndex;//
            rowIndex++;
            lastSrc = src;
        }
        g.edges[colIndex] = dest;
        colIndex++;
        g.degree[src]++;
    }
    g.maxDegree = findMax(g.degree);
    g.minDegree = findMin(g.degree);
    p.close();
}
//..........................................
int main() {
    graph g;
    string fileName = "network6.txt";
    readFile(g, fileName);

    //printGraph(g);
    
    g.setLevels();
    clock_t start = clock();
    //g.printLevels();
    g.kCore();
    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    cout << "\n" << "cpu_time_used  " << cpu_time_used << "\n";
    //g.printCores();

}
