#include <exception>
#include <memory>
#include <type_traits>
#include <utility>
namespace qc {

// tag类,表示空
struct nullopt_t {
    explicit nullopt_t() = default;
};
inline nullopt_t nullopt;

class bad_optional_access : public std::exception {
public:
    bad_optional_access() = default;
    virtual ~bad_optional_access() = default;

    const char* what() const noexcept override { return "bad optional access"; }
};

template <class T>
class optional {
public:
    bool _has_value;
    /// @brief cpp中union包含的数据成员不会默认初始化,因为有可能不使用它
    union {
        T _value;
        nullopt_t _nullopt;
    };

public:
    optional(T value) : _value(std::move(value)), _has_value(true) {}

    optional() : _has_value(false) {}
    /// @brief 用户使用nullopt调用下面这个函数,nullopt的类型就是nullopt_t
    /// 构造函数 + Tag类来重载构造函数
    optional(nullopt_t) : _has_value(false) {}

    optional(optional const& that) : _has_value(that._has_value) {
        if (_has_value)  // new (&_value) T(that._value);
            // _value = T(that._value) -> T& operator=(T const&)
            std::construct_at(std::addressof(_value), that._value);
    }

    optional(optional&& that) noexcept : _has_value(that._has_value) {
        if (_has_value)
            std::construct_at(std::addressof(_value), std::move(that._value));
    }

    optional& operator=(optional const& that) {
        if (std::addressof(that) == this) return *this;
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        if (that._has_value) {
            std::construct_at(&_value, that._value);
            _has_value = true;
        }
        return *this;
    }

    optional& operator=(optional&& that) noexcept {
        if (std::addressof(that) == this) return *this;
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        if (that._has_value) {
            std::construct_at(std::addressof(_value), std::move(that._value));
            _has_value = true;
        }
        that._has_value = false;
        return *this;
    }

    optional& operator=(nullopt_t const&) {
        if (_has_value) std::destroy_at(std::addressof(_value));
        _has_value = false;
        return *this;
    }

    optional& operator=(T const& val) {
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        std::construct_at(std::addressof(_value), val);
        _has_value = true;
        return *this;
    }

    optional& operator=(T&& val) noexcept {
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        std::construct_at(std::addressof(_value), std::move(val));
        _has_value = true;
        return *this;
    }
    // 采用万能引用&& + 完美转发std::forward<Ts> 实现的理论是折叠引用
    template <class... Ts>
    void emplace(Ts&&... value_args) {
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        // new (&_value) T(value_args...);
        // std::construct_at(&_value,
        // std::forward<decltype(value_args)>(value_args)...);
        std::construct_at(std::addressof(_value),
                          std::forward<Ts>(value_args)...);
        _has_value = true;  // 不能提前开香槟
    }

    void reset() noexcept {
        if (_has_value) {
            std::destroy_at(std::addressof(_value));
            _has_value = false;
        }
        // *this = nullopt;
    }

    ~optional() noexcept {
        if (_has_value) std::destroy_at(std::addressof(_value));
    }

    bool has_value() const noexcept { return _has_value; }

    T const& value() const& {
        if (!_has_value) throw qc::bad_optional_access();
        return std::as_const(_value);
    }

    T& value() & {
        if (!_has_value) throw qc::bad_optional_access();
        return _value;
    }
    // for: std::move(qc::optional<int>(1)).value();
    // auto s1 = std::move(opts.value());
    // auto s2 = std::move(opts).value();
    // 上面两个是等价的
    // 兼容右值 qc::optional<int> parseInt(std::string s);
    // parseInt("42").value() -> 右值
    T const&& value() const&& {
        if (!_has_value) throw qc::bad_optional_access();
        return std::move(_value);
    }

    T&& value() && {
        if (!_has_value) throw qc::bad_optional_access();
        return std::move(_value);
    }

    T const& value_or(T const& default_value) & {
        if (!_has_value) return default_value;
        return _value;
    }

