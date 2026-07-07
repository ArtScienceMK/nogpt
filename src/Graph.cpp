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

bool isRussianAlphaOrDigit(const string &s, size_t idx)
{
    if (idx >= s.length())
        return false;
    unsigned char ch = s[idx];
    if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
    {
        return true;
    }
    if (ch == 0xD0 || ch == 0xD1)
    {
        return true;
    }
    if (idx > 0)
    {
        unsigned char prev_ch = s[idx - 1];
        if (prev_ch == 0xD0 || prev_ch == 0xD1)
        {
            return true;
        }
    }
    return false;
}

string Graph::pathToSentence(vector<int> &path) const
{
    int last = 0;
    string ans = "";
    for (auto it : path)
    {
        auto word = m_words[it];
        if (word.empty())
            continue;

        if (isRussianAlphaOrDigit(word, 0) && !isdigit((unsigned char)word[0]))
        {
            ans += " " + word;
            last = 0;
        }
        else if (isdigit((unsigned char)word[0]))
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

    return ans;
}

Graph::Graph() : m_graph(0),
                 m_revgraph(0),
                 m_words(0)
{
}

void toWords(const string &s, vector<string> &words)
{
    string now = "";

    for (size_t i = 0; i < s.length(); ++i)
    {
        char ch = s[i];

        if (ch == ' ')
        {
            if (now != "")
                words.push_back(now);
            now = "";
        }
        else if (!isRussianAlphaOrDigit(s, i))
        {
            if (now != "")
                words.push_back(now);
            now = ch;
            words.push_back(now);
            now = "";
        }
        else
        {
            now += ch;
        }
    }
    if (now.size() > 0)
    {
        words.push_back(now);
    }
}

vector<string> Graph::toCorrectWords(const string &s)
{
    vector<string> blocks(0);
    toWords(s, blocks);

    vector<string> correct_words;

    for (string block : blocks)
    {
        int curMatches = 0;
        int bestMatches = INT_MIN;
        string best = "ERROR";
        bool stopped = false;
        for (string word : m_words)
        {
            curMatches = 0;
            for (int i = 0; i < (int)min(word.size(), block.size()); i++)
            {
                if (block[i] == word[i])
                {
                    curMatches++;
                }
            }
            curMatches -= abs((int)word.size() - (int)block.size());
            if (curMatches == (int)block.size())
            {
                stopped = true;
                correct_words.push_back(word);
                break;
            }
            if (curMatches > bestMatches)
            {
                bestMatches = curMatches;
                best = word;
            }
        }
        if (!stopped)
        {
            correct_words.push_back(best);
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
    vector<int> path;

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
                if (m_words[u] == "." && dist[u] >= 2)
                {
                    int curv = u;
                    while (curv != start)
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

    return path;
}

vector<int> Graph::findKbestBFS(int start, int k)
{
    vector<int> dist(1e6, INT_MAX);
    vector<int> pred(1e6, -1);
    dist[start] = 0;
    queue<int> q;
    q.push(start);
    vector<int> path;
    int left = k;
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
                if (m_words[u] == "." && dist[u] >= 2)
                {
                    left--;
                    if (left != 0) continue;
                    int curv = u;
                    while (curv != start)
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

    return path;
}

int Graph::getStart(const vector<string> &correct)
{
    vector<int> endv(0);
    for (int i = 0; i < m_graph.size(); i++)
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
    vector<int> starts_end(0);
    while (q.size())
    {
        auto cur = q.front();
        q.pop();

        if (last > 0 && m_words[cur.first] != correct[last])
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

    return starts_end[Generator::getInstance().getSizeT(0, starts_end.size() - 1)];
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
    vector<string> good_sentence = toCorrectWords(sentence);
    int start = getStart(good_sentence);
    auto path = findKbestBFS(start, Generator::getInstance().getInt(1, 50));
    return pathToSentence(path);
}
