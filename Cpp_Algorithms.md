# 算法题复习笔记
## 一、链表
#### 10.15： 反转链表、判断回文串、合并两个有序链表、相交链表
1. 反转链表：迭代，直接套模板
    
```cpp
ListNode* reverse(ListNode* node)
{
    if (!node || !node->next)
    {
        return node;
    }

    // 设置反转的逻辑
    ListNode* temp = reverse(head);
    node->next->next = node;
    node->next = nullptr;
    
    return temp;
}
```
2. 判断回文串：
    (1) 栈操作：利用栈模拟反向遍历。先遍历列表入栈，再遍历并与校验是否与栈顶元素相同
    (2) 快慢指针：慢走1、快走2，让慢指针到达中心(注意链表奇偶性)；再用头尾指针(尾指针使用反转链表操作)，分别正向、反向读取并校验
3. 合并两个有序链表:一个为空时接上另一个；全不为空时按值的大小走较小值
    (1) 常规方法：双指针；全不为空时，接入对应链表节点
    (2) 复杂度小：递归；全不为空时，用下一个待接入的链表节点获取下个结果节点

```
方法1：
while (l1 && l2)
{
    if (l1->val < l2->val)
    {
        pre->next = l1;
        l1 = l1->next;
    }
    else
    {
        pre->next = l2;
        l2 = l2->next;
    }
    pre = pre->next;
}

if (!l1)
{
    pre->next = l2;
}
if (!l2)
{
    pre->next = l1;
}

```

```
方法2：
if (l1-val < l2->val)
{
    cur->next = function(l1->next, l2)
}
// 其他情况同理

```
4. 相交链表：双指针，各自走完自己的路后再走另一条路；相交时走过的距离固定为x+y+z,其中z为公共部分长度

#### 10.16： 环形链表(I/II)、删除链表倒数的第n个节点
1. 环形链表：快慢指针
    (1) 快慢指针先第一次相遇，判断是否有还
    (2) 牢记 a=c，即让快指针回起点，两者同时走1格，下一次相遇的节点必为环节点
2. 删除倒数节点：双指针(倒数类型的题目)
    (1) 快指针比慢指针提前n个节点
    (2) 一起走，直到快指针走到NULL
    (3) 此时慢指针的下一个节点即为倒数第n个节点

#### 10.17： 两两交换链表中的节点
1. 两两交换：链表节点的操作、两数相加
    (1) 创建首节点，之后两两交换；
    (2) 交互时注意操作顺序：如何把0,1,2 变为 0,2,1; 相邻两个节点，即cur->next 和 cur->next->next，存在；若不存在，则还剩一节点或全部遍历完(链表奇偶性)，无需操作
    (3) 注意返回首节点而非head
```
ListNode* temp = new ListNode(0);
temp->next = head;
ListNode* realHead = temp;
while (temp->next && temp->next->next)
{
    ListNode* l1 = temp->next;
    ListNode* l2 = temp->next->next;
    // 两两交换
    temp->next = l2;
    l1->next = l2->next;
    l2->next = l1;
    // 注意交换后 后移两步
    temp = temp->next->next;
}
return realHead->next;
```
    
2. 两数相加：核心是计算两数的和，要加上进位！！！
    (1) 从两链表同时出发，计算每一次相加的和；有一个链表为空，就从另一个链表出发
    (2) 求和结果 sum = l1->val + l2->val + carry;carry为进位, 即 carry = sum / 10
    (3) 降低耗时的关键: 不用if拦截, 用while循环, 几种情况分别循环！
    (4) 最后记得把进位加到链表末尾

```
while (l1 && l2)
{
    sum = l1->val + l2->val + carry;
    // 创建新节点，存放 sum % 10
    ......
}
while (l1)
{
    sum = l1->val + carry;
    ......
}
while (l2)
{
    sum = l2->val + carry;
    ......
}
if (carry)
{
    // 存放进位
}
```
3. 两数相加II：注意链表是高位到低位存储
    (1) 利用栈，先将元素入栈，再逐个取栈顶元素，实现低位到高位的求和；取栈顶: s.top(); 入栈: s.push(); 出栈: s.pop()
    (2) 其余步骤和<两数相加I>类似
    (3) 注意遍历栈时，结果链表指针不断向前
    (4) 得到的结果链表是低位到高位，需要反转后输出；注意反转链表的逻辑不要忘！！！；结果链表的头节点为空，先取next后再反转

