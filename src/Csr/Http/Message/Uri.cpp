#include "Uri.hpp"
#include "Shared.hpp"

#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdexcept>
#include <string>

namespace Csr {
namespace Http {
namespace Message {

using std::invalid_argument;

namespace {

/**
 * Convert string to all lowercase characters.
 *
 * @param str String to convert to lowercase.
 */
inline void strtolower(char* str) {
    while (str && *str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

/**
 * Convert string to uint16.
 *
 * The uint must be 16-bits as this is intended to be used for network ports.
 *
 * @param value String to convert to uint16.
 * @return Uint16 value of the string.
 */
inline uint16_t strtouint(const char* value) {
    char* result;
    errno = 0;
    unsigned long val = strtoul(value, &result, 10);
    if (errno) {
        std::string error = strerror(errno);
        std::string message = "Invalid port '" + std::string(value)
            + "': " + error + ".";
        throw invalid_argument(message);
    }
    return (uint16_t)val;
}

/**
 * Convert uint16 to string.
 *
 * The uint must be 16-bits as this is intended to be used for network ports.
 *
 * @param value The uint16 to convert to string.
 * @return String value of the uint16.
 */
inline char* uinttostr(uint16_t value) {
    // Maximum length for uint16_t in decimal is 5 digits + null terminator.
    char buffer[6];

    int len = snprintf(buffer, sizeof(buffer), "%u", value);

    // Check for snprintf error.
    if (len < 0) return NULL;

    // Get the result copy it to the result.
    char* result = new char[len + 1];
    snprintf(result, len + 1, "%u", value);

    return result;
}

} // namespace

Uri::Uri(const char* uri) {
    this->scheme = NULL;
    this->authority = NULL;
    this->userInfo = NULL;
    this->host = NULL;
    this->port = NULL;
    this->path = NULL;
    this->query = NULL;
    this->fragment = NULL;
    this->string = NULL;

    if (!uri) throw invalid_argument("Cannot parse NULL uri.");

    const char *p, *q, *at, *colon, *slash, *hash, *quest;
    size_t len;

    p = strstr(uri, "://");
    if (p) {
        this->scheme = copynstr(uri, p - uri);
        strtolower(this->scheme);
        p += 3;
    } else {
        p = uri;
    }

    at = strchr(p, '@');
    slash = strchr(p, '/');
    quest = strchr(p, '?');
    hash = strchr(p, '#');

    // Parse user and password.
    char* user = NULL;
    char* pass = NULL;

    const char* hostStart = p;
    if (at
        && (!slash || at < slash)
        && (!quest || at < quest)
        && (!hash || at < hash))
    {
        colon = (const char*)memchr(p, ':', at - p);
        if (colon) {
            user = copynstr(p, colon - p);
            pass = copynstr(colon + 1, at - colon - 1);
        } else {
            user = copynstr(p, at - p);
        }
        hostStart = at + 1;
    }

    // Form user info.
    if (user && pass) {
        size_t length = strlen(user) + strlen(pass) + 2; // For ':' and '\0'.

        this->userInfo = new char[length];
        *this->userInfo = '\0';

        strcat(this->userInfo, user);
        strcat(this->userInfo, ":");
        strcat(this->userInfo, pass);
    } else if (user) {
        this->userInfo = copystr(user);
    }

    delete[] user;
    delete[] pass;

    // Parse host and port.
    const char* hostEnd = hostStart;
    while (*hostEnd && *hostEnd != '/' && *hostEnd != '?' && *hostEnd != '#') {
        hostEnd++;
    }
    colon = NULL;
    for (const char* c = hostStart; c < hostEnd; ++c) {
        if (*c == ':') {
            colon = c;
            break;
        }
    }
    if (colon) {
        this->host = copynstr(hostStart, colon - hostStart);
        strtolower(this->host);
        this->port = copynstr(colon + 1, hostEnd - colon - 1);
    } else {
        this->host = copynstr(hostStart, hostEnd - hostStart);
        strtolower(this->host);
    }

    // Parse path.
    if (*hostEnd == '/') {
        const char* pathStart = hostEnd;
        const char* pathEnd = pathStart;
        while (*pathEnd && *pathEnd != '?' && *pathEnd != '#') pathEnd++;
        this->path = copynstr(pathStart, pathEnd - pathStart);
        hostEnd = pathEnd;
    }

    // Parse query.
    if (*hostEnd == '?') {
        const char* queryStart = hostEnd + 1;
        const char* queryEnd = queryStart;
        while (*queryEnd && *queryEnd != '#') queryEnd++;
        this->query = copynstr(queryStart, queryEnd - queryStart);
        hostEnd = queryEnd;
    }

    // Parse fragment.
    if (*hostEnd == '#') {
        this->fragment = copynstr(hostEnd + 1, strlen(hostEnd + 1));
    }

    // Default to null-terminated strings.
    if (!this->scheme) this->scheme = nullstr();
    if (!this->authority) this->authority = nullstr();
    if (!this->userInfo) this->userInfo = nullstr();
    if (!this->host) this->host = nullstr();
    if (!this->port) this->port = nullstr();
    if (!this->path) this->path = nullstr();
    if (!this->query) this->query = nullstr();
    if (!this->fragment) this->fragment = nullstr();
    if (!this->string) this->string = nullstr();
}

const char* Uri::getScheme() {
    return this->scheme;
}

const char* Uri::getAuthority() {
    delete[] this->authority;

    size_t length = 1; // For '\0'.
    if (*this->userInfo) length += strlen(this->userInfo) + 1; // For '@'.
    if (*this->host) length += strlen(this->host);
    if (*this->port) length += strlen(this->port) + 1; // For ':'.

    this->authority = new char[length];
    *this->authority = '\0';

    if (*this->userInfo) {
        strcat(this->authority, this->userInfo);
        strcat(this->authority, "@");
    }
    if (*this->host) strcat(this->authority, this->host);
    if (*this->port) {
        strcat(this->authority, ":");
        strcat(this->authority, this->port);
    }

    return this->authority;
}

const char* Uri::getUserInfo() {
    return this->userInfo;
}

const char* Uri::getHost() {
    return this->host;
}

uint16_t Uri::getPort() {
    return strtouint(this->port);
}

const char* Uri::getPath() {
    return this->path;
}

const char* Uri::getQuery() {
    return this->query;
}

const char* Uri::getFragment() {
    return this->fragment;
}

void Uri::setScheme(const char* scheme) {
    delete[] this->scheme;
    this->scheme = copystr(scheme);
    strtolower(this->scheme);
}

void Uri::setUserInfo(const char* user, const char* password) {
    delete[] this->userInfo;

    size_t length = 1; // For '\0'.
    if (user) length += strlen(user);
    if (*password) length += strlen(password) + 1; // For ':'.

    this->userInfo = new char[length];
    *this->userInfo = '\0';

    if (user) strcat(this->userInfo, user);
    if (*password) {
        strcat(this->userInfo, ":");
        strcat(this->userInfo, password);
    }
}

void Uri::setHost(const char* host) {
    delete[] this->host;
    this->host = copystr(host);
    strtolower(this->host);
}

void Uri::setPort(uint16_t port) {
    delete[] this->port;
    this->port = uinttostr(port);
}

void Uri::setPath(const char* path) {
    delete[] this->path;
    this->path = copystr(path);
}

void Uri::setQuery(const char* query) {
    delete[] this->query;
    this->query = copystr(query);
}

void Uri::setFragment(const char* fragment) {
    delete[] this->fragment;
    this->fragment = copystr(fragment);
}

const char* Uri::toString() {
    delete[] this->string;

    size_t length = 1; // For '\0'.
    if (*this->scheme) length += strlen(this->scheme) + 3; // For "://".
    if (*this->userInfo) length += strlen(this->userInfo) + 1; // For '@'.
    if (*this->host) length += strlen(this->host);
    if (*this->port) length += strlen(this->port) + 1; // For ':'.
    if (*this->path) length += strlen(this->path) + 1; // For '/'.
    if (*this->query) length += strlen(this->query) + 1; // For '?'.
    if (*this->fragment) length += strlen(this->fragment) + 1; // For '#'.

    this->string = new char[length];
    *this->string = '\0';

    if (*this->scheme) {
        strcat(this->string, this->scheme);
        strcat(this->string, "://");
    }

    if (*this->userInfo) {
        strcat(this->string, this->userInfo);
        strcat(this->string, "@");
    }

    if (*this->host) {
        strcat(this->string, this->host);
    }

    if (*this->port) {
        strcat(this->string, ":");
        strcat(this->string, this->port);
    }

    if (*this->path) {
        if (*this->path != '/') strcat(this->string, "/");
        strcat(this->string, this->path);
    }

    if (*this->query) {
        strcat(this->string, "?");
        strcat(this->string, this->query);
    }

    if (*this->fragment) {
        strcat(this->string, "#");
        strcat(this->string, this->fragment);
    }

    return this->string;
}

Uri::~Uri() {
    delete[] this->scheme;
    delete[] this->authority;
    delete[] this->userInfo;
    delete[] this->host;
    delete[] this->port;
    delete[] this->path;
    delete[] this->query;
    delete[] this->fragment;
    delete[] this->string;
}

}}} // Csr::Http::Message
