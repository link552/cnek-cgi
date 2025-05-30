#ifndef CSR_HTTP_MESSAGE_REQUEST

#include "Message.hpp"
#include "Uri.hpp"

namespace Csr {
namespace Http {
namespace Message {

/**
 * Representation of an outgoing, client-side request.
 *
 * Per the HTTP specification, this interface includes properties for
 * each of the following:
 *
 * - Protocol version
 * - HTTP method
 * - URI
 * - Headers
 * - Message body
 *
 * During construction, implementations MUST attempt to set the Host header from
 * a provided URI if no Host header is provided.
 */
class Request : public Message {
    char* requestTarget;
    char* method;
    Uri* uri;

    public:
    /**
     * Create a new request.
     *
     * Implementations MUST attempt to set the Host header from a provided URI
     * if no Host header is provided.
     *
     * @param method The HTTP method associated with the request.
     * @param uri The URI string associated with the request. 
     * @throws std::invalid_argument Invalid HTTP method.
     */
    Request(const char* method, const char* uri);

    /**
     * Create a new request.
     *
     * Implementations MUST attempt to set the Host header from a provided URI
     * if no Host header is provided.
     *
     * @param method The HTTP method associated with the request.
     * @param uri The URI pointer associated with the request. 
     * @throws std::invalid_argument Invalid HTTP method.
     */
    Request(const char* method, Uri* uri);

    /**
     * Retrieves the message's request-target.
     *
     * Retrieves the message's request-target either as it will appear (for
     * clients), as it appeared at request (for servers), or as it was
     * specified for the instance (see setRequestTarget()).
     *
     * In most cases, this will be the origin-form of the composed URI,
     * unless a value was provided to the concrete implementation (see
     * setRequestTarget() below).
     *
     * If no URI is available, and no request-target has been specifically
     * provided, this method MUST return the string "/".
     *
     * @return Message's request-target.
     */
    const char* getRequestTarget();

    /**
     * Change the specific request-target.
     *
     * If the request needs a non-origin-form request-target — e.g., for
     * specifying an absolute-form, authority-form, or asterisk-form —
     * this method may be used to create an instance with the specified
     * request-target, verbatim.
     *
     * @see http://tools.ietf.org/html/rfc7230#section-5.3 (for the various
     *     request-target forms allowed in request messages)
     * @param requestTarget The request-target.
     */
    void setRequestTarget(const char* requestTarget);

    /**
     * Retrieves the HTTP method of the request.
     *
     * @return The request method.
     */
    const char* getMethod();

    /**
     * Change the provided HTTP method.
     *
     * While HTTP method names are typically all uppercase characters, HTTP
     * method names are case-sensitive and thus implementations SHOULD NOT
     * modify the given string.
     *
     * @param method Case-sensitive method.
     * @throws std::invalid_argument Invalid HTTP method.
     */
    void setMethod(const char* method);

    /**
     * Retrieves the URI instance.
     *
     * @see http://tools.ietf.org/html/rfc3986#section-4.3
     * @return The URI of the request.
     */
    Uri* getUri();

    /**
     * Change the provided URI.
     *
     * This method MUST update the Host header of the returned request by
     * default if the URI contains a host component. If the URI does not
     * contain a host component, any pre-existing Host header MUST be carried
     * over to the returned request.
     *
     * You can opt-in to preserving the original state of the Host header by
     * setting `preserveHost` to `true`. When `preserveHost` is set to 'true',
     * this method interacts with the Host header in the following ways:
     *
     * - If the Host header is missing or empty, and the new URI contains
     *   a host component, this method MUST update the Host header in the returned
     *   request.
     * - If the Host header is missing or empty, and the new URI does not contain a
     *   host component, this method MUST NOT update the Host header in the returned
     *   request.
     * - If a Host header is present and non-empty, this method MUST NOT update
     *   the Host header in the returned request.
     *
     * @see http://tools.ietf.org/html/rfc3986#section-4.3
     * @param uri New request URI to use.
     * @param preserveHost Preserve the original state of the Host header.
     */
    void setUri(Uri* uri, bool preserveHost = false);

    ~Request();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_REQUEST
#endif // CSR_HTTP_MESSAGE_REQUEST
