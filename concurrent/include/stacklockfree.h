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

			struct CountedReference
			{
				int externalCounter;
				Node* pointer;
			};

			struct Node
			{
				Node(const std::shared_ptr<T>& data)
					: data(data)
					, next(nullptr)
					, internalCounter(0)
				{
				}

				std::shared_ptr<T> data;
				CountedReference next;
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
				std::unique_ptr<Node> node = std::make_unique<Node>(std::make_shared<T>(data));
				node->next = m_head.load();

				CountedReference newCountedPointer;
				newCountedPointer.externalCounter = 1;
				newCountedPointer.pointer = node.get();

				while (!m_head.compare_exchange_weak(node->next, newCountedPointer));

				node.release();
			}

			std::shared_ptr<T> pop()
			{
				CountedReference oldHead = m_head.load();

				for (;;)
				{
					increaseHeadExternalCounter(oldHead);

					while (m_head.compare_exchange_strong(oldHead, oldHead.pointer->next));
					{
						std::shared_ptr<T> result;
						result.swap(oldHead.pointer->data);

						freeNode(oldHead);

						return result;
					}

					oldHead.pointer->internalCounter.fetch_sub(1);
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
				while (!m_head.compare_exchange_weak(oldHead, increasedCounterNode));

				oldHead = increasedCounterNode;
			}

			void freeNode(CountedReference& node)
			{
				//node.pointer->internalCounter.fetch_sub(1);
			}

		private:
			std::atomic<CountedReference> m_head;
		};
	}
}