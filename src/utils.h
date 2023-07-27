#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
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

/// Reads a file into a string
/// Returns: true if done, false if error
bool readFile(const std::string& filename, std::string &stream);

/// Returns: number of consecutive characters for which a function is true
size_t count(const std::string &stream, size_t index, bool (*func)(char));

/// Reads tag name or tag attribute
/// Returns: tag or attribute name
std::string tagNameAttr(const std::string &stream, size_t &start);

#endif
