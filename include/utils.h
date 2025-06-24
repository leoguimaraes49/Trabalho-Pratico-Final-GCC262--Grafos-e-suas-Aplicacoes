#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

int safeStoi(const std::string &s, int def = 0);
std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &line);

#endif // UTILS_H
