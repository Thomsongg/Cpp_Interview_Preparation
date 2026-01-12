// LeetCode 200: 岛屿数量
#include <iostream>
#include <vector>

using namespace std;

class Solution
{
public:
    int ans = 0;
    int numIslands(vector<vector<char>>& grid)
    {
        int m = grid.size(), n = grid[0].size();
        if (m == 0 || n == 0)
        {
            return 0;
        }

        // 主干部分：每个为 '1' 的位置都尝试一遍
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (grid[i][j] == '1')
                {
                    dfs(i, j, grid, m, n);
                    ans++;
                }
            }
        }
    }
private:
    void dfs (int row, int col, vector<vector<char>>& grid, int& m, int& n)
    {
        // 回溯条件：走到海水 / 越界 / 路已走过
        if (row < 0 || row >= m || col < 0 || col >= n || grid[row][col] != '1')
        {
            return;
        }

        // 走过的路做标记
        grid[row][col] = '\0';

        // 四处走
        dfs(row - 1, col, grid, m, n);
        dfs(row + 1, col, grid, m, n);
        dfs(row, col - 1, grid, m, n);
        dfs(row, col + 1, grid, m, n);
    }
};

int main()
{
    Solution sol;
    vector<vector<char>> test_instance = { 
        {'1','1','0','0','0'},
        {'1','1','0','0','0'},
        {'0','0','1','0','0'},
        {'0','0','0','1','1'}
    };

    sol.numIslands(test_instance);

    std::cout << "Result: " << sol.ans;
}
