#include <cmath>
#include <cstdio>

double invcdf(double p)
{
    if (p < 0.5) {
        double LR = p / (1 - p);
        return 5.5310 * (std::pow(LR, 0.1193) - 1);
    }

    double LR = (1 - p) / p;
    return -5.5310 * (std::pow(LR, 0.1193) - 1);
}

int main()
{
    printf("%f", invcdf(0.1));
}
