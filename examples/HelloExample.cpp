#include "Cnek.hpp"

#include <string.h>

// Settings.
#define REQUEST_BODY_BUFFER_SIZE 4096
#define MAX_REQUEST_BODY_SIZE 4194304
#define UPLOAD_LINE_SIZE 4096
#define MAX_UPLOAD_FILE_SIZE 4194304
#define MAX_HEADER_COUNT 32
#define MAX_HEADER_LENGTH 1024
#define REQUEST_BODY_BUFFER_SIZE 4096
#define MAX_REQUEST_BODY_SIZE 4194304
#define UPLOAD_MOVE_BUFFER_SIZE 4096

using Csr::Http::Message::ServerRequest;
using Csr::Http::Message::Response;
using Csr::Http::Message::Stream;

int main() {
    // Setup.
    Cnek::Cnek cnek;
    ServerRequest* serverRequest = cnek.getServerRequest(environ, stdin);
    Response* response = new Response();
    Stream* body = response->getBody();

    response->setHeader("Content-Type", "text/html");

    // Handle request.
    if (!strcmp(serverRequest->getMethod(), "GET")) {
        body->write("<form method=\"POST\"><label>First name:</label><input type=\"text\" name=\"first\"/><br/><label>Last name:</label><input type=\"text\" name=\"last\"/><br/><input type=\"submit\"/></form>");
    } else {
        const char* first = serverRequest->getBodyParam("first");
        const char* last = serverRequest->getBodyParam("last");
        body->write("Hello, ");
        if (*first && *last) {
            body->write(first);
            body->write(" ");
            body->write(last);
        }
        body->write("!");
    }

    // Emit response.
    cnek.emitResponse(response, stdout);
}
