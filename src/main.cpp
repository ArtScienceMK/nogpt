#include "Graph.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#elif __linux__
#include <unistd.h>
#endif

std::filesystem::path getAbsoluteFromExe(const std::filesystem::path &relativePath)
{
    std::filesystem::path exePath;

#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    exePath = std::filesystem::path(buffer);

#elif __APPLE__
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) == 0)
    {
        exePath = std::filesystem::path(buffer.data());
    }

#elif __linux__
    exePath = std::filesystem::read_symlink("/proc/self/exe");
#endif
    std::filesystem::path exeDir = exePath.parent_path();
    std::filesystem::path targetPath = std::filesystem::weakly_canonical(exeDir / relativePath);
    std::filesystem::create_directories(targetPath.parent_path());

    return targetPath;
}

int main()
{
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    std::filesystem::path dataFile = getAbsoluteFromExe("../data/data");

    std::cout << "\n======== NoGpt ========\n"
              << std::endl;
    std::cout << "Loading...\n\n";

    Graph g;
    g.loadFromFile(dataFile);

    std::string player = "";
    std::cout << "Команды:\n"
              << " q - выйти и получить статистику\n";
    std::cout << "\n======== NoGpt ========"
              << std::endl;

    Statistic stat;

    while (true)
    {
        std::cout << "\n💁 > ";
        std::getline(std::cin, player);
        if (player == "q")
            break;
        g.answerTo(player, stat);
    }

    std::ofstream file(getAbsoluteFromExe("./stat"));

    std::cout << "\n=== NoGpt Statistic ===\n"
              << std::endl;

    size_t maxSize = 0;
    for (size_t i = 0; i < stat.algsName.size(); i++)
    {
        maxSize = std::max(stat.algsName[i].size(), maxSize);
    }

    for (size_t i = 0; i < maxSize; i++)
    {
        std::cout << " ";
    }
    std::cout << "  Точность   Количество   Сумма оценок\n";

    for (size_t i = 0; i < stat.algsName.size(); i++)
    {
        std::cout << stat.algsName[i];
        for (size_t _ = 0; _ < maxSize - stat.algsName[i].size(); _++)
        {
            std::cout << " ";
        }
        std::cout << ": " << std::fixed << std::setprecision(2) << (float)stat.result_alg[i].second / (float)stat.result_alg[i].first;
        std::cout << "       " << stat.result_alg[i].first << "            " << stat.result_alg[i].second << std::endl;
    }

    std::cout << "\n======== NoGpt ========\n"
              << std::endl;

    std::cout << "\n-----------------------\n"
              << std::endl;
    for (auto &it : stat.uncorrect_words)
    {
        std::cout << it << ", ";
    }

    file << "\n=== NoGpt Statistic ===\n"
         << std::endl;
    for (size_t i = 0; i < maxSize; i++)
    {
        file << " ";
    }
    file << "  Точность   Количество   Сумма оценок\n";
    for (size_t i = 0; i < stat.algsName.size(); i++)
    {
        file << stat.algsName[i];
        for (size_t _ = 0; _ < maxSize - stat.algsName[i].size(); _++)
        {
            file << " ";
        }
        file << ": " << std::fixed << std::setprecision(2) << (float)stat.result_alg[i].second / (float)stat.result_alg[i].first;
        file << "       " << stat.result_alg[i].first << "            " << stat.result_alg[i].second << std::endl;
    }
    file << "\n======== NoGpt ========\n"
              << std::endl;
    file << "\n-----------------------\n"
              << std::endl;
    for (auto &it : stat.uncorrect_words)
    {
        file << it << ", ";
    }
}