#include <iostream>
#include <vector>
#include <memory>

using namespace std;

/*
 * 题目：LeetCode : 反转链表2
 * 使用智能指针 unique_ptr 实现
 */

// 链表结构体
// 节点使用 unique_ptr 管理
struct ListNode
{
	int val_;
	unique_ptr<ListNode> next_;

	ListNode(int val) : val_(val), next_(nullptr) {}
};

class Solution {
public:
	unique_ptr<ListNode> reverseBetween(unique_ptr<ListNode> head, int left, int right) {
		if (!head || left >= right) return head;

		auto dummy = make_unique<ListNode>(0);
		dummy->next_ = move(head);

		ListNode* prev = dummy.get();
		for (int i = 0; i < left - 1; i++) {
			if (prev->next_) prev = prev->next_.get();
		}

		// 【注意】此时 prev_next_ 可能为空，会导致 move(prev->next_) 失效
		//unique_ptr<ListNode> cur = move(prev->next_);		// 错误示范
		ListNode* cur = prev->next_.get();

		for (int i = 0; i < right - left; i++) {
			if (cur && cur->next_) {
				unique_ptr<ListNode> next_node = move(cur->next_);

				if (!next_node)
				{
					break;
				}

				cur->next_ = move(next_node->next_);
				next_node->next_ = move(prev->next_);
				prev->next_ = move(next_node);
			}
		}

		return move(dummy->next_);
	}
};

// 辅助函数
unique_ptr<ListNode> createList(const vector<int>& vec)
{
	auto dummy = make_unique<ListNode>(0);

	// 通过原始指针动态创建节点
	ListNode* cur = dummy.get();
	for (auto num : vec)
	{
		cur->next_ = make_unique<ListNode>(num);
		cur = cur->next_.get();
	}

	return std::move(dummy->next_);
}

// 打印链表
void printList(const ListNode* head)
{
	while (head)
	{
		cout << head->val_ << (head->next_ ? " -> " : "");
		head = head->next_.get();
	}
	cout << " -> nullptr\n";
}


int main()
{
	vector<int> nums{1, 2, 3, 4, 5, 6};
	int left = 1, right = 5;

	auto head = createList(nums);

	Solution sol;
	auto head2 = sol.reverseBetween(move(head), left, right);

	printList(head2.get());

	return 0;
}
