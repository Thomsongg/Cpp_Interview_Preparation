#include <iostream>
#include <vector>
#include <queue>
#include <stack>	// 迭代中序遍历需要

using namespace std;

/*
 * 题目：LeetCode 94: 二叉树的中序遍历
 */

// 定义二叉树
struct TreeNode
{
	int val;
	TreeNode* left;
	TreeNode* right;
	TreeNode() : val(0), left(nullptr), right(nullptr) {}
	TreeNode(int value) : val(value), left(nullptr), right(nullptr) {}
	TreeNode(int value, TreeNode* leftNode, TreeNode* rightNode) : val(value), left(leftNode), right(rightNode) {}
};

class Solution
{
private:
	void dfsRecord(TreeNode* root, vector<int>& ans)
	{
		if (!root)
		{
			return;
		}

		// 中序遍历
		dfsRecord(root->left, ans);
		ans.emplace_back(root->val);
		dfsRecord(root->right, ans);
	}

public:
	// 算法：中序遍历 (DFS)
	vector<int> inorderTraversal(TreeNode* root)
	{
		if (!root)
		{
			return {};
		}

		TreeNode* curr = root;

		vector<int> ans;
		dfsRecord(curr, ans);

		return ans;
	}

	// 迭代求中序遍历
	vector<int> inorderTraversal_iteration(TreeNode* root)
	{
		TreeNode* curr = root;
		vector<int> ans;
		stack<TreeNode*> stk;

		// 中序遍历：左 -> 根 -> 右
		while (curr != nullptr || !stk.empty())
		{
			// 从最左侧开始遍历
			while (curr != nullptr)
			{
				stk.push(curr);
				curr = curr->left;
			}

			// 以当前节点为根，向右遍历
			curr = stk.top();
			stk.pop();
			ans.emplace_back(curr->val);
			curr = curr->right;
		}

		return ans;
	}
};

// 辅助函数
// 二叉树的创建：层序构建（BFS）
TreeNode* createBinaryTree(const vector<int>& nodes)
{
	if (nodes.size() == 0 || nodes[0] == -1)
	{
		return nullptr;
	}

	queue<TreeNode*> queue_nodes;
	TreeNode* root = new TreeNode(nodes[0]);
	queue_nodes.push(root);

	// BFS 层序构建：根节点出队，再构建下一层的左、右两个方向
	int i = 1;
	while (!queue_nodes.empty() && i < nodes.size())
	{
		// 上一层根节点
		TreeNode* curr = queue_nodes.front();
		queue_nodes.pop();

		// 构建左节点，null 除外
		if (i < nodes.size() && nodes[i] != -1)
		{
			curr->left = new TreeNode(nodes[i]);
			queue_nodes.push(curr->left);
		}
		i++;

		// 构建右节点
		if (i < nodes.size() && nodes[i] != -1)
		{
			curr->right = new TreeNode(nodes[i]);
			queue_nodes.push(curr->right);
		}
		i++;
	}

	return root;
}

// 删除二叉树（递归）
void deleteTree(TreeNode* root)
{
	if (!root)
	{
		return;
	}

	deleteTree(root->left);
	deleteTree(root->right);

	delete root;
}

int main()
{
	Solution sol;

	// 用 -1 代表 null
	vector<int> nodes_vec = {1, -1, 3, 2};

	TreeNode* root = createBinaryTree(nodes_vec);

	vector<int> ans;
	ans = sol.inorderTraversal_iteration(root);

	cout << "Ret: [";

	for (const int& num : ans)
	{
		cout << " " << num << " ";
	}

	cout << "]\n";

	deleteTree(root);

	return 0;
}
