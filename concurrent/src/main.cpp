#include "stacklockfree.h"
#include "stackmutualexclusion.h"

constexpr int c_iterationsCount = 30000000;

std::atomic_bool startSignal(false);

struct TestData
{
	int a1;
	int a2;
	int a3;
	int a4;
	int a5;
	int a6;
	int a7;
	int a8;
	int a9;
	int a10;
};

template <typename T>
void func(T& stack)
{
	while (!startSignal.load(std::memory_order_acquire));

	while (stack.size() < c_iterationsCount)
	{
		stack.push(TestData());
	}

	startSignal.store(false, std::memory_order_release);
}

int main(int, char)
{
	concurrent::lockfree::stack<TestData> lockFreeStack;
	concurrent::mutualexclusion::stack<TestData> mutualExclutionStack;

	std::vector<std::thread> threads;

	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		threads.emplace_back(std::thread([&]() { func(lockFreeStack); }));
	}

	auto start = std::chrono::high_resolution_clock::now();

	startSignal.store(true, std::memory_order_release);
	for (int i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "lock free stack result:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	//////////////////////////////////////////////////////////////////////////

	threads.clear();

	for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		threads.emplace_back(std::thread([&]() { func(mutualExclutionStack); }));
	}

	start = std::chrono::high_resolution_clock::now();

	startSignal.store(true, std::memory_order_release);
	for (int i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}

	end = std::chrono::high_resolution_clock::now();

	std::cout << "mutual exclusion stack result:" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cin.get();
}
