#ifndef CSR_HTTP_MESSAGE_STREAM

#include <stdio.h>

namespace Csr {
namespace Http {
namespace Message {

/**
 * Describes a data stream.
 *
 * Typically, an instance will wrap a FILE*; this interface provides
 * a wrapper around the most common operations, including serialization of
 * the entire stream to a string.
 */
class Stream {
    FILE* resource;
    char* readBuffer;
    bool writable;
    bool readable;

    public:

    /**
     * Creates a default stream.
     *
     * The stream SHOULD be created with a temporary resource.
     *
     * @throws std::runtime_error The temporary resource could not be created.
     */
    Stream();

    /**
     * Create a stream from an existing file.
     *
     * The file MUST be opened using the given mode, which may be any mode
     * supported by the `fopen` function.
     *
     * The `filename` MAY be any string supported by `fopen()`.
     *
     * @see https://cplusplus.com/reference/cstdio/fopen/
     * @param filename The filename or stream URI to use as basis of stream.
     * @param mode The mode with which to open the underlying filename/stream.
     * @throws std::runtime_error The file cannot be opened or mode is invalid.
     */
    Stream(const char* filename, const char* mode = "r");

    /**
     * Create a new stream from an existing resource.
     *
     * The stream MUST be readable and may be writable.
     *
     * @param resource The resource to use as the basis for the stream.
     */
    Stream(FILE* resource);

    /**
     * Reads all data from the stream into a string, from the beginning to end.
     *
     * This method MUST attempt to seek to the beginning of the stream before
     * reading data and read the stream until the end is reached.
     *
     * Warning: This could attempt to load a large amount of data into memory.
     *
     * This method MUST NOT raise an exception.
     *
     * @return Stream data as a string.
     */
    const char* toString();

    /**
     * Closes the stream and any underlying resources.
     */
    void close();

    /**
     * Separates any underlying resources from the stream.
     *
     * After the stream has been detached, the stream is in an unusable state.
     *
     * @return Underlying resource or NULL.
     */
    FILE* detach();

    /**
     * Get the size of the stream if known.
     *
     * @return The size in bytes if known, or -1 if unknown.
     */
    long getSize();

    /**
     * Get the current position of the file read/write pointer.
     *
     * @return Position of the file pointer.
     * @throws std::runtime_error Unexpected error.
     */
    long tell();

    /**
     * Checks if the stream is at the end.
     *
     * @return True of end-of-file, false if not.
     */
    bool eof();

    /**
     * Checks whether or not the stream is seekable.
     *
     * @return True if the stream is seekable, false if not.
     */
    bool isSeekable();

    /**
     * Sets the position indicator for the stream.
     *
     * @see https://en.cppreference.com/w/c/io/fseek
     * @param offset Stream offset in bytes.
     * @param whence Specifies how the cursor position will be calculated
     *     based on the seek offset. Valid values are identical to the built-in
     *     origin values for `fseek()`. SEEK_SET: Set position equal to offset
     *     bytes. SEEK_CUR: Set position to current location plus offset.
     *     SEEK_END: Set position to end-of-stream plus offset.
     * @throws std::runtime_error Unexpected error.
     */
    void seek(long offset, int whence = SEEK_SET);

    /**
     * Moves position indicator to the beginning of the stream.
     *
     * @see https://en.cppreference.com/w/c/io/rewind
     */
    void rewind();

    /**
     * Checks whether or not the stream is writable.
     *
     * @return True if the stream is writable, false if not.
     */
    bool isWritable();

    /**
     * Write data to the stream.
     *
     * @param string The string that is to be written.
     * @return The number of bytes written to the stream.
     * @throws std::runtime_error Unexpected error.
     */
    size_t write(const char* string);

    /**
     * Checks whether or not the stream is readable.
     *
     * @return True if the stream is readable, false if not.
     */
    bool isReadable();

    /**
     * Read data from the stream.
     *
     * @param length Read up to `length` bytes from the object and return
     *     them. Fewer than `length` bytes may be returned if underlying stream
     *     call returns fewer bytes.
     * @return The data read from the stream, or an empty string if no bytes
     *     are available.
     * @throws std::runtime_error Unexpected error.
     */
    const char* read(size_t length);

    /**
     * Returns the remaining contents from the current position.
     *
     * @return The remaining contents.
     * @throws std::runtime_error Unable to read or unexpected error.
     */
    const char* getContents();

    ~Stream();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_STREAM
#endif // CSR_HTTP_MESSAGE_STREAM
