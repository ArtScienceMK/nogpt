#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include "Generator.hpp"
#include "Graph.hpp"

#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

using vec_int = std::vector<int>;

size_t get_current_memory_kb() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
}

size_t get_peak_memory_kb() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize / 1024;
    }
    return 0;
}

void test_mem_ShortestBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\ShortestBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 0);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_ProbbestDijkstra(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\ProbbestDijkstra.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 1);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_KbestBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\KbestBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 2);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_KlenBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\KlenBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 3);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Krandom1000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Krandom1000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 4);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Krandom10000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Krandom10000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 5);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Krandom100000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Krandom100000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 6);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Krandom1000000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Krandom1000000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 7);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Kgreedy(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Kgreedy.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 8);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

void test_mem_Genetic(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\mem_data\\Genetic.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        int before = get_current_memory_kb();
        g.answerTo(sentence, stat, 9);
        int after = get_peak_memory_kb();
        out_file << before << " ; " << after << " ; " << after - before << std::endl;
    }
    out_file.close();
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    for (int num_words = 3; num_words <= 3; num_words++) {
        test_mem_ShortestBFS(50, num_words);
        test_mem_ProbbestDijkstra(10, num_words);
        test_mem_KbestBFS(50, num_words);
        test_mem_KlenBFS(50, num_words);
        test_mem_Krandom1000(50, num_words);
        test_mem_Krandom10000(50, num_words);
        test_mem_Krandom100000(50, num_words);
        test_mem_Krandom1000000(10, num_words);
        test_mem_Kgreedy(10, num_words);
        test_mem_Genetic(10, num_words);
    }
    return 0;
}
