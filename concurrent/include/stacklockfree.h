#pragma once

namespace concurrent
{
	namespace lockfree
	{
		template <typename T>
		class Stack final
		{
		private:
			struct Node;

			struct CountedRefs
			{
				int externalCounter;
				Node* pointer;
			};

			struct Node
			{
				Node(const T& data)
					: data(data)
					, next(nullptr)
					, internalCounter(0)
				{
				}

				T data;
				CountedRefs next;
				std::atomic<int> internalCounter;
			};

		public:
			Stack()
			{
				// set start value of m_head
			}

			~Stack()
			{
			}

			void push(const T& data)
			{
				std::unique_ptr<Node> node = std::make_unique<Node>(data);
				node->next = m_head.load();

				CountedRefs newCountedPointer;
				newCountedPointer.externalCounter = 1;
				newCountedPointer.pointer = node.get();

				while (!m_head.compare_exchange_weak(node->next, newCountedPointer));
			}

			std::shared_ptr<T> pop()
			{
				CountedRefs oldHead = m_head.load();

				for (;;)
				{
					increaseHeadExternalCounter(oldHead);

					while (!m_head.compare_exchange_weak(oldHead, oldHead.pointer->next));

					oldHead.pointer->internalCounter.fetch_sub(1);
				}
			}

		private:
			void increaseHeadExternalCounter(CountedRefs& oldHead)
			{
				CountedRefs increasedCounterNode;

				do
				{
					increasedCounterNode = oldHead;
					++increasedCounterNode.externalCounter;
				}
				while (!m_head.compare_exchange_weak(oldHead, increasedCounterNode));

				oldHead = increasedCounterNode;
			}

			void freeNode(CountedRefs& node)
			{
				//node.pointer->internalCounter.fetch_sub(1);
			}

		private:
			std::atomic<CountedRefs> m_head;
		};
	}
}