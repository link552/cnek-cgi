#ifndef CSR_HTTP_MESSAGE_SHARED

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

/**
 * Creates a null-terminated c-string.
 *
 * This function MUST allocate the string.
 *
 * @return Pointer to the null-terminated c-string.
 */
inline char* nullstr() {
    char* str = new char[1];
    *str = '\0';
    return str;
}

/**
 * Case-insensitive string comparison.
 *
 * This function MUST return false if one or both of the input parameters
 * are null pointers.
 *
 * For internal use only.
 *
 * @param a First string to compare.
 * @param b Second string to compare.
 * @return True if the strings are equal, false if not.
 */
inline bool strcasecmp(const char* a, const char* b) {
    // Null pointers default to false comparison.
    if (!a || !b) return false;

    // While each character is not null or null-terminator,
    // compare their lowercase form.
    while (*a && *b) {
        if (tolower(*a) != tolower(*b)) {
            // Not equal.
            return false;
        }
        a++;
        b++;
    }

    // Ensure their last character is equal.
    return *a == *b;
}

} // namespace

/**
 * Case-insensitive string comparison with length.
 *
 * This function MUST return false if one or both of the input parameters
 * are null pointers.
 *
 * For internal use only.
 *
 * @param a First string to compare.
 * @param b Second string to compare.
 * @param len Max number of bytes to compare.
 * @return True if the strings are equal up to the given length, false if not.
 */
static inline bool strncasecmp(const char* a, const char* b, size_t len) {
    // Null pointers default to false comparison.
    if (!a || !b) return false;

    // While each character is not null or null-terminator,
    // compare their lowercase form.
    size_t i = 1;
    while (*a && *b && i < len) {
        if (tolower(*a) != tolower(*b)) {
            // Not equal.
            return false;
        }
        a++;
        b++;
        i++;
    }

    // Ensure their last character is equal.
    return *a == *b;
}

/**
 * Creates a copy of a string.
 *
 * If this function cannot copy the string then it MUST
 * return a null-terminated string.
 *
 * Similar to C's strdup().
 *
 * @param str String to copy.
 * @return Pointer to a newly allocated, null-terminated copy of str,
 *     or a null-terminated string if it cannot copy.
 */
static inline char* copystr(const char* str) {
    if (!str) return nullstr();
    size_t len = strlen(str);
    char* copy = new char[len + 1]; // +1 for null-terminator.
    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

/**
 * Creates a copy of a string up to a given length.
 *
 * If this function cannot copy the string then it MUST
 * return a null-terminated string.
 *
 * Similar to C's strdup().
 *
 * @param str String to copy.
 * @param len Number of bytes to copy.
 * @return Pointer to a newly allocated, null-terminated copy of str,
 *     or a null-terminated string if it cannot copy.
 */
static inline char* copynstr(const char* str, size_t len) {
    if (!str) return nullstr();
    char* copy = new char[len + 1]; // +1 for null-terminator.
    memcpy(copy, str, len);
    copy[len] = '\0';
    return copy;
}

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_SHARED
#endif // CSR_HTTP_MESSAGE_SHARED
