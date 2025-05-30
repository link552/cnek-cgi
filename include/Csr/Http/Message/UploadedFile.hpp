#ifndef CSR_HTTP_MESSAGE_UPLOADEDFILE

#include "Stream.hpp"

#include <stdlib.h>

namespace Csr {
namespace Http {
namespace Message {

/**
 * Codes to indicate file upload error status.
 */
enum UploadError {
    /** There is no error, the file uploaded with success. */
    UPLOAD_ERR_OK = 0,

    /** The uploaded file exceeds the server's maximum upload size. */
    UPLOAD_ERR_INI_SIZE = 1,

    /**
     * The uploaded file exceeds the server's maximum upload size that was
     * specified in the HTML form.
     */
    UPLOAD_ERR_FORM_SIZE = 2,

    /** The uploaded file was only partially uploaded. */
    UPLOAD_ERR_PARTIAL = 3,

    /** No file was uploaded. */
    UPLOAD_ERR_NO_FILE = 4,

    /** Missing a temporary folder. */
    UPLOAD_ERR_NO_TMP_DIR = 6,

    /** Failed to write file to disk. */
    UPLOAD_ERR_CANT_WRITE = 7,

    /** An extension stopped the file upload. */
    UPLOAD_ERR_EXTENSION = 8
};

/**
 * Value object representing a file uploaded through an HTTP request.
 */
class UploadedFile {
    Stream* stream;
    long size;
    UploadError error;
    char* clientFileName;
    char* clientMediaType;

    public:
    /**
     * Create a new uploaded file.
     *
     * If a size is not provided it will be determined by checking the size of
     * the stream.
     *
     * @param stream The underlying stream representing the uploaded file
     *     content.
     * @param size The size of the file in bytes.
     * @param error The file upload error.
     * @param clientFileName The filename as provided by the client, if any.
     * @param clientMediaType The media type as provided by the client, if any.
     * @throws std::invalid_argument The file resource is not readable.
     */
    UploadedFile(
        Stream* stream,
        long size = 0,
        UploadError error = UPLOAD_ERR_OK,
        const char* clientFileName = NULL,
        const char* clientMediaType = NULL);

    /**
     * Retrieve a stream representing the uploaded file.
     *
     * This method MUST return a Stream instance, representing the uploaded
     * file. The purpose of this method is to allow utilizing native stream
     * functionality file upload.
     *
     * If the moveTo() method has been called previously, this method MUST raise
     * an exception.
     *
     * @return Stream representation of the uploaded file.
     * @throws std::runtime_error Stream is unavailable or can't be created.
     */
    Stream* getStream();

    /**
     * Move the uploaded file to a new location.
     *
     * This method is guaranteed to work in both SAPI and non-SAPI environments.
     * Implementations must determine which environment they are in, and use the
     * appropriate function to perform the operation.
     *
     * targetPath may be an absolute path, or a relative path.
     *
     * The original file or stream MUST be removed on completion.
     *
     * If this method is called more than once, any subsequent calls MUST raise
     * an exception.
     *
     * If you wish to move to a stream, use getStream(), as SAPI operations
     * cannot guarantee writing to stream destinations.
     *
     * @param targetPath Path to which to move the uploaded file.
     * @throws std::invalid_argument The targetPath specified is invalid.
     * @throws std::runtime_error Error during the move operation or
     *     subsequent calls to the method.
     */
    void moveTo(const char* targetPath);

    /**
     * Retrieve the file size.
     *
     * @return The file size in bytes or 0 if unknown.
     */
    long getSize();

    /**
     * Retrieve the error associated with the uploaded file.
     *
     * If the file was uploaded successfully, this method MUST return
     * UPLOAD_ERR_OK.
     *
     * @return One of the upload errors.
     */
    UploadError getError();

    /**
     * Retrieve the filename sent by the client.
     *
     * Do NOT trust the value returned by this method. A client could send
     * a malicious filename with the intention to corrupt or hack your
     * application.
     *
     * @return The filename sent by the client or null-terminated string if
     *     none were provided.
     */
    const char* getClientFileName();

    /**
     * Retrieve the media type sent by the client.
     *
     * Do NOT trust the value returned by this method. A client could send
     * a malicious media type with the intention to corrupt or hack your
     * application.
     *
     * @return The media type sent by the client or null-terminated string if
     *     none were provided.
     */
    const char* getClientMediaType();

    ~UploadedFile();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_UPLOADEDFILE
#endif // CSR_HTTP_MESSAGE_UPLOADEDFILE
