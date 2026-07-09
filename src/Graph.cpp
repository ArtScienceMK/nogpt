#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <climits>
#include <string>
#include <cctype>
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

    return exp(-0.05 * abs(L - 7)) * P * exp(-0.5 * (U - 1));
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

void Graph::cropPath(vector<int> &path)
{
    vector<int> cur;
    for (int i = 0; i < (int)path.size(); i++)
    {
        cur.push_back(path[i]);
        if (m_words[path[i]] == "." && i != 0)
        {
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

bool isRussianAlphaOrDigitDop(const string &s, size_t i)
{
    unsigned char c = s[i];
    if (isalnum(c))
        return true;
    if (c >= 0x80)
        return true;
    if (c == '\'' || c == '-')
        return true;
    return false;
}

bool isNumberWithSeparators(const string &s)
{
    if (s.empty())
        return false;
    bool hasSeparator = false;
    bool hasDigit = false;
    for (char c : s)
    {
        if (isdigit((unsigned char)c))
        {
            hasDigit = true;
        }
        else if (c == '.' || c == ',')
        {
            hasSeparator = true;
        }
        else
        {
            return false;
        }
    }
    return hasDigit && hasSeparator;
}

bool isAbbreviation(const string &s)
{
    if (s.size() < 3 || s.find('.') == string::npos)
        return false;

    size_t start = 0;
    int parts = 0;
    for (size_t i = 0; i <= s.size(); i++)
    {
        if (i == s.size() || s[i] == '.')
        {
            if (i == start)
                return false;
            if (i - start > 3)
                return false;
            for (size_t j = start; j < i; j++)
            {
                if (!isalpha((unsigned char)s[j]) || (unsigned char)s[j] >= 0x80)
                {
                    return false;
                }
            }
            parts++;
            start = i + 1;
        }
    }
    return parts >= 2;
}

bool startsWithURL(const string &s, size_t pos)
{
    if (pos + 7 <= s.size() && s.substr(pos, 7) == "http://")
        return true;
    if (pos + 8 <= s.size() && s.substr(pos, 8) == "https://")
        return true;
    if (pos + 4 <= s.size() && s.substr(pos, 4) == "www.")
        return true;
    return false;
}

string extractURL(const string &s, size_t &pos)
{
    string url;
    while (pos < s.size() && !isspace((unsigned char)s[pos]) &&
           s[pos] != '"' && s[pos] != '\'' && s[pos] != ')' && s[pos] != ']')
    {
        url += s[pos];
        pos++;
    }
    while (!url.empty() && (url.back() == '.' || url.back() == ','))
    {
        if (pos >= s.size() || isspace((unsigned char)s[pos]))
        {
            url.pop_back();
        }
        else
        {
            break;
        }
    }
    return url;
}

vector<string> expandURL(const string &url)
{
    vector<string> parts;
    size_t i = 0;

    if (url.substr(0, 8) == "https://")
        i = 8;
    else if (url.substr(0, 7) == "http://")
        i = 7;
    else if (url.substr(0, 4) == "www.")
        i = 4;

    string current;
    for (; i < url.size(); i++)
    {
        char c = url[i];
        if (isalnum((unsigned char)c) || c >= 0x80 || c == '-' || c == '_')
        {
            current += c;
        }
        else
        {
            if (!current.empty())
            {
                parts.push_back(current);
                current.clear();
            }
            if (c == '.' || c == '/' || c == ':' || c == '?' || c == '&' || c == '=')
            {
                parts.push_back(string(1, c));
            }
        }
    }
    if (!current.empty())
        parts.push_back(current);

    return parts;
}

vector<string> expandNumber(const string &num)
{
    vector<string> digits;
    for (char c : num)
    {
        if (isdigit((unsigned char)c))
        {
            digits.push_back(string(1, c));
        }
    }
    return digits;
}

vector<string> expandAbbreviation(const string &abbr)
{
    vector<string> parts;
    for (char c : abbr)
    {
        parts.push_back(string(1, c));
    }
    return parts;
}

void addToken(const string &token, vector<string> &words)
{
    if (token.empty())
        return;

    if (isNumberWithSeparators(token))
    {
        if (Generator::getInstance().getInt(0, 1) == 0)
        {
            words.push_back(token);
        }
        else
        {
            auto digits = expandNumber(token);
            for (const auto &d : digits)
            {
                words.push_back(d);
            }
        }
    }
    else if (isAbbreviation(token))
    {
        if (Generator::getInstance().getInt(0, 1) == 0)
        {
            words.push_back(token);
        }
        else
        {
            auto parts = expandAbbreviation(token);
            for (const auto &p : parts)
            {
                words.push_back(p);
            }
        }
    }
    else
    {
        words.push_back(token);
    }
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
            {
                addToken(now, words);
                now = "";
            }
        }
        else if (startsWithURL(s, i))
        {
            if (now != "")
            {
                addToken(now, words);
                now = "";
            }
            string url = extractURL(s, i);

            if (Generator::getInstance().getInt(0, 1) == 0)
            {
                words.push_back(url);
            }
            else
            {
                auto parts = expandURL(url);
                for (const auto &p : parts)
                {
                    words.push_back(p);
                }
            }
            i--;
        }
        else if (!isRussianAlphaOrDigitDop(s, i))
        {
            if (now != "")
            {
                addToken(now, words);
                now = "";
            }
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
        addToken(now, words);
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
    vector<int> dist(m_graph.size(), INT_MAX);
    vector<int> pred(m_graph.size(), -1);
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
                if ((m_words[u] == "." || m_words[u] == "!" || m_words[u] == "?") && dist[u] >= 3)
                {
                    int curv = u;
                    while (curv != start)
                    {
                        path.push_back(curv);
                        curv = pred[curv];
                    }
                    path.pop_back();
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
    vector<float> dist(m_graph.size(), 0);
    vector<int> pred(m_graph.size(), -1);
    dist[start] = 0;
    set<pair<float, int>, greater<pair<float, int>>> q;
    q.insert({0, start});
    vector<int> path;
    int ops = 2e5;
    while (!q.empty())
    {
        ops--;
        if (ops == 0)
            break;
        int v = (*q.begin()).second;
        q.erase(q.begin());
        for (auto [u, prob] : m_graph[v])
        {
            if (dist[u] < dist[v] + prob)
            {
                q.erase({dist[u], u});
                dist[u] = dist[v] + prob;
                pred[u] = v;
                q.insert({dist[u], u});
            }
        }
    }
    int mxIdx = -1;
    for (int v = 0; v < (int)m_graph.size(); v++)
    {
        if (mxIdx == -1 || dist[v] > dist[mxIdx])
        {
            mxIdx = v;
        }
    }
    // cout << "MxIdx: " << mxIdx << "\n";
    int curv = mxIdx;
    while (curv != start && (int)path.size() <= 7)
    {
        path.push_back(curv);
        if ((int)path.size() > 2 && m_words[curv] == ".")
            break;
        curv = pred[curv];
    }
    path.pop_back();
    // cout << "Path has len: " << (int)path.size() << "\n";
    reverse(path.begin(), path.end());
    return path;
}

vector<int> Graph::findKbestBFS(int start, int k)
{
    vector<int> dist(m_graph.size(), INT_MAX);
    vector<int> pred(m_graph.size(), -1);
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
                    path.pop_back();
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
    vector<int> dist(m_graph.size(), INT_MAX);
    vector<int> pred(m_graph.size(), -1);
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
                    path.pop_back();
                    reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }
    while (k >= 3)
    {
        k--;
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
                path.pop_back();
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
        vector<int> cur(Generator::getInstance().getInt(3, 7));
        cur[0] = start;
        for (int j = 1; j < (int)cur.size(); j++)
        {
            cur[j] = Generator::getInstance().getInt(0, (int)m_graph.size() - 1);
        }
        res[i] = cur;
        if ((int)cur.size() < 2)
            throw "too small size";
    }
    double best_score = -2e9;
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
    // cout << "has path with len: " << (int)path.size() << "\n";
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

void Graph::generatePokolenie(int count, set<pair<double, vector<int>>, greater<>> &pokolenie, int size, int start)
{
    for (unsigned int _ = 0; _ < count; _++)
    {
        vector<int> path{start};

        for (size_t i = 0; i < size; i++)
        {
            path.push_back(Generator::getInstance().getInt(0, m_words.size() - 1));
        }

        double score = getPathScore(path);
        path.erase(path.begin());
        pokolenie.insert({score, path});
    }
}

const int FIRST_POKOLENIE = 300;
const int CHANCE_TO_MUTIR_CHILD = 15;
const int COUNT_POKOLENIY = 5000;

std::vector<int> Graph::findGenetic(int len, int start)
{
    std::set<std::pair<double, std::vector<int>>, greater<>> pokolenie;

    generatePokolenie(FIRST_POKOLENIE, pokolenie, len, start);

    std::set<std::pair<double, std::vector<int>>, greater<>> pokolenie_next;

    std::pair<double, std::vector<int>> best = *pokolenie.begin();

    unsigned int size_path = len, path13 = size_path / 3, path23 = path13 * 2;

    for (int _ = 0; _ < COUNT_POKOLENIY; _++)
    {
        // take +-2/3 best
        auto mid23 = pokolenie.begin();
        std::advance(mid23, pokolenie.size() / 3 * 2);

        for (auto it = pokolenie.begin(); it != mid23; it++)
        {
            auto first_parent = (*it).second;
            it++;
            auto second_parent = (*it).second;

            // get childs

            for (int i = 0; i < path13; i++)
            {
                first_parent[i] = second_parent[i];
                second_parent[i] = first_parent[i];
            }
            for (int i = path23; i < size_path; i++)
            {
                first_parent[i] = second_parent[i];
                second_parent[i] = first_parent[i];
            }

            vector<int> copy = {start};
            copy.insert(copy.end(), first_parent.begin(), first_parent.end());
            double score1 = getPathScore(copy);
            copy = {start};
            copy.insert(copy.end(), second_parent.begin(), second_parent.end());
            double score2 = getPathScore(copy);

            pokolenie_next.insert({score1, first_parent});
            pokolenie_next.insert({score2, second_parent});

            // mutacia
            if (Generator::getInstance().getInt(0, CHANCE_TO_MUTIR_CHILD - 1) == 0)
            {
                std::swap(first_parent[Generator::getInstance().getSizeT(0, size_path - 1)], first_parent[Generator::getInstance().getSizeT(0, size_path - 1)]);
                copy = {start};
                copy.insert(copy.end(), first_parent.begin(), first_parent.end());
                double score = getPathScore(copy);
                pokolenie_next.insert({score, first_parent});
            }
        }

        // +-random 1 / 3
        if (pokolenie_next.size() < pokolenie.size())
        {
            generatePokolenie(pokolenie.size() - pokolenie_next.size(), pokolenie_next, len, start);
        }

        pokolenie = pokolenie_next;
        if (best.first < pokolenie_next.begin()->first)
        {
            best = *pokolenie_next.begin();
        }
        pokolenie_next.clear();
    }

    return best.second;
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
    while (!q.empty() && last >= 0)
    {
        auto cur = q.front();
        q.pop();

        if (m_words[cur.first] != correct[last])
        {
            --last;
        }
        if (last == 0 || q.size() == 0)
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

    if (!file.is_open())
    {
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

string Graph::answerTo(string &sentence, Statistic &stat, int algf, bool markAlgos)
{
    int alg_size = stat.algsName.size();
    stat.result_alg.resize(alg_size);

    auto ToCorrectWords = toCorrectWords(sentence);
    vector<string> good_sentence = ToCorrectWords.first;
    vector<string> uncorrect = ToCorrectWords.second;

    for (size_t i = 0; i < uncorrect.size(); i++)
    {
        stat.uncorrect_words.push_back(uncorrect[i]);
    }

    int start = getStart(good_sentence);

    vector<int> path;
    int alg = algf;
    if (algf == -2 || algf == -1)
        alg = Generator::getInstance().getInt(0, alg_size - 1);

    if (algf == -1)
        cout << "🤖 (" << stat.algsName[alg] << ")\n";

    if (alg == 0)
        path = findShortestBFS(start);
    else if (alg == 1)
        path = findProbbestDijkstra(start);
    else if (alg == 2)
        path = findKbestBFS(start, Generator::getInstance().getInt(1, 50));
    else if (alg == 3)
        path = findKlenBFS(start, Generator::getInstance().getInt(2, 7));
    else if (alg == 4)
        path = findKrandom(start, 1000);
    else if (alg == 5)
        path = findKrandom(start, 10000);
    else if (alg == 6)
        path = findKrandom(start, 100000);
    else if (alg == 7)
        path = findKrandom(start, 1000000);
    else if (alg == 8)
        path = findKgreedy(start, Generator::getInstance().getInt(2, 15));
    else
        path = findGenetic(Generator::getInstance().getInt(2, 15), start);

    cropPath(path);
    int mark = 0;
    string smark = "";
    string answer = pathToSentence(path);
    std::cout << "🤖 > " << answer << std::endl;
    if (markAlgos)
    {
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
        catch (const std::exception &e)
        {
            std::cout << "Ошибка ввода: " << e.what() << std::endl;
        }
    }
    return answer;
}
