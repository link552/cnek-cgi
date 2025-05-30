#include "Stream.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>

namespace Csr {
namespace Http {
namespace Message {

using std::runtime_error;

namespace {

void testDefaultStream() {
    // Given we have a default stream.
    Stream* stream = new Stream();

    // When we check if it's seekable, readable, and writable.
    // Then we see it is seekable, readable, and writable.
    assert(stream->isSeekable());
    assert(stream->isReadable());
    assert(stream->isWritable());

    // Teardown.
    stream->close();
    delete stream;
}

void testToString() {
    // Setup.
    Stream* stream = new Stream();

    // Given we write string "abc122" to the stream.
    stream->write("abc123");

    // And we seek to the end of the string.
    stream->seek(0, SEEK_END);

    // When we get the stream as a string.
    const char* string = stream->toString();

    // Then we see "abc123".
    assert(!strcmp(string, "abc123"));

    // Teardown.
    stream->close();
    delete stream;
}

void testClose() {
    // Given we have a valid stream.
    Stream* stream = new Stream();

    // When we close the stream.
    stream->close();

    // Then we can no longer seek.
    assert(!stream->isSeekable());

    // Teardown.
    delete stream;
}

void testDetach() {
    // Given we have a valid stream.
    Stream* stream = new Stream();

    // When we detach the stream.
    FILE* resource = stream->detach();

    // Then we see the resource is valid.
    assert(resource);

    // And the stream is in an unusable state.
    assert(!stream->isSeekable());
    assert(!stream->isReadable());
    assert(!stream->isWritable());

    // Teardown.
    delete stream;
    fclose(resource);
}

void testGetSize() {
    // Given we have a valid stream.
    Stream* stream = new Stream();

    // When we write 10 bytes to it.
    stream->write("0123456789");

    // Then we see the size is 10 bytes.
    assert(stream->getSize() == 10);

    // Teardown.
    stream->close();
    delete stream;
}

void testSeekTellRewind() {
    // Setup.
    FILE* file = tmpfile();
    Stream* stream = new Stream(file);

    // Given we write "0123456789" to the stream.
    stream->write("0123456789");

    // When we seek to position 5.
    stream->seek(5);

    // Then we see the position is 5.
    assert(stream->tell() == 5);

    // When we rewind.
    stream->rewind();

    // Then we see the position is 0.
    assert(!stream->tell());

    // When we close the stream's underlying resource file.
    fclose(file);

    // Then we see seek() fails.
    bool error = false;
    try {stream->seek(0);} catch (runtime_error) {error = true;}
    assert(error);

    // And we see tell() fails.
    error = false;
    try {stream->tell();} catch (runtime_error) {error = true;}
    assert(error);

    // Teardown.
    stream->close();
    delete stream;
}

void testEof() {
    // Setup.
    Stream* stream = new Stream();

    // Given we write "0123456789" to the stream.
    stream->write("0123456789");

    // And we seek to the end.
    stream->seek(0, SEEK_END);

    // When we read 1 byte.
    stream->read(1);

    // Then we see it is end-of-file.
    assert(stream->eof());

    // Teardown.
    stream->close();
    delete stream;
}

void testReadWriteEof() {
    // Setup.
    FILE* file = tmpfile();
    Stream* stream = new Stream(file);

    // Given we write "0123456789" to the stream.
    stream->write("0123456789");

    // And we rewind to the beginning.
    stream->rewind();

    // When we read 11 bytes from the stream.
    const char* contents = stream->read(11);

    // Then we see "0123456789".
    assert(!strcmp(contents, "0123456789"));

    // And the stream is at end-of-file.
    assert(stream->eof());

    // When we close the stream's underlying resource file.
    fclose(file);

    // Then we see write() fails.
    bool error = false;
    try {stream->write("test");} catch (runtime_error) {error = true;}
    assert(error);

    // And we see read() fails.
    error = false;
    try {stream->read(11);} catch (runtime_error) {error = true;}
    assert(error);

    // Teardown.
    stream->close();
    delete stream;
}

void testGetContents() {
    // Setup.
    Stream* stream = new Stream();

    // Given we write "0123456789" to the stream.
    stream->write("0123456789");

    // When we seek to position 5.
    stream->seek(5);

    // Then we see the contents are "56789".
    assert(!strcmp(stream->getContents(), "56789"));

    // Teardown.
    stream->close();
    delete stream;
}

} // namespace

void StreamTest() {
    testDefaultStream();
    // NOTE: No automated test for creating stream from existing file. This
    // should be tested manually.
    testToString();
    testClose();
    testDetach();
    testGetSize();
    testSeekTellRewind();
    testReadWriteEof();
    testGetContents();
    printf("StreamTest passed!\n");
}

}}} // Csr::Http::Message
