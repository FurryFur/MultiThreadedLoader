#pragma once

#if !defined(__UTILS_H__)
#define __UTILS_H__

// Library Includes
#include <Windows.h>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <functional>

// Local Includes

// Types

// Constants

// Prototypes

#define VALIDATE(a) if (!a) return (false)

extern std::mutex g_mutex;

template<class Fn, class... Args >
auto SafeFn(Fn fn, Args... args) -> decltype(fn(std::forward<Args>(args)...))
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return fn(std::forward<Args>(args)...);
}

template<typename Container>
void DistributeWork(Container& container, size_t numThreads, 
	std::function<void(size_t elementIdx, size_t threadIdx, size_t stride, decltype(container.at(0)) element)> fn, 
	size_t minStride = 1, size_t skipAmount = 1)
{
	// Do nothing if container contains zero elements
	if (container.size() == 0)
		return;

	// Avoid creating extra threads
	numThreads = (numThreads < container.size()) ? numThreads : container.size();

	// Allocate memory for threads
	std::vector<std::thread> threads;
	threads.reserve(numThreads);
	
	// Create worker threads
	for (size_t threadIdx = 0; threadIdx < numThreads; ++threadIdx)
	{
		// Check if this is the last thread
		bool lastThread = (threadIdx == (numThreads - 1)) ? true : false;

		// Divide up work
		size_t stride = container.size() / (skipAmount * numThreads);
		if (stride < minStride) stride = minStride;
		size_t startIdx = threadIdx * stride * skipAmount;
		size_t endIdx = lastThread ? container.size() : (startIdx + stride * skipAmount);
		if (endIdx > container.size()) endIdx = container.size();

		if (startIdx >= container.size())
			break;

		threads.emplace_back([startIdx, endIdx, skipAmount, fn, threadIdx, stride, &container]() {
			for (size_t i = startIdx; i < endIdx; i += skipAmount)
			{
				fn(i, threadIdx, stride, container.at(i));
			}
		});
	}

	// Wait for all threads to complete
	for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

template<typename Container>
void DistributeWork(Container& container, size_t numThreads, 
	std::function<void(size_t elementIdx, size_t threadIdx, decltype(container.at(0)) element)> fn, 
	size_t minStride = 1, size_t skipAmount = 1)
{
	DistributeWork(container, numThreads, [fn](size_t i, size_t threadIdx, size_t stride, decltype(container.at(0)) element) {
		fn(i, threadIdx, element);
	}, minStride, skipAmount);
}

template<typename Container>
void DistributeWork(Container& container, size_t numThreads, 
	std::function<void(size_t elementIdx, decltype(container.at(0)) element)> fn, 
	size_t minStride = 1, size_t skipAmount = 1)
{
	DistributeWork(container, numThreads, [fn](size_t i, size_t threadIdx, size_t stride, decltype(container.at(0)) element) {
		fn(i, element);
	}, minStride, skipAmount);
}

template<typename Container>
void DistributeWork(Container& container, size_t numThreads, 
	std::function<void(decltype(container.at(0)) element)> fn, 
	size_t minStride = 1, size_t skipAmount = 1)
{
	DistributeWork(container, numThreads, [fn](size_t i, size_t threadIdx, size_t stride, decltype(container.at(0)) element) {
		fn(element);
	}, minStride, skipAmount);
}

class ResourceLoadException : public std::exception
{
public:
	virtual const char* what() const noexcept override {
		return "Error loading resource";
	}
};

template<typename T>
std::string ToString(const T& _value)
{
	std::strstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

template<typename T>
std::wstring ToWString(const T& _value)
{
	std::wstringstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

#endif