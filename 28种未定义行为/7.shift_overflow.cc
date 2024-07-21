#define _GLIBCXX_DEBUG
#include <iostream>
#include <memory>

#include "qc.hpp"

//[[gnu::noinline]] 告诉gnu不要内联
[[gnu::noinline]] constexpr int shift_overflow_int() { return 1 << 32; }
[[gnu::noinline]] constexpr unsigned int shift_overflow_uint() { return 1 << 32; }

int main() {
    // 加constexpr会报错(编译器来判断),不加的话会执行返回0,因为编译器不知道用户是否是故意的,int x = shift_overflow_int(); 用户就要0 所以编译器不会报错,但会警告.
    constexpr int x = shift_overflow_int();
    PRINT(x);
    constexpr int y = shift_overflow_uint();
    PRINT(y);
    return 0;
}