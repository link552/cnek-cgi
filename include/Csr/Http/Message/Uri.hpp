#ifndef CSR_HTTP_MESSAGE_URI

#include <stdint.h>

namespace Csr {
namespace Http {
namespace Message {

/**
 * Value object representing a URI.
 *
 * This interface is meant to represent URIs according to RFC 3986 and to
 * provide methods for most common operations. Additional functionality for
 * working with URIs can be provided on top of the interface or externally.
 * Its primary use is for HTTP requests, but may also be used in other
 * contexts.
 *
 * Typically the Host header will also be present in the request message.
 * For server-side requests, the scheme will typically be discoverable in the
 * server parameters.
 *
 * @see http://tools.ietf.org/html/rfc3986 (the URI specification)
 */
class Uri {
    char* scheme;
    char* authority;
    char* userInfo;
    char* host;
    char* port;
    char* path;
    char* query;
    char* fragment;
    char* string;

    public:
    /**
     * Create a new URI.
     *
     * @param uri The URI to parse.
     * @throws std::invalid_argument The given URI cannot be parsed.
     */
    Uri(const char* uri = "");

    /**
     * Retrieve the scheme component of the URI.
     *
     * If no scheme is present, this method MUST return a null-terminated
     * string.
     *
     * The value returned MUST be normalized to lowercase, per RFC 3986
     * Section 3.1.
     *
     * The trailing ":" character is not part of the scheme and MUST NOT be
     * added.
     *
     * @see https://tools.ietf.org/html/rfc3986#section-3.1
     * @return The URI scheme.
     */
    const char* getScheme();

    /**
     * Retrieve the authority component of the URI.
     *
     * If no authority information is present, this method MUST return a
     * null-terminated string.
     *
     * The authority syntax of the URI is:
     *
     * <pre>
     * [user-info@]host[:port]
     * </pre>
     *
     * If the port component is not set or is the standard port for the current
     * scheme, it SHOULD NOT be included.
     *
     * @see https://tools.ietf.org/html/rfc3986#section-3.2
     * @return The URI authority, in "[user-info@]host[:port]" format.
     */
    const char* getAuthority();

    /**
     * Retrieve the user information component of the URI.
     *
     * If no user information is present, this method MUST return a
     * null-terminatd string.
     *
     * If a user is present in the URI, this will return that value;
     * additionally, if the password is also present, it will be appended to the
     * user value, with a colon (":") separating the values.
     *
     * The trailing "@" character is not part of the user information and MUST
     * NOT be added.
     *
     * @return The URI user information, in "username[:password]" format.
     */
    const char* getUserInfo();

    /**
     * Retrieve the host component of the URI.
     *
     * If no host is present, this method MUST return a null-terminated string.
     *
     * The value returned MUST be normalized to lowercase, per RFC 3986
     * Section 3.2.2.
     *
     * @see http://tools.ietf.org/html/rfc3986#section-3.2.2
     * @return The URI host.
     */
    const char* getHost();

    /**
     * Retrieve the port component of the URI.
     *
     * If a port is present, and it is non-standard for the current scheme,
     * this method MUST return it as a 16-bit unsigned integer. If the port
     * is the standard port used with the current scheme, this method SHOULD
     * return 0.
     *
     * If no port is present, and no scheme is present, this method MUST
     * return 0.
     *
     * If no port is present, but a scheme is present, this method MAY return
     * the standard port for that scheme, but SHOULD return 0.
     *
     * @return The URI port.
     */
    uint16_t getPort();

    /**
     * Retrieve the path component of the URI.
     *
     * The path can either be empty or absolute (starting with a slash) or
     * rootless (not starting with a slash). Implementations MUST support all
     * three syntaxes.
     *
     * Normally, the empty path "" and absolute path "/" are considered equal as
     * defined in RFC 7230 Section 2.7.3. But this method MUST NOT automatically
     * do this normalization because in contexts with a trimmed base path, e.g.
     * the front controller, this difference becomes significant. It's the task
     * of the user to handle both "" and "/".
     *
     * The value returned MUST be percent-encoded, but MUST NOT double-encode
     * any characters. To determine what characters to encode, please refer to
     * RFC 3986, Sections 2 and 3.3.
     *
     * As an example, if the value should include a slash ("/") not intended as
     * delimiter between path segments, that value MUST be passed in encoded
     * form (e.g., "%2F") to the instance.
     *
     * @see https://tools.ietf.org/html/rfc3986#section-2
     * @see https://tools.ietf.org/html/rfc3986#section-3.3
     * @return The URI path.
     */
    const char* getPath();

    /**
     * Retrieve the query string of the URI.
     *
     * If no query string is present, this method MUST return a null-terminated
     * string.
     *
     * The leading "?" character is not part of the query and MUST NOT be
     * added.
     *
     * The value returned MUST be percent-encoded, but MUST NOT double-encode
     * any characters. To determine what characters to encode, please refer to
     * RFC 3986, Sections 2 and 3.4.
     *
     * As an example, if a value in a key/value pair of the query string should
     * include an ampersand ("&") not intended as a delimiter between values,
     * that value MUST be passed in encoded form (e.g., "%26") to the instance.
     *
     * @see https://tools.ietf.org/html/rfc3986#section-2
     * @see https://tools.ietf.org/html/rfc3986#section-3.4
     * @return The URI query string.
     */
    const char* getQuery();

    /**
     * Retrieve the fragment component of the URI.
     *
     * If no fragment is present, this method MUST return a null-terminated
     * string.
     *
     * The leading "#" character is not part of the fragment and MUST NOT be
     * added.
     *
     * The value returned MUST be percent-encoded, but MUST NOT double-encode
     * any characters. To determine what characters to encode, please refer to
     * RFC 3986, Sections 2 and 3.5.
     *
     * @see https://tools.ietf.org/html/rfc3986#section-2
     * @see https://tools.ietf.org/html/rfc3986#section-3.5
     * @return The URI fragment.
     */
    const char* getFragment();

    /**
     * Change the specified scheme.
     *
     * Implementations MUST support the schemes "http" and "https" case
     * insensitively, and MAY accommodate other schemes if required.
     *
     * An empty scheme is equivalent to removing the scheme.
     *
     * @param scheme The scheme to use with the new instance.
     * @throws std::invalid_argument Invalid or unsupported scheme.
     */
    void setScheme(const char* scheme);

    /**
     * Change the specified user information.
     *
     * Password is optional, but the user information MUST include the
     * user; an empty string for the user is equivalent to removing user
     * information.
     *
     * @param user The user name to use for authority.
     * @param password The password associated with `user`.
     */
    void setUserInfo(const char* user, const char* password = "");

    /**
     * Change the specified host.
     *
     * An empty host value is equivalent to removing the host.
     *
     * @param host The hostname to use with the new instance.
     * @throws std::invalid_argument Invalid hostname.
     */
    void setHost(const char* host);

    /**
     * Change the specified port.
     *
     * Implementations MUST raise an exception for ports outside the
     * established TCP and UDP port ranges.
     *
     * A 0 value provided for the port is equivalent to removing the port
     * information.
     *
     * @param port The port to use with the new instance; a 0 value
     *     removes the port information.
     * @throws std::invalid_argument Invalid port number.
     */
    void setPort(uint16_t port);

    /**
     * Change the specified path.
     *
     * The path can either be empty or absolute (starting with a slash) or
     * rootless (not starting with a slash). Implementations MUST support all
     * three syntaxes.
     *
     * If an HTTP path is intended to be host-relative rather than path-relative
     * then it must begin with a slash ("/"). HTTP paths not starting with a slash
     * are assumed to be relative to some base path known to the application or
     * consumer.
     *
     * Users can provide both encoded and decoded path characters.
     * Implementations ensure the correct encoding as outlined in getPath().
     *
     * @param path The path to use with the new instance.
     * @throws std::invalid_argument Invalid path.
     */
    void setPath(const char* path);

    /**
     * Change the specified query string.
     *
     * Users can provide both encoded and decoded query characters.
     * Implementations ensure the correct encoding as outlined in getQuery().
     *
     * An empty query string value is equivalent to removing the query string.
     *
     * @param query The query string to use with the new instance.
     * @throws std::invalid_argument Invalid query string.
     */
    void setQuery(const char* query);

    /**
     * Change the specified URI fragment.
     *
     * Users can provide both encoded and decoded fragment characters.
     * Implementations ensure the correct encoding as outlined in getFragment().
     *
     * An empty fragment value is equivalent to removing the fragment.
     *
     * @param fragment The fragment to use with the new instance.
     */
    void setFragment(const char* fragment);

    /**
     * Return the string representation as a URI reference.
     *
     * Depending on which components of the URI are present, the resulting
     * string is either a full URI or relative reference according to RFC 3986,
     * Section 4.1. The method concatenates the various components of the URI,
     * using the appropriate delimiters:
     *
     * - If a scheme is present, it MUST be suffixed by ":".
     * - If an authority is present, it MUST be prefixed by "//".
     * - The path can be concatenated without delimiters. But there are two
     *   cases where the path has to be adjusted to make the URI reference
     *   valid as this method SHOULD NOT throw exceptions:
     *     - If the path is rootless and an authority is present, the path MUST
     *       be prefixed by "/".
     *     - If the path is starting with more than one "/" and no authority is
     *       present, the starting slashes MUST be reduced to one.
     * - If a query is present, it MUST be prefixed by "?".
     * - If a fragment is present, it MUST be prefixed by "#".
     *
     * @see http://tools.ietf.org/html/rfc3986#section-4.1
     * @return The full URI represented as a string.
     */
    const char* toString();

    ~Uri();
};

}}} // Csr::Http::Message
#define CSR_HTTP_MESSAGE_URI
#endif // CSR_HTTP_MESSAGE_URI
