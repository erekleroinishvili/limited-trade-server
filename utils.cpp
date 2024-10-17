#ifndef UTILS
#define UTILS
#include <string>
#include "config.cpp"

/**
 * @brief Convert a number into its comma-separated string representation.
 * E.g.: 1001 -> "1,001"
 *
 * @param n original number to convert to its comma-separated representation
 * @param separator a character to use as a separator. Defaults to comma
 *
 * @return string with comma-separated representation of the original number
 */
std::string thousandsSeparated(unsigned int n, char separator = THOUSAND_SEPARATOR) {
    const std::string s = std::to_string(n);
    std::string reply(s.length() + (s.length() - 1) / 3, '0'); // Pre-allocate string
    for (int i = s.length() - 1, j = reply.length() - 1; j >= 0; --i, --j) {
        reply[j] = s[i];
        if (i && (s.length() - i) % 3 == 0)
            reply[--j] = separator;
    }
    return reply;
}

#endif
