/**
 * @file qc_vector.cc
 * @author qc
 * @brief 扩充基本的vector类
 * @details 添加扩容机制
 * @version 0.4
 * @date 2024-07-02
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace qc {

template <class T>
class vector {
public:
    // 杜绝隐式转换,避免用户手滑写错的情况
    explicit vector(size_t n) {
        m_data = new T[n]{};
        memset(m_data, 0, n * sizeof(T));
        m_size = n;
        m_capacity = n;
    }

    // 拷贝构造函数, 深拷贝
    vector(const vector& v) {
        m_size = v.m_size;
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
        v.m_data = nullptr;
        v.m_size = 0;
    }

    // 移动拷贝函数
    vector& operator=(const vector&& v) noexcept {
        clear();
        m_data = v.m_data;
        m_size = v.m_size;
        v.m_data = nullptr;
        v.m_size = 0;
    }

    // 拷贝赋值函数
    vector& operator=(vector const& v) {
        clear();
        m_size = v.m_size;
        if (m_size) {
            m_data = new T[m_size]{};
            memcpy(m_data, v.m_data, sizeof(T) * m_size);
        }
        return *this;
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

    void resize(size_t n) {
        _grow_capacity_until(n);
        m_size = n;
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
    void push_back(T val) {
        resize(m_size + 1);
        back() = val;
    }

    void erase(size_t index) {
        for (size_t j = index + 1; j < m_size; ++j) {
            // 后面那个东西已经不需要移动给当前位置
            m_data[j - 1] = std::move(m_data[j]);
        }
        resize(m_size - 1);
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
    for (size_t i = 0; i < 16; ++i) a.push_back(i);

    return 0;
}