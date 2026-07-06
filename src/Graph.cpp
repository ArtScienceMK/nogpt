#include "Graph.hpp"
#include <iostream>
#include <vector>

using namespace std;

Graph::Graph() : m_graph(0),
                 m_words(0){}

vector<string> Graph::correct(const string & s) {
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

void Graph::loadFromFile(string &filename)
{
    ifstream file(filename);

    int V;
    file >> V;
}