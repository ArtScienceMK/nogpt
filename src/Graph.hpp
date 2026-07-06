#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Graph
{
private:
    vector<vector<pair<int, float>>> m_graph;
    vector<string> correct(const string &s);
    vector<string> m_words;
    vector<int> findShortestBFS(int start);
    int getStart(const vector<string>&  correct);
    double getPathScore(vector<int>& path) const;
    double getWeight(int from, int to) const;

public:
    Graph();
    void loadFromFile(string &filename);
};