#include "Uri.hpp"

#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>

namespace Csr {
namespace Http {
namespace Message {

using std::invalid_argument;

namespace {

void testCreateUri() {
    // Given we a null URI.
    Uri* uri = NULL;
    bool error = false;
    try {
        uri = new Uri(NULL);
    } catch (invalid_argument exception) {
        error = true;
    }

    // Then we see there is an exception.
    assert(error);

    // Teardown.
    delete uri;
}

void testScheme() {
    // Given we have uri "http://host/".
    Uri* uri = new Uri("http://host/");

    // Then the scheme is "http".
    assert(!strcmp(uri->getScheme(), "http"));

    // When we set the scheme to uppercase "HTTPS".
    uri->setScheme("HTTPS");

    // Then we see the scheme is lowercase "https".
    assert(!strcmp(uri->getScheme(), "https"));

    // When we set the scheme to null.
    uri->setScheme(NULL);

    // Then the scheme is a null-terminated string.
    assert(!*uri->getScheme());

    // Teardown.
    delete uri;
}

void testAuthority() {
    // Given we have uri "http://user:pass@host:port/".
    Uri* uri = new Uri("http://user:pass@host:port/");

    // Then the authority is "user:pass@host:port".
    assert(!strcmp(uri->getAuthority(), "user:pass@host:port"));

    // Teardown.
    delete uri;
}

void testUserInfo() {
    // Given we have uri "http://user:pass@host/".
    Uri* uri = new Uri("http://user:pass@host/");

    // Then the user info is "user:pass".
    assert(!strcmp(uri->getUserInfo(), "user:pass"));

    // When we set the user info user "Foo" and password "Bar".
    uri->setUserInfo("Foo", "Bar");

    // Then we see the user info "Foo:Bar".
    assert(!strcmp(uri->getUserInfo(), "Foo:Bar"));

    // When we set the user to null.
    uri->setUserInfo(NULL);

    // Then the user info is a null-terminated string.
    assert(!*uri->getUserInfo());

    // Teardown.
    delete uri;
}

void testHost() {
    // Given we have uri "/path".
    Uri* uri = new Uri("http://host/path");

    // Then the host is "host".
    assert(!strcmp(uri->getHost(), "host"));

    // When we set the host to "DOMAIN".
    uri->setHost("DOMAIN");

    // Then we see the host is "domain".
    assert(!strcmp(uri->getHost(), "domain"));

    // When we set the host to null.
    uri->setHost(NULL);

    // Then the host is a null-terminated string.
    assert(!*uri->getHost());

    // Teardown.
    delete uri;
}

void testPort() {
    // Given we have uri "http://host/".
    Uri* uri = new Uri("http://host:1234/");

    // Then the port is "1234".
    assert(uri->getPort() == 1234);

    // When we set the port to "1212".
    uri->setPort(1212);

    // Then we see the port is "1212".
    assert(uri->getPort() == 1212);

    // Teardown.
    delete uri;

    // Given we have uri "http://host/".
    uri = new Uri("http://host/");

    // Then the port is "0".
    assert(!uri->getPort());

    // Teardown.
    delete uri;
}

void testPath() {
    // Given we have uri "http://host/path1".
    Uri* uri = new Uri("http://host/path1");

    // Then the path is "/path1".
    assert(!strcmp(uri->getPath(), "/path1"));

    // When we set the path to "".
    uri->setPath("");

    // Then the path is "".
    assert(!strcmp(uri->getPath(), ""));

    // When we set the path to "/".
    uri->setPath("/");

    // Then the path is "/".
    assert(!strcmp(uri->getPath(), "/"));

    // When we set the path to "path2".
    uri->setPath("path%202");

    // Then the path is "path2".
    assert(!strcmp(uri->getPath(), "path%202"));

    // Teardown.
    delete uri;

    // Given we have uri "/path3".
    uri = new Uri("/path3");

    // Then the path is "/path3".
    assert(!strcmp(uri->getPath(), "/path3"));

    // Teardown.
    delete uri;
}

void testQuery() {
    // Given we have uri "/path?a=b&c=d%20e".
    Uri* uri = new Uri("/path?a=b&c=d%20e");

    // Then the query is "a=b&c=d".
    assert(!strcmp(uri->getQuery(), "a=b&c=d%20e"));

    // When we set the query to null.
    uri->setQuery(NULL);

    // Then the query is a null-terminated string.
    assert(!*uri->getQuery());

    // Teardown.
    delete uri;
}

void testFragment() {
    // Given we have uri "/path?a=b&c=d".
    Uri* uri = new Uri("/path#frag%20ment");

    // Then the fragment is "fragment".
    assert(!strcmp(uri->getFragment(), "frag%20ment"));

    // When we set the fragment to null.
    uri->setFragment(NULL);

    // Then the fragment is a null-terminated string.
    assert(!*uri->getFragment());

    // Teardown.
    delete uri;
}

void testUriToString() {
    // Given we have uri "".
    Uri* uri = new Uri("");

    // When we set the scheme to "HTTP".
    uri->setScheme("HTTP");

    // And we set the user info to "foo:bar".
    uri->setUserInfo("foo", "bar");

    // And we set the host to "DOMAIN".
    uri->setHost("DOMAIN");

    // And we set the port to "8080".
    uri->setPort(8080);

    // And we set the path to "hello%20world".
    uri->setPath("hello%20world");

    // And we set the query to "a=b&c=d%20e".
    uri->setQuery("a=b&c=d%20e");

    // And we set the fragment to "frag%20ment".
    uri->setFragment("frag%20ment");

    // Then we see the uri string is
    // "http://foo:bar@domain:8080/hello%20world?a=b&c=d%20e#frag%20ment".
    const char* s = "http://foo:bar@domain:8080/hello%20world?a=b&c=d%20e#frag%20ment";
    assert(!strcmp(uri->toString(), s));

    // Teardown.
    delete uri;
}

} // namespace

void UriTest() {
    testCreateUri();
    testScheme();
    testAuthority();
    testUserInfo();
    testHost();
    testPort();
    testPath();
    testQuery();
    testFragment();
    testUriToString();
    printf("UriTest passed!\n");
}

}}} // Csr::Http::Message
