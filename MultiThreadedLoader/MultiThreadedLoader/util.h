#pragma once

#if !defined(__UTILS_H__)
#define __UTILS_H__

// Library Includes
#include <Windows.h>
#include <sstream>
#include <mutex>
#include <stdexcept>

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