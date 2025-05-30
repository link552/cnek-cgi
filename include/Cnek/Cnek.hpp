#ifndef CNEK_CNEK

#include "ServerRequest.hpp"
#include "Response.hpp"

namespace Cnek {

/**
 * Main service for creating server-side HTTP requests and emitting HTTP
 * responses based on the CSR "C Standard Recommendations".
 */
class Cnek {
    Csr::Http::Message::ServerRequest* serverRequest;

    public:
    Cnek();

    /**
     * Retrieves a server request from the CGI environment.
     *
     * The first time this is called, it will create the request. Each call
     * after will return a pointer to the same request.
     *
     * It's expected that `environment` will be the global `environ` variable,
     * so this method MUST NOT modify the data within.
     *
     * Also, it's expected that `input` will be the global file `stdin`, so
     * this method MUST NOT read from it more than once, since `stdin` is
     * a non-seekable stream.
     *
     * @param environment Server API (SAPI) environment variables to build
     *     the request from.
     * @param input Body of the server request.
     * @return Server request created from `environment` and `input` or
     *     previously created server request.
     * @throws std::invalid_argument Invalid input parameters or missing
     *     required variables.
     * @throws std::runtime_error Failed to read or parse inputs.
     */
    Csr::Http::Message::ServerRequest* getServerRequest(
        char** environment,
        FILE* input);

    /**
     * Forms an appropriate HTTP response message and sends it.
     *
     * This method MUST delete/free the response after use.
     *
     * It's expected that `output` will be the global file `stdout`, so this
     * method MUST NOT attempt to seek it, since `stdout` is a non-seekable
     * stream.
     *
     * @param response Response to emit.
     * @param output Stream to write the response to.
     * @throws std::runtime_error Failed to write outputs.
     */
    void emitResponse(Csr::Http::Message::Response* response, FILE* output);

    ~Cnek();
};

} // Cnek
#define CNEK_CNEK
#endif // CNEK_CNEK
