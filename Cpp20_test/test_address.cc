#include <bits/stdc++.h>

using namespace std;

#include <iostream>
#include <vector>
#include <memory>

namespace qc {
    using vector = std::vector<int, std::allocator<int>>;
}

struct S {
    qc::vector v;
    std::allocator<int> a;

    S() : v(a) {} // 用同一个分配器初始化vector
};

int main() {
    S s;
    s.v.push_back(1);
    s.v.push_back(2);

    // 打印分配器的地址
    std::cout << "Address of allocator in struct S: " << &s.a << std::endl;
   // std::cout << "Address of allocator in vector: " << &(s.v.get_allocator()) << std::endl;

    return 0;
}
