#include <bits/stdc++.h>
using namespace std;

int main() {
    std::vector v = {4, 3, 2, 1};
    // int sum = reduce(v.begin(), v.end());
    int sum = reduce(v.begin(), v.end(), 0, std::plus{});
    std::cout << sum << std::endl;

    return 0;
}