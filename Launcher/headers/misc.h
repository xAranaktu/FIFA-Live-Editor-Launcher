#pragma once
#include <windows.h>
#include <filesystem>
#include <string>

#pragma warning(disable: 4996)

namespace fs = std::filesystem;

std::string ToUTF8String(const std::filesystem::path& value);
std::filesystem::path FromUTF8String(const std::string& value);