#### 10.21： 删除排序链表中的重复元素II
思路：双指针遍历；
1. 创建一个头节点，接上原链表head，便于遍历
2. 右指针遍历，跳过所有重复元素(cur->next->val == cur->val)
3. 跳过后，注意cur可能仍为最后一个重复的元素。此时判断前一个元素是否重复(pre->next == cur)
    3.1 重复则将其跳过，pre与cur->next相连
    3.2 不重复，则将pre指向cur的位置
代码如下：

```
ListNode* dummy = new ListNode(0);
dummy->next = head;
ListNode* pre = dummy;
ListNode* cur = dummy->next;

// 遍历过程
while (cur)
{
    // 跳过所有两两重复的元素
    while (cur->next && cur->next->val == cur->val)
    {
        cur = cur->next;
    }
    // 前一个元素如果重复则跳过，否则刷新 pre 的位置
    if (pre->next == cur)
    {
        pre->next = cur->next;
    }
    else
    {
        pre = cur;
    }
    cur = cur->next;
}
// 返回真正的头节点
return dummy->next;
```
## 二、二叉树
基础：(1) 前、中、后序遍历; (2) 二叉树的构造
### 1 二叉树的遍历
#### 10.22： 翻转二叉树、路径总和(I & II)、填充每个节点的下一个右侧节点指针(I & II)、二叉树展开为链表
1. 翻转二叉树：前序遍历
    (1) 翻转二叉树，交换左右叶子节点
    (2) 前序遍历二叉树

2. 路径总和(I & II)：前序遍历
    (1) 记录走过的节点，求和
    (2) 走完一条路，即到达叶子节点时，判断求和是否满足要求并记录结果

```
// 路径总和II:
vector<vector<int>> ret;
vector<int> tmp;
void dfs(TreeNode* root, int sum)
{
    // 回溯的前提条件
    if (!root)
    {
        return;
    }

    // 可直接用入参做减法
    sum -= root->val;
    tmp.push_back(root->val);
    if (!root->left && !root->right && sum == 0)
    {
        ret.emplace_back(tmp);
    }

    // 前序遍历
    dfs(root->left, sum);
    dfs(root->right, sum);

    // 回溯时出队
    tmp.pop_back();
}
```

3. 填充右侧节点指针(I & II)：
    (1) 完全二叉树：采用前序遍历
    (2) 非完全二叉树，部分只存在右子节点：后序遍历

4. 二叉树展开为链表：
    (1) 先前序遍历到末尾，回溯时操作
    (2) 将右子树断链接左节点，原右节点接到左子树最末端
    (3) 左子树最末端 按tmp->right循环遍历得到

### 2 二叉树的构造
#### 10.23： 最大二叉树、前序和中序(中序和后序)构造二叉树
1. **最大二叉树：**最基本的二叉树构造题。思路：找出最大值，分别向左、右构造子树；注意构造时起点和终点的位置

2. **【重要】前序和中序构造二叉树：**思路与上题类似；定义前序、中序的头、尾分别为 preStart, preEnd, inStart, inEnd
    (1) 前序遍历，首元素即为根节点；
    (2) 中序遍历，根节点为中间元素，左右分别为待构造的子树，即 (inStart, index) 和 (index + 1, inEnd)
    (3) 前序遍历，左子树到实际根节点结束，需要找到根节点在中序的位置。设左子树空间为leftSize，则前序的左右区间分别为 (preStart, preStart + leftSize) 和 (preStart + leftSize + 1, preEnd)，其中 leftSize = index - inEnd, index为根节点在中序的位置
    (4) 回溯的条件: inStart >= inEnd

3. 中序和后续构造二叉树：思路与前、中序相同，找到根节点在中序的位置，计算后序的偏移量；不做赘述

#### 10.24： 寻找重复的子树、监控二叉树(困难)、最大路径和(困难)
1. **寻找重复的子树：**
    思路：对每个节点，找自己、找别人
    (1) 找自己：后序遍历，拼接出完整的子树，即 左、右、根
    (2) 找别人：每次遍历时，记录树结构，出现次数+1
    核心：使用HashMap存储树结构；题目要求输出的子树不重复，用次数 == 1 判断

