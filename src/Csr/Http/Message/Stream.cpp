#include "Stream.hpp"

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <string>
#include <sstream>
#include <stdlib.h>

// Size of buffer used in read().
// NOTE: Saves on memory.
#ifndef STREAM_BUFFER_SIZE
#define STREAM_BUFFER_SIZE 4096 // Default 4KB.
#endif // STREAM_BUFFER_SIZE

// Max number of bytes to allocate for read(), toString(), and getContents().
// NOTE: Prevents DoS attacks.
#ifndef MAX_STREAM_READ_SIZE
#define MAX_STREAM_READ_SIZE 4194304 // Default 4MB.
#endif // MAX_STREAM_READ_SIZE

namespace Csr {
namespace Http {
namespace Message {

using std::strerror;
using std::snprintf;
using std::runtime_error;
using std::ostringstream;

namespace {

/**
 * Get the string length of a size value.
 *
 * @param value Size value to get the string length of.
 * @return String length of the size value.
 */
inline size_t sizelen(size_t value) {
    return snprintf(NULL, 0, "%zu", value);
}

} // namespace

Stream::Stream() : readBuffer(NULL) {
    errno = 0;
    this->resource = tmpfile();
    if (errno || !this->resource) {
        std::string error = strerror(errno);
        std::string message = "Failed to open default stream: " + error + ".";
        throw runtime_error(message);
    }
    this->readable = true;
    this->writable = true;
}

Stream::Stream(const char* filename, const char* mode) : readBuffer(NULL) {
    errno = 0;
    this->resource = fopen(filename, mode);
    if (errno || !this->resource) {
        std::string error = strerror(errno);
        std::string message = "Failed to open file '" + std::string(filename)
            + "' with mode '" + std::string(mode)
            + "': " + error + ".";
        throw runtime_error(message);
    }

    if (strchr(mode, '+')) {
        this->readable = true;
        this->writable = true;
    } else if (strchr(mode, 'r')) {
        this->readable = true;
        this->writable = false;
    } else if (strchr(mode, 'w') || strchr(mode, 'a')) {
        this->readable = false;
        this->writable = true;
    }
}

Stream::Stream(FILE* resource) : readBuffer(NULL) {
    this->resource = resource;

    // NOTE: It's hard to tell if an existing FILE* is readable or writable
    // without attempting to do so. For now, we will assume it can do both.
    // The truth of the matter will be revealed when the caller attempts to
    // read or write.
    this->readable = true;
    this->writable = true;
}

const char* Stream::toString() {
    if (!this->resource) {
        throw runtime_error("Attempted toString() on closed or detached stream.");
    }

    long cursor = this->tell();
    this->seek(0);
    const char* string = this->read(MAX_STREAM_READ_SIZE);
    this->seek(cursor);
    return string;
}

void Stream::close() {
    if (!this->resource) {
        throw runtime_error("Attempted close() on closed or detached stream.");
    }

    fclose(this->resource);
    this->resource = NULL;
    this->readable = false;
    this->writable = false;
}

FILE* Stream::detach() {
    if (!this->resource) {
        throw runtime_error("Attempted detach() on closed or detached stream.");
    }

    FILE* temp = this->resource;
    this->resource = NULL;
    this->readable = false;
    this->writable = false;
    return temp;
}

long Stream::getSize() {
    if (!this->resource) {
        throw runtime_error("Attempted getSize() on closed or detached stream.");
    }

    errno = 0;
    long cursor = ftell(this->resource);
    if (errno) {
        errno = 0;
        return -1;
    }

    this->seek(0, SEEK_END);
    long size = ftell(this->resource);
    this->seek(cursor);

    return size;
}

long Stream::tell() {
    if (!this->resource) {
        throw runtime_error("Attempted tell() on closed or detached stream.");
    }

    errno = 0;
    long position = ftell(this->resource);
    if (errno) {
        std::string error = strerror(errno);
        std::string message = "Unexpected error during Stream::tell(): " + error + ".";
        throw runtime_error(message);
    }
    return position;
}

bool Stream::eof() {
    if (!this->resource) {
        throw runtime_error("Attempted eof() on closed or detached stream.");
    }

    return feof(this->resource) != 0;
}

bool Stream::isSeekable() {
    if (!this->resource) return false;
    return fseek(this->resource, 0, SEEK_CUR) == 0;
}

void Stream::seek(long offset, int whence) {
    if (!this->resource) {
        throw runtime_error("Attempted seek() on closed or detached stream.");
    }

    errno = 0;
    int result = fseek(this->resource, offset, whence);
    if (errno || result) {
        std::string error = strerror(errno);
        std::string message = "Unexpected error during Stream::seek(): " + error + ".";
        throw runtime_error(message);
    }
}

void Stream::rewind() {
    if (!this->resource) {
        throw runtime_error("Attempted rewind() on closed or detached stream.");
    }

    std::rewind(this->resource);
}

bool Stream::isWritable() {
    return this->writable;
}

size_t Stream::write(const char* string) {
    if (!this->resource) {
        throw runtime_error("Attempted write() on closed or detached stream.");
    }

    if (!string || !*string) return 0;
    size_t length = strlen(string);
    errno = 0;
    size_t count = fwrite(string, 1, length, this->resource);
    if (count < length && ferror(this->resource)) {
        ostringstream oss;
        oss << "Unexpected error after writing " << count
            << " of " << length
            << " bytes: " << strerror(errno) << ".";
        throw runtime_error(oss.str());
    }
    return count;
}

bool Stream::isReadable() {
    return this->readable;
}

const char* Stream::read(size_t length) {
    if (!this->resource) {
        throw runtime_error("Attempted read() on closed or detached stream.");
    }

    free(this->readBuffer);

    // Limit length to max read size for added security.
    if (length > MAX_STREAM_READ_SIZE) length = MAX_STREAM_READ_SIZE;

    size_t bufferSize = STREAM_BUFFER_SIZE;

    // Limit buffer size to length to save on memory.
    if (bufferSize > length) bufferSize = length;

    this->readBuffer = (char*)malloc(bufferSize);
    if (!this->readBuffer) {
        throw runtime_error("Unexpected error when allocating stream buffer.");
    }

    size_t bytesRead = 0;
    size_t totalBytes = 0;

    // Read chunks until the length is reached.
    while (
        (bytesRead = fread(
                this->readBuffer + totalBytes,
                1,
                bufferSize,
                this->resource)
        ) > 0)
    {
        totalBytes += bytesRead;

        size_t bytesRemain = length - totalBytes;
        if (!bytesRemain) break;

        if (bytesRemain < STREAM_BUFFER_SIZE) bufferSize = bytesRemain;

        this->readBuffer = (char*)realloc(
            this->readBuffer,
            totalBytes + bufferSize);

        if (!this->readBuffer) {
            throw runtime_error(
                "Unexpected error when reallocating stream buffer.");
        }
    }

    // Read error checks.
    if (totalBytes < length && ferror(this->resource)) {
        ostringstream oss;
        oss << "Unexpected error after reading " << bytesRead
            << " of " << length
            << " bytes: " << strerror(errno) << ".";
        throw runtime_error(oss.str());
    }

    // Don't forget null-terminator!
    this->readBuffer[totalBytes] = '\0';

    return this->readBuffer;
}

const char* Stream::getContents() {
    if (!this->resource) {
        throw runtime_error("Attempted getContents() on closed or detached stream.");
    }

    return this->read(MAX_STREAM_READ_SIZE);
}

Stream::~Stream() {
    // NOTE: We probably don't want to free this->resource because it could
    // be stdin or something. Users should be using close() or detach() when
    // they're done.
    free(this->readBuffer);
}

}}} // Csr::Http::Message
