#include <bits/stdc++.h>
#include <chrono>
#include <thread>
using namespace std;


class LRUCache {
public:
    int get(int k);

    int put(int k); 
private:
    int capacity;
    // mylists
    list<int> lruLists;
    // key -> pair<value, time>
    unordered_map <int, pair<int, chrono::system_clock::time_point>> cache;
};