```
string dfs(TreeNode* root)
{
    if (!root)
    {
        return "";
    }

    // 拼接出完整的树结构
    string leftVal = dfs(root->left);
    string rightVal = dfs(root->right);
    string tree = leftVal + "," + rightVal + "," + to_string(root->val);

    if (hashMap[tree] == 1)
    {
        ret.push_back(tree);
    }

    hashMap[tree]++;
    return tree;
}
```
2. **监控二叉树：**
    思路：贪心算法，通过子节点推测根节点状态，故后序遍历，使监控数量最少，从下往上推测
    (1) 子节点都被监控到 -> 父节点不需要监控 -> 父节点未被监控
    (2) 任一子节点未被监控 -> 父节点需要摄像头，数量+1
    (3) 任一子节点有摄像头 -> 父节点已被监控
    注意：空节点视为被监控到，防止干扰

3. **最大路径和：**
    思路：观察局部，每个根节点路径和由三部分组成，根节点值、左子树路径和、右子树路径和
    (1) 每个根节点子树内部的最大路径和 = 根节点值 + 左子树+右子树
    (2) 当前根节点最终的最大路径和 = 根节点值 + 左右子树路径和中的最大值；用于递归给上层根节点使用
    (3) 某个根节点为负，则直接记为0忽略掉 -> 某条子树路径和为负，计为0


```
int ret = 0;
int dfs(TreeNode* root)
{
    if (!root)
    {
        return 0;
    }

    int left = dfs(root->left);
    int right = dfs(root->right);

    // 计算当前根节点所有子树内部的最大值
    int internMax = root->val + left + right;
    ret = max(internMax, ret);

    // 计算当前根节点最终路径的最大值
    int curMax = root->val + max(0, max(left, right));
    if (curMax < 0)
    {
        return 0;
    }
    return curMax;
}

```

#### 10.25： 最近的公共祖先、二叉树的层序遍历
1. 最近的公共祖先
    思路：找节点的公共祖先，适合从下往上找 -> 前序遍历，从局部推广至整体
    通过子树中是否存在要求的节点p,q，考虑根节点root
    (1) 遍历到根节点，返回null
    (2) 考虑局部：节点自身也是自己的公共祖先-> 遍历到p或q时，根节点root即为p,q，此时返回root
    (3) 考虑整体：节点左右子树是否包含p,q，影响了root的情况：
        <1> 递归的左右子树left都没有p,q(即null)，root不是公共祖先，返回null
        <2> 左右子树其一不为null，则该子树包含节点，root返回该子树left/right

代码：

```
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) 
{
    if (!root | root == p || root == q)
    {
        return root;
    }

    // 前序遍历
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);

    // 根据子树推断根节点是否为公共祖先
    if (!left)
    {
        return right;
    }

    if (!right)
    {
        return left;
    }

    // 左右子树都没有p,q 按null处理
    if (!left && !right)
    {
        return nullptr;
    }

    return root;
}
```

2. 层序遍历二叉树(BFS 广度优先)

#### 【二叉搜索树(BST)】
**特点：**满足左节点 < 根节点 < 右节点，且每个子树都为BST
**注意：**BST的中序遍历一定是升序！！！
**常用技巧：**中序遍历(中序可实现升序排序，根据题目特点选择先左后右 或 先右后左)、BST的增删改查
**搜索二叉搜索树指定节点：**使用递归遍历，模板如下

```
TreeNode* search(TreeNode* root, int val)
{
    // 到达底层叶子节点后回溯
    if (!root)
    {
        return nullptr;
    }
    // 找到节点，执行特定逻辑
    if (root->val == val)
    {
        // 执行逻辑...
    }
    // 利用BST子树特点，通过递归向左/右 搜索
    if (val < root->val)
    {
        return search(root->left, val);
    }
    else
    {
        return search(root->right, val);
    }
    return nullptr;
}
```

