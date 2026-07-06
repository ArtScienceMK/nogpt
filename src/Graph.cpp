#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <climits>
#include "Generator.hpp"

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

    double U = (double)summCount / (double)L;

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

string Graph::pathToSentence(vector<int> &path) const
{
    /*  0 - word
        1 - dight
        2 - symbol
    */
    int last = 0;
    string ans = "";
    for (auto it : path)
    {
        auto word = m_words[it];
        if (isalpha(word[0]))
        {
            ans += " " + word;
            last = 0;
        }
        else if (isdigit(word[0]))
        {
            if (last != 1)
                ans += " ";
            ans += word;
            last = 1;
        }
        else
        {
            if (last != 0)
                ans += " ";
            ans += word;
            last = 2;
        }
    }
}

Graph::Graph() : m_graph(0),
                 m_revgraph(0),
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

vector<int> Graph::findShortestBFS(int start)
{
    vector<int> dist(1e6, INT_MAX);
    vector<int> pred(1e6, -1);
    dist[start] = 0;
    queue<int> q;
    q.push(start);
    while (!q.empty())
    {
        int v = q.front();
        q.pop();
        for (auto [u, trash] : m_graph[v])
        {
            if (dist[u] > dist[v] + 1)
            {
                dist[u] = dist[v] + 1;
                pred[u] = v;
                q.push(u);
                if ((int)m_words[u].size() == 1 && !isalpha(m_words[u][0]) && !isdigit(m_words[u][0]) && dist[u] >= 2)
                {
                    vector<int> path;
                    int curv = v;
                    while (curv != -1)
                    {
                        path.push_back(curv);
                        curv = pred[curv];
                    }
                    reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }
}

int Graph::getStart(const vector<string> &correct)
{
    vector<int> endv(0);
    for (int i = 0; i < (int)m_graph.size(); i++)
    {
        if (m_words[i] == correct.back())
        {
            endv.push_back(i);
        }
    }

    queue<pair<int, int>> q;
    for (auto it : endv)
    {
        q.push({it, it});
    }

    auto last = correct.size() - 1;
    vector<int> starts_end;
    while (q.size())
    {
        auto cur = q.front();
        q.pop();
        if (m_words[cur.first] != correct[last])
        {
            --last;
        }
        if (last == 0 || q.size() == 1)
        {
            starts_end.push_back(cur.second);
        }
        else
        {
            auto need_word = m_words[last - 1];
            for (auto it : m_revgraph[cur.first])
            {
                if (m_words[it.first] == need_word)
                {
                    q.push({it.first, cur.second});
                }
            }
        }
    }

    return starts_end[Generator::getInstance().getSizeT(0, starts_end.size())];
}

void Graph::loadFromFile(filesystem::path &filename)
{
    ifstream file(filename);

    int V;
    file >> V;
    m_words.resize(V);
    m_graph.resize(V);
    m_revgraph.resize(V);
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
        m_revgraph[b].push_back({a, w});
    }
}

string Graph::answerTo(string &sentence)
{
    vector<string> good_sentence(correct(sentence));
    int start = getStart(good_sentence);
    vector<int> path = findShortestBFS(start);
    return pathToSentence(path);
}
