#include <iterator> // Include this header for std::random_access_iterator
#include <vector>
#include <concepts>
#include <iostream>

// F5 走的是task.json
template <std::random_access_iterator InputIt>
void print_elements(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';
}

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    print_elements(vec.begin(), vec.end());
    return 0;
}
