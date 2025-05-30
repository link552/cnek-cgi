#include "UploadedFile.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

void testUploadGetStream() {
    // Setup.
    Stream* stream = new Stream();

    // Given we have an uploaded file with a given stream.
    UploadedFile* uploadedFile = new UploadedFile(stream);

    // Then we see the given stream in the uploaded file.
    assert(uploadedFile->getStream() == stream);

    // Teardown.
    // NOTE: Deleting uploadedFile will delete the underlying stream.
    delete uploadedFile;
}

void testUploadGetSize() {
    // Setup.
    UploadedFile* uploadedFile = NULL;

    // Given we have an uploaded file with size "100".
    uploadedFile = new UploadedFile(NULL, 100);

    // Then we see the size is "100".
    assert(uploadedFile->getSize() == 100);

    // Teardown.
    delete uploadedFile;
}

void testGetError() {
    // Setup.
    UploadedFile* uploadedFile = NULL;

    // Given we have an uploaded file with error UPLOAD_ERR_NO_FILE.
    uploadedFile = new UploadedFile(NULL, 0, UPLOAD_ERR_NO_FILE);

    // Then we see the error is UPLOAD_ERR_NO_FILE.
    assert(uploadedFile->getError() == UPLOAD_ERR_NO_FILE);

    // Teardown.
    delete uploadedFile;
}

void testGetClientFileName() {
    // Setup.
    UploadedFile* uploadedFile = NULL;

    // Given we have an uploaded file with client file name "test.txt".
    uploadedFile = new UploadedFile(NULL, 0, UPLOAD_ERR_OK, "test.txt");

    // Then we see the client file name is "test.txt".
    assert(!strcmp(uploadedFile->getClientFileName(), "test.txt"));

    // Teardown.
    delete uploadedFile;
}

void testGetClientMediaType() {
    // Setup.
    UploadedFile* uploadedFile = NULL;

    // Given we have an uploaded file with client media type "PDF".
    uploadedFile = new UploadedFile(NULL, 0, UPLOAD_ERR_OK, "file.pdf", "PDF");

    // Then we see the client media type is "PDF".
    assert(!strcmp(uploadedFile->getClientMediaType(), "PDF"));

    // Teardown.
    delete uploadedFile;
}

} // namespace

void UploadedFileTest() {
    testUploadGetStream();
    testUploadGetSize();
    testGetError();
    testGetClientFileName();
    testGetClientMediaType();
    printf("UploadedFileTest passed!\n");
}

}}} // Csr::Http::Message
