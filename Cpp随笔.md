# routine_test
1. Cpp标准库中begin和end一般用于成员函数,first和last用于函数参数

2. 关于 int 和 size_t
    在比较中如果同时出现这两个,编译器默认会将int转换为size_t,这在判断 >=0 ? --i 非常危险!

3. 关于VsCode中 F5
    F5 对应 tasks.json 和 c_cpp_properties.json 和 launch.json, 三个文件里编译器的版本都要正确F5才能正确RUN, 前两个负责编译,最后一个负责运行

4. 关于模板
    除了一般常见的模板C++20提供了#include<iterator>, template <std::random_access_iterator InputIt> -> 约束类型为随机访问迭代器(约束机制) -> 使用概念(concepts)约束类型参数
    std::random_access_iterator是C++20引入的一个概念(concept)
    一下是一些常见的概念:
        std::integral:用于整数类型。
        std::floating_point:用于浮点类型。
        std::signed_integral:用于带符号整数类型。
        std::unsigned_integral:用于无符号整数类型。
        std::input_iterator:用于输入迭代器。
        std::output_iterator:用于输出迭代器。
        std::forward_iterator:用于前向迭代器。
        std::bidirectional_iterator:用于双向迭代器。
        std::random_access_iterator:用于随机访问迭代器。
    模板分为类型模板和非类型模板
    template <class T> 和 template <size_t N> 后者用于传递常量值