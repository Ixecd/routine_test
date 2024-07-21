// 检测未定义行为
#define _GLIBCXX_DEBUG
#include <bits/stdc++.h>
#include "qc.hpp"
using namespace std;

// 定义一个变量,不初始化
// int i; --> gcc 大概率 0, msvc 大概率 0xcccccccc / 0xcdcdcdcd / 上一次填写过的值

/* 使用constexpr来检测是否发生未定义行为 */
/* 引入未定义行为是为了方便编译器优化 */
int main() {
    std::unique_ptr<int> p = nullptr;
    // 不会出现未定义行为,封装好了
    if (p.get() == nullptr) std::cout << p.get() << std::endl;
    // 这里发生了未定义行为,但不会报错
    // std::cout << &*p << std::endl;
    PRINT(&*p);

    return 0;
}