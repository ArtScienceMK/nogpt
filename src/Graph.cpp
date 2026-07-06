#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>

using namespace std;

double Graph::getPathScore(vector<int> &path) const
{
    int L = path.size();
    double summP = 0.0;
    unordered_map<string, int> m;

    ++m[m_words[path[0]]];
    for (size_t i = 1; i < L; i++)
    {
        summP += getWeight(path[i - 1], path[i]);
        ++m[m_words[path[i]]];
    }

    double P = summP / L;
    int summCount = 0;

    for (auto it : m)
    {
        summCount += it.second;
    }

    double U = (double) summCount / (double) L;

    return exp(-0.05 * abs(L - 15)) * P * exp(-0.5 * (U - 1));
}

double Graph::getWeight(int from, int to) const
{
    for (auto it : m_graph[from])
    {
        if (it.first == to)
            return it.second;
    }
    return 0.0;
}

Graph::Graph() : m_graph(0),
                 m_words(0)
{
}

vector<string> Graph::correct(const string &s)
{
    vector<string> words;
    string cur = "";
    for (char c : s)
    {
        if (!isalpha(c) && !isdigit(c))
        {
            words.push_back(cur);
            cur = "";
        }
        else
        {
            cur += c;
        }
    }
    if ((int)cur.size() > 0)
    {
        words.push_back(cur);
    }
    vector<string> correct_words;
    for (string cur : words)
    {
        int curMatches = 0;
        int bestMatches = 0;
        string best = "";
        bool stopped = false;
        for (string word : m_words)
        {
            curMatches = 0;
            if (word.size() != cur.size())
                continue;
            for (int i = 0; i < (int)word.size(); i++)
            {
                if (cur[i] == word[i])
                {
                    curMatches++;
                }
            }
            if (curMatches == (int)cur.size())
            {
                stopped = true;
                correct_words.push_back(cur);
                break;
            }
            if (curMatches > bestMatches)
            {
                bestMatches = curMatches;
                best = cur;
            }
        }
        if (!stopped)
        {
            correct_words.push_back(cur);
        }
    }
    return correct_words;
}

vector<int> Graph::findShortest(int start) {
    vector<int> dist(1e6, INT_MAX);
    dist[start] = 0;
    queue<int> q;
    q.push(start);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto [u, trash] : m_graph[v]) {
            if (dist[u] > dist[v] + 1) {
                dist[u] = dist[v] + 1;
                q.push(u);
            }
        }
    }
}

void loadFromFile(string &filename)
{
    ifstream file(filename);

    int V;
    file >> V;
    m_words.resize(V);
    m_graph.resize(V);
    for (size_t i = 0; i < V; i++)
    {
        file >> m_words[i];
    }
    int R;
    file >> R;
    for (size_t i = 0; i < R; i++)
    {
        int a, b;
        float w;
        file >> a >> b >> w;
        m_graph[a].push_back({b, w});
    }
}
