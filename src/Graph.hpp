#include <iostream>
#include <vector>

using namespace std;

class Graph
{
    vector<vector<pair<int, float>>> m_graph;
    string findCurrentWord(const string &s);

public:
    Graph();
};