/**
 * @file qc_list.cc
 * @author qc
 * @brief list仿STL实现
 * @version 0.1
 * @date 2024-07-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <alloca.h>

#include <concepts>
#include <coroutine>
#include <cstddef>  // for size_t uint64_t ...
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <stdexcept>

namespace qc {

// list的allocator中的元素类型应该为ListNode而非T,所以要rebind
template <class T, class Alloc = std::allocator<T>>
class list {
public:
    // T() ==> int() = 0, float() = 0.0f, string() = "", pointer() = nullptr;
    using value_type = T;
    using allocator_type = Alloc;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;

    [[no_unique_address]] allocator_type m_alloc;

private:
    // struct PrivateConstruct {
    //     // 正经的还是得靠友元函数
    //     explicit PrivateConstruct() = default; //
    //     一定要声明,避免用户使用{}初始化,list<int>::const_iterator({},
    //     nullptr) / list<int>::const_iterator(nullptr)创建迭代器
    // };
public:
    struct ListNode {
        ListNode() {
            std::construct_at(&m_val);
            m_prev = this;
            m_next = this;
        }

        explicit ListNode(T const &val, ListNode *prev = nullptr,
                          ListNode *next = nullptr) {
            std::construct_at(&m_val, std::move(val));
            m_prev = prev;
            m_next = next;
        }

        T m_val;
        ListNode *m_prev;
        ListNode *m_next;
    };

public:
    list() noexcept { m_head = &m_dummy; }

    explicit list(size_t n) {
        // if (0 == n) {
        //     m_head = &m_dummy;
        //     return;
        // }
        // ListNode *curr = new ListNode(), *prev = nullptr;
        // m_head = prev = curr;
        // for (size_t i = 1; i < n; ++i) {
        //     curr = new ListNode();
        //     curr->m_prev = prev;
        //     prev->m_next = curr;
        //     prev = curr;
        // }
        // // pre->m_next = nullptr; // STL中使用的是双向循环链表(back()->O(1))
        // prev->m_next = &m_dummy;
        // m_dummy.m_prev = prev;
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = new ListNode();
            prev->m_next = node;
            node->m_prev = prev;
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
        m_head = m_dummy.m_next;
    }

    explicit list(size_t n, T const &val) {
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = new ListNode(val);
            prev->m_next = node;
            node->m_prev = prev;
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
        m_head = m_dummy.m_next;
    }

    // template <class InputIt>
    //     requires std::random_access_iterator<InputIt> <===> template
    //     <std::random_access_iterator InputIt>
    // qc::list<int> lst(3, 4) ---> {4,4,4} 所以random_access_iterator不行

    // input_iterator 支持 *it it++ ++it it!=it it==it
    // output_iterator 支持 *it=val it++ ++it it!=it it==it
    // forward_iterator 支持 *it *it=val it++ ++it it!=it it==it
    // bidirectional 支持 *it *it=val it++ ++it --it it-- it!=it it==it
    // random_access_iterator 支持 *it *it=val it[n] it[n]=val it++ ++it it+=n
    // --it it-- it-=n it+n it-n it!=it it==it
    // 一般set和mat的iterator类型为bidirectional,如果想要和random_access_iterator效果一样的话需要使用std::advance(it,
    // n)来向前或者向后移动n
    template <std::input_iterator InputIt>
    list(InputIt first, InputIt last) {
        // if (first == last) {
        //     m_head = &m_dummy;
        //     return;
        // }
        // ListNode *curr = new ListNode(*first++), *prev = &m_dummy;
        // std::cout << "the first node val = " << *first << std::endl;
        // ++first;
        ListNode *prev = &m_dummy;
        // m_head = prev = curr;
        // std::cout << "m_head->val = " << m_head->m_val << std::endl;
        while (first != last) {
            ListNode *node = new ListNode(*first++);
            node->m_prev = prev;
            prev->m_next = node;
            prev = node;
            // ++first;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
        m_head = m_dummy.m_next;
    }
    // 初始化列表中的iterator类型为random_access_iterator,其是input_iterator的超集,所以直接转发没有任何问题
    list(std::initializer_list<T> ilist) : list(ilist.begin(), ilist.end()) {}

public:
    // O(1)
    size_t size() const { return m_size; }

    bool empty() const noexcept { return m_dummy.m_prev == m_dummy.m_next; }

    T &front() noexcept { return m_dummy.m_next->m_val; }

    const T &front() const noexcept { return m_dummy.m_next->m_val; }

    T &back() noexcept { return m_dummy.m_prev->m_val; }

    T const &back() const noexcept { return m_dummy.m_prev->m_val; }
    // Visitor => lambda
    // template <std::invocable<T &> Visitor> 也有const 版本
    template <class Visitor>
    void foreach (Visitor visit) {
        // if (m_head == &m_dummy) return;
        ListNode *curr = m_head;
        // std::cout << "in foreach curr->val = " << curr->m_val << std::endl;
        // visit(curr->m_val);
        // curr = curr->m_next;
        while (curr != &m_dummy) {
            visit(curr->m_val);
            // std::cout << "in foreach curr->val = " << curr->m_val <<
            // std::endl;
            curr = curr->m_next;
        }
    }

    struct iterator {
        // 迭代器类型
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        friend list;

    private:
        ListNode *m_curr;

        // explicit const_iterator(PrivateConstruct, ListNode const *curr) :
        // m_curr(curr) {}
        explicit iterator(ListNode *curr) : m_curr(curr) {}

    public:
        iterator() = default;  // 用户直接list<int>::iterator it; 默认初始构造
        // ++it;
        iterator &operator++() {
            m_curr = m_curr->m_next;
            return *this;
        }
        // it++ 返回纯右值
        iterator operator++(int) {
            // 基于前置++
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        // --it;
        iterator &operator--() {
            m_curr = m_curr->m_prev;
            return *this;
        }
        // it-- 返回纯右值
        iterator operator--(int) {
            // 基于前置--
            auto tmp = *this;
            --*this;
            return tmp;
        }

        T &operator*() const { return m_curr->m_val; }

        bool operator!=(iterator const &that) const {
            return m_curr != that.m_curr;
        }

        bool operator==(iterator const &that) const {
            return m_curr == that.m_curr;  // equal to !(*this != that);
        }
    };

    /// @brief 在const_iterator中
    //  const_iterator(ListNode const *curr);
    //  const_iterator(0);
    // 在外部不能构造出一个迭代器
    struct const_iterator {
        // 迭代器类型
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        friend list;  // 添加友元之后下面的end front就可以把PrivateConstruct去掉
    private:
        ListNode const *m_curr;

        // explicit const_iterator(PrivateConstruct, ListNode const *curr) :
        // m_curr(curr) {}
        explicit const_iterator(ListNode const *curr) : m_curr(curr) {}

    public:
        const_iterator() = default;

        // 从普通迭代器构造常量迭代器
        const_iterator(iterator that) : m_curr(that.m_curr) {}

        const_iterator operator++() {
            m_curr = m_curr->m_next;
            return *this;
        }

        // it++ 返回纯右值
        const_iterator operator++(int) {
            // 基于前置++
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        // --it;
        const_iterator &operator--() {
            m_curr = m_curr->m_prev;
            return *this;
        }
        // it-- 返回纯右值
        const_iterator operator--(int) {
            // 基于前置--
            auto tmp = *this;
            --*this;
            return tmp;
        }

        T &operator*() const { return m_curr->m_val; }

        bool operator!=(const_iterator const &that) const {
            return m_curr != that.m_curr;
        }

        bool operator==(const_iterator const &that) const {
            return m_curr == that.m_curr;  // equal to !(*this != that);
        }
    };
    // 每一次co_yield yield之前都会往下一波操作类似于上面的curr = curr->m_next;
    //! 协程 C++23(20就有概念) coroutine 不支持bidirectional_iterator 和
    //! random_access_iterator,所以下面的实现是不合理的
    // 协程本质上是保存一个栈帧
    // 协程返回类型coroutine_handle里面装了一个iterator
    // coroutine<int> begin() {
    //     if (m_head == nullptr) co_return;
    //     ListNode *curr = m_head;
    //     co_yield curr->m_val;
    //     curr = curr->m_next;
    //     while (curr != m_head) {
    //         co_yield curr->m_val;
    //         curr = curr->m_next;
    //     }
    // }

    // iterator begin() { return iterator{PrivateConstruct{}, m_dummy.m_next}; }

    iterator begin() { return iterator{m_dummy.m_next}; }

    iterator end() { return iterator{&m_dummy}; }

    const_iterator cbegin() const { return const_iterator{m_dummy.m_next}; }

    const_iterator cend() const { return const_iterator{&m_dummy}; }

    const_iterator begin() const { return cbegin(); }

    const_iterator end() const { return cend(); }

    // ====================================================
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    // ====================================================
    reverse_iterator rbegin() {
        return std::make_reverse_iterator(iterator{&m_dummy});
    }

    reverse_iterator rend() {
        return std::make_reverse_iterator(iterator{m_dummy.m_next});
    }

    reverse_const_iterator crbegin() const {
        return std::make_reverse_iterator(const_iterator{&m_dummy});
    }

    reverse_const_iterator crend() const {
        return std::make_reverse_iterator(const_iterator{m_dummy.m_next});
    }

    reverse_const_iterator rbegin() const { return crbegin(); }

    reverse_const_iterator rend() const { return crend(); }

public:
    ListNode *get_head() const { return m_head; }

    ListNode const *get_dummy() const { return &m_dummy; }

    ListNode *get_dummy() { return &m_dummy; }

private:
    // 为了结果end()中不能直接return m_head->m_prev, 标准库发力 直接定义
    // m_dummy不存储任何val 实现中不要出现nullptr
    ListNode *m_head;
    ListNode m_dummy;
    size_t m_size;
};

// template <class T>
// list<T>::ListNode list<T>::m_dummy;

}  // namespace qc

template <class T>
void print(const std::string &name, qc::list<T> const &lst) {
    std::cout << "print : " << name << std::endl;
    // for (auto it = lst.begin(); it != lst.end(); ++it) std::cout << *it << "
    // "; std::cout << std::endl;
    typename qc::list<T>::ListNode *head = nullptr;
    typename qc::list<T>::ListNode *tail = nullptr;
    // head = tail = lst.get_head();
    head = lst.get_head();
    // 注意如果class中有一个结构体成员变量,这时候外部函数参数为const该类,也不能通过指针去指向这个对象中的结构体成员变量,除非这个指针也是const或者使用const_cast去掉const
    // 所以最好的解决方案是在list中只有一个ListNode *dummy 不需要head
    tail = const_cast<qc::list<T>::ListNode *>(lst.get_dummy());
    // tail = tail->m_prev;
    while (head != tail) {
        std::cout << head->m_val << " ";
        head = head->m_next;
    }
    std::cout << std::endl;
}

void test_construct_iterator() {
    std::cout << "========= test_construct_iterator =========" << std::endl;
    std::list<int> slst({1, 2, 3, 4});
    qc::list<int> lst(slst.begin(), slst.end());
    print<int>("construct_iteartor lst", lst);
}

void test_initializer_list() {
    std::cout << "========= test_initializer_list =========" << std::endl;
    qc::list<int> lst{1, 2, 3, 4};
    print<int>("inilializer_list lst", lst);
}
// equal to print but in class
void test_foreach() {
    qc::list<int> lst{2, 3, 4};
    print<int>("test_foreach", lst);
    size_t i = 0;
    lst.foreach ([&](int &val) {
        std::cout << "lst[" << i << "].val = " << val << std::endl, ++i;
    });
}

void test_rtraversal() {
    qc::list<int> lst{1, 2, 3};
    print<int>("before reverse tarversal lst", lst);
    std::cout << "after reverse : " << std::endl;
    for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
        int &val = *it;
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

int main() {
    // test_construct_iterator();

    // test_initializer_list();

    // test_foreach();

    test_rtraversal();

    return 0;
}