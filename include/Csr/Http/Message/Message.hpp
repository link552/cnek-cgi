#ifndef CSR_HTTP_MESSAGE_MESSAGE

#include "Stream.hpp"

namespace Csr {
namespace Http {
namespace Message {

/**
 * Header value node for a header value linked list.
 */
struct ValueNode {
    char* value;
    ValueNode* next;

    /**
     * Stores a copy of value.
     *
     * @param value Header value to store.
     */
    ValueNode(const char* value);

    ~ValueNode();
};

/**
 * Header value linked list.
 */
struct ValueList {
    ValueNode* head;
    ValueNode* tail;
    char* line;

    ValueList();

    /**
     * Appends a value to the end of the linked list.
     *
     * @param value The value to append.
     */
    void addValue(const char* value);

    /**
     * Creates a comma-separated string of the list's values.
     *
     * @return The comma-separated string of values.
     */
    const char* getLine();

    ~ValueList();
};

/**
 * Iterates over a list of header values.
 */
class ValueIterator {
    ValueNode* first;
    ValueNode* cursor;

    public:
    /**
     * Create iterator and initialize first value.
     *
     * @param first First value in the list. For an empty iterator, set this
     *     to NULL.
     */
    ValueIterator(ValueNode* first);

    /**
     * Moves cursor to next value in the list.
     *
     * This method MUST be called at least once before values
     * can be retrieved.
     *
     * @return True if there was a next value to move to, false if not.
     */
    bool next();

    /**
     * Moves cursor back to the beginning of the list.
     */
    void reset();

    /**
     * Gets the value of the current node.
     *
     * @return Value of the current node.
     */
    const char* getValue();
};

/**
 * Header node for a header linked list.
 */
struct HeaderNode {
    char* name;
    ValueList* values;
    HeaderNode* next;

    /**
     * Stores a copy of name.
     *
     * @param name Header name to store.
     * @param value First value of the header.
     */
    HeaderNode(const char* name, const char* value);

    ~HeaderNode();
};

/**
 * Header linked list.
 */
struct HeaderList {
    HeaderNode* head;
    HeaderNode* tail;

    HeaderList();

    /**
     * Appends a header to the end of the linked list.
     *
     * If the header already exists, replace it.
     *
     * @param name Name of the header to append.
     * @param value Initial value for the header.
     */
    void addHeader(const char* name, const char* value);

    /**
     * Appends a value to the end of the header's values.
     *
     * @param name Name of the header to append.
     * @param value Value to append.
     */
    void addHeaderValue(const char* name, const char* value);

    /**
     * Gets the values of a header by the given case-insensitive name.
     *
     * @param name Case-insensitive header to retrieve values for.
     * @return Values for the header.
     */
    ValueList* getHeader(const char* name);

    /**
     * Checks if a header exists by the given case-insensitive name.
     *
     * @param name Case-insensitive header to search for.
     * @return True if the header exists, false if not.
     */
    bool hasHeader(const char* name);

    /**
     * Deletes a case-insensitive header from the linked list.
     *
     * @param name Case-insensitive name of the header to delete.
     */
    void removeHeader(const char* name);

    ~HeaderList();
};

/**
 * Iterates over a list of headers.
 */
class HeaderIterator {
    HeaderNode* first;
    HeaderNode* cursor;

    public:
    /**
     * Creates iterator and initializes first header.
     *
     * @param first First header in the list. For an empty iterator, set this
     *     to NULL.
     */
    HeaderIterator(HeaderNode* first);

    /**
     * Moves cursor to next header in the list.
     *
     * This method MUST be called at least once before headers
     * can be retrieved.
     *
     * @return True if there was a next header to move to, false if not.
     */
    bool next();

    /**
     * Moves cursor back to the beginning of the list.
     */
    void reset();

    /**
     * Gets the name of the current header.
     *
     * @return Name of the current header.
     */
    const char* getName();

    /**
     * Get an iterator of values for the current header.
     *
     * @return Iterator of values for the current header.
     */
    ValueIterator getValues();
};

/**
 * HTTP messages consist of requests from a client to a server and responses
 * from a server to a client. This interface defines the methods common to
 * each.
 *
 * @see http://www.ietf.org/rfc/rfc7230.txt
 * @see http://www.ietf.org/rfc/rfc7231.txt
 */
class Message {
    char* version;
    HeaderList headers;
    Stream* body;

    public:
    /**
     * Initializes an HTTP message.
     *
     * Messages should be initializes with a default temp stream body.
     */
    Message();

