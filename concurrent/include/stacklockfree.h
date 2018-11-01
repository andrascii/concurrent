#pragma once

namespace concurrent
{
	namespace lockfree
	{
		template <typename T>
		class stack
		{
		private:
			struct node
			{
				T data;
				node* next;

				node(const T& data)
					: data(data)
				{
				}

				node(T&& data)
					: data(std::move(data))
				{
				}
			};

		public:
			stack()
				: _head(nullptr)
				, _size(0)
			{
			}

			void push(const T& data)
			{
				node* newNode = new node(data);
				push_element_helper(newNode);
			}

			void push(T&& data)
			{
				node* newNode = new node(std::move(data));
				push_element_helper(newNode);
			}

			std::shared_ptr<T> pop()
			{
				node* oldHead = _head.load();
				while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next, std::memory_order_relaxed));

				std::shared_ptr<T> result;

				if (oldHead)
				{
					result = std::make_shared<T>(oldHead->data);
					_size.fetch_sub(1, std::memory_order_release);
				}

				return result;
			}

			unsigned size() const
			{
				return _size.load(std::memory_order_acquire);
			}

		private:
			void push_element_helper(node* newNode)
			{
				newNode->next = _head.load();
				while (_head.compare_exchange_weak(newNode->next, newNode, std::memory_order_relaxed));
				_size.fetch_add(1, std::memory_order_release);
			}

		private:
			std::atomic<node*> _head;
			std::atomic<unsigned> _size;
		};
	}
}