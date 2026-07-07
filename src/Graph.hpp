#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

class Graph
{
private:
    std::vector<std::vector<std::pair<int, float>>> m_graph;
    std::vector<std::vector<std::pair<int, float>>> m_revgraph;
    std::vector<std::string> toCorrectWords(const std::string &s);
    std::vector<std::string> m_words;
    std::vector<int> findShortestBFS(int start);
    std::vector<int> findKbestBFS(int start, int k);
    int getStart(const std::vector<std::string> &correct);
    double getPathScore(std::vector<int> &path) const;
    double getWeight(int from, int to) const;

    std::string pathToSentence(std::vector<int> &path) const;

public:
    Graph();
    void loadFromFile(std::filesystem::path &filename);
    std::string answerTo(std::string &sentence);
};