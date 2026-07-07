#include "Graph.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

using namespace std;

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#elif __linux__
#include <unistd.h>
#endif

filesystem::path getAbsoluteFromExe(const filesystem::path &relativePath)
{
    filesystem::path exePath;

#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    exePath = filesystem::path(buffer);

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
    filesystem::path exeDir = exePath.parent_path();
    filesystem::path targetPath = filesystem::weakly_canonical(exeDir / relativePath);
    filesystem::create_directories(targetPath.parent_path());

    return targetPath;
}

int main()
{
    filesystem::path dataFile = getAbsoluteFromExe("../data/data.txt");

    Graph g;
    g.loadFromFile(dataFile);

    string player = "";
    cout << "Write 'q' to exit" << endl;

    while (true)
    {
        getline(cin, player);
        if (player == "q") break;
        cout << g.answerTo(player) << endl;
    }
}