#include "Request.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

void testCreateRequestWithUriString() {
    // Setup.
    Request* request = NULL;

    // Given we have a request with method "GET" and uri string
    // "http://host/path".
    request = new Request("GET", "http://HOST/path");

    // Then the method is "GET".
    assert(!strcmp(request->getMethod(), "GET"));

    // And the host header is "host".
    ValueIterator values = request->getHeader("Host");
    assert(values.next());
    assert(!strcmp(values.getValue(), "host"));

    // And the request-target is "/path".
    assert(!strcmp(request->getRequestTarget(), "/path"));

    // Teardown.
    delete request;
}

void testCreateRequestWithUri() {
    // Setup.
    Request* request = NULL;

    // Given we have a request with method "GET" and uri "http://host/path".
    request = new Request("GET", new Uri("http://HOST/path"));

    // Then the method is "GET".
    assert(!strcmp(request->getMethod(), "GET"));

    // And the host header is "host".
    ValueIterator values = request->getHeader("Host");
    assert(values.next());
    assert(!strcmp(values.getValue(), "host"));

    // And the request-target is "/path".
    assert(!strcmp(request->getRequestTarget(), "/path"));

    // Teardown.
    // NOTE: Deleting the request should delete the uri too.
    delete request;
}

void testRequestTarget() {
    // Setup.
    Request* request = NULL;

    // Given we have a valid request with uri string "http://HOST/path".
    request = new Request("GET", "http://HOST/path");

    // When we set the request target to "test".
    request->setRequestTarget("test");

    // Then we see it is "test".
    assert(!strcmp(request->getRequestTarget(), "test"));

    // Teardown.
    delete request;
}

void testMethod() {
    // Setup.
    Request* request = NULL;

    // Given we have a valid request.
    request = new Request("GET", "http://HOST/path");

    // When we set the method to "post".
    request->setMethod("post");

    // Then we see it is "post".
    assert(!strcmp(request->getMethod(), "post"));

    // Teardown.
    delete request;
}

void testRequestUri() {
    // Setup.
    Request* request = NULL;

    // Given we have a request with method "GET" and uri "http://host/path".
    request = new Request("GET", new Uri("http://host/path"));

    // When we set the uri to "http://domain/path" WITHOUT preserveHost.
    request->setUri(new Uri("http://domain/path"));

    // Then we see the uri is "http://domain/path".
    assert(!strcmp(request->getUri()->toString(), "http://domain/path"));

    // And we see the host header is "host".
    ValueIterator values = request->getHeader("Host");
    assert(values.next());
    assert(!strcmp(values.getValue(), "host"));

    // When we set the uri to "http://domain/path" WITH preserveHost.
    request->setUri(new Uri("http://domain/path"), true);

    // And we see the host header is "domain".
    values = request->getHeader("Host");
    assert(values.next());
    assert(!strcmp(values.getValue(), "domain"));

    // Teardown.
    // NOTE: Setting the uri as well as deleting the request should delete the
    // underlying uri, so don't worry about memory leaks.
    delete request;
}

} // namespace

void RequestTest() {
    testCreateRequestWithUriString();
    testCreateRequestWithUri();
    testRequestTarget();
    testMethod();
    testRequestUri();
    printf("RequestTest passed!\n");
}

}}} // Csr::Http::Message