#### 10.29： 二叉搜索树中的搜索、二叉搜索树中第K小的元素、把二叉搜索树转换为累加树
1. 二叉搜索树中的搜索
    思路：套用搜索模板，利用BST每个子树的特性，根据当前值与root-val的对比，选择向左或向右递归遍历
2. 二叉搜索树中第K小的元素
    思路：利用BST中序遍历是升序的特点，从小到大寻找第k个元素
3. 把二叉搜索树转换为累加树
    思路：发现从右到左累加，可利用中序遍历，从右向左递归

```
int curSum = 0;
TreeNode* convertBST(TreeNode* root)
{
    if (!root)
    {
        return nullprt;
    }
    // 从右向左的中序遍历
    convertBST(root->right);
    // 处理逻辑：计算累加和，作为当前节点值
    root->val += sum;
    sum = root->val;
}
```

#### 10.30： 验证二叉搜索树、二叉搜索树中的插入操作、删除二叉搜索树中的节点
1. 验证二叉搜索树
    思路：通过中序遍历，检查节点值是否为升序；先定义一个值为最小值 (long long) INT_MIN - 1，遍历过程中不断刷新
2. 二叉搜索树中的插入操作
    思路：和BST的查找类似，注意插入操作要在遍历到底层叶子节点时进行，代码如下

```
TreeNode* insertNode(TreeNode* root, int val)
{
    // 在底层叶子节点添加
    if (!root)
    {
        return new TreeNode(val);
    }
}
```
3. 删除二叉搜索树中的节点
    思路：总体上按BST的搜索模板；难点在于找到后删除指点的节点。
    删除指定节点：
    (1) 无左右子节点：直接返回null，删除
    (2) 只包含一个子节点：返回另一个子树即可
    (3) 同时包含左右节点：保证待删除后，新的子树仍为BST
    方法：
    <1> 从当前节点(root)的左子树中寻找，找到左子树中的最大值，成为新的root，按BST拼接；或
    <2> 从当前节点(root)的右子树中寻找，找到右子树中的最小值，替代root，并按BST拼接
    拼接完成后，返回拼接后root的子树即可
    以方法<1>为例：
    ![](./images/image.png)

```
TreeNode* deleteNode(TreeNode* root, int key)
{
    // 其他部分的逻辑省略...
    if (root->val == key)
    {
        // 省略其他部分
        if (root->left && root->right)
        {
            // 向右遍历，寻找左子树的最大值
            TreeNode* temp = root->left;
            while (temp->right)
            {
                temp = temp->right;
            }
            // root子节点拼接到左子树，保证BST
            temp->right = root->right;

            // 返回删除节点的左子树
            return root->left;
        }
    }
}
```

#### 11.3： 不同的二叉搜索树(I&II)、二叉树的右视图
1. 不同的二叉搜索树(I)
    思路：只需计算总数，采用动态规划
    总数量G(n) = sigma(f(i)), i = 1, ..., n
    每个节点的子树数量f(i) = G(i-1) * G(n-i)

2. 不同的二叉搜索树(II)
    思路：对不同的根节点，每次构造不同的BST子树；先获取左、右子树列表(如[1,2]和[4,5])，在确定左子树时遍历右子树节点，每次构造子树；
    注意：(1) 根节点的创建要在构造子树时进行！(2) 当前子树为空时，要加上null，防止左右子树节点列表为空，影响下次构造

```
TreeNode* construct(int start, int end)
{
    vector<TreeNode*> curTree;
    if (start > end)
    {
        // 注意当前子树为空时，要加null
        // 若不加null，下次获取的左右子树节点列为空，无法构造新子树
        curTree.push_back(nullptr);
        return curTree;
    }

    // 遍历构造，注意起点、终点的索引
    for (int i = start; i <= end; i++)
    {
        vector<TreeNode*> leftList = construct(start, i - 1);
        vector<TreeNode*> rightList = construct(i + 1, end);

        // 用左右子树节点列表，构造不同的左右子树
        // 固定左子树时，遍历右子树
        for (auto left : leftList)
        {
            for (auto right : rightList)
            {
                // 注意！在此创建根节点
                TreeNode* root = new TreeNode(i);
                root->left = left;
                root->right = root->right;
                curTree.push_back(root);
            }
        }
    }

    return curTree;
}

```

## 三、数组&字符串