    T const& value_or(T const& default_value) const& noexcept(
        noexcept(T(std::declval<T const&>()))) {
        if (!_has_value) return default_value;
        return _value;
    }
    // 当前对象是右值,马上要弃之不用了,将资源转移
    T&& value_or(T&& default_value) && noexcept(
        noexcept(T(std::declval<T&&>()))) {
        if (!_has_value) return default_value;
        return std::move(_value);
    }

    // 将optional当指针来使用(四个版本)
    T const& operator*() const& {
        if (!_has_value) throw qc::bad_optional_access();
        return _value;
    }
    // 默认走
    T& operator*() & {
        if (!_has_value) throw qc::bad_optional_access();
        return _value;
    }

    T&& operator*() && {
        if (!_has_value) throw qc::bad_optional_access();
        return std::move(_value);
    }

    T const&& operator*() const&& {
        if (!_has_value) throw qc::bad_optional_access();
        return std::move(_value);
    }
    // 标准库中调用 -> 如果没有值是未定义的行为,这里为了安全,设置异常管理
    T* operator->() {
        if (!_has_value) throw qc::bad_optional_access();
        return std::addressof(
            _value);  // 这里的&有可能不工作,如果用户在自己的类(C)中定义了函数
                      // int operator&() const {...}
    }

    T const* operator->() const {
        if (!_has_value) throw qc::bad_optional_access();
        return std::addressof(
            _value);  // 当&运算符被重载的时候,std::addressof会正确取出_value的地址
    }

    // 下面是定义一个隐式转换, 为了避免 int i = opt; 加上explicit 必须强制转换
    // int i = (bool)opt; ok
    explicit operator bool() const noexcept { return _has_value; }

public:
    // --- extension ----
    bool operator==(T const& value) const noexcept {
        if (!_has_value) return false;
        return _value == value;
    }

    bool operator!=(T const& value) const noexcept {
        if (!_has_value) return true;
        return _value == value;
    }

    bool operator==(nullopt_t) const noexcept { return !_has_value; }
    // friend 相当于全局函数没有this指针
    friend bool operator==(nullopt_t, optional const& self) noexcept {
        return !self._has_value;
    }

    bool operator!=(nullopt_t) const noexcept { return _has_value; }
    // friend 相当于全局函数没有this指针
    friend bool operator!=(nullopt_t, optional const& self) noexcept {
        return self._has_value;
    }

    template <class U>
    bool operator==(optional<U> const& that) const noexcept {
        if (_has_value != that._has_value) return false;
        if (_has_value) return _value == that._value;
        return true;
    }

    template <class U>
    bool operator!=(optional<U> const& that) const noexcept {
        return !operator==(that);
    }

    template <class U>
    bool operator>(optional<U> const& that) const noexcept {
        if (!_has_value || !that._has_value) return false;
        return _value > that._value;
    }

    template <class U>
    bool operator<(optional<U> const& that) const noexcept {
        if (!_has_value || !that._has_value) return false;
        return _value < that._value;
    }

    template <class U>
    bool operator<=(optional<U> const& that) const noexcept {
        if (!_has_value || !that._has_value) return false;
        return _value <= that._value;
    }

    template <class U>
    bool operator>=(optional<U> const& that) const noexcept {
        if (!_has_value || !that._has_value) return false;
        return _value >= that._value;
    }

    template <class F>
    auto and_then(F&& f) {
        // decay_t 去引用 主要是针对参数的,对返回类型并没有那么严格 int[] ->
        // int*   int &-> int 更严格应该使用std::remove_cvref_t C++20引入的
        using RetType = std::remove_cvref_t<decltype(f(_value))>;
        // std::remove_cv_t<std::remove_reference_t<decltype(f(_value))>>;
        if (_has_value) {
            return std::forward<F>(f(_value));
        } else
            return RetType{};
    }

    // F 可能是lambda会捕获环境变量 std::unique_ptr<int>up -> 捕获到这个
    // 使用万能引用,一进来
    template <class F>
    auto transform(F&& f)
        -> optional<
            std::remove_cv_t<std::remove_reference_t<decltype(f(_value))>>> {
        if (_has_value)
            return std::forward<F>(f(_value));
        else
            return nullopt;
    }
};

#if __cpp_deduction_guides
template <class T>
optional(T) -> optional<T>;
#endif

}  // namespace qc

