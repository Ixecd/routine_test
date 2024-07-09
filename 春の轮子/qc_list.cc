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
#include <assert.h>

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
#include <type_traits>

namespace qc {

// list的allocator中的元素类型应该为ListNode而非T,所以要rebind
template <class T, class Alloc = std::allocator<T>>
class list {
public:
    // 下面是优化(将val和其他属性分隔开) 将list大小从8 + 8 + sizeof(T) -> 8 + 8
    struct ListValueNode;
    struct ListNode {
        ListNode *m_prev;
        ListNode *m_next;
        inline T &value() { return static_cast<ListValueNode &>(*this).m_val; }
        inline T const &value() const {
            return static_cast<ListValueNode const &>(*this).m_val;
        }
    };

    struct ListValueNode : public ListNode {
        T m_val;
    };

public:
    // T() ==> int() = 0, float() = 0.0f, string() = "", pointer() = nullptr;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;
    using AllocNode = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ListValueNode>;

private:
    // struct PrivateConstruct {
    //     // 正经的还是得靠友元函数
    //     explicit PrivateConstruct() = default; //
    //     一定要声明,避免用户使用{}初始化,list<int>::const_iterator({},
    //     nullptr) / list<int>::const_iterator(nullptr)创建迭代器
    // };
public:
    // struct ListNode {
    //     ListNode() {
    //         std::construct_at(&m_val);
    //         m_prev = this;
    //         m_next = this;
    //     }

    //     explicit ListNode(T const &val, ListNode *prev = nullptr,
    //                       ListNode *next = nullptr) {
    //         std::construct_at(&m_val, std::move(val));
    //         m_prev = prev;
    //         m_next = next;
    //     }

    //     T m_val;
    //     ListNode *m_prev;
    //     ListNode *m_next;
    // };

public:
    list() noexcept {}

