#include "Message.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace Csr {
namespace Http {
namespace Message {

namespace {

void testProtocolVersion() {
    // Setup.
    Message* message = new Message();

    // Given the protcol version is "\0".
    assert(!*message->getProtocolVersion());

    // When we set the protocol version to "1.1".
    message->setProtocolVersion("1.1");

    // Then we see the protocol version is "1.1".
    assert(!strcmp(message->getProtocolVersion(), "1.1"));

    // Teardown.
    delete message;
}

void testGetHeaders() {
    // Setup.
    Message* message = new Message();

    // Given we have NOT set any headers.
    assert(!message->getHeaders().next());

    // And we set header "Content-Type: text/html".
    message->setHeader("Content-Type", "text/html");

    // And we set header "Content-Length: 100".
    message->setHeader("Content-Length", "100");

    // And we set header "Set-Cookie: A=B".
    message->setHeader("Set-Cookie", "A=B");

    // When we get all headers.
    HeaderIterator headers = message->getHeaders();

    // Then we will see case preserved "Content-Type: text/html".
    assert(headers.next());
    assert(!strcmp(headers.getName(), "Content-Type"));
    {
        ValueIterator values = headers.getValues();
        assert(values.next());
        assert(!strcmp(values.getValue(), "text/html"));
    }

    // And we will see case preserved "Content-Length: 100".
    assert(headers.next());
    assert(!strcmp(headers.getName(), "Content-Length"));
    {
        ValueIterator values = headers.getValues();
        assert(values.next());
        assert(!strcmp(values.getValue(), "100"));
    }

    // And we will see case preserved "Set-Cookie: A=B".
    assert(headers.next());
    assert(!strcmp(headers.getName(), "Set-Cookie"));
    ValueIterator values = headers.getValues();
    {
        assert(values.next());
        assert(!strcmp(values.getValue(), "A=B"));
    }

    // Teardown.
    delete message;
}

void testHasHeader() {
    // Setup.
    Message* message = new Message();

    // Given we do NOT have case-insensitive header "content-type".
    assert(!message->hasHeader("content-type"));

    // When we set header "Content-Type: text/html".
    message->setHeader("Content-Type", "text/html");

    // Then we will have case-insensitive header "content-type".
    assert(message->hasHeader("content-type"));

    // Teardown.
    delete message;
}

void testSetGetHeader() {
    // Setup.
    Message* message = new Message();

    // Given we set header "Content-Type: text/html".
    message->setHeader("Content-Type", "text/html");

    // When we get case-insensitive header "content-type".
    ValueIterator values = message->getHeader("content-type");

    // Then we should see value "text/html".
    assert(values.next());
    assert(!strcmp(values.getValue(), "text/html"));

    // TODO: Test error for invalid header names or values (setHeader):
    //
    // Invalid header names contain characters outside the allowed set,
    // such as spaces, control characters, or special symbols like [, ],
    // (, ), comma, ;, :, <, >, @, \, ", /, {, }, ?, =, and whitespace.
    //
    // Values can include visible ASCII characters and horizontal tabs,
    // but not line breaks or other control codes.

    // Teardown.
    delete message;
}

void testSetAddedGetHeaderLine() {
    // Setup.
    Message* message = new Message();

    // Given the header line for header "Header" is a null-terminated string.
    assert(!*message->getHeaderLine("Header"));

    // And the "Content-Type" header is set to "text/html".
    message->setHeader("Content-Type", "text/html");
    // NOTE: This is targetting a specific bug where setAddedHeader() was not
    // adding non-existing headers when one already existed.

    // When we set added header "Header: A".
    message->setAddedHeader("Header", "A");

    // And we set added header "Header: B".
    message->setAddedHeader("Header", "B");

    // Then we should see value "A,B".
    const char* headerLine = message->getHeaderLine("header");
    assert(!strcmp(headerLine, "A,B"));

    // TODO: Test error for invalid header names or values (setAddedHeader):
    //
    // Invalid header names contain characters outside the allowed set,
    // such as spaces, control characters, or special symbols like [, ],
    // (, ), comma, ;, :, <, >, @, \, ", /, {, }, ?, =, and whitespace.
    //
    // Values can include visible ASCII characters and horizontal tabs,
    // but not line breaks or other control codes.

    // Teardown.
    delete message;
}

void testRemoveHeader() {
    // Setup.
    Message* message = new Message();

    // Given we set header "Content-Type: text/html".
    message->setHeader("Content-Type", "text/html");

    // And it CANNOT be removed with case-insensitive name "content-type".
    message->removeHeader("content-type");
    assert(message->hasHeader("Content-Type"));
    {
        ValueIterator values = message->getHeader("Content-Type");
        assert(values.next());
    }

    // When we remove the case-SENSITIVE "Content-Type" header.
    message->removeHeader("Content-Type");

    // Then there should be NO results.
    assert(message->hasHeader("Content-Type") == false);
    {
        ValueIterator values = message->getHeader("Content-Type");
        assert(!values.next());
    }

    // Teardown.
    delete message;
}

void testGetBody() {
    // Setup.
    Message* message = new Message();
    Stream* stream = new Stream();

    // Given we have a default body stream.
    assert(message->getBody());

    // And we set the body to a new stream.
    message->setBody(stream);

    // When we get the body stream.
    Stream* body = message->getBody();

    // Then it should be equal to the new stream.
    assert(body == stream);

    // TODO: Test if the body is invalid.

    // Teardown.
    delete message;
}

} // namespace

void MessageTest() {
    testProtocolVersion();
    testGetHeaders();
    testHasHeader();
    testSetGetHeader();
    testSetAddedGetHeaderLine();
    testRemoveHeader();
    testGetBody();
    printf("MessageTest passed!\n");
}

}}} // Csr::Http::Message
