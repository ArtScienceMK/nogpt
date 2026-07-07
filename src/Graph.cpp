#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <climits>
#include <string>
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

void Graph::cropPath(vector<int> & path) {
    vector<int> cur;
    for (int i = 0; i < (int)path.size(); i++) {
        cur.push_back(path[i]);
        if (m_words[path[i]] == "." && i != 0) {
            break;
        }
    }
    path = cur;
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

pair<vector<string>, vector<string>> Graph::toCorrectWords(const string &s)
{
    vector<string> blocks(0);
    toWords(s, blocks);

    vector<string> correct_words;
    vector<string> uncorrect_words;

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
            uncorrect_words.push_back(block);
            correct_words.push_back(best);
        }
    }
    return {correct_words, uncorrect_words};
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
                if (m_words[u] == "." && dist[u] >= 3)
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

vector<int> Graph::findProbbestDijkstra(int start)
{
    vector<float> dist(1e6, 0);
    vector<int> pred(1e6, -1);
    dist[start] = 0;
    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>> q;
    q.push({0, start});
    vector<int> path;
    int ops = 6e5;
    while (!q.empty())
    {
        ops--;
        if (ops == 0)
            break;
        int v = q.top().second;
        q.pop();
        for (auto [u, prob] : m_graph[v])
        {
            if (dist[u] < dist[v] + prob)
            {
                dist[u] = dist[v] + prob;
                pred[u] = v;
                q.push({dist[u], u});
            }
        }
    }
    float mxIdx = start;
    for (int v = 0; v < (int)m_graph.size(); v++)
    {
        if (dist[v] != 2e9 && dist[v] > dist[mxIdx])
        {
            mxIdx = v;
        }
    }
    // cout << "MxIdx: " << mxIdx << "\n";
    int curv = mxIdx;
    while (curv != start && (int)path.size() <= 15)
    {
        path.push_back(curv);
        if (m_words[curv] == ".")
            break;
        curv = pred[curv];
    }
    // cout << "Path has len: " << (int)path.size() << "\n";
    reverse(path.begin(), path.end());
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
                if (m_words[u] == "." && dist[u] >= 3)
                {
                    left--;
                    if (left != 0)
                        continue;
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

vector<int> Graph::findKlenBFS(int start, int k)
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
                if (m_words[u] == "." && dist[u] >= k + 2)
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
    while (k >= 3)
    {
        for (int v = 0; v < (int)m_graph.size(); v++)
        {
            if (m_words[v] == "." && dist[v] >= k + 1)
            {
                int curv = v;
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
    return path;
}

vector<int> Graph::findKrandom(int start, int k)
{
    vector<vector<int>> res(k);
    for (int i = 0; i < k; i++)
    {
        vector<int> cur(Generator::getInstance().getInt(2, 15));
        cur[0] = start;
        for (int j = 1; j < (int)cur.size(); j++)
        {
            cur[j] = Generator::getInstance().getInt(0, (int)m_graph.size() - 1);
        }
        res[i] = cur;
    }
    double best_score = 0;
    vector<int> path;
    for (int i = 0; i < k; i++)
    {
        double cur_score = getPathScore(res[i]);
        if (cur_score > best_score)
        {
            best_score = cur_score;
            path = res[i];
        }
    }
    path.erase(path.begin());
    return path;
}

vector<int> Graph::findKgreedy(int start, int k)
{
    vector<int> path;
    path.push_back(start);
    while ((int)path.size() < k + 1)
    {
        float best_prob = 0;
        int best_u = -1;
        for (auto [u, prob] : m_graph[path.back()])
        {
            if (prob > best_prob)
            {
                best_prob = prob;
                best_u = u;
            }
        }
        path.push_back(best_u);
    }
    path.erase(path.begin());
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

void Graph::loadFromFile(std::filesystem::path &filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return;
    }

    int V;
    file.read((char *)&V, sizeof(V));
    
    m_words.resize(V);
    m_graph.resize(V);
    m_revgraph.resize(V);

    for (size_t i = 0; i < V; i++)
    {
        int length;
        file.read((char *)&length, sizeof(length));
        
        m_words[i].resize(length);
        file.read(&m_words[i][0], length);
    }

    int R;
    file.read((char *)&R, sizeof(R));

    for (size_t i = 0; i < R; i++)
    {
        int a, b;
        float w;
        
        file.read((char *)&a, sizeof(a));
        file.read((char *)&b, sizeof(b));
        file.read((char *)&w, sizeof(w));
        
        m_graph[a].push_back({b, w});
        m_revgraph[b].push_back({a, w});
    }

    file.close();
}

void Graph::answerTo(string &sentence, Statistic &stat)
{
    int alg_size = 9;
    stat.result_alg.resize(alg_size);
    stat.algsName = {"ShortestBFS", "ProbbestDijkstra", "KbestBFS", "KlenBFS", "Krandom1000", "Krandom10000", "Krandom100000", "Krandom1000000", "Kgreedy"};

    auto ToCorrectWords = toCorrectWords(sentence);
    vector<string> good_sentence = ToCorrectWords.first;
    vector<string> uncorrect = ToCorrectWords.second;

    for (size_t i = 0; i < uncorrect.size(); i++)
    {
        stat.uncorrect_words.push_back(uncorrect[i]);
    }

    int start = getStart(good_sentence);

    vector<int> path;
    int alg = Generator::getInstance().getInt(0, alg_size - 1);
    if (alg == 0)
        path = findShortestBFS(start);
    else if (alg == 1)
        path = findProbbestDijkstra(start);
    else if (alg == 2)
        path = findKbestBFS(start, Generator::getInstance().getInt(1, 50));
    else if (alg == 3)
        path = findKlenBFS(start, Generator::getInstance().getInt(2, 30));
    else if (alg == 4)
        path = findKrandom(start, 1000);
    else if (alg == 5)
        path = findKrandom(start, 10000);
    else if (alg == 6)
        path = findKrandom(start, 100000);
    else if (alg == 7)
        path = findKrandom(start, 1000000);
    else
        path = findKgreedy(start, Generator::getInstance().getInt(2, 30));
    cropPath(path);
    int mark = 0;
    string smark = "";
    std::cout << "🤖 > " << pathToSentence(path) << std::endl;
    cout << "⭐ (Оценка алгоритма х/10) > ";
    getline(cin, smark);
    try
    {
        mark = stoi(smark);
        if (mark >= 0 && mark <= 10)
        {
            ++stat.result_alg[alg].first;
            stat.result_alg[alg].second += mark;
        }
    }
    catch (const std::string &error_message)
    {
        std::cout << "Ошибка: " << error_message << std::endl;
    }
}
