/**
 * @file qc_vector.cc
 * @author qc
 * @brief 扩充基本的vector类
 * @details 添加initializer_list, random_access_iterator, Vscode中F5走的是task.json 和 c_cpp_pro 两个里面都要设置相应的标准
 * @version 0.6
 * @date 2024-07-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace qc {

template <class T>
class vector {
public:
    typedef T* iterator;

public:
    // 杜绝隐式转换,避免用户手滑写错的情况
    // explicit vector(size_t n) {
    //     m_data = new T[n]{};
    //     memset(m_data, 0, n * sizeof(T));
    //     m_size = n;
    //     m_capacity = n;
    // }

    explicit vector(size_t n, T val = 0) {
        // m_data = new T[n]{val}; 只会初始化第一个为val
        m_data = new T[n];
        m_size = m_capacity = n;
        for (size_t i = 0; i < m_size; ++i) m_data[i] = val;
    }

    vector(std::initializer_list<T> ilist)
        : vector(ilist.begin(), ilist.end()) {}

    // 这里允许隐式转换,一般类型直接推导即可
    // c20标准 用来约束模板InputIt必须是 random_access_iterator类型
    template <std::random_access_iterator InputIt>
    vector(InputIt first, InputIt last) {
        size_t n = last - first;
        m_data = new T[n];
        for (size_t i = 0; i < n; ++i) m_data[i] = *first++;
        m_capacity = m_size = n;
    }

    // 拷贝构造函数, 深拷贝
    vector(const vector& v) {
        m_capacity = m_size = v.m_size;
        if (m_size) {
            m_data = new T[m_size]{};
            memcpy(m_data, v.m_data, sizeof(T) * m_size);
        } else {
            m_data = nullptr;
        }
    }

    vector() noexcept {
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    // 移动构造函数 把v的变成自己的
    // 不需要深拷贝
    // 移动构造一般都用noexcept修饰,不会申请新的内存
    vector(vector&& v) noexcept {
        m_data = v.m_data;
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        v.m_data = nullptr;
        v.m_size = 0;
        v.m_capacity = 0;
    }

    // 移动赋值函数
    vector& operator=(vector&& v) noexcept {
        // clear();
        if (m_data) delete[] m_data;
        m_data = v.m_data;
        m_size = v.m_size;
        m_capacity = v.m_capacity;
        v.m_data = nullptr;
        v.m_size = 0;
        v.m_capacity = 0;
        return *this;
    }

    // 拷贝赋值函数
    vector& operator=(vector const& v) {
        // 判断是否自我赋值
        if (v == *this) return v;
        // clear();
        reserve(v.m_size);
        m_capacity = m_size = v.m_size;
        if (m_size) {
            // m_data = new T[m_size]{};
            memcpy(m_data, v.m_data, sizeof(T) * m_size);
        }
        return *this;
    }

    // operator = 中无法实现多个参数
    // assign相当于祛魅版
    template <std::random_access_iterator InputIt>
    void assgin(InputIt first, InputIt last) {
        size_t diff = last - first;
        for (size_t i = 0; i < diff; ++i) {
            m_data[i] = *first++;
        }
    }

    void assgin(size_t n, T val) {
        reserve(n);  // reserve -> 容量扩增并不会改变m_size
        m_size = n;
        for (size_t i = 0; i < n; ++i) {
            m_data[i] = val;
        }
    }
    // initializer_list直接转发给迭代器版本即可
    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    ~vector() noexcept { delete[] m_data; }

public:
    void clear() noexcept { resize(0); }

    // 赋值函数一定要把之前的释放掉
    // 要把原来的东西拷贝过去
    // _name 内部api
    void _recapacity(size_t n) {
        {
            if (m_data) {
                if (0 == n) {
                    delete[] m_data;
                    m_data = nullptr;
                    m_capacity = 0;
                    return;
                }
                // new出来之后使用{}初始化为0
                T* tmp = new T[n]{};
                // 如果size变小就要把多余的砍掉
                memcpy(tmp, m_data, std::min(m_capacity, n) * sizeof(T));
                delete[] m_data;
                m_data = tmp;
                m_capacity = n;
            } else {
                if (0 == n) return;
                m_data = new T[n]{};
                m_capacity = n;
            }
        }

        // {
        //     T* old_data = m_data;
        //     size_t old_size = m_size;
        //     if (n == 0) {
        //         if (m_data) {
        //             delete[] m_data;
        //             m_data = nullptr;
        //             m_size = 0;
        //         }
        //         return;
        //     }
        //     m_data = new T[n]{};
        //     m_size = n;
        //     memcpy(m_data, old_data, std::min(old_size, n) * sizeof(T));
        //     if (old_data)
        //         delete[] old_data;
        // }
    }

    // 缩小容量(请求,并不一定会执行)
    // 不想浪费内存,将大内存释放掉,得到小内存
    void shrink_to_fit() {
        T* old_data = m_data;
        m_capacity = m_size;
        // m_size == 0 不代表没有new过
        if (m_size == 0) {
            m_data = nullptr;
        } else {
            m_data = new T[m_size];
            memcpy(m_data, old_data, m_size * sizeof(T));
        }
        if (old_data) delete[] old_data;
    }
    // 扩容从0到1的本质是max(n, m_capacity * 2)
    // 如果一开始size = 0, 那push_back的时候这个函数的n就是1, 之后扩充
    void _grow_capacity_until(size_t n) {
        if (n <= m_capacity) return;
        // 两倍增长
        n = std::max(n, m_capacity * 2);
        printf("vector capacity from %zd to %zd\n", m_capacity, n);
        T* old_data = m_data;
        if (n == 0) {
            m_data = nullptr;
            m_capacity = 0;
        } else {
            m_data = new T[n]{};
            m_capacity = n;
        }
        if (old_data) {
            if (m_size) memcpy(m_data, old_data, sizeof(T) * m_size);
            delete[] old_data;
        }
    }

    void resize(size_t n, T val = 0) {
        _grow_capacity_until(n);
        if (n > m_size) {
            for (size_t i = m_size; i < n; ++i) m_data[i] = val;
        }
        m_size = n;
    }

    void reserve(size_t n) {
        // 不考虑之前有元素
        // T *old_data = m_data;
        // m_capacity = n;
        // m_data = new T[n]{};
        // m_size = 0;
        // delete[] old_data;
        _grow_capacity_until(n);
    }

    size_t size() const { return m_size; }

    size_t capacity() const { return m_capacity; }

public:
    /// @brief 下标类型一定是size_t类型,如果是int最多表示2亿也就是2^31 - 1,
    /// size_t -> 2 ^ 64 - 1 arr[i] = i -->
    /// 如果返回int为纯右值,返回之后存到rax寄存器了,没啥效果
    T& operator[](size_t i) { return m_data[i]; }

    // 这里如果直接返回T会产生一次拷贝,开销可能会很大
    const T& operator[](size_t i) const { return m_data[i]; }

    // 提供越界保护
    T& at(size_t i) {
        // 小概率事件 加[[unlikely]] 修饰, 编译器能优化的好一点,初始化
        if (i >= m_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return m_data[i];
    }

    T const& at(size_t i) const {
        if (i >= m_size) [[unlikely]]
            throw std::out_of_range("vector::at");
        return m_data[i];
    }

    T& back() noexcept { return m_data[m_size - 1]; }

    T const& back() const noexcept { return m_data[m_size - 1]; }

    T& front() noexcept { return m_data[0]; }

    T const& front() const noexcept { return m_data[0]; }

public:
    // 迭代器, 注意在Class中 typedef也受访问控制
    // typedef T* iterator;

    const iterator begin() const { return m_data; }

    const iterator end() const { return m_data + m_size; }

public:
    void push_back(T val) {
        resize(size() + 1);
        back() = val;
        // (*this)[size() - 1] = val;
    }
    // 区间删除(下标版本)
    void erase(size_t index) {
        for (size_t j = index + 1; j < m_size; ++j) {
            // 后面那个东西已经不需要移动给当前位置
            m_data[j - 1] = std::move(m_data[j]);
        }
        resize(size() - 1);
        // size_t right = m_size - index - 1;
        // memcpy(m_data + index, m_data + index + 1, sizeof(T) * right);
    }

    void erase(size_t ibeg, size_t iend) {
        size_t diff = iend - ibeg;
        for (size_t j = iend; j < m_size; ++j) {
            m_data[j - diff] = std::move(m_data[j]);
        }
        resize(m_size - diff);
    }

    // 区间删除(迭代器版本)
    void erase(iterator const it) {
        for (iterator i = it + 1; i < end(); ++i) *(i - 1) = std::move(*i);
        resize(size() - 1);
    }

    void erase(iterator const first, iterator const last) {
        size_t diff = last - first;
        for (iterator it = last; it != end(); ++it) {
            *(it - diff) = std::move(*it);
        }
        resize(size() - diff);
    }

    void insert(iterator const it, size_t n, T val = 0) {
        size_t j = it - m_data;
        size_t l = end() - it;
        if (n == 0) [[unlikely]]
            return;
        reserve(size() + n);
        m_size += n;

        // 像这种要将元素往后移动的情况必须倒过来
        // 如果添加的数量小于头尾之间的数量,就会造成覆盖情况
        for (int i = j + n - 1; i >= (int)j; --i)
            m_data[i + n] = std::move(m_data[i]);

        for (size_t i = j; i < j + n; ++i) m_data[i] = val;
    }

    // C++20
    template <std::random_access_iterator InputIt>
    void insert(iterator const it, InputIt first, InputIt last) {
        size_t n = last - first;
        size_t j = it - m_data;
        size_t l = end() - it;
        if (n == 0) [[unlikely]]
            return;
        reserve(size() + n);
        m_size += n;
        // j ~ m_size -> j + n ~ m_size + n
        // 有BUG如果n小于他们之间的间隙就会出现覆盖丢失数据
        // for (size_t i = j; i < size(); ++i) {
        //     m_data[i + n] = std::move(m_data[i]);
        // }

        // 必须倒过来
        // std::cout << "l = " << l << std::endl;
        // 注意int 和 size_t类型的比较问题,如果比较时同时出现int 和 size_t
        // 按照size_t来处理
        for (int i = j + l - 1; i >= (int)j; --i) {
            m_data[i + n] = std::move(m_data[i]);
        }
        for (size_t i = j; i < j + n; ++i) {
            m_data[i] = *first++;  // 先解引用再++
        }
    }

    void insert(iterator const it, std::initializer_list<T> ilist) {
        insert(it, ilist.begin(), ilist.end());
    }

private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;
};

}  // namespace qc

int main() {
    // const qc::vector<int> arr(3);
    // qc::vector<int> arr = 3; -> 隐式构造函数杜绝 -> {0, 0, 0}
    qc::vector<int> arr(4);

    qc::vector<int> ary;
    for (size_t i = 0; i < arr.size(); ++i) arr[i] = i + 1;

    // 一旦定义了析构函数必须同时定义拷贝构造函数/移动构造函数,否则double
    // free问题
    auto bar = arr;

    arr.resize(3);  // -> 覆盖了之前new的int[2],内存泄露

    for (size_t i = 0; i < bar.size(); ++i) {
        int val_1 = arr[i];          // -> non const -> once copy
        auto const& val_2 = arr[i];  // non const -> no copy
    }

    bar.resize(7);

    for (size_t i = 0; i < arr.size(); ++i) {
        printf("arr[%zd] = %d\n", i, arr[i]);
    }

    bar[3] = 100;

    for (size_t i = 0; i < bar.size(); ++i) {
        printf("bar[%zd] = %d\n", i, bar[i]);
    }

    std::cout << "==================================" << std::endl;

    qc::vector<int> a;
    a.reserve(4);
    for (size_t i = 0; i < 4; ++i) a.push_back(i);

    qc::vector<int> shrink;
    shrink.reserve(1000);
    shrink.shrink_to_fit();
    std::cout << "shrink.size() = " << shrink.size()
              << " shrink.capacity() = " << shrink.capacity() << std::endl;

    for (qc::vector<int>::iterator it = a.begin(); it != a.end(); ++it)
        printf("a = %d\n", *it);

    std::cout << "======== test_iterator ========" << std::endl;
    qc::vector<int> b;
    b.resize(10);
    int value = 1;
    for (qc::vector<int>::iterator it = b.begin(); it != b.end(); ++it) {
        *it = value++;
        std::cout << *it << std::endl;
    }
    qc::vector<int>::iterator it = b.begin() + 1;
    // 删除第二个元素
    b.erase(it);
    for (qc::vector<int>::iterator it = b.begin(); it != b.end(); ++it) {
        std::cout << *it << std::endl;
    }

    auto first = b.begin() + 1, last = b.begin() + 3;
    b.erase(first, last);
    for (qc::vector<int>::iterator it = b.begin(); it != b.end(); ++it) {
        std::cout << *it << std::endl;
    }

    std::cout << "========== test insert ============" << std::endl;

    qc::vector<int> c(5);
    for (int i = 0; i < c.size(); ++i) {
        c[i] = i + 1;
        std::cout << c[i] << "";
    }
    std::cout << std::endl;

    int c_1[5]{8, 7, 6, 5, 4};
    int *f_1 = c_1 + 1, *l_1 = c_1 + 3;
    c.insert(c.begin(), f_1, l_1);
    for (int i = 0; i < c.size(); ++i) std::cout << c[i] << " ";
    std::cout << std::endl;
    c.erase(c.begin());
    std::cout << "after erase" << std::endl;
    for (int i = 0; i < c.size(); ++i) std::cout << c[i] << " ";
    std::cout << std::endl;

    std::cout << "=========== assign ===========" << std::endl;
    qc::vector<int> d;
    d.assgin(5, 100);  // equal to -> d = qc::vector<int>(5, 100);
    for (auto it = d.begin(); it != d.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    std::cout << "=========== initializer_list ===========" << std::endl;
    d = qc::vector<int>({1, 2, 3, 4});
    for (auto it = d.begin(); it != d.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    qc::vector<int> e{1, 3, 4, 5};
    for (auto it = e.begin(); it != e.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    std::cout << "insert an initializer_list" << std::endl;
    e.insert(e.begin(), {1, 2, 3});
    std::cout << "after insert an initializer_list" << std::endl;
    for (auto it = e.begin(); it != e.end(); ++it) std::cout << *it << " ";
    std::cout << std::endl;

    return 0;
}