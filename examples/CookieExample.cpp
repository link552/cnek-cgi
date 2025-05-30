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
    const char* foo = serverRequest->getCookieParam("foo");
    if (!*foo) {
        response->setAddedHeader("Set-Cookie", "foo=barbaz and pizza!");
        response->setAddedHeader("Set-Cookie", "a=b");
        body->write("<button onclick=\"location.reload();\">Refresh Page</button>");
    } else {
        body->write("foo=");
        body->write(foo);
        body->write("&amp;");

        const char* a = serverRequest->getCookieParam("a");
        body->write("a=");
        body->write(a);
    }

    // Emit response.
    cnek.emitResponse(response, stdout);
}
