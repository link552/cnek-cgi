#include "Cnek.hpp"

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
    const char* item = serverRequest->getQueryParam("item");
    if (!*item) {
        body->write("<form method=\"GET\"><input type=\"text\" name=\"item\"/><input type=\"submit\"/></form>");
    } else {
        body->write("You asked for item '");
        body->write(item);
        body->write("'!");
    }

    // Emit response.
    cnek.emitResponse(response, stdout);
}
