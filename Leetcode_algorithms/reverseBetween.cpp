#include<iostream>
#include<vector>

using namespace std;

struct ListNode
{
	int val;
	ListNode* next;
	ListNode() : val(0), next(nullptr) {}
	ListNode(int value) : val(value), next(nullptr) {}
	ListNode(int value, ListNode* nxt) : val(value), next(nxt) {}
};

class Solution
{
public:
	ListNode* reverseBetween(ListNode* head, int left, int right)
	{
		// 边界条件
		if (head == nullptr || left == right)
		{
			return head;
		}

		// 创建一个哨兵节点 dummy；使用堆内存创建，生命周期结束后自动销毁
		// dummy 后接上头节点 head
		ListNode dummy(0);
		dummy.next = head;

		// 找到 left 前一个节点 prev, 后一个节点即 
		ListNode* prev = &dummy;
		for (int i = 0; i < left - 1; i++)
		{
			prev = prev->next;
		}

		ListNode* curr = prev->next;

		// 将 (left, right) 范围内的节点反转
		ListNode* next_node = nullptr;
		for (int i = 0; i < right - left; i++)
		{
			next_node = curr->next;

			curr->next = next_node->next;
			next_node->next = prev->next;
			prev->next = next_node;
		}

		return dummy.next;
	}
};

// 辅助函数：创建链表、打印链表、销毁链表
// 
// 创建链表：通过 vector<int> 创建
ListNode* creatList(const vector<int>& nums)
{
	if (nums.size() == 0)
	{
		return nullptr;
	}

	// 在栈内存上(可自动销毁) 创建头节点，而非哨兵节点
	// 用 nums[0] 创建头节点，后续 i 从 1 开始创建
	ListNode dummy(0);
	ListNode* curr = &dummy;

	// 从头节点开始，逐个创建节点，并连线
	for (int val : nums)
	{
		curr->next = new ListNode(val);
		curr = curr->next;
	}

	return dummy.next;
}

// 打印链表
void printList(ListNode* head)
{
	if (head == nullptr)
	{
		return;
	}

	ListNode* curr = head;
	while (curr)
	{
		cout << curr->val;
		if (curr->next)
		{
			cout << "->";
		}
		curr = curr->next;
	}

	cout << "->nullptr\n";
}

// 删除链表（创建后必须删除）
void deleteList(ListNode* head)
{
	if (head == nullptr)
	{
		return;
	}

	while (head)
	{
		// 注意先经过下一个，再销毁节点
		ListNode* temp = head;

		// 防止删除哨兵节点
		head = head->next;
		delete temp;
	}
}

int main()
{
	// 准备测试用例
	vector<int> testList = { 1, 2, 3, 4, 5 };
	int left = 2, right = 4;

	// 按 vector 创建一个链表
	ListNode* head = creatList(testList);

	// 先打印原链表作对比
	printList(head);

	// 创建 Solution 实例（栈内存 可自动销毁），调用算法
	Solution sol;

	ListNode* newHead = sol.reverseBetween(head, left, right);

	// 再打印反转后的链表
	printList(newHead);

	// 将其销毁
	deleteList(newHead);

	return 0;
}
