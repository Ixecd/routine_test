#include <bits/stdc++.h>
#include <format>
using namespace std;

int main() {

    int number = 10;
    format_to(cout, "number is {}\n", move(number));

    return 0;
}