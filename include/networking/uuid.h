/*
The MIT License (MIT)

Copyright (c) 2014 Graeme Hill (http://graemehill.ca)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include <iomanip>

#ifdef GUID_ANDROID
#include <jni.h>
#endif

#define BEGIN_XG_NAMESPACE namespace xg {
#define END_XG_NAMESPACE }

BEGIN_XG_NAMESPACE

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.
class Uuid
{
public:
	Uuid(const std::vector<unsigned char> &bytes);
	Uuid(const unsigned char *bytes);
	Uuid(const std::string &fromString);
	Uuid();
	Uuid(const Uuid &other);
	Uuid &operator=(const Uuid &other);
	bool operator==(const Uuid &other) const;
	bool operator!=(const Uuid &other) const;

	std::string str() const;
	const char* c_str() const;

	operator std::string() const;

	void swap(Uuid &other);

private:

	// actual data
	std::vector<unsigned char> _bytes;

	// make the << operator a friend so it can access _bytes
	friend std::ostream &operator<<(std::ostream &s, const Uuid &guid);
};

Uuid newUuid();

#ifdef GUID_ANDROID
struct AndroidUuidInfo
{
	static AndroidUuidInfo fromJniEnv(JNIEnv *env);

	JNIEnv *env;
	jclass uuidClass;
	jmethodID newUuidMethod;
	jmethodID mostSignificantBitsMethod;
	jmethodID leastSignificantBitsMethod;
};

extern AndroidUuidInfo androidInfo;

void initJni(JNIEnv *env);
#endif

END_XG_NAMESPACE

namespace std
{
	// Template specialization for std::swap<Uuid>() --
	// See guid.cpp for the function definition
	template <>
	void swap(xg::Uuid &guid0, xg::Uuid &guid1);

	// Specialization for std::hash<Uuid> -- this implementation
	// uses std::hash<std::string> on the stringification of the guid
	// to calculate the hash
	template <>
	struct hash<xg::Uuid>
	{
		typedef xg::Uuid argument_type;
		typedef std::size_t result_type;

		result_type operator()(argument_type const &guid) const
		{
			std::hash<std::string> hasher;
			return static_cast<result_type>(hasher(guid.str()));
		}
	};
}
