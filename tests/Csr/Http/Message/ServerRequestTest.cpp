#include "ServerRequest.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

void testCreateServerRequest() {
    // Setup.
    ServerRequest* serverRequest = NULL;

    char** serverParams = new char*[3];

    const char* contentLength = "HTTP_FOO=Bar";
    serverParams[0] = new char[strlen(contentLength) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], contentLength);

    const char* serverProtocol = "SERVER_PROTOCOL=HTTP/1.0";
    serverParams[1] = new char[strlen(serverProtocol) + 1];
    *serverParams[1] = '\0';
    strcpy(serverParams[1], serverProtocol);

    serverParams[2] = NULL;

    // Given we have a server request with method "GET", uri "/path",
    // and server params "HTTP_FOO=Bar" and "SERVER_PROTOCOL=HTTP/1.0".
    serverRequest = new ServerRequest("GET", "/path", serverParams); 

    // Then we see the method is "GET".
    assert(!strcmp(serverRequest->getMethod(), "GET"));

    // And the uri is "/path".
    assert(!strcmp(serverRequest->getUri()->toString(), "/path"));

    // And the Foo header is "Bar".
    const char* header = serverRequest->getHeaderLine("Foo");
    assert(!strcmp(header, "Bar"));

    // And the protcol version is "1.0";
    assert(!strcmp(serverRequest->getProtocolVersion(), "1.0"));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetServerParam() {
    // Setup.
    ServerRequest* serverRequest = NULL;

    char** serverParams = new char*[2];

    const char* param = "H_R=123";
    serverParams[0] = new char[strlen(param) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], param);

    serverParams[1] = NULL;

    // Given we have a server request with server param "H_R=123"
    serverRequest = new ServerRequest("GET", "/path", serverParams); 

    // Then we see the server param "H_R" is "123".
    assert(!strcmp(serverRequest->getServerParam("H_R"), "123"));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetCookieParam() {
    // Setup.
    ServerRequest* serverRequest = NULL;

    char** serverParams = new char*[2];

    const char* cookie = "HTTP_COOKIE=A=B; C=D";
    serverParams[0] = new char[strlen(cookie) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], cookie);

    serverParams[1] = NULL;

    // Given we have a server request with cookie header "A=B; C=D".
    serverRequest = new ServerRequest("GET", "/path", serverParams); 

    // Then we see the cookie param "A" is "B".
    assert(!strcmp(serverRequest->getCookieParam("A"), "B"));

    // And the cookie param "C" is "D".
    assert(!strcmp(serverRequest->getCookieParam("C"), "D"));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetQueryParam() {
    // Setup.
    ServerRequest* serverRequest = NULL;

    char** serverParams = new char*[2];

    const char* query = "QUERY_STRING=A=B&C=D";
    serverParams[0] = new char[strlen(query) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], query);

    serverParams[1] = NULL;

    // Given we have a server request with query string "A=B&C=D".
    serverRequest = new ServerRequest("GET", "/path", serverParams); 

    // Then we see the query param "A" is "B".
    assert(!strcmp(serverRequest->getQueryParam("A"), "B"));

    // And the server param "C" is "D".
    assert(!strcmp(serverRequest->getQueryParam("C"), "D"));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetUploadedFile() {
    // Setup.
    ServerRequest* serverRequest = NULL;
    UploadedFile* uploadedFile = NULL;

    char** serverParams = new char*[2];

    const char* contentType = "CONTENT_TYPE=multipart/form-data; boundary=----WebKitFormBoundaryBSpdgBCbkl5eCIOX";
    serverParams[0] = new char[strlen(contentType) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], contentType);

    serverParams[1] = NULL;

    // Given we have a server request with method "POST", Content-Type
    // "multipart/form-data; boundary=----WebKitFormBoundaryBSpdgBCbkl5eCIOX",
    // and a body containing a field named "field" with value "Foo+Bar", and
    // two files named "fileONE" with contents "This is file 1." and "fileTWO"
    // with contents "This is file 2.".
    serverRequest = new ServerRequest("POST", "/path", serverParams); 
    serverRequest->getBody()->write("------WebKitFormBoundaryBSpdgBCbkl5eCIOX\nContent-Disposition: form-data; name=\"field\"\n\nFoo+Bar\n------WebKitFormBoundaryBSpdgBCbkl5eCIOX\nContent-Disposition: form-data; name=\"fileONE\"; filename=\"file1.txt\"\nContent-Type: text/plain\n\nThis is file 1.\n------WebKitFormBoundaryBSpdgBCbkl5eCIOX\nContent-Disposition: form-data; name=\"fileTWO\"; filename=\"file2.txt\"\nContent-Type: text/plain\n\nThis is file 2.\n------WebKitFormBoundaryBSpdgBCbkl5eCIOX--");

    // Then we see the value for body param "field" is "Foo Bar".
    assert(!strcmp(serverRequest->getBodyParam("field"), "Foo Bar"));

    // And the content for uploaded file "fileONE" is "This is file 1."
    uploadedFile = serverRequest->getUploadedFile("fileONE");
    assert(uploadedFile);
    assert(!strcmp(uploadedFile->getStream()->toString(), "This is file 1."));

    // And the content for uploaded file "fileTWO" is "This is file 2."
    uploadedFile = serverRequest->getUploadedFile("fileTWO");
    assert(uploadedFile);
    assert(!strcmp(uploadedFile->getStream()->toString(), "This is file 2."));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetBodyParam() {
    // Setup.
    ServerRequest* serverRequest = NULL;
    UploadedFile* uploadedFile = NULL;

    char** serverParams = new char*[2];

    const char* contentType = "CONTENT_TYPE=application/x-www-form-urlencoded";
    serverParams[0] = new char[strlen(contentType) + 1];
    *serverParams[0] = '\0';
    strcpy(serverParams[0], contentType);

    serverParams[1] = NULL;

    // Given we have a server request with method "POST", Content-Type
    // "application/x-www-form-urlencoded", and a body containing
    // "field1=Foo%20Bar&field2=Foo%20Baz".
    serverRequest = new ServerRequest("POST", "/path", serverParams); 
    serverRequest->getBody()->write("field1=Foo%20Bar&field2=Foo%20Baz");

    // Then we see the value for body param "field1" is "Foo Bar".
    assert(!strcmp(serverRequest->getBodyParam("field1"), "Foo Bar"));

    // Then we see the value for body param "field2" is "Foo Baz".
    assert(!strcmp(serverRequest->getBodyParam("field2"), "Foo Baz"));

    // Teardown.
    for (char** p = serverParams; *p; p++) delete[] *p;
    delete[] serverParams;
    delete serverRequest;
}

void testGetSetRemoveAttribute() {
    // Setup.
    ServerRequest* serverRequest = NULL;
    UploadedFile* uploadedFile = NULL;

    char** serverParams = new char*[1];
    serverParams[0] = NULL;

    // Given we have a valid server request.
    serverRequest = new ServerRequest("GET", "/path", serverParams); 

    // When we set attribute "Foo" to value "Bar".
    serverRequest->setAttribute("Foo", "Bar");

    // Then we see attribute "Foo" is value "Bar".
    assert(!strcmp(serverRequest->getAttribute("Foo"), "Bar"));

    // When we remove attribute "Foo".
    serverRequest->removeAttribute("Foo");

    // Then we see attribute "Foo" is default value "Baz".
    assert(!strcmp(serverRequest->getAttribute("Foo", "Baz"), "Baz"));

    // Teardown.
    delete[] serverParams;
    delete serverRequest;
}

} // namespace

void ServerRequestTest() {
    testCreateServerRequest();
    testGetServerParam();
    testGetCookieParam();
    testGetQueryParam();
    testGetUploadedFile();
    testGetBodyParam();
    testGetSetRemoveAttribute();
    printf("ServerRequestTest passed!\n");
}

}}} // Csr::Http::Message
