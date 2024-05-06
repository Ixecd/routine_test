#include <time.h>

#include <iostream>
#include <vector>

void test_timer() {
    srand(time(NULL));
    std::cout << rand() << " " << std::rand() << std::endl;
    std::cout << "RAND_MAX = " << RAND_MAX << std::endl;
}

// 分区函数
int partition(std::vector<int> &arr, int low, int high) {
    int pivot = arr[low];
    while (low < high) {
        while (low < high && arr[high] >= pivot) high--;
        arr[low] = arr[high];  // 此时枢轴的位置被覆盖
        while (low < high && arr[low] <= pivot) low++;
        arr[high] = arr[low];
    }
    if (low == high) arr[low] = pivot;
    // else std::cout << "low != high" << std::endl;
    return low;
}

void qSort(std::vector<int> &arr, int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);
        qSort(arr, low, pivot - 1);
        qSort(arr, pivot + 1, high);
    }
}

int Partition(std::vector<int> &arr, int low, int high) {
    int pivot = arr[low];
    while (low < high) {
        while (low < high && arr[high] >= pivot) high--;
        arr[low] = arr[high];
        while (low < high && arr[low] <= pivot) low++;
        arr[high] = arr[low];
    }
    if (low == high) arr[low] = pivot;
    return low;
}

void QSort(std::vector<int> &arr, int low, int high) {
    if (low < high) {
        int pivot = Partition(arr, low, high);

        QSort(arr, low, pivot - 1);
        QSort(arr, pivot + 1, high);
    }
}

int main() {
    // 6 7 11 5 12 13
    // std::vector<int> arr = {12, 7, 11, 13, 5, 6};
    // int n = arr.size();

    // QSort(arr, 0, n - 1);

    // std::cout << "Sorted array: ";
    // for (int i = 0; i < n; i++) {
    //     std::cout << arr[i] << " ";
    // }
    // std::cout << std::endl;
    test_timer();

    return 0;
}