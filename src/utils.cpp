#include "utils.h"
#include <fstream>

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

bool readFile(const std::string &filename, std::string &stream){
	stream.clear();
	std::ifstream in(filename);
	if (!in)
		return false;
	char c;
	while (in >> std::noskipws >> c)
		stream += c;
	in.close();
	return true;
}

size_t count(const std::string &stream, size_t index, bool(*func)(char)){
	size_t count = 0;
	while (index + count < stream.size() && func(stream[index + count]))
		count ++;
	return count;
}

std::string tagNameAttr(const std::string &stream, size_t &index){
	if ((index + 1 >= stream.length()) ||
			(stream[index] == '<' && stream[index + 1] == '>'))
		return "";
	index += count(stream, index, [](char c) {return !isAlphaNum(c);});
	const size_t start = index;
	index += count(stream, index, &isAlphaNum);
	return stream.substr(start, index - start);
}