    explicit list(size_t n) {
        // if (0 == n) {
        //     m_head = &m_dummy;
        //     return;
        // }
        // ListNode *curr = new ListValueNode(), *prev = nullptr;
        // m_head = prev = curr;
        // for (size_t i = 1; i < n; ++i) {
        //     curr = new ListValueNode();
        //     curr->m_prev = prev;
        //     prev->m_next = curr;
        //     prev = curr;
        // }
        // // pre->m_next = nullptr; // STL中使用的是双向循环链表(back()->O(1))
        // prev->m_next = &m_dummy;
        // m_dummy.m_prev = prev;
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            // node->value() = T();
            std::construct_at(&node->value());
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    explicit list(size_t n, T const &val) {
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            std::construct_at(&node->value(), val);
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    // template <class InputIt>
    //     requires std::random_access_iterator<InputIt> <===> template
    //     <std::random_access_iterator InputIt>
    // qc::list<int> lst(3, 4) ---> {4,4,4} ? {3,4} ?
    // 所以random_access_iterator不行

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
            ListNode *node = newNode();
            node->m_prev = prev;
            prev->m_next = node;
            std::construct_at(&node->value(), *first++);
            prev = node;
            // ++first;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }
    // 初始化列表中的iterator类型为random_access_iterator,其是input_iterator的超集,所以直接转发没有任何问题
    list(std::initializer_list<T> ilist) : list(ilist.begin(), ilist.end()) {}

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        ListNode *prev = &m_dummy;
        // m_head = prev = curr;
        // std::cout << "m_head->val = " << m_head->m_val << std::endl;
        while (first != last) {
            ListNode *node = newNode();
            node->m_prev = prev;
            prev->m_next = node;
            std::construct_at(&node->value(), *first++);
            prev = node;
            // ++first;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    void assign(size_t n, T const &val) {
        clear();
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = newNode();
            node->m_prev = prev;
            prev->m_next = node;
            std::construct_at(&node->value(), val);
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    void assign(size_t n) {
        clear();
        ListNode *prev = &m_dummy;
        for (size_t i = 0; i < n; ++i) {
            ListNode *node = newNode();
            node->m_prev = prev;
            prev->m_next = node;
            std::construct_at(&node->value());
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    // 拷贝构造函数
    list(list<T> const &lst) {
        ListNode *prev = &m_dummy;
        for (typename list<T>::iterator it = lst.begin(); it != lst.end();
             ++it) {
            ListNode *node = newNode();
            prev->m_next = node;
            node->m_prev = prev;
            // node->value() = *it;
            std::construct_at(&node->value(), *it);
            prev = node;
        }
        prev->m_next = &m_dummy;
        m_dummy.m_prev = prev;
    }

    ~list() noexcept {
        ListNode *curr = m_dummy.m_next;
        while (curr != &m_dummy) {
            // 就地析构对象 T 可能被析构两遍
            std::destroy_at(&curr->value());
            // curr->value().~T();
            auto nxt = curr->m_next;
            deleteNode(curr);
            curr = nxt;
        }
    }

public:
    void clear() noexcept {
        ListNode *curr = m_dummy.m_next;
        while (curr != &m_dummy) {
            // 就地析构对象 T 可能被析构两遍
            std::destroy_at(&curr->value());
            // curr->value().~T();
            auto nxt = curr->m_next;
            deleteNode(curr);
            curr = nxt;
        }
        m_dummy.m_next = m_dummy.m_prev = &m_dummy;
    }

public:
    // O(1)
    // size_t size() const noexcept { return m_size; }

    bool empty() const noexcept { return m_dummy.m_prev == m_dummy.m_next; }

    T &front() noexcept { return m_dummy.m_next->value(); }

    const T &front() const noexcept { return m_dummy.m_next->value(); }

    T &back() noexcept { return m_dummy.m_prev->value(); }

    T const &back() const noexcept { return m_dummy.m_prev->value(); }
    // Visitor => lambda
    // template <std::invocable<T &> Visitor> 也有const 版本template
    // <std::invocable<T const &> Visitor>
    template <class Visitor>
    void foreach (Visitor visit) noexcept {
        // if (m_head == &m_dummy) return;
        ListNode *curr = m_dummy.m_next;
        // std::cout << "in foreach curr->val = " << curr->value() << std::endl;
        // visit(curr->value());
        // curr = curr->m_next;
        while (curr != &m_dummy) {
            visit(curr->value());
            // std::cout << "in foreach curr->val = " << curr->value() <<
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
        explicit iterator(ListNode *curr) noexcept : m_curr(curr) {}

    public:
        iterator() = default;  // 用户直接list<int>::iterator it; 默认初始构造
        // ++it;
        iterator &operator++() noexcept {
            m_curr = m_curr->m_next;
            return *this;
        }
        // it++ 返回纯右值
        iterator operator++(int) noexcept {
            // 基于前置++
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        // --it;
        iterator &operator--() noexcept {
            m_curr = m_curr->m_prev;
            return *this;
        }
        // it-- 返回纯右值
        iterator operator--(int) noexcept {
            // 基于前置--
            iterator tmp = *this;
            --*this;
            return tmp;
        }

        T &operator*() const noexcept { return m_curr->value(); }

        bool operator!=(iterator const &that) const noexcept {
            return m_curr != that.m_curr;
        }

        bool operator==(iterator const &that) const noexcept {
            return m_curr == that.m_curr;  // equal to !(*this != that);
        }
    };

    /// @brief 在const_iterator中
    /// 表示迭代器里面的value()为const,迭代器本身并不是const
    //  const_iterator(ListNode const *curr);
    //  const_iterator(0);
    // 在外部不能构造出一个迭代器
    struct const_iterator {
        // 迭代器类型
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T const;
        using difference_type = ptrdiff_t;
        using pointer = T const *;
        using reference = T const &;

        friend list;  // 添加友元之后下面的end
                      // front就可以把PrivateConstruct去掉,这里是为了保证只有list类才可以创建迭代器,外部不可以,但是标准库规定必须有默认构造函数
        // 例如外部使用 qc::list<int>::iterator it; 之后对it赋值,再进行操作
    private:
        ListNode const *m_curr;

        // explicit const_iterator(PrivateConstruct, ListNode const *curr) :
        // m_curr(curr) {}
        explicit const_iterator(ListNode const *curr) noexcept : m_curr(curr) {}

    public:
        const_iterator() = default;

        // 从普通迭代器构造常量迭代器
        const_iterator(iterator const &that) noexcept : m_curr(that.m_curr) {}

        const_iterator &operator++() noexcept {
            // 可以吗? 可以! 这里改的是指针,并没有更改m_curr指向的内容
            m_curr = m_curr->m_next;
            return *this;
        }

        // it++ 返回纯右值
        const_iterator operator++(int) noexcept {
            // 基于前置++
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        // --it;
        const_iterator &operator--() noexcept {
            m_curr = m_curr->m_prev;
            return *this;
        }
        // it-- 返回纯右值
        const_iterator operator--(int) noexcept {
            // 基于前置--
            auto tmp = *this;
            --*this;
            return tmp;
        }

        T const &operator*() const noexcept { return m_curr->value(); }

        bool operator!=(const_iterator const &that) const noexcept {
            return m_curr != that.m_curr;
        }

        bool operator==(const_iterator const &that) const noexcept {
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
    //     co_yield curr->value();
    //     curr = curr->m_next;
    //     while (curr != m_head) {
    //         co_yield curr->value();
    //         curr = curr->m_next;
    //     }
    // }

public:
    // 注意对迭代器的操作只有在类中才可以
    // iterator begin() { return iterator{PrivateConstruct{}, m_dummy.m_next}; }

    iterator begin() noexcept { return iterator{m_dummy.m_next}; }

    iterator end() noexcept { return iterator{&m_dummy}; }

    const_iterator cbegin() const noexcept { return const_iterator{m_dummy.m_next}; }

    const_iterator cend() const noexcept { return const_iterator{&m_dummy}; }

    const_iterator begin() const noexcept { return cbegin(); }

    const_iterator end() const noexcept { return cend(); }

    // ====================================================
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    // ====================================================
    reverse_iterator rbegin() noexcept {
        return std::make_reverse_iterator(iterator{&m_dummy});
    }

    reverse_iterator rend() noexcept {
        return std::make_reverse_iterator(iterator{m_dummy.m_next});
    }

    reverse_const_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(const_iterator{&m_dummy});
    }

    reverse_const_iterator crend() const noexcept {
        return std::make_reverse_iterator(const_iterator{m_dummy.m_next});
    }

    reverse_const_iterator rbegin() const noexcept { return crbegin(); }

    reverse_const_iterator rend() const noexcept { return crend(); }

public:
    // 如果push_back不在乎性能,直接可以转发给emplace_back
    void push_back(T const &val) {
        //emplace_back(val);
        ListNode *node = newNode();
        std::construct_at(&node->value(), std::move(val));
        ListNode *prev = m_dummy.m_prev;
        node->m_next = &m_dummy;
        node->m_prev = prev;
        prev->m_next = node;
        m_dummy.m_prev = node;
    }

    void push_back(T &&val) {
        emplace_back(std::move(val));
    }

    void push_front(T const &val) {
        //emplace_front(val);
        ListNode *node = newNode();
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        node->m_next = m_dummy.m_next;
        node->m_prev = &m_dummy;
        m_dummy.m_next->m_prev = node;
        m_dummy->m_next = node;
    }

    void push_front(T &&val) {
        emplace_front(std::move(val));
    }

    template <class ...Args>
    T &emplace_front(Args &&...args) {
        ListNode *node = newNode();
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        node->m_next = m_dummy.m_next;
        node->m_prev = &m_dummy;
        m_dummy.m_next->m_prev = node;
        m_dummy->m_next = node;
    }

    // 从任意参数中构造一个node
    template <class ...Args>
    T &emplace_back(Args &&...args) {
        ListNode *node = newNode();
        // 完美转发一下
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        ListNode *prev = m_dummy.m_prev;
        node->m_next = &m_dummy;
        node->m_prev = prev;
        prev->m_next = node;
        m_dummy.m_prev = node;
        return node->value();
    }


public:
    ListNode *get_head() const noexcept { return m_dummy.m_next; }

    ListNode const *get_dummy() const noexcept { return &m_dummy; }

    ListNode *get_dummy() noexcept { return &m_dummy; }

private:
    // 为了结果end()中不能直接return m_head->m_prev, 标准库发力 直接定义
    // m_dummy不存储任何val 实现中不要出现nullptr
    // ListNode *m_head;
    ListNode m_dummy;

    static ListNode *newNode() {
        return AllocNode{}.allocate(sizeof(ListValueNode));
    }

    static void deleteNode(ListNode *node) noexcept {
        AllocNode{}.deallocate(static_cast<ListValueNode *>(node), 1);
    }
};

}  // namespace qc

// equal to (const std::string &name, qc::list<T> lst)
template <class T>
void print_(const std::string &name, qc::list<T> const &lst) {
    // better use const & can reduce once construct, 拷贝构造函数
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
    // tail = lst.get_dummy();

    // 这里如果lst参数不是const &就不能单纯使用head != tail来比较 原因如下:
    // 在test_print中会使用初始化列表构造一次这一次的dummy是本次构造的
    // 之后再以值的方式传递再使用初始化列表的方式构造一次这一次的dummy是这一次的,所以两次dummy的地址不一样,但是head是相同的,因为dummy的值一样
    // 解决方案:添加拷贝构造函数.
    while (head != tail) {
        // while (head->m_next != tail->m_next) {
        std::cout << head->value() << " ";
        head = head->m_next;
    }
    std::cout << std::endl;
}

// equal to (const std::string &name, qc::list<T> lst)
template <class T>
void print(const std::string &name, qc::list<T> const &lst) {
    std::cout << "print : " << name << std::endl;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void test_print() {
    std::cout << "test_print initializer_list" << std::endl;
    print<int>("test_print ", qc::list<int>{1, 2, 3, 4, 5});
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

void test_detach_pointer_val() {
    qc::list<int> lst({1, 2, 3, 4});
    print<int>("test_detach_pointer_val lst ", lst);
    std::cout << "after insert val sizeof(ListNode) = "
              << sizeof(*lst.get_dummy()) << std::endl;
}

int main() {
    std::cout << "sizeof(qc::list<int>) : " << sizeof(qc::list<int>)
              << std::endl;

    test_construct_iterator();

    test_initializer_list();

    test_foreach();

    test_rtraversal();

    test_print();

    test_detach_pointer_val();

    return 0;
}