#include "Response.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

void testCreateResponse() {
    // Setup.
    Response* response = NULL;

    // Given we have a response with status code "200" and reason phrase "OK".
    response = new Response(200, "OK");

    // Then the status code is "200".
    assert(response->getStatusCode() == 200);

    // And the reason phrase is "OK".
    assert(!strcmp(response->getReasonPhrase(), "OK"));

    // Teardown.
    delete response;
}

void testStatusCodeReasonPhrase() {
    // Setup.
    Response* response = NULL;

    // Given we have a valid response.
    response = new Response(200, "OK");

    // When we set the status code to "400" and reason phrase to "Bad Request".
    response->setStatus(400, "Bad Request");

    // Then we see the status code is "400".
    assert(response->getStatusCode() == 400);

    // And we see the reason phrase is "Bad Request".
    assert(!strcmp(response->getReasonPhrase(), "Bad Request"));

    // Teardown.
    delete response;
}

} // namespace

void ResponseTest() {
    testCreateResponse();
    testStatusCodeReasonPhrase();
    printf("ResponseTest passed!\n");
}

}}} // Csr::Http::Message
