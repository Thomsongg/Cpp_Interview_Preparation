#include <iostream>
#include <string>
#include <unordered_set>

using namespace std;

/*
 * 题目：LeetCode 3: 无重复最长子串
 */

class Solution
{
public:
	int lengthOfLongestSubstring(string s)
	{
		// 边界条件
		if (s.size() < 1)
		{
			return 0;
		}

		// 滑动窗口法
		// 使用 unordered_set 检查窗口内是否有重复元素
		unordered_set<char> uset;
		int ans = 0;
		int left = 0;

		for (int right = 0; right < s.size(); right++)
		{
			while (uset.count(s[right]) > 0)
			{
				uset.erase(s[left++]);
			}

			uset.emplace(s[right]);
			ans = max(ans, right - left + 1);
		}

		return ans;
	}
};

int main()
{
	Solution sol;

	// 准备测试用例
	string test_str = "bbbbb";
	cout << "input string: " << test_str << "\n";

	int ret = 0;
	ret = sol.lengthOfLongestSubstring(test_str);

	cout << "Ret: " << ret << "\n";

	return 0;
}
