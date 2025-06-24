#include "utils.h"
#include <sstream>
#include <cctype>

int safeStoi(const std::string &s, int def)
{
    try
    {
        size_t p;
        int v = std::stoi(s, &p);
        return (p == s.size() ? v : def);
    }
    catch (...)
    {
        return def;
    }
}

std::string trim(const std::string &s)
{
    size_t f = s.find_first_not_of(" \t\r\n");
    if (f == std::string::npos)
        return "";
    size_t l = s.find_last_not_of(" \t\r\n");
    return s.substr(f, l - f + 1);
}

std::vector<std::string> split(const std::string &line)
{
    std::vector<std::string> tk;
    std::istringstream is(line);
    std::string x;
    while (is >> x)
        tk.push_back(x);
    return tk;
}
