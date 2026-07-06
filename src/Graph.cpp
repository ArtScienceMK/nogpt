#include "Graph.hpp"
#include <iostream>
#include <vector>

using namespace std;

Graph::Graph() 
{
}

vector<string> Graph::correct(const string & s) {
    vector<string> words;
    string cur = "";
    for (auto c : s) {
        if (!is_alpha(c) && !is_digit(c)) {
            words.push_back(cur);
            cur = "";
        } else {
            cur += c;
        }
    }
    if ((int)cur.size() > 0) {
        words.push_back(cur);
    }
    for (string words : words) {
        int curMatches = 0;
        int bestMatches = 0;
        string
    }
}