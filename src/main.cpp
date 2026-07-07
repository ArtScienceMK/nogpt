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
    vector<char> buffer(size);
    if (_NSGetExecutablePath(buffer.data(), &size) == 0)
    {
        exePath = filesystem::path(buffer.data());
    }

#elif __linux__
    exePath = filesystem::read_symlink("/proc/self/exe");
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
    std::filesystem::path dataFile = getAbsoluteFromExe("../data/data.txt");

    Graph g;
    g.loadFromFile(dataFile);

    std::string player = "";
    std::cout << "Write 'q' to exit" << std::endl;
    

    while (true)
    {
        std::getline(std::cin, player);
        if (player == "q") break;
        std::cout << g.answerTo(player) << std::endl;
    }
}