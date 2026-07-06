#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Graph
{
    vector<vector<pair<int, float>>> m_graph;
    vector<string> correct(const string &s);
    vector<string> m_words;

public:
    Graph();
    void loadFromFile(string &filename);
};