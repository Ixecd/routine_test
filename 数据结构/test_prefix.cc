#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    void getPrefix() {
        int n, m, a;
        cin >> n >> m;
        vector<vector<int>> prefix(n + 1, vector<int>(m + 1, 0));
        for (int i = 1; i <= n; ++i)
            for (int j = 1; j <= m; ++j)
                cin >> a, prefix[i][j] = prefix[i - 1][j] + prefix[i][j - 1] -
                                         prefix[i - 1][j - 1] + a;

        int ques;
        cin >> ques;
        while (ques--) {
            int x1, y1, x2, y2;
            cin >> x1 >> y1 >> x2 >> y2;
            cout << PrefixSum(x1 + 1, y1 + 1, x2 + 1, y2 + 1, prefix) << endl;
        }
    }

    int PrefixSum(int x1, int y1, int x2, int y2, vector<vector<int>>& prefix) {
        return prefix[x2][y2] - prefix[x1 - 1][y2] - prefix[x2][y1 - 1] + prefix[x1 - 1][y1 - 1];
    }
};

int main() { 
    
    Solution s1;
    s1.getPrefix();
    
    return 0; 
}
