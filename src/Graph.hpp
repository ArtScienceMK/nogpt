#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;

class Graph
{
private:
    vector<vector<pair<int, float>>> m_graph;
    vector<vector<pair<int, float>>> m_revgraph;
    vector<string> correct(const string &s);
    vector<string> m_words;
    vector<int> findShortestBFS(int start);
    int getStart(const vector<string> &correct);
    double getPathScore(vector<int> &path) const;
    double getWeight(int from, int to) const;

    string pathToSentence(vector<int> &path) const;

public:
    Graph();
    void loadFromFile(filesystem::path &filename);
    string answerTo(string &sentence);
};