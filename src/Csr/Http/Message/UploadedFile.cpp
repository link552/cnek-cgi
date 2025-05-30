#include "UploadedFile.hpp"
#include "Shared.hpp"

#include <float.h>

// Size of buffer used to move uploaded files.
#ifndef UPLOAD_MOVE_BUFFER_SIZE
#define UPLOAD_MOVE_BUFFER_SIZE 4096 // Default 4KB.
#endif // UPLOAD_MOVE_BUFFER_SIZE

namespace Csr {
namespace Http {
namespace Message {

UploadedFile::UploadedFile(
    Stream* stream,
    long size,
    UploadError error,
    const char* clientFileName,
    const char* clientMediaType)
{
    this->stream = stream;
    this->size = size;
    this->error = error;
    this->clientFileName = copystr(clientFileName);
    this->clientMediaType = copystr(clientMediaType);
}

Stream* UploadedFile::getStream() {
    return this->stream;
}

void UploadedFile::moveTo(const char* targetPath) {
    // TODO: For security purposes, the caller could be putting user-controlled
    // data in `targetPath`, causing a path traversal attack. If possible, the
    // `targetPath` should be sanitized and validated.
    Stream* outFile = new Stream(targetPath, "wb");
    if (!outFile) return;

    char buffer[UPLOAD_MOVE_BUFFER_SIZE];

    // Start at the beginning.
    this->stream->rewind();

    while (!this->stream->eof()) {
        const char* content = this->stream->read(sizeof(buffer));
        outFile->write(content);
    }

    outFile->close();
    delete outFile;
}

long UploadedFile::getSize() {
    return this->size;
}

UploadError UploadedFile::getError() {
    return this->error;
}

const char* UploadedFile::getClientFileName() {
    return this->clientFileName;
}

const char* UploadedFile::getClientMediaType() {
    return this->clientMediaType;
}

UploadedFile::~UploadedFile() {
    delete[] this->clientFileName;
    delete[] this->clientMediaType;
    if (this->stream) this->stream->close();
    delete this->stream;
}

}}} // Csr::Http::Message
