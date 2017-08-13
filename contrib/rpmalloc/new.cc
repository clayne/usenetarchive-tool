/* new.cc  -  Memory allocator  -  Public Domain  -  2017 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform lock free thread caching malloc implementation in C11.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/rpmalloc
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <new>
#include <cstdint>
#include <cstdlib>

#include "rpmalloc.h"

using namespace std;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#endif

void* operator new(size_t size) {
	return rpmalloc(size);
}

void operator delete(void* ptr) noexcept {
	rpfree(ptr);
}

void* operator new[](size_t size) {
	return rpmalloc(size);
}

void operator delete[](void* ptr) noexcept {
	rpfree(ptr);
}

void* operator new(size_t size, const std::nothrow_t&) noexcept {
	return rpmalloc(size);
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept {
	return rpmalloc(size);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
	return rpfree(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
	return rpfree(ptr);
}

#if 0

void operator delete(void* ptr, size_t) noexcept {
	free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
	free(ptr);
}

#endif
