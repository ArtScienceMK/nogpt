#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

struct Statistic
{
    std::vector<std::string> uncorrect_words;
    std::vector<std::string> algsName;
    std::vector<std::pair<int, long long>> result_alg;
};

class Graph
{
private:
    std::vector<std::vector<std::pair<int, float>>> m_graph;
    std::vector<std::vector<std::pair<int, float>>> m_revgraph;
    std::pair<std::vector<std::string>, std::vector<std::string>> toCorrectWords(const std::string &s);
    std::vector<std::string> m_words;
    std::vector<int> findShortestBFS(int start);
    std::vector<int> findProbbestDijkstra(int start);
    std::vector<int> findKbestBFS(int start, int k);
    std::vector<int> findKlenBFS(int start, int k);
    std::vector<int> findKrandom(int start, int k);
    std::vector<int> findKgreedy(int start, int k);
    int getStart(const std::vector<std::string> &correct);
    double getPathScore(std::vector<int> &path) const;
    double getWeight(int from, int to) const;

    void cropPath(std::vector<int>& path);
    std::string pathToSentence(std::vector<int> &path) const;

public:
    Graph();
    void loadFromFile(std::filesystem::path &filename);
    void answerTo(std::string &sentence, Statistic &stat);
};