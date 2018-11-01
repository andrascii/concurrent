#pragma once

namespace concurrent
{
	namespace mutualexclusion
	{
		template <typename T>
		class stack
		{
		public:
			void push(const T& data)
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_stack.push(data);
			}

			bool pop(T& result)
			{
				std::lock_guard<std::mutex> lock(_mutex);

				if (_stack.empty())
				{
					return false;
				}

				result = std::move(_stack.top());
				_stack.pop();

				return true;
			}

			size_t size() const
			{
				std::lock_guard<std::mutex> lock(_mutex);
				return _stack.size();
			}

		private:
			mutable std::mutex _mutex;
			std::stack<T> _stack;
		};
	}
}