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
