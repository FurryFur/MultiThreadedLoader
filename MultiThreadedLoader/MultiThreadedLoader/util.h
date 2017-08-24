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

template<typename T, typename Container>
void ForEachThreaded(Container container, size_t numThreads, std::function<void(size_t idx, T element)> fn)
{
	std::vector<std::thread> threads;
	threads.reserve(numThreads);
	
	for (int threadIdx = 0; threadIdx < numThreads; ++threadIdx)
	{
		// Check if this is the last thread to be dispatched
		bool lastThread = (threadIdx == (numThreads - 1)) ? true : false;

		// Create thread / delegate work
		threads.emplace_back([&]() {
			size_t stride = (container.size() < numThreads) ? 1 : (container.size() / numThreads);
			size_t startIdx = threadIdx * stride;
			size_t endIdx = lastThread ? container.size() : startIdx + stride;
			for (size_t i = startIdx; i < endIdx; ++i)
			{
				fn(i, container.at(i));
			}
		});
	}
	for_each(threads.begin(), threads.end(), [](std::thread& thread) { if (thread.joinable()) thread.join(); });
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