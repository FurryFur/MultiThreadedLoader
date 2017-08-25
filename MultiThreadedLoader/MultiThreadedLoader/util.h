//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// File Name	: util.h
// Description	: Header file for utility functions and variables
// Author		: Lance Chaney
// Mail			: lance.cha7337@mediadesign.school.nz
//

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

// Wraps function call with a global lock
template<class Fn, class... Args >
auto SafeFn(Fn fn, Args... args) -> decltype(fn(std::forward<Args>(args)...))
{
	std::lock_guard<std::mutex> lock(g_mutex);
	return fn(std::forward<Args>(args)...);
}

// Distributes work to the specified number of threads
// The supplied function is run on every element in the specified container.
// Function execution is spread across mutliple threads.
template<typename Container>
void DistributeWork(Container& container, size_t numThreads, std::function<void(size_t elementIdx, size_t threadIdx, decltype(container.at(0)) element)> fn)
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
		threads.emplace_back([&container, numThreads, fn, threadIdx]() {
			// Check if this is the last thread
			bool lastThread = (threadIdx == (numThreads - 1)) ? true : false;

			// Divide up work
			size_t stride = (container.size() < numThreads) ? 1 : (container.size() / numThreads);
			size_t startIdx = threadIdx * stride;
			size_t endIdx = lastThread ? container.size() : startIdx + stride;
			for (size_t i = startIdx; i < endIdx; ++i)
			{
				fn(i, threadIdx, container.at(i));
			}
		});
	}

	// Wait for all threads to complete
	for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

// Distributes work to the specified number of threads
// The supplied function is run on every element in the specified container.
// Function execution is spread across mutliple threads.
template<typename Container>
void DistributeWork(Container& container, size_t numThreads, std::function<void(size_t elementIdx, decltype(container.at(0)) element)> fn)
{
	DistributeWork(container, numThreads, [&container, fn](size_t i, size_t threadIdx, decltype(container.at(0)) element) {
		fn(i, container.at(i));
	});
}

// Distributes work to the specified number of threads
// The supplied function is run on every element in the specified container.
// Function execution is spread across mutliple threads.
template<typename Container>
void DistributeWork(Container& container, size_t numThreads, std::function<void(decltype(container.at(0)) element)> fn)
{
	DistributeWork(container, numThreads, [&container, fn](size_t i, size_t threadIdx, decltype(container.at(0)) element) {
		fn(container.at(i));
	});
}

// An exception that should be thrown when a resource fails to load
class ResourceLoadException : public std::exception
{
public:
	virtual const char* what() const noexcept override {
		return "Error loading resource";
	}
};

// Convert the supplied object to a string
template<typename T>
std::string ToString(const T& _value)
{
	std::strstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

// Convert the supplied object to a wstring
template<typename T>
std::wstring ToWString(const T& _value)
{
	std::wstringstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}

#endif