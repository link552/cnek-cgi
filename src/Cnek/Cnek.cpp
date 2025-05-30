#include "Cnek.hpp"
#include "Message.hpp"

#include <stdio.h>
#include <stdexcept>
#include <cstring>
#include <string>

// Size of buffer used to read the request body.
// NOTE: Saves on memory.
#ifndef REQUEST_BODY_BUFFER_SIZE
#define REQUEST_BODY_BUFFER_SIZE 4096 // Default 4KB.
#endif // REQUEST_BODY_BUFFER_SIZE

// Max number of bytes to read from the request body.
// NOTE: Prevents DoS attacks.
#ifndef MAX_REQUEST_BODY_SIZE
#define MAX_REQUEST_BODY_SIZE 4194304 // Default 4MB.
#endif // MAX_REQUEST_BODY_SIZE

namespace Cnek {

using Csr::Http::Message::ServerRequest;
using Csr::Http::Message::Response;
using Csr::Http::Message::Stream;
using Csr::Http::Message::HeaderIterator;
using Csr::Http::Message::ValueIterator;

using std::runtime_error;
using std::invalid_argument;
using std::strerror;
using std::string;

Cnek::Cnek() : serverRequest(NULL) {}

ServerRequest* Cnek::getServerRequest(char** environment, FILE* input) {
    if (this->serverRequest) return this->serverRequest;

    if (!environment || !input) {
        throw invalid_argument("Failed to create server request.");
    }

    // Gather method and uri from environment variables.
    char* method = NULL;
    char* uri = NULL;
    for (char** env = environment; *env; env++) {
        // Each environment variable is in the form NAME=VALUE.
        char* eq = strchr(*env, '=');
        if (eq && !strncmp(*env, "REQUEST_METHOD", eq - *env)) {
            method = eq + 1;
        } else if (eq && !strncmp(*env, "REQUEST_URI", eq - *env)) {
            uri = eq + 1;
        }

        if (method && uri) break;
    }

    if (!method) {
        throw invalid_argument(
            "Missing required environment variable 'REQUEST_METHOD'.");
    }

    if (!uri) {
        throw invalid_argument(
            "Missing required environment variable 'REQUEST_URI'.");
    }

    this->serverRequest = new ServerRequest(method, uri, environment);
    Stream* body = this->serverRequest->getBody();

    // Write input to server request body.
    char buffer[REQUEST_BODY_BUFFER_SIZE];
    size_t bufferSize = sizeof(buffer);

    size_t bytesRead = 0;
    size_t totalBytes = 0;

    errno = 0;
    while ((bytesRead = fread(buffer, 1, bufferSize, input)) > 0) {
        totalBytes += bytesRead;

        // If we are approaching the max request body size then read the
        // remaining bytes on the next iteration.
        if (totalBytes + bufferSize > MAX_REQUEST_BODY_SIZE) {
            bufferSize = MAX_REQUEST_BODY_SIZE - totalBytes;
        }

        size_t bytesWritten = body->write(buffer);
        if (bytesWritten != bytesRead && errno) {
            string error = strerror(errno);
            string message =
                "Failed to create server request while writing: " + error + ".";
            throw runtime_error(message);
        }

        // Limit how much we read from request body.
        if (totalBytes == MAX_REQUEST_BODY_SIZE) break;
    }

    if (ferror(input)) {
        string error = strerror(errno);
        string message =
            "Failed to create server request while reading: " + error + ".";
        throw runtime_error(message);
    }

    return this->serverRequest;
}

void Cnek::emitResponse(Response* response, FILE* output) {
    // Output headers.
    HeaderIterator headers = response->getHeaders();
    while (headers.next()) {
        ValueIterator values = headers.getValues();
        while (values.next()) {
            fprintf(output,
                    "%s: %s\r\n",
                    headers.getName(),
                    values.getValue());
        }
    }

    // Output status.
    fprintf(output,
            "Status: %d %s\r\n",
            response->getStatusCode(),
            response->getReasonPhrase());

    // Header and body separator.
    fprintf(output, "\r\n");

    // Output body.
    Stream* body = response->getBody();

    // If the body was previous written to, it'll be at the end. Rewind
    // before reading.
    if (body->isSeekable()) body->rewind();

    const unsigned short bufferSize = 4069;
    errno = 0;

    while (!body->eof()) {
        const char* data = body->read(bufferSize);
        size_t bytesWritten = fprintf(output, "%s", data);
        if (bytesWritten < 0 || errno || ferror(output)) {
            string error = strerror(errno);
            string message =
                "Failed to emit response while writing: " + error + ".";
            throw runtime_error(message);
        }
    }

    delete response;
}

Cnek::~Cnek() {
    delete this->serverRequest;
}

} // Cnek
