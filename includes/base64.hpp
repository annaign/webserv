#ifndef _BASE64_H_
#define _BASE64_H_

#include <vector>
#include <string>

class Base64
{
public:
	static std::string encode(const std::string &str);
	static std::string encode(const unsigned char *buf, unsigned int bufLen);
	static std::string decode(std::string encoded_string);
};

#endif