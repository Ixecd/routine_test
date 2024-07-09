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

5. 关于STL中的容器
    vector:在vector中迭代器直接就是T*,类中成员只有T* m_datas, size_t m_size, size_t capacity.扩容的关键是std::max(n, (1.5)or(2.0)*m_capacity), n是当前想要的大小.在vector中分配器使用
        std::allocator<T>就行但要保证这个分配器是有状态的.如果直接使用new(malloc)有小概率得到的内存是不对其的.并且使用new得到的内存会被赋值一遍,使用分配器得到的内存是干净的,之后再到构造函数中使用
        std::constrcut_at来给成员变量赋值.之后要注意的就是各种构造函数/重载函数/操作函数,要注意各种const的使用(对数据的操作,迭代器的操作都要有两个版本).作为函数参数或者返回值时, T const & equal
        to T,推荐使用前者,会少一次拷贝构造函数,拷贝构造函数的类型也为&最好是const &,因为在C++的实现中,const &可以传递右值.还要注意insert中元素的搬移方向.使用C++20中template<concepts Input>
        来约束函数类型.
    list:再list中迭代器需要单独编写,并且有两个版本itearto和const_iterator,这里的const是常量指针,表示的是指针指向的内容不能改,仍然可以进行operat++/--操作.list中目前的成员只有struct ListNode, iterator,
        const_iterator, 和 ListNode dummy;list是双向循环链表,dummy表示end,之后就是各种构造函数/重载函数(尤其是对迭代器++/++(int)/--/--(int)/*/==/!=),还有各种对迭代器的操作(const / non_const...)

6. 关于iterator
    input_iterator 支持 *it it++ ++it it!=it it==it
    output_iterator 支持 *it=val it++ ++it it!=it it==it
    forward_iterator 支持 *it *it=val it++ ++it it!=it it==it
    bidirectional 支持 *it *it=val it++ ++it --it it-- it!=it it==it
    random_access_iterator 支持 *it *it=val it[n] it[n]=val it++ ++it it+=n --it it-- it-=n it+n it-n it!=it it==it
    下面的是上面的超集
    一般list、set、map的iterator类型为bidirectional,如果想要和random_access_iterator效果一样的话需要使用std::advance(it, n) n > 0向后移动 n < 0 向前移动.

7. 关于lambda
    在类模板中可以使用lambda作为类型,用户调用的时候直接在参数中写自己的lambda函数,在函数体中直接就会调用用户写的这个函数,小技巧.也可以直接使用template<std::invocable<T const &>Visitor> 更方便(俩版本).

8. 关于noexcept
    能加的都加上,编译器很多时候不对代码进行优化的原因就是害怕程序抛出异常,加上之后允许编译器尽情优化.