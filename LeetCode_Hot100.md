# LeetCode Hot100题

### 1 哈希

#### 1.1 两数之和

#### 1.2 字母异位词分组

#### 1.3 最长连续序列

## 2 双指针

### 2.1 [移动零](https://leetcode.cn/problems/move-zeroes/description/?envType=study-plan-v2&envId=top-100-liked)

### 2.2 [盛最多水的容器](https://leetcode.cn/problems/container-with-most-water/solutions/94102/on-shuang-zhi-zhen-jie-fa-li-jie-zheng-que-xing-tu/?envType=study-plan-v2&envId=top-100-liked)

[经典题解](https://leetcode.cn/problems/container-with-most-water/solutions/94102/on-shuang-zhi-zhen-jie-fa-li-jie-zheng-que-xing-tu/?envType=study-plan-v2&envId=top-100-liked)

**思路：**双指针

**核心：** **缩减搜索空间**。
假设左边边长最短。右边向左移动时，根据右边边长会有如下两种情况：

1. 右边边长大于左边：有效边长仍为左边边长，而长度(j - i)缩短，容积减小
2. 右边边长小于左边：有效边长为右边边长，此时长度缩短，容积减小
   我们得到，如果一边边长不变，挪动另一边是徒劳的。此时可以排除该边下另一边的所有情况。

用一个矩阵可以更直观的展示：
(1) 初始时 i = 0, j = len - 1, 如果所有情况都遍历一遍，时间复杂度为O(n^2), 但本题最精妙的地方就在于此。

(2) 上述我们总结而得，如果某一边较短，另一边的所有情况均被排除。这里假设i=0时最短，在j=7时面积已最大，那么排除掉所有其他j的情况。i向下移动，即i++。

(3) 此时i=1, 计算此时新的最大容积。

比较两边边长，如果j侧较短，则排除所有其他i的情况。然后j向左移动，即j--

持续以上步骤，直到遍历完所有的情况，并输出最大值。

### 2.3 三数之和

**核心思路：**

1. 先排序：从小到大顺序，便于去重和剪枝
2. 双指针：大循环i遍历，双指针依次遍历求和，判断是否满足条件，注意双指针也要轮循去重
3. 满足条件时，每次构造新的vector，再通过push_back输出

**优化：**使用emplace_back + 初始化列表，直接构造一个对象，减少一次移动的操作；且初始化列表可以自动退到

```cpp
ans.emplace_back(initializer_list<int>{val1, val2, val3})
```

## 3 滑动窗口

样板：由双指针组成一个队列，右指针遍历时，根据特定规则将队列进行放缩。

常用滑动窗口队列：集合Set、队列queue

常用算法：定长窗口、不定长窗口（最多）

特征/步骤：

1. 右侧遍历：右指针元素入队
2. 队内检查：右侧入队后，循环遍历窗口元素，根据特定规则判断
3. 左侧出队：满足特定要求时，擦除左侧元素并移动左指针
4. 输出结果

### 3.1 [无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/)

滑动窗口的经典题型。

**核心思路：**

1. 无重复：使用unordered_set，保证窗口队列元素唯一；寻找元素: set.find(), 消除元素: set.erase(), 插入元素: set.insert()
2. 滑动窗口：双指针，右指针遍历的同时，检查当前元素是否已在队内
3. 排除重复元素：循环判断当前元素是否重复，是则擦除左指针并使其移动

### 3.2 [找到字符串中所有字母异位词](https://leetcode.cn/problems/find-all-anagrams-in-a-string/)

本题由 定长窗口、不定长窗口 两种解法。

#### 3.2.1 定长窗口解法

注意到：要求的子串是给定字符串p的异位词子串，故满足要求的子串长度 == p的长度

我们可以使用一个固定长度的滑动窗口，其逻辑非常简单直接。

**思路如下：**

1. 右侧入队
2. 检查子串和p是否一致，是则满足要求、记录结果
3. 左侧出队

**详细代码如下：**

```cpp
vector<int> findAnagrams(string s, string p) {
    // 我们定义两个数组，分别记录 子串 和 p 各个字母出现的次数
    array<int, 26> cnt_p;
    array<int, 26> cnt_temp;
    vector<int> ans;

    // 先记录p中各个字母出现的次数
    for (char ele : p)
    {
        cnt_p[ele - 'a']++;
    }

    // 定长滑动窗口遍历，右指针先进
    for (int right = 0; right < s.size(); right++)
    {
        // 右指针元素入队
        cnt_temp[s[right] - 'a']++;
        int left = right - p.size() + 1;
        if (left < 0)
        {
            continue;
        }

        // 当子串各元素次数与p中相等，即cnt_temp == cnt_p时，满足要求
        if (cnt_temp == cnt_p)
        {
            ans.push_back(left);
        }

        // 左指针元素出队
        cnt_temp[s[left] - 'a']--;
    }

    return ans;
}
```

#### 3.2.2 不定长窗口解法

这次我们使用常规的不定长窗口求解。思路按经典的不定长窗口解法来。

**思路：**

1. 遍历：右侧元素入队
2. 内部循环检查：子串内各字母次数是否大于p中次数，是则左指针右移
3. 满足要求条件：子串长度 == p的长度，则子串内所有字母次数等于p的次数（[有证明](https://leetcode.cn/problems/find-all-anagrams-in-a-string/solutions/2969498/liang-chong-fang-fa-ding-chang-hua-chuan-14pd/?envType=study-plan-v2&envId=top-100-likedhttps:/)）

**详细代码如下：**

```cpp
vector<int> findAnagrams(string s, string p) {
    array<int, 26> cnt_p;
    array<int, 26> cnt_temp;
    vector<int> ans;

    for (char ele : p)
    {
        cnt_p[ele - 'a']++;
    }

    int left = 0;
    for (int right = 0; right < s.size(); right++)
    {
         cnt_temp[s[right] - 'a']++;

         // 内循环检查，子串是否有元素出现过多，控制左指针右移
         while (cnt_temp[s[right] - 'a'] > cnt_p[s[right] - 'a'])
         {
             cnt_temp[s[left] - 'a']--;
             left++;
         }

         // 满足要求条件：子串和p的长度相等
         if (cnt_temp.size() == cnt_p.size())
         {
             ans.push_back(left);
         }
    }

    return ans;
}
```
