#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

/*
 * 题目：LeetCode 146: LRU 缓存
 */

class LRUCache
{
	using CacheNode = pair<int, int>;

	size_t capacity_;

	// 双向链表
	list<CacheNode> cache_list_;

	// 哈希表
	unordered_map<int, list<CacheNode>::iterator> key_to_iter_;

public:
	explicit LRUCache(size_t capacity) : capacity_(capacity)
	{

	}

	// 禁用拷贝构造和赋值运算符
	LRUCache(LRUCache& other) = delete;
	LRUCache& operator=(LRUCache& other) = delete;

	// 查询
	int get(int key)
	{
		// 查到则更新，查不到返回 -1
		auto it = key_to_iter_.find(key);
		if (it == key_to_iter_.end())
		{
			return -1;
		}

		// 更新并返回查询结果
		cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
		return it->second->second;
	}

	// 存入
	void put(int key, int value)
	{
		// 查询
		// 查到则更新值
		auto it = key_to_iter_.find(key);
		if (it != key_to_iter_.end())
		{
			it->second->second = value;
			cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
			return;
		}

		// 查不到则压入
		// 空间已满，则删除最旧的值
		if (cache_list_.size() >= capacity_)
		{
			// 最旧的值位于链表末尾
			int key_to_remove = cache_list_.back().first;
			key_to_iter_.erase(key_to_remove);
			cache_list_.pop_back();
		}

		// 存入新元素
		cache_list_.emplace_front(key, value);
		key_to_iter_[key] = cache_list_.begin();
	}
};

int main()
{

	return 0;
}