    /**
     * Retrieves the HTTP protocol version as a string.
     *
     * The string MUST contain only the HTTP version number (e.g., "1.1", "1.0").
     *
     * @return HTTP protocol version.
     */
    const char* getProtocolVersion();

    /**
     * Sets the specified HTTP protocol version.
     *
     * The version string MUST contain only the HTTP version number (e.g.,
     * "1.1", "1.0").
     *
     * @param version HTTP protocol version.
     */
    void setProtocolVersion(const char* version);

    /**
     * Retrieves all message header values.
     *
     * The keys represent the header name as it will be sent over the wire, and
     * each value is an iterator of strings associated with the header.
     *
     *     // Represent the headers as a string.
     *     HeaderIterator headers = message->getHeaders();
     *     while (headers.next()) {
     *         const char* name = headers.getName();
     *         printf("%s: %s\n", name, message->getHeaderLine(name));
     *     }
     *
     *     // Emit headers iteratively.
     *     HeaderIterator headers = message->getHeaders();
     *     while (headers.next()) {
     *         ValueIterator values = headers.getValues();
     *         while(values.next()) {
     *             printf("%s: %s\n", headers.getName(), values.getValue());
     *         }
     *     }
     *
     * If there are no headers in the message, this method MUST return an
     * empty iterator.
     *
     * While header names are not case-sensitive, getHeaders() will preserve the
     * exact case in which headers were originally specified.
     *
     * @return Iterator for all headers. If there are no headers in the message,
     *     this method MUST return an empty iterator.
     */
    HeaderIterator getHeaders();

    /**
     * Checks if a header exists by the given case-insensitive name.
     *
     * @param name Case-insensitive header field name.
     * @return True if any header names match the given header name using a
     *     case-insensitive string comparison. False if no matching header name
     *     is found in the message.
     */
    bool hasHeader(const char* name);

    /**
     * Retrieves a message header value by the given case-insensitive name.
     *
     * This method returns an iterator of all the header values of the given
     * case-insensitive header name.
     *
     * If the header does not appear in the message, this method MUST return an
     * empty iterator.
     *
     * @param name Case-insensitive header field name.
     * @return An iterator of string values as provided for the given header.
     *     If the header does not appear in the message, this method MUST
     *     return an empty iterator.
     */
    ValueIterator getHeader(const char* name);

    /**
     * Retrieves a comma-separated string of the values for a single header.
     *
     * This method returns all of the header values of the given
     * case-insensitive header name as a string concatenated together using
     * a comma.
     *
     * NOTE: Not all header values may be appropriately represented using
     * comma concatenation. For such headers, use getHeader() instead
     * and supply your own delimiter when concatenating.
     *
     * If the header does not appear in the message, this method MUST return
     * a null-terminated string.
     *
     * @param name Case-insensitive header field name.
     * @return A string of values as provided for the given header concatenated
     *     together using a comma. If the header does not appear in the message,
     *     this method MUST return an empty iterator.
     */
    const char* getHeaderLine(const char* name);

    /**
     * Set the specified header with the provided value.
     *
     * If the specified case-insensitive header already exists, replace it's
     * values with the provided value.
     *
     * While header names are case-insensitive, the casing of the header will
     * be preserved by this function, and returned from getHeaders().
     *
     * @param name Case-insensitive header field name.
     * @param value Header value.
     * @throws std::runtime_error Invalid header names or values.
     */
    void setHeader(const char* name, const char* value);

    /**
     * Sets the specified header appended with the given value.
     *
     * Existing values for the specified header will be maintained. The new
     * values will be appended to the existing list. If the header did not
     * exist previously, it will be added.
     *
     * @param name Case-insensitive header field name to add.
     * @param value Header value.
     * @throws std::runtime_error Invalid header names or values.
     */
    void setAddedHeader(const char* name, const char* value);

    /**
     * Removes the specified header with it's values.
     *
     * Header resolution MUST be done without case-sensitivity.
     *
     * @param name Case-insensitive header field name to remove.
     */
    void removeHeader(const char* name);

    /**
     * Gets the body of the message.
     *
     * @return The body as a stream.
     */
    Stream* getBody();

    /**
     * Sets the specified message body.
     *
     * The body MUST be a Stream object.
     *
     * @param body The body stream.
     * @throws std::runtime_error The body is not valid.
     */
    void setBody(Stream* body);

    ~Message();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_MESSAGE
#endif // CSR_HTTP_MESSAGE_MESSAGE
