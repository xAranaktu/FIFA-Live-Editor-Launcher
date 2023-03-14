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