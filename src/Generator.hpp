#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <random>

class Generator {
public:
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    static Generator& getInstance();

    int getInt(int min, int max);
    long long getLongLong(long long min, long long max);
    double getProbability();
    size_t getSizeT(size_t min, size_t max);

private:
    Generator();

    std::mt19937_64 rng_;
};

#endif // GENERATOR_HPP