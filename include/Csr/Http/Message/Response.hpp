#ifndef CSR_HTTP_MESSAGE_RESPONSE

#include "Message.hpp"

namespace Csr {
namespace Http {
namespace Message {

/**
 * Representation of an outgoing, server-side response.
 *
 * Per the HTTP specification, this interface includes properties for
 * each of the following:
 *
 * - Protocol version
 * - Status code and reason phrase
 * - Headers
 * - Message body
 */
class Response : public Message {
    unsigned short code;
    char* reasonPhrase;

    public:
    /**
     * Create a new response.
     *
     * @param code The HTTP status code. Defaults to 200.
     * @param reasonPhrase The reason phrase to associate with the status code
     *     in the generated response. If none is provided, implementations MAY
     *     use the defaults as suggested in the HTTP specification.
     */
    Response(unsigned short code = 200, const char* reasonPhrase = "");

    /**
     * Gets the response status code.
     *
     * The status code is a 3-digit integer result code of the server's attempt
     * to understand and satisfy the request.
     *
     * @return Status code.
     */
    unsigned short getStatusCode();

    /**
     * Changes the specified status code and, optionally, reason phrase.
     *
     * If no reason phrase is specified, implementations MAY choose to default
     * to the RFC 7231 or IANA recommended reason phrase for the response's
     * status code.
     *
     * @see http://tools.ietf.org/html/rfc7231#section-6
     * @see http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
     * @param code The 3-digit integer result code to set.
     * @param reasonPhrase The reason phrase to use with the
     *     provided status code; if none is provided, implementations MAY
     *     use the defaults as suggested in the HTTP specification.
     * @throws std::invalid_argument Invalid status code arguments.
     */
    void setStatus(unsigned short code, const char* reasonPhrase = "");

    /**
     * Gets the response reason phrase associated with the status code.
     *
     * Because a reason phrase is not a required element in a response
     * status line, the reason phrase value MAY be empty. Implementations MAY
     * choose to return the default RFC 7231 recommended reason phrase (or those
     * listed in the IANA HTTP Status Code Registry) for the response's
     * status code.
     *
     * @see http://tools.ietf.org/html/rfc7231#section-6
     * @see http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
     * @return Reason phrase; must return a null-terminator string if none present.
     */
    const char* getReasonPhrase();

    ~Response();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_RESPONSE
#endif // CSR_HTTP_MESSAGE_RESPONSE
