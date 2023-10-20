#include "misc.h"

std::string ToUTF8String(const std::filesystem::path& value) {
    auto result = value.u8string();

    return { result.begin(), result.end() };
}

std::filesystem::path FromUTF8String(const std::string& value) {
    int slength = (int)value.length() + 1;
    int len = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), slength, 0, 0);

    wchar_t* buf = new wchar_t[len];

    std::filesystem::path result(buf);

    delete[] buf;
    return result;
}

std::vector<std::string> splitStr(std::string str, std::string delim) {
    std::vector<std::string> result;

    std::size_t current, previous = 0;
    current = str.find_first_of(delim);
    if (current == std::string::npos) {
        result.push_back(str);
        return result;
    }
    std::string to_add = "";
    while (current != std::string::npos) {
        to_add = str.substr(previous, current - previous);
        if (!to_add.empty())
            result.push_back(to_add);
        previous = current + 1;
        current = str.find_first_of(delim, previous);
    }
    result.push_back(str.substr(previous, current - previous));
    return result;
}
