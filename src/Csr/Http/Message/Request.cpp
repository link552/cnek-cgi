#include "Request.hpp"
#include "Shared.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdexcept>
#include <string>

namespace Csr {
namespace Http {
namespace Message {

using std::string;
using std::invalid_argument;

namespace {

/**
 * Validates an HTTP method.
 *
 * @param value The value of the HTTP method.
 * @throws std::invalid_argument Invalid HTTP method.
 */
inline void validatemethod(const char* value) {
    if (!value || (
            strcmp(value, "GET") != 0
            && strcmp(value, "HEAD") != 0
            && strcmp(value, "POST") != 0
            && strcmp(value, "PUT") != 0
            && strcmp(value, "DELETE") != 0
            && strcmp(value, "CONNECT") != 0
            && strcmp(value, "OPTIONS") != 0
            && strcmp(value, "TRACE") != 0
            && strcmp(value, "PATCH") != 0)
       )
    {
        string message = "Invalid HTTP method '" + string(value) + "'.";
        throw invalid_argument(message);
    }
}

} // namespace

Request::Request(const char* method, const char* uri) {
    this->requestTarget = NULL;
    this->method = NULL;
    this->uri = NULL;

    validatemethod(method);
    this->method = copystr(method);

    this->uri = new Uri(uri);
    this->setHeader("Host", this->uri->getHost());

    const char* path = this->uri->getPath();
    const char* query = this->uri->getQuery();

    // If the URI does not provide a path or query then default the
    // request-target to "/".
    if (!*path && !*query) {
        this->requestTarget = new char[2];
        this->requestTarget[0] = '/';
        this->requestTarget[1] = '\0';
        return;
    }

    // Otherwise default the request-target to origin-form.
    size_t length = 1; // For '\0';
    if (*path) length += strlen(path);
    if (*query) length += strlen(query) + 1; // For '?'.

    this->requestTarget = new char[length];
    *this->requestTarget = '\0';

    if (*path) {
        strcat(this->requestTarget, path);
    }

    if (*query) {
        const char* connector = "?";
        strcat(this->requestTarget, connector);

        strcat(this->requestTarget, query);
    }
}

Request::Request(const char* method, Uri* uri) {
    this->requestTarget = NULL;
    this->method = NULL;
    this->uri = NULL;

    validatemethod(method);

    this->method = copystr(method);

    if (!uri) return;

    this->uri = uri;
    this->setHeader("Host", this->uri->getHost());

    const char* path = this->uri->getPath();
    const char* query = this->uri->getQuery();

    // If the URI does not provide a path or query then default the
    // request-target to '/'.
    if (!*path && !*query) {
        this->requestTarget = new char[2];
        this->requestTarget[0] = '/';
        this->requestTarget[1] = '\0';
        return;
    }

    // Otherwise default the request-target to origin-form.
    size_t length = 1; // For '\0'.
    if (*path) length += strlen(path);
    if (*query) length += strlen(query) + 1; // For '?'.

    this->requestTarget = new char[length];
    *this->requestTarget = '\0';

    if (*path) {
        strcat(this->requestTarget, path);
    }

    if (*query) {
        const char* connector = "?";
        strcat(this->requestTarget, connector);

        strcat(this->requestTarget, query);
    }
}

const char* Request::getRequestTarget() {
    return this->requestTarget;
}

void Request::setRequestTarget(const char* requestTarget) {
    delete[] this->requestTarget;
    this->requestTarget = copystr(requestTarget);
}

const char* Request::getMethod() {
    return this->method;
}

void Request::setMethod(const char* method) {
    delete[] this->method;
    this->method = copystr(method);
}

Uri* Request::getUri() {
    return this->uri;
}

void Request::setUri(Uri* uri, bool preserveHost) {
    delete this->uri;
    this->uri = uri;
    if (uri && preserveHost) this->setHeader("Host", uri->getHost());
}

Request::~Request() {
    delete[] this->requestTarget;
    delete[] this->method;
    delete this->uri;
}

}}} // Csr::Http::Message
