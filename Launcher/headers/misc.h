#pragma once
#include <windows.h>
#include <filesystem>
#include <string>
#include <vector>

#pragma warning(disable: 4996)

namespace fs = std::filesystem;

std::string ToUTF8String(const std::filesystem::path& value);
std::filesystem::path FromUTF8String(const std::string& value);
std::vector<std::string> splitStr(std::string str, std::string delim);
