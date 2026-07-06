#include "Generator.hpp"

Generator &Generator::getInstance()
{
    static Generator instance;
    return instance;
}

Generator::Generator()
{
    std::random_device rd;
    rng_.seed(rd());
}

int Generator::getInt(int min, int max)
{
    if (min > max)
        return min;
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

long long Generator::getLongLong(long long min, long long max)
{
    if (min > max)
        return min;
    std::uniform_int_distribution<long long> dist(min, max);
    return dist(rng_);
}

double Generator::getProbability()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng_);
}

size_t Generator::getSizeT(size_t min, size_t max)
{
    if (min > max)
    {
        return min;
    }

    std::uniform_int_distribution<size_t> dist(min, max);
    return dist(rng_);
}
