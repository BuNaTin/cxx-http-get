#include "utils/strFmt.h"

namespace {

using iter = std::string::const_iterator;
bool _cmpImpl(iter l_b, iter l_e, iter r_b, iter r_e) {
    while (l_b != l_e && r_b != r_e) {
        if (*l_b == *r_b) {
            ++l_b;
            ++r_b;
            continue;
        }
        if (*l_b == '%' && l_b != l_e - 1) {
            ++l_b;
            while (r_b != r_e && *r_b != *l_b) {
                ++r_b;
            }
            continue;
        }
        if (*l_b == '%' && l_b == l_e - 1) {
            return true;
        }
        if (*l_b != *r_b) {
            return false;
        }
    }

    if (l_b == l_e && r_b == r_e) {
        return true;
    }
    return false;
}
std::string _valueImpl(iter l_b,
                       iter l_e,
                       iter r_b,
                       iter r_e,
                       std::size_t pos) noexcept {
    // if(!_cmpImpl(l_b, l_e, r_b, r_e)) {
    //     return {};
    // }
    std::size_t cur = 0;
    while (l_b != l_e && r_b != r_e) {
        if (*l_b == *r_b) {
            ++l_b;
            ++r_b;
            continue;
        }
        if (*l_b == '%' && l_b != l_e - 1) {
            ++l_b;
            iter nr_b = r_b;
            if (cur == pos) {
                while (nr_b != r_e && *nr_b != *l_b) {
                    ++nr_b;
                }
                return {r_b, nr_b};
            } else {
                while (r_b != r_e && *r_b != *l_b) {
                    ++r_b;
                }
                continue;
            }

            ++cur;
        }
        if (*l_b == '%' && l_b == l_e - 1) {
            return {r_b, r_e};
        }
        if (*l_b != *r_b) {
            return {};
        }
    }

    return {};
}

} // namespace

namespace utils { inline namespace fmt {

bool cmp(const std::string &format, const std::string &value) noexcept {
    return _cmpImpl(
            format.begin(), format.end(), value.begin(), value.end());
}

std::string value(const std::string &format,
                  const std::string &value,
                  std::size_t pos) noexcept {
    return _valueImpl(format.begin(),
                      format.end(),
                      value.begin(),
                      value.end(),
                      pos);
}
}} // namespace utils::fmt