#include <iostream>
struct C {
    C(int x, int y) : m_x(x), m_y(y) {
        std::cout << "C(int, int)" << std::endl;
    }

    C(C const& that) : m_x(that.m_x), m_y(that.m_y) {
        std::cout << "C(C const&)" << std::endl;
    }

    C(C&&) { std::cout << "C(C &&)" << std::endl; }

    ~C() { std::cout << "~C()" << std::endl; }

    int operator&() const { return m_x; }

    int value() const { return m_x; }

    int m_x;
    int m_y;
};

void test_union() {
    union U {
        ~U() {}
        C c;
        int i;
    } u = {.i = 1};

    new (std::addressof(u.c)) C(1, 2);  // ok
    // C++17 CTAD
    // qc::optional(100);
    // optional(T) -> optional<T>

    std::cout << u.c.m_x << " " << u.c.m_y << std::endl;

    u.c.~C();
}

void test_base() {
    qc::optional<int> opt(2);
    // 隐式转换了,将qc::nullopt转换为option类型
    opt = qc::nullopt;

    std::cout << opt.has_value() << std::endl;

    std::cout << opt.value() << std::endl;
}

void test_operator() {
    qc::optional<int> opt(1);

    opt = qc::nullopt;
    std::cout << opt.has_value() << std::endl;  // 0

    opt = 42;

    std::cout << opt.has_value() << std::endl;  // 1
    std::cout << opt.value() << std::endl;      // 42
    std::cout << opt.value_or(0) << std::endl;  // 42

    opt = qc::optional<int>(33);

    std::cout << opt.has_value() << std::endl;  // 1
    std::cout << opt.value() << std::endl;      // 33
    std::cout << opt.value_or(0) << std::endl;  // 33

    opt = qc::optional<int>(qc::nullopt);
    std::cout << opt.has_value() << std::endl;  // 0
}

qc::optional<int> parseInt(std::string s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return qc::nullopt;
    }
}

void test_emplace() {
    // emplace少了一次移动赋值
    // C (int, int)
    // C (C&&)
    //  ~C()
    //  ~C()
    // emplace就是为了就地构造,防止 C(C&&) (如果类C非常庞大成员变量非常多)
    qc::optional<C> optc = qc::nullopt;
    // optc = std::move(C(1, 2));
    optc.emplace(1, 2);  // optional::emplace(int x, int y) { construct_at...}
    // C++17
    // 所有版本if中的变量都会强制转换为bool类型
    // if (auto opt = parseInt("42"); opt) {
    if (auto opt = parseInt("42")) {
        std::cout << *opt << std::endl;
    } else
        std::cout << "error" << std::endl;
}

void test_pointer() {
    qc::optional<C> opt = qc::nullopt;
    opt.emplace(2, 3);
    // *opt要返回引用
    int x = (*opt).value();
    int y = opt->m_y;
    // -> 支持
    std::cout << "opt's xvalue = " << x << std::endl;
    std::cout << "opt's yvalue = " << y << std::endl;
}

void test_as_bool() {
    qc::optional<int> opt;
    opt = 100;
    if (opt)
        std::cout << "opt is not nullopt" << std::endl;
    else
        std::cout << "opt is nullopt" << std::endl;
    opt = qc::nullopt;
    if (opt)
        std::cout << "opt is not nullopt" << std::endl;
    else
        std::cout << "opt is nullopt" << std::endl;

    // ! int i = opti; 不会发生任何错误
    int i = (bool)opt;
    std::cout << "i = " << i << std::endl;  // int i = (int)((bool)opt)
}

int main() {
    // qc::optional<C> opt(C(1, 2));
    // 这里optional<C>
    // 使用nullopt来初始化,里面仍然会调用_value()->默认构造函数,C里面没有->union可以不初始化
    // qc::optional<C> opt2(qc::nullopt);

    // std::cout << opt.has_value() << std::endl;

    // std::cout << opt.value() << std::endl;

    // test_union();

    // test_operator();

    test_emplace();

    // test_pointer();

    // test_as_bool();

    return 0;
}