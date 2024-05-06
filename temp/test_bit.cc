#include <bits/stdc++.h>
using namespace std;

unsigned int countZeroBits(unsigned int n) {
    unsigned int count = 0;
    while (n) {
        if (n == 1) break;
        if (n & 1) {
            n >>= 1;
            continue;
        }
        n |= (n + 1);
        if (n & 1) ++count;
        n >>= 1;
    }
    return count;
}

unsigned int countOneBits(unsigned int n) {
    unsigned int count = 0;
    while (n) {
        // n = 100 n - 1 = 011 -> n &= (n - 1)-> 0
        n &= (n - 1);
        count++;
    }
    return count;
}
int main() {
    cout << countZeroBits(5) << endl;  // 101
    return 0;
}

int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[low];
    while (low < high) {
        while (low < high && arr[high] >= pivot) high--;
        arr[low] = arr[high];
        while (low < high && arr[low] <= pivot) low++;
        arr[high] = arr[low];
    }
    arr[low] = pivot;
    return low;
}

void qsort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);

        qsort(arr, low, pivot - 1);

        qsort(arr, pivot + 1, high);
    }
}