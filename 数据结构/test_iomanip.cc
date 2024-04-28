#include <iomanip>
#include <iostream>

int main() {
    double num = 30.1415926;
    // std::setprecision(4) 一共4位数
    std::cout << std::setprecision(5) << num << std::endl;

    // 使用std::fixed和 std::setprecision(2)来控制输出到小数点后多少位
    std::cout << std::fixed << std::setprecision(2) << num << std::endl;

    std::cout << std::setprecision(3) << num << std::endl;

    return 0;
}