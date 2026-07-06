#include <iostream>
#include <vector>

using namespace std;

class Graph{
private:
    vector<string> allWords_;
    vector<vector<pair<int, float>>> graph_;
Graph();
string findCurrentWord(const string & s);
};