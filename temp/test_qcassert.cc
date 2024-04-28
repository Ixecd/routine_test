#include <iostream>

#define MY_ASSERT(condition)                                          \
    do {                                                              \
        if (!(condition)) {                                           \
            std::cerr << "Assertion failed: " << #condition << " at " \
                      << __FILE__ << ":" << __LINE__ << std::endl;    \
            std::abort();                                             \
        }                                                             \
    } while (0)

int main() {
    int x = 5;
    MY_ASSERT(x == 10);  // 测试断言
    return 0;
}