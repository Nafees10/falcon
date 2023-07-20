#include "utils.h"

void lowercase(std::string &str){
	for (size_t i = 0; i < str.length(); i ++)
		str[i] += 32 * (str[i] >= 'A' && str[i] <= 'Z');
}

bool isRelevantTag(const std::string &tagName){
	return tagName == "if" ||
		tagName == "else" ||
		tagName == "elif" ||
		tagName == "for";
}

bool isWhite(char c){
	return c == ' ' ||
		c == '\t' ||
		c == '\r' ||
		c == '\n';
}

bool isAlphabet(char c){
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z');
}

bool isAlphaNum(char c){
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9');
}

std::string readAlphaNumeric(std::istream &stream){
	char c;
	while (stream >> std::noskipws >> c && isWhite(c));
	if (stream.eof())
		return nullptr;
	std::string ret;
	ret += c;
	while (stream >> std::noskipws >> c && isAlphaNum(c))
		ret += c;
	return ret;
}
