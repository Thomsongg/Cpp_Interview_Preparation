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
