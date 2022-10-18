#pragma once

#include <string>

namespace {

template<typename T,
         typename std::enable_if<
                 std::is_constructible<std::string, T>::value,
                 bool>::type = true>
std::string to_string(const T &str) {
    return str;
}
template<typename T,
         typename std::enable_if<std::is_arithmetic<T>::value,
                                 bool>::type = true>
std::string to_string(T num) {
    return std::to_string(num);
}

void _strFmtImpl(std::string &ans, const char *format) {
    ans += format;
}

template<typename T, typename... Targs>
void _strFmtImpl(std::string &ans,
                 const char *format,
                 T value,
                 Targs... args) {
    for (; *format != '\0'; format++) {
        if (*format == '%') {
            ans += to_string(value);
            _strFmtImpl(ans, format + 1, args...);
            return;
        }
        ans += *format;
    }
}

} // namespace

namespace utils { inline namespace fmt {

inline std::string strFmt(const std::string &format) { return format; }

template<typename T, typename... Targs>
std::string strFmt(const std::string &format, T value, Targs... args) {
    std::string ans;
    ans.reserve(format.size());
    _strFmtImpl(ans, format.c_str(), value, args...);
    return ans;
}

bool cmp(const std::string &format, const std::string &value) noexcept;

std::string value(const std::string &format,
                  const std::string &value,
                  std::size_t pos = 0) noexcept;

}} // namespace utils::fmt