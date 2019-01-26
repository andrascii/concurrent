#pragma once

namespace concurrent { namespace lockfree {

template <typename T>
class Stack final
{
private:
	struct Node;

	struct CountedReference
	{
		int externalCounter;
		Node* pointer;
	};

	struct Node
	{
		Node(const T& data)
			: data(std::make_shared<T>(data))
			, next{}
			, internalCounter(0)
		{
		}

		Node(T&& data)
			: data(std::make_shared<T>(std::move(data)))
			, next{}
			, internalCounter(0)
		{
		}

		std::shared_ptr<T> data;
		CountedReference next;
		std::atomic<int> internalCounter;
	};

public:
	Stack()
		: m_head{}
	{
		std::cout << "CountedReference is lock free: " << m_head.is_always_lock_free << std::endl;
	}

	~Stack()
	{
		while (pop());
	}

	void push(T data)
	{
		CountedReference newCountedPointer;
		newCountedPointer.externalCounter = 1;
		newCountedPointer.pointer = new Node(std::move(data));
		newCountedPointer.pointer->next = std::atomic_load_explicit(&m_head, std::memory_order_relaxed);
		while (!m_head.compare_exchange_weak(newCountedPointer.pointer->next, newCountedPointer, std::memory_order_release, std::memory_order_relaxed));
	}

	std::shared_ptr<T> pop()
	{
		CountedReference oldHead = std::atomic_load_explicit(&m_head, std::memory_order_relaxed);

		for (;;)
		{
			increaseHeadExternalCounter(oldHead);

			// because in case of CAS failure oldHead will be changed
			// but we need to decrement internal counter for this node
			// because we already incremented external counter for it
			Node* oldPointer = oldHead.pointer;

			if (oldHead.pointer == nullptr)
			{
				return std::shared_ptr<T>(nullptr);
			}

			if (m_head.compare_exchange_strong(oldHead, oldHead.pointer->next, std::memory_order_relaxed, std::memory_order_relaxed))
			{
 				std::shared_ptr<T> result;
 				result.swap(oldHead.pointer->data);

				const int decrementValue = oldHead.externalCounter - 2;

				if (std::atomic_fetch_add_explicit(&oldPointer->internalCounter, decrementValue, std::memory_order_release) == -decrementValue)
				{
					delete oldHead.pointer;
				}

				return result;
			}
			else if (std::atomic_fetch_add_explicit(&oldPointer->internalCounter, -1, std::memory_order_relaxed) == 1)
			{
				std::atomic_load_explicit(&oldPointer->internalCounter, std::memory_order_acquire);
				delete oldHead.pointer;
			}
		}
	}

private:
	void increaseHeadExternalCounter(CountedReference& oldHead)
	{
		CountedReference increasedCounterNode;

		do
		{
			increasedCounterNode = oldHead;
			++increasedCounterNode.externalCounter;
		}
		while (!m_head.compare_exchange_strong(oldHead, increasedCounterNode, std::memory_order_acquire, std::memory_order_relaxed));

		oldHead = increasedCounterNode;
	}

private:
	std::atomic<CountedReference> m_head;
};

}

}