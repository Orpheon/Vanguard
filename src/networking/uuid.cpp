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

#include <cstring>
#include "networking/uuid.h"

#ifdef GUID_LIBUUID
#include <uuid/uuid.h>
#endif

#ifdef GUID_CFUUID
#include <CoreFoundation/CFUUID.h>
#endif

#ifdef GUID_WINDOWS
#include <objbase.h>
#endif

#ifdef GUID_ANDROID
#include <jni.h>
#endif

using namespace std;

BEGIN_XG_NAMESPACE

#ifdef GUID_ANDROID
AndroidUuidInfo androidInfo;

AndroidUuidInfo AndroidUuidInfo::fromJniEnv(JNIEnv *env)
{
	AndroidUuidInfo info;
	info.env = env;
	info.uuidClass = env->FindClass("java/util/UUID");
	info.newUuidMethod = env->GetStaticMethodID(
		info.uuidClass, "randomUUID", "()Ljava/util/UUID;");
	info.mostSignificantBitsMethod = env->GetMethodID(
		info.uuidClass, "getMostSignificantBits", "()J");
	info.leastSignificantBitsMethod = env->GetMethodID(
		info.uuidClass, "getLeastSignificantBits", "()J");
	return info;
}

void initJni(JNIEnv *env)
{
	androidInfo = AndroidUuidInfo::fromJniEnv(env);
}
#endif

// overload << so that it's easy to convert to a string
ostream &operator<<(ostream &s, const Uuid &guid)
{
	return s << hex << setfill('0')
		<< setw(2) << (int)guid._bytes[0]
		<< setw(2) << (int)guid._bytes[1]
		<< setw(2) << (int)guid._bytes[2]
		<< setw(2) << (int)guid._bytes[3]
		<< "-"
		<< setw(2) << (int)guid._bytes[4]
		<< setw(2) << (int)guid._bytes[5]
		<< "-"
		<< setw(2) << (int)guid._bytes[6]
		<< setw(2) << (int)guid._bytes[7]
		<< "-"
		<< setw(2) << (int)guid._bytes[8]
		<< setw(2) << (int)guid._bytes[9]
		<< "-"
		<< setw(2) << (int)guid._bytes[10]
		<< setw(2) << (int)guid._bytes[11]
		<< setw(2) << (int)guid._bytes[12]
		<< setw(2) << (int)guid._bytes[13]
		<< setw(2) << (int)guid._bytes[14]
		<< setw(2) << (int)guid._bytes[15];
}

// convert to string using std::snprintf() and std::string
std::string Uuid::str() const
{
	char one[10], two[6], three[6], four[6], five[14];

	snprintf(one, 10, "%02x%02x%02x%02x",
		_bytes[0], _bytes[1], _bytes[2], _bytes[3]);
	snprintf(two, 6, "%02x%02x",
		_bytes[4], _bytes[5]);
	snprintf(three, 6, "%02x%02x",
		_bytes[6], _bytes[7]);
	snprintf(four, 6, "%02x%02x",
		_bytes[8], _bytes[9]);
	snprintf(five, 14, "%02x%02x%02x%02x%02x%02x",
		_bytes[10], _bytes[11], _bytes[12], _bytes[13], _bytes[14], _bytes[15]);
	const std::string sep("-");
	std::string out(one);

	out += sep + two;
	out += sep + three;
	out += sep + four;
	out += sep + five;

	return out;
}

// convert to a C-style string
const char *Uuid::c_str() const
{
	return str().c_str();
}

// conversion operator for std::string
Uuid::operator std::string() const
{
	return str();
}

// create a guid from vector of bytes
Uuid::Uuid(const vector<unsigned char> &bytes)
{
	_bytes = bytes;
}

// create a guid from array of bytes
Uuid::Uuid(const unsigned char *bytes)
{
	_bytes.assign(bytes, bytes + 16);
}

// converts a single hex char to a number (0 - 15)
unsigned char hexDigitToChar(char ch)
{
	if (ch > 47 && ch < 58)
		return ch - 48;

	if (ch > 96 && ch < 103)
		return ch - 87;

	if (ch > 64 && ch < 71)
		return ch - 55;

	return 0;
}

// converts the two hexadecimal characters to an unsigned char (a byte)
unsigned char hexPairToChar(char a, char b)
{
	return hexDigitToChar(a) * 16 + hexDigitToChar(b);
}

// create a guid from string
Uuid::Uuid(const string &fromString)
{
	_bytes.clear();

	char charOne, charTwo;
	bool lookingForFirstChar = true;

	for (const char &ch : fromString)
	{
		if (ch == '-')
			continue;

		if (lookingForFirstChar)
		{
			charOne = ch;
			lookingForFirstChar = false;
		}
		else
		{
			charTwo = ch;
			auto byte = hexPairToChar(charOne, charTwo);
			_bytes.push_back(byte);
			lookingForFirstChar = true;
		}
	}
}

// create empty guid
Uuid::Uuid()
{
	_bytes = vector<unsigned char>(16, 0);
}

// copy constructor
Uuid::Uuid(const Uuid &other)
{
	_bytes = other._bytes;
}

// overload assignment operator
Uuid &Uuid::operator=(const Uuid &other)
{
	Uuid(other).swap(*this);
	return *this;
}

// overload equality operator
bool Uuid::operator==(const Uuid &other) const
{
	return _bytes == other._bytes;
}

// overload inequality operator
bool Uuid::operator!=(const Uuid &other) const
{
	return !((*this) == other);
}

// member swap function
void Uuid::swap(Uuid& other)
{
	_bytes.swap(other._bytes);
}

// This is the linux friendly implementation, but it could work on other
// systems that have libuuid available
#ifdef GUID_LIBUUID
Uuid newUuid()
{
	uuid_t id;
	uuid_generate(id);
	return id;
}
#endif

// this is the mac and ios version
#ifdef GUID_CFUUID
Uuid newUuid()
{
	auto newId = CFUUIDCreate(NULL);
	auto bytes = CFUUIDGetUUIDBytes(newId);
	CFRelease(newId);

	const unsigned char byteArray[16] =
	{
		bytes.byte0,
		bytes.byte1,
		bytes.byte2,
		bytes.byte3,
		bytes.byte4,
		bytes.byte5,
		bytes.byte6,
		bytes.byte7,
		bytes.byte8,
		bytes.byte9,
		bytes.byte10,
		bytes.byte11,
		bytes.byte12,
		bytes.byte13,
		bytes.byte14,
		bytes.byte15
	};
	return byteArray;
}
#endif

// obviously this is the windows version
#ifdef GUID_WINDOWS
Uuid newUuid()
{
	GUID newId;
	CoCreateUuid(&newId);

	const unsigned char bytes[16] =
	{
		(newId.Data1 >> 24) & 0xFF,
		(newId.Data1 >> 16) & 0xFF,
		(newId.Data1 >> 8) & 0xFF,
		(newId.Data1) & 0xff,

		(newId.Data2 >> 8) & 0xFF,
		(newId.Data2) & 0xff,

		(newId.Data3 >> 8) & 0xFF,
		(newId.Data3) & 0xFF,

		newId.Data4[0],
		newId.Data4[1],
		newId.Data4[2],
		newId.Data4[3],
		newId.Data4[4],
		newId.Data4[5],
		newId.Data4[6],
		newId.Data4[7]
	};

  return bytes;
}
#endif

// android version that uses a call to a java api
#ifdef GUID_ANDROID
Uuid newUuid()
{
	jobject javaUuid = androidInfo.env->CallStaticObjectMethod(
		androidInfo.uuidClass, androidInfo.newUuidMethod);
	jlong mostSignificant = androidInfo.env->CallLongMethod(javaUuid,
		androidInfo.mostSignificantBitsMethod);
	jlong leastSignificant = androidInfo.env->CallLongMethod(javaUuid,
		androidInfo.leastSignificantBitsMethod);

	unsigned char bytes[16] =
	{
		(mostSignificant >> 56) & 0xFF,
		(mostSignificant >> 48) & 0xFF,
		(mostSignificant >> 40) & 0xFF,
		(mostSignificant >> 32) & 0xFF,
		(mostSignificant >> 24) & 0xFF,
		(mostSignificant >> 16) & 0xFF,
		(mostSignificant >> 8) & 0xFF,
		(mostSignificant) & 0xFF,
		(leastSignificant >> 56) & 0xFF,
		(leastSignificant >> 48) & 0xFF,
		(leastSignificant >> 40) & 0xFF,
		(leastSignificant >> 32) & 0xFF,
		(leastSignificant >> 24) & 0xFF,
		(leastSignificant >> 16) & 0xFF,
		(leastSignificant >> 8) & 0xFF,
		(leastSignificant) & 0xFF,
	};
	return bytes;
}
#endif

END_XG_NAMESPACE

// Specialization for std::swap<Uuid>() --
// call member swap function of lhs, passing rhs
namespace std
{
	template <>
	void swap(xg::Uuid& lhs, xg::Uuid& rhs)
	{
		lhs.swap(rhs);
	}
}
