#include <bits/stdc++.h>

#include <bitset>  // for bit map

using namespace std;

// 在数据量为40亿的数据中,快速判断一个数是否存在
const uint Sum = (uint)4e9 + 1;

bitset<Sum> bitmap;

void test() {
    bitmap.set(1);
    bitmap.set(2);
    bitmap.set(3);
    cout << bitmap.test(1) << '\n';
}

int main() {
    test();
    //cout << bitmap << endl;

    return 0;
}