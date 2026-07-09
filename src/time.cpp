#include <iostream>
#include <chrono>
#include <fstream>
#include "Generator.hpp"
#include "Graph.hpp"

using clocks = std::chrono::high_resolution_clock;
using miliseconds = std::chrono::nanoseconds;

void time_test_ShortestBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\ShortestBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 0);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_ProbbestDijkstra(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\ProbbestDijkstra.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 1);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_KbestBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\KbestBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 2);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_KlenBFS(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\KlenBFS.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 3);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Krandom1000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Krandom1000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 4);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Krandom10000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Krandom10000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 5);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Krandom100000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Krandom100000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 6);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Krandom1000000(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Krandom1000000.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 7);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Kgreedy(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Kgreedy.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 8);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

void time_test_Genetic(int ops, int num_words) {
    Graph g;
    std::filesystem::path dataFile = ("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\data\\data");
    g.loadFromFile(dataFile);\
    std::ofstream out_file("C:\\Users\\Artem\\projects\\lshpi2026\\project\\nogpt\\time_data\\Genetic.csv");
    Statistic stat;
    for (int i = 0; i < ops; ++i) {
        std::string sentence;
        for (int j = 0; j < num_words; j++) {
            sentence += g.m_words[Generator::getInstance().getInt(0, (int)g.m_words.size()-1)];
        } 
        auto start = clocks::now();
        g.answerTo(sentence, stat, 9);
        auto elapsed = clocks::now() - start;
        long long milisec = std::chrono::duration_cast<miliseconds>(elapsed).count();
        out_file << milisec << std::endl;
    }
    out_file.close();
}

int main(int argc, const char * argv[]) {
    for (int num_words = 2; num_words <= 2; num_words++) {
        time_test_ShortestBFS(50, num_words);
        time_test_ProbbestDijkstra(10, num_words);
        time_test_KbestBFS(50, num_words);
        time_test_KlenBFS(50, num_words);
        time_test_Krandom1000(50, num_words);
        time_test_Krandom10000(50, num_words);
        time_test_Krandom100000(50, num_words);
        time_test_Krandom1000000(10, num_words);
        time_test_Kgreedy(10, num_words);
        time_test_Genetic(10, num_words);
    }
    return 0;
}
