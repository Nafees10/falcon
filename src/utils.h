#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <cstdint>

/// Lowercases a string
void lowercase(std::string&);

/// Returns: true if a tag name (in lowercase) is relevant
bool isRelevantTag(const std::string&);

/// Returns: true if a character is whitespace
bool isWhite(char c);

/// Returns: true if a character is alphabet
bool isAlphabet(char c);

/// Returns: true if a character is a alphanumeric
bool isAlphaNum(char c);



/// Reads consecutive alphanumeric characters, skipping any initial whitespace
std::string readAlphaNumeric(std::istream &stream);

#endif
