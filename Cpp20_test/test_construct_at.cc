#include <bits/stdc++.h>
using namespace std;

struct Foo {
    int val;
    int* ptr;
    Foo() {
        ptr = new int{1};
        cout << "Foo() construct..." << endl;
    }
    ~Foo() {
        delete ptr;
        cout << "~Foo() destruct..." << endl;
    }
};

struct Goo {
    int val;
    Goo() { cout << "Goo() construct..." << endl; }
    ~Goo() { cout << "~Goo() destruct..." << endl; }
};

// 构造初始化
// std::construct_at(&addr,
// data)在指定的内存地址上构造对象。它是为了提供一种类型安全的方式来在未初始化的内存上构造对象，特别是在需要显式控制对象的构造和析构时
// 和 placement new 非常类似
int main() {
    allocator<Foo> alloc;
    Foo* data = alloc.allocate(1);
    Foo fo;
    construct_at(data, fo);

    // construct_at 是移动拷贝
    if (fo.ptr == data->ptr) cout << "move" << endl;

    cout << "before destroy" << endl;
    destroy_at(data);
    cout << "after destroy" << endl;
    alloc.deallocate(data, 1);
    cout << "after deallocate" << endl;
    return 0;
}