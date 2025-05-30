#include "Cnek.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Cnek {

using Csr::Http::Message::ServerRequest;
using Csr::Http::Message::Response;

namespace {

void testGetServerRequest() {
    // Setup.
    char** env = new char*[3];

    const char* requestMethod = "REQUEST_METHOD=GET";
    env[0] = new char[strlen(requestMethod) + 1];
    *env[0] = '\0';
    strcpy(env[0], requestMethod);

    const char* requestUri = "REQUEST_URI=/foo/bar";
    env[1] = new char[strlen(requestUri) + 1];
    *env[1] = '\0';
    strcpy(env[1], requestUri);

    env[2] = NULL;

    const char* requestBody = "Hello, World!";
    FILE* input = tmpfile();
    fputs("Hello, World!", input);
    rewind(input);

    // Given we have a cnek with request method "GET", request uri "/foo/bar",
    // and request body "Hello, World!".
    Cnek cnek;

    // When we get the server request.
    ServerRequest* serverRequest = cnek.getServerRequest(env, input);

    // Then we see the method is "GET".
    assert(!strcmp(serverRequest->getMethod(), "GET"));

    // And we see the uri path is "/foo/bar".
    assert(!strcmp(serverRequest->getUri()->getPath(), "/foo/bar"));

    // And we see the body is "Hello, World!".
    assert(!strcmp(serverRequest->getBody()->toString(), "Hello, World!"));

    // Teardown.
    for (char** p = env; *p; p++) delete[] *p;
    delete[] env;
    fclose(input);
}

void testEmitResponse() {
    // Setup.
    FILE* output = tmpfile();
    Cnek cnek;

    // Given we have a response with Content-Type header "text/html", Status
    // of "400 Bad Request", and body of "An error occurred!".
    Response* response = new Response(400, "Bad Request");
    response->setHeader("Content-Type", "text/html");
    response->getBody()->write("An error occurred!");

    // When we emit the response.
    cnek.emitResponse(response, output);
    rewind(output);

    // Then we see the output content is as follows:
    //     Content-Type: text/html
    //     Status: 400 Bad Request
    //
    //     An error occurred!
    const char* expected =
        "Content-Type: text/html\r\nStatus: 400 Bad Request\r\n\r\nAn error occurred!";
    char* content = new char[strlen(expected)];
    fread(content, 1, 70, output);
    assert(!strcmp(content, expected));

    // Teardown.
    // NOTE: The response is freed when it is emitted.
    delete[] content;
    fclose(output);
}

} // namespace

void CnekTest() {
    testGetServerRequest();
    testEmitResponse();
    printf("CnekTest passed!\n");
}

} // Cnek
