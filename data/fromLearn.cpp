#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cmath>
#include <cctype>

struct Edge
{
    int a, b;
    float w;
};

bool isNumberWithSeparators(const std::string &s)
{
    if (s.empty())
        return false;
    bool hasSeparator = false;
    bool hasDigit = false;
    for (char c : s)
    {
        if (std::isdigit((unsigned char)c))
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

bool isAbbreviation(const std::string &s)
{
    if (s.size() < 3 || s.find('.') == std::string::npos)
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
                if (!std::isalpha((unsigned char)s[j]) || (unsigned char)s[j] >= 0x80)
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

std::vector<std::string> expandURL(const std::string &url)
{
    std::vector<std::string> parts;
    size_t i = 0;

    if (url.substr(0, 8) == "https://")
        i = 8;
    else if (url.substr(0, 7) == "http://")
        i = 7;
    else if (url.substr(0, 4) == "www.")
        i = 4;

    std::string current;
    for (; i < url.size(); i++)
    {
        char c = url[i];
        if (std::isalnum((unsigned char)c) || c >= 0x80 || c == '-' || c == '_')
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
                parts.push_back(std::string(1, c));
            }
        }
    }
    if (!current.empty())
        parts.push_back(current);

    return parts;
}

std::vector<std::string> expandAbbreviation(const std::string &abbr)
{
    std::vector<std::string> parts;
    for (char c : abbr)
    {
        parts.push_back(std::string(1, c));
    }
    return parts;
}

std::vector<std::string> tokenize(const std::string &text)
{
    std::vector<std::string> tokens;
    size_t i = 0;

    while (i < text.size())
    {
        unsigned char c = text[i];

        if (std::isspace(c))
        {
            i++;
            continue;
        }

        if (text.substr(i, 7) == "http://" || text.substr(i, 8) == "https://" || text.substr(i, 4) == "www.")
        {
            std::string url;
            while (i < text.size() && !std::isspace((unsigned char)text[i]) && text[i] != '"' && text[i] != '\'' && text[i] != ')' && text[i] != ']')
            {
                url += text[i];
                i++;
            }
            while (!url.empty() && (url.back() == '.' || url.back() == ','))
            {
                if (i >= text.size() || std::isspace((unsigned char)text[i]))
                {
                    url.pop_back();
                }
                else
                {
                    break;
                }
            }
            tokens.push_back(url);
            continue;
        }

        std::string current;
        while (i < text.size())
        {
            c = text[i];

            if (std::isspace(c))
                break;

            if (std::isalnum(c) || c >= 0x80 || c == '\'' || c == '-')
            {
                current += (char)c;
                i++;
            }
            else if ((c == '.' || c == ',') && i + 1 < text.size() && (std::isalnum((unsigned char)text[i + 1]) || (unsigned char)text[i + 1] >= 0x80))
            {
                current += (char)c;
                i++;
            }
            else
            {
                break;
            }
        }

        if (!current.empty())
        {
            tokens.push_back(current);
        }

        if (i < text.size() && !std::isspace((unsigned char)text[i]))
        {
            tokens.push_back(std::string(1, text[i]));
            i++;
        }
    }

    return tokens;
}

int main()
{
    std::ifstream infile("./learn.txt");
    if (!infile.is_open())
    {
        std::cerr << "Не удалось открыть ./learn.txt" << std::endl;
        return 1;
    }
    std::string text((std::istreambuf_iterator<char>(infile)),
                     std::istreambuf_iterator<char>());
    infile.close();

    if (text.empty())
    {
        std::cerr << "Файл пустой" << std::endl;
        return 1;
    }

    std::vector<std::string> tokens = tokenize(text);
    std::cout << "Токенов (до расширения): " << tokens.size() << std::endl;

    struct ExpandInfo
    {
        std::string unified;
        int prevPos;
        int nextPos;
    };
    std::vector<ExpandInfo> expandInfos;
    std::vector<std::string> expanded;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        const std::string &token = tokens[i];
        int prevPos = (int)expanded.size() - 1;

        bool isExpanded = false;

        if (isNumberWithSeparators(token))
        {
            for (char c : token)
            {
                if (std::isdigit((unsigned char)c))
                {
                    expanded.push_back(std::string(1, c));
                }
            }
            isExpanded = true;
        }
        else if (token.size() >= 7 && (token.substr(0, 7) == "http://" || token.substr(0, 8) == "https://") ||
                 (token.size() >= 4 && token.substr(0, 4) == "www."))
        {
            auto parts = expandURL(token);
            for (const auto &p : parts)
            {
                expanded.push_back(p);
            }
            isExpanded = true;
        }
        else if (isAbbreviation(token))
        {
            auto parts = expandAbbreviation(token);
            for (const auto &p : parts)
            {
                expanded.push_back(p);
            }
            isExpanded = true;
        }
        else
        {
            expanded.push_back(token);
        }

        int nextPos = (int)expanded.size();

        if (isExpanded)
        {
            expandInfos.push_back({token, prevPos, nextPos});
        }
    }

    int N = expanded.size();
    std::cout << "Токенов (после расширения): " << N << std::endl;

    std::unordered_map<std::string, int> wordCount;
    for (const auto &t : expanded)
        wordCount[t]++;

    std::cout << "Уникальных токенов: " << wordCount.size() << std::endl;

    std::vector<std::string> vertexString;
    std::unordered_map<std::string, std::vector<int>> wordToVertices;
    std::vector<int> vertexUses;

    for (auto &[word, count] : wordCount)
    {
        int K = std::max(1, (int)std::ceil(std::sqrt((double)count)));
        auto &verts = wordToVertices[word];
        for (int k = 0; k < K; k++)
        {
            verts.push_back((int)vertexString.size());
            vertexString.push_back(word);
            vertexUses.push_back(0);
        }
    }

    std::cout << "Вершин (с копиями, до чисел): " << vertexString.size() << std::endl;

    std::map<std::pair<int, int>, int> edgeCount;
    std::vector<int> positionVertex(N, -1);

    int prev_v = -1;
    for (int i = 0; i < N; i++)
    {
        const std::string &word = expanded[i];
        const auto &candidates = wordToVertices[word];

        int chosen = -1;

        if (prev_v != -1 && vertexString[prev_v] == word)
        {
            chosen = prev_v;
        }

        if (chosen == -1 && prev_v != -1)
        {
            int best = -1;
            int bestCnt = 0;
            for (int v : candidates)
            {
                auto it = edgeCount.find({prev_v, v});
                if (it != edgeCount.end() && it->second > bestCnt)
                {
                    bestCnt = it->second;
                    best = v;
                }
            }
            if (best != -1)
                chosen = best;
        }

        if (chosen == -1)
        {
            chosen = candidates[0];
            for (int v : candidates)
            {
                if (vertexUses[v] < vertexUses[chosen])
                {
                    chosen = v;
                }
            }
        }

        vertexUses[chosen]++;
        positionVertex[i] = chosen;
        if (prev_v != -1)
        {
            edgeCount[{prev_v, chosen}]++;
        }
        prev_v = chosen;
    }

    for (const auto &ei : expandInfos)
    {
        int newVertex = (int)vertexString.size();
        vertexString.push_back(ei.unified);
        vertexUses.push_back(0);
        wordToVertices[ei.unified].push_back(newVertex);

        if (ei.prevPos >= 0 && positionVertex[ei.prevPos] != -1)
        {
            int pv = positionVertex[ei.prevPos];
            edgeCount[{pv, newVertex}]++;
        }

        if (ei.nextPos >= 0 && ei.nextPos < N && positionVertex[ei.nextPos] != -1)
        {
            int nv = positionVertex[ei.nextPos];
            edgeCount[{newVertex, nv}]++;
        }
    }

    int V = (int)vertexString.size();
    std::cout << "Вершин (с копиями + единые токены): " << V << std::endl;

    std::vector<int> totalOut(V, 0);
    for (auto &[uv, cnt] : edgeCount)
    {
        totalOut[uv.first] += cnt;
    }

    std::vector<Edge> edges;
    for (auto &[uv, cnt] : edgeCount)
    {
        int u = uv.first;
        int v = uv.second;
        float w = (totalOut[u] > 0) ? (float)cnt / (float)totalOut[u] : 0.0f;
        edges.push_back({u, v, w});
    }

    int R = (int)edges.size();

    std::ofstream outfile("./data", std::ios::binary);
    if (!outfile.is_open())
    {
        std::cerr << "Не удалось создать ./data" << std::endl;
        return 1;
    }

    outfile.write(reinterpret_cast<const char *>(&V), sizeof(V));

    for (int i = 0; i < V; i++)
    {
        int len = (int)vertexString[i].size();
        outfile.write(reinterpret_cast<const char *>(&len), sizeof(len));
        outfile.write(vertexString[i].c_str(), len);
    }

    outfile.write(reinterpret_cast<const char *>(&R), sizeof(R));

    for (const auto &e : edges)
    {
        outfile.write(reinterpret_cast<const char *>(&e.a), sizeof(e.a));
        outfile.write(reinterpret_cast<const char *>(&e.b), sizeof(e.b));
        outfile.write(reinterpret_cast<const char *>(&e.w), sizeof(e.w));
    }

    outfile.close();

    std::cout << "Граф: " << V << " вершин, " << R << " рёбер" << std::endl;
    std::cout << "Записано в ./data" << std::endl;

    return 0;
}
