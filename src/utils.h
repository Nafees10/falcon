#ifndef UTILS_H
#define UTILS_H

#include <string>

/// Lowercases a string
void lowercase(std::string&);

/// Returns: true if a tag name (in lowercase) is relevant
bool isRelevantTag(const std::string&);

#endif
