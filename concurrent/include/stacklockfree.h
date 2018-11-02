#pragma once

namespace concurrent
{
	namespace lockfree
	{
		template <typename T>
		class Stack final
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

			static std::thread::id _currentDestroyThreadId;

		public:
			Stack()
				: _head(nullptr)
				, _forPendingDelete(nullptr)
				, _size(0)
				, _threadsInPop(0)
			{
			}

			~Stack()
			{
				deleteNodesChain(_head.load(std::memory_order_relaxed));
			}

			void push(const T& data)
			{
				node* newNode = new node(data);
				pushElementHelper(newNode);

				_size.fetch_add(1, std::memory_order_relaxed);
			}
			void push(T&& data)
			{
				node* newNode = new node(std::move(data));
				pushElementHelper(newNode);

				_size.fetch_add(1, std::memory_order_relaxed);
			}

			void pushIfSizeLessThan(const T& data, size_t size)
			{
				try
				{
					if (_size.fetch_add(1, std::memory_order_relaxed) >= size)
					{
						_size.fetch_sub(1, std::memory_order_relaxed);
						return;
					}

					node* newNode = new node(data);
					pushElementHelper(newNode);
				}
				catch (...)
				{
					_size.fetch_sub(1, std::memory_order_relaxed);
				}
			}
			void pushIfSizeLessThan(T&& data, size_t size)
			{
				try
				{
					if (_size.fetch_add(1, std::memory_order_relaxed) >= size)
					{
						_size.fetch_sub(1, std::memory_order_relaxed);
						return;
					}

					node* newNode = new node(std::move(data));
					pushElementHelper(newNode);
				}
				catch (...)
				{
					_size.fetch_sub(1, std::memory_order_relaxed);
				}
			}

			std::shared_ptr<T> pop()
			{
				_threadsInPop.fetch_add(1, std::memory_order_relaxed);

				node* oldHead = _head.load();
				while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next));

				std::shared_ptr<T> result;

				if (oldHead)
				{
					result = std::make_shared<T>(oldHead->data);
					_size.fetch_sub(1, std::memory_order_relaxed);

					tryDeleteNode(oldHead);
				}

				return result;
			}

			unsigned size() const
			{
				return _size.load(std::memory_order_relaxed);
			}

		private:
			void pushElementHelper(node* newNode) noexcept
			{
				newNode->next = _head.load();
				while (_head.compare_exchange_weak(newNode->next, newNode));
			}

			void deleteNodesChain(node* chain)
			{
				if (_currentDestroyThreadId != std::thread::id() &&
					_currentDestroyThreadId != std::this_thread::get_id())
				{
					assert(!"This function cannot be run from different threads simultaneously");
				}

				_currentDestroyThreadId = std::this_thread::get_id();

				node* element = chain;

				while (element)
				{
					node* nextElement = element->next;

					delete element;

					element = nextElement;
				}

				_currentDestroyThreadId = std::thread::id();
			}

			void tryDeleteNode(node* toDelete)
			{
				if (_threadsInPop == 1)
				{
					node* forPendingDelete = _forPendingDelete.exchange(nullptr, std::memory_order_relaxed);
				}
				else
				{

				}
			}

			void addNodesToPendingDeletion(node* toDelete)
			{

			}

		private:
			std::atomic<node*> _head;
			std::atomic<node*> _forPendingDelete;
			std::atomic<unsigned> _size;
			std::atomic<unsigned> _threadsInPop;
		};

		template <typename T>
		std::thread::id Stack<T>::_currentDestroyThreadId;
	}
}