#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Graph
{
    vector<vector<pair<int, float>>> m_graph;
    vector<string> m_words;
    string findCurrentWord(const string &s);

public:
    Graph();
    void loadFromFile(string &filename);
};