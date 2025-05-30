#include "Response.hpp"
#include "Shared.hpp"

#include <stdexcept>
#include <sstream>

namespace Csr {
namespace Http {
namespace Message {

using std::ostringstream;
using std::invalid_argument;

namespace {

/**
 * Validates whether an HTTP status code is one of the official accepted
 * status codes.
 *
 * @see http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
 * @param code HTTP status code to validate.
 * @throws std::invalid_argument Status code is not one of the accepted HTTP
 *     status codes.
 */
inline void validateStatusCode(unsigned short code) {
    if ((code >= 100 && code <= 104)
        || (code >= 200 && code <= 208)
        || (code == 226)
        || (code >= 300 && code <= 308)
        || (code >= 400 && code <= 418)
        || (code >= 421 && code <= 431)
        || (code == 451)
        || (code >= 500 && code <= 511))
    {
        return;
    }

    ostringstream oss;
    oss << "Invalid status code: " << code << ".";
    throw invalid_argument(oss.str());
}

} // namespace

Response::Response(unsigned short code, const char* reasonPhrase) {
    this->code = code;
    this->reasonPhrase = copystr(reasonPhrase);
}

unsigned short Response::getStatusCode() {
    return this->code;
}

void Response::setStatus(unsigned short code, const char* reasonPhrase) {
    validateStatusCode(code);
    this->code = code;

    delete[] this->reasonPhrase;
    this->reasonPhrase = copystr(reasonPhrase);
}

const char* Response::getReasonPhrase() {
    return this->reasonPhrase;
}

Response::~Response() {
    delete[] this->reasonPhrase; 
}

}}} // Csr::Http::Message
