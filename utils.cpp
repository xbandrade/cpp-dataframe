#include "utils.hpp"

std::vector<std::string> splitStr(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (getline(token_stream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}