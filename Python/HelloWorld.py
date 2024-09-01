# 单行注释

# Python 中无需提前声明变量
# x = -3
# x
# f = 3.14167
# f
# # Python 中没有类型的概念,解释器会根据赋值或运算自动推算变量类型
# type(x)
"""
多行注释
"""

# python 中 str是不可变类型 实际+=也是创建了新的对象

# 1.四行,以列的方式输入
# u, v, w = ([] for i in range(3))
# for i in range(4):
#     _u, _v, _w = [int(x) for x in input().split()]
#     u.append(_u), v.append(_v), w.append(_w);
# print(u, v, w)

# # 
# u, v, w = [], [], []
# s = input() # Python 中赋值语句不能放在条件表达式中
# while s:    # 这里不能像 C 那样 while(!scanf())
#     # 用切片拼接避免append()
#     u[len(u):], v[len(v):], w[len(w):] = [int(x) for x in s.split()]
#     s.input()

# while s := input():
#     u[len(u):], v[len(v):], w[len(w):] = [int(x) for x in s.split()]
# print(u, v, w)

# Python中没有switch语句
# Python3.8以上可以使用:=
if 4 >= 3 > 2 and 3 != 5 == 5 != 7:
    print("关系运算符可以连续使用")
    x = None or [] or -2
    print("&& || !", "与 或 非", "and or not", sep = "\n")
    print("多用and / or可以省行")
    if not x:
        print("负数也是True,不执行本行")
    elif x & 1:
        print("用elif而不是else if\n" "位运算符与C相近,偶数&1 = 0")
    else:
        print("也有三目运算符") if x else print("注意结构")


# Python 中的异常处理
s = "Hello"
pat = "World"
x = s.find(pat) # 找不到返回-1
try:
    y = s.index(pat) #报错,找不到
    print(y) #跳过
except ValueError:
    print("没找到")
    try:
        print(y)
    except NameError as e:
        print("无法输出y")
        print("原因:", e)

# Python中文件读写
a = []

print(type(a))

with open("in.txt") as f:
    N = int(f.readline()) # 文件第一行的数量N
    a[len(a):] = [[int(x) for x in f.readline().split()] for i in range(N)]

with open("out.txt", "w") as f:
    f.write("1\n")

print(a)
print(type(a))

# 内置容器
# 除了list还有tuple/dict和set
tup = tuple([[1,2], 4])
# equal to tup = ([1,2], 4)
tup[0].append(3)
print(tup)
a, b = 0, "Hello_World"
print(id(a), id(b))
print(a, b)

b, a = a, b
print(id(a), id(b))

print(a, b)

# dict
dic = {"key":"value"}
dic = {chr(i): i for i in range(65, 91)}
print(dic)

dic = {dic[k]: k for k in dic}
print(dic)

dic = {chr(i): i for i in range(65, 91)}

print(dic['A'])

if 'b' in dic:
    print(dic['b'])
else:
    dic['b'] = 98
print(dic['b'])

# 经典场景,统计出现次数
# cnter = {str: int}
# try:
#     cnter[key] += 1
# except:
#     cnter[key] = 1

# 编写函数
# 动态类型优势,ab也可以是字符串
def add(a, b):
    return a + b

def add_no_swap(a, b):
    print("in func #1:", id(a), id(b))
    a += b
    b, a = a, b
    print("in fun #2:", id(a), id(b))
    #return a, b

lst1 = [1, 2]
lst2 = [3, 4]

print("outside func #1:", id(lst1), id(lst2))

add_no_swap(lst1, lst2)

print("outside func #2:", id(lst1), id(lst2))

print(lst1, lst2)

# 在Python中使用默认参数很有可能遇到坑
# 默认参数的值应该是不可变对象,使用None占位是一种最佳实践
# def append_to(element, to=[]): to = [] 是引用
def append_to(element, to=None):
    if to is None:
        to = []
    to.append(element)
    return to

lst3 = append_to(12)
lst4 = append_to(42)
print(lst3, lst4) # [12] [42]

# 类型标注
if False:
        1 + "two" # 类型错误
else:
    1 + 2

# Python是一个动态类型检查的语言，以灵活但隐式的方式处理类型
# Python解释器仅仅在运行时检查类型是否正确，并且允许在运行时改变变量类型
# 动态类型一时爽，代码重构火葬场

def headline(
    text, # type:str
    width=50, #type:int
    fill_char="-", #type:str
): # type: (...)->str
    return f"{text.title()}".center(width, fill_char)

print(headline("type comments work", width = 40))

# def headline(
#     text,  # type: str
#     width=80,  # type: int
#     fill_char="-",  # type: str
# ):  # type: (...) -> str
#     return f"{text.title()}".center(width, fill_char)

print(__annotations__)

# print(headline("type comments work", width=40))

# 变量也是可以类型标注,可以通过 __annotations__ 来查看函数中所有的类型标注

# 装饰器
# 装饰器是一个函数，接受一个函数或方法作为其唯一的参数，并返回一个新函数或方法，其中整合了修饰后的函数或方法，并附带了一些额外的功能

@lru_cache(maxsize=None)
def fib(n):
    if n < 2:
        return n
    return fib(n - 1) + fib(n - 2)