#include "to_string.hpp"

std::string my::to_string(float _val) {
    char buff[128];
    snprintf(buff, sizeof(buff), "%G", _val);
    return {buff};
}

std::string my::to_string(double _val) {
    char buff[128];
    snprintf(buff, sizeof(buff), "%G", _val);
    return {buff};
}

std::string my::to_string(uint32_t _val) {
    char buff[128];
    snprintf(buff, sizeof(buff), "%lu", _val);
    return {buff};
}

std::string my::to_string(int32_t _val) {
    char buff[128];
    snprintf(buff, sizeof(buff), "%ld", _val);
    return {buff};
}

