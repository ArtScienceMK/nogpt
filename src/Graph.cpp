#include "Graph.hpp"
#include <iostream>
#include <vector>

Graph::Graph() : m_graph(0),
                 m_words(0)
{
}

string Graph::findCurrentWord(const string &s)
{
    int need = (int)s.size();
    string cur = "";
    for (char c : s)
    {
        if (!is_alpha(c) && !is_digit(c))
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
        for (string word : allWords_)
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

void Graph::loadFromFile(string &filename)
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