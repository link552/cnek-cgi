#include "ServerRequest.hpp"
#include "UploadedFile.hpp"
#include "Shared.hpp"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdexcept>

// Size of buffer to parse upload file body lines.
// NOTE: Prevents DoS attacks.
// NOTE: If the upload line size is less than a critical metadata line,
// such as the Content-Disposition line, then it will be unable to extract
// the metadata and the file or field will not be available.
#ifndef UPLOAD_LINE_SIZE
#define UPLOAD_LINE_SIZE 4096 // Default 4KB.
#endif // UPLOAD_LINE_SIZE

// Max number of bytes to allocate for uploaded files.
// NOTE: Prevents DoS attacks.
#ifndef MAX_UPLOAD_FILE_SIZE
#define MAX_UPLOAD_FILE_SIZE 4194304 // Default 4MB.
#endif // MAX_UPLOAD_FILE_SIZE

// Maximum number of headers to read from the request.
// NOTE: Prevents DoS attacks.
#ifndef MAX_HEADER_COUNT
#define MAX_HEADER_COUNT 32 // Default 32B.
#endif // MAX_HEADER_COUNT

// Maximum number of bytes per header.
// NOTE: Prevents DoS attacks.
#ifndef MAX_HEADER_LENGTH
#define MAX_HEADER_LENGTH 1024 // Default 1KB.
#endif // MAX_HEADER_LENGTH

namespace Csr {
namespace Http {
namespace Message {

using std::runtime_error;

namespace {

/**
 * Decode a URL-encoded string.
 *
 * This function MUST return a null-terminated string if it cannot decode
 * the given string.
 *
 * @param str String to decode.
 */
inline char* urldecode(const char* str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    char* dst = new char[len + 1];
    char* start = dst;

    unsigned short i = 0;
    while (*str && i <= len) {
        if (*str == '+') {
            *dst++ = ' ';
            str++;
        } else if (*str == '%' && str[1] && str[2]) {
            char hex[3] = {str[1], str[2], '\0'};
            *dst++ = (char)strtol(hex, NULL, 16);
            str += 3;
        } else *dst++ = *str++;

        i++;
    }

    *dst = '\0';

    return start;
}

/**
 * Read a line from a file.
 *
 * This function replaces new line characters with null-terminators.
 *
 * @param file The file to read from.
 * @param buffer Buffer to write the line to.
 * @param size Max number of bytes to read.
 */
inline int readline(FILE* file, char* buffer, size_t size) {
    if (!fgets(buffer, size, file)) return 0;
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
    return 1;
}

} // namespace

/*******************************************************************************
 * ServerParamNode
 ******************************************************************************/
ServerParamNode::ServerParamNode(const char* name, const char* value) {
    this->name = copystr(name);
    this->value = copystr(name);
    this->next = NULL;
}

ServerParamNode::~ServerParamNode() {
    delete[] this->name;
    delete[] this->value;
}

/*******************************************************************************
 * ServerParamList
 ******************************************************************************/
ServerParamList::ServerParamList() : head(NULL), tail(NULL) {}

void ServerParamList::addServerParam(const char* name, const char* value) {
    if (!name || !value) return;

    ServerParamNode* node = new ServerParamNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

const char* ServerParamList::getServerParam(const char* name) {
    ServerParamNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return "";
}

ServerParamList::~ServerParamList() {
    // Start at head.
    ServerParamNode * node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        ServerParamNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * CookieNode
 ******************************************************************************/
CookieNode::CookieNode(const char* name, const char* value) {
    this->name = copystr(name);
    this->value = copystr(value);
    this->next = NULL;
}

CookieNode::~CookieNode() {
    delete[] this->name;
    delete[] this->value;
}

/*******************************************************************************
 * CookieList
 ******************************************************************************/
CookieList::CookieList() : head(NULL), tail(NULL) {}

void CookieList::addCookie(const char* name, const char* value) {
    if (!name || !value) return;

    CookieNode* node = new CookieNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

const char* CookieList::getCookie(const char* name) {
    CookieNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return "";
}

CookieList::~CookieList() {
    // Start at head.
    CookieNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        CookieNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * QueryParamNode
 ******************************************************************************/
QueryParamNode::QueryParamNode(const char* name, const char* value) {
    this->name = urldecode(name);
    this->value = urldecode(value);
    this->next = NULL;
}

QueryParamNode::~QueryParamNode() {
    delete[] this->name;
    delete[] this->value;
}

/*******************************************************************************
 * QueryParamList
 ******************************************************************************/
QueryParamList::QueryParamList() : head(NULL), tail(NULL) {}

void QueryParamList::addQueryParam(const char* name, const char* value) {
    if (!name || !value) return;

    QueryParamNode* node = new QueryParamNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

const char* QueryParamList::getQueryParam(const char* name) {
    QueryParamNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return "";
}

QueryParamList::~QueryParamList() {
    // Start at head.
    QueryParamNode * node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        QueryParamNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * UploadedFileNode
 ******************************************************************************/
UploadedFileNode::UploadedFileNode(const char* name, UploadedFile* value) {
    this->name = copystr(name);
    this->value = value;
    this->next = NULL;
}

UploadedFileNode::~UploadedFileNode() {
    delete[] this->name;
    delete this->value;
}

/*******************************************************************************
 * UploadedFileList
 ******************************************************************************/
UploadedFileList::UploadedFileList() : head(NULL), tail(NULL) {}

void UploadedFileList::addUploadedFile(const char* name, UploadedFile* value) {
    if (!name || !value) return;

    UploadedFileNode* node = new UploadedFileNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

UploadedFile* UploadedFileList::getUploadedFile(const char* name) {
    UploadedFileNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return NULL;
}

UploadedFileList::~UploadedFileList() {
    // Start at head.
    UploadedFileNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        UploadedFileNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * BodyParamNode
 ******************************************************************************/
BodyParamNode::BodyParamNode(const char* name, const char* value) {
    this->name = urldecode(name);
    this->value = urldecode(value);
    this->next = NULL;
}

BodyParamNode::~BodyParamNode() {
    delete[] this->name;
    delete[] this->value;
}

/*******************************************************************************
 * BodyParamList
 ******************************************************************************/
BodyParamList::BodyParamList() : head(NULL), tail(NULL) {}

void BodyParamList::addBodyParam(const char* name, const char* value) {
    if (!name || !value) return;

    BodyParamNode* node = new BodyParamNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

const char* BodyParamList::getBodyParam(const char* name) {
    BodyParamNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return "";
}

BodyParamList::~BodyParamList() {
    // Start at head.
    BodyParamNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        BodyParamNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * AttributeNode
 ******************************************************************************/
AttributeNode::AttributeNode(const char* name, const char* value) {
    this->name = copystr(name);
    this->value = copystr(value);
    this->next = NULL;
}

AttributeNode::~AttributeNode() {
    delete[] this->name;
    delete[] this->value;
}

/*******************************************************************************
 * AttributeList
 ******************************************************************************/
AttributeList::AttributeList() : head(NULL), tail(NULL) {}

void AttributeList::addAttribute(const char* name, const char* value) {
    if (!name || !value) return;

    AttributeNode* node = new AttributeNode(name, value);

    // Handle first body param in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    // Append to tail.
    this->tail->next = node;

    // Set next tail.
    this->tail = node;
}

const char* AttributeList::getAttribute(const char* name) {
    AttributeNode* curr = this->head;

    while (curr) {
        if (!strcmp(name, curr->name)) {
            return curr->value;
        }

        curr = curr->next;
    }

    return "";
}

void AttributeList::removeAttribute(const char* name) {
    AttributeNode* prev = NULL;
    AttributeNode* curr = this->head;

    while (curr) {
        // If names are equal, delete the node.
        if (!strcmp(name, curr->name)) {
            // If it's the only node in the list, clear it.
            if (curr == this->head && curr == this->tail) {
                this->head = NULL;
                this->tail = NULL;
            }
            // If it's the head, replace it.
            else if (curr == this->head) {
                this->head = curr->next;
            }
            // If it's the tail, replace it and reset prev's next.
            else if (curr == this->tail) {
                this->tail = prev;
            }

            // If there is a previous node, reset it's next.
            if (prev) {
                prev->next = curr->next;
            }

            delete curr;
        }

        prev = curr;
        curr = curr->next;
    }
}

AttributeList::~AttributeList() {
    // Start at head.
    AttributeNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        AttributeNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * ServerRequest
 ******************************************************************************/
void ServerRequest::parseBody() {
    // Only run this routine once.
    if (this->isBodyParsed) return;
    this->isBodyParsed = true;

    // Parse body parameters and uploaded files.
    // Only parse the body if the HTTP method is POST and the Content-Type is
    // either application/x-www-form-urlencoded or multipart/form-data.
    const char* method = this->getMethod();
    const char* contentType = this->getServerParam("CONTENT_TYPE");
    const char* boundary = strstr(contentType, "boundary=");
    if (!strcmp(method, "POST")
        && strstr(contentType, "application/x-www-form-urlencoded"))
    {
        const char* content = this->getBody()->toString();
        char* copy = copystr(content);

        char* token = strtok(copy, "&");
        while (token && copy) {
            char* eq = strchr(token, '=');
            if (eq) {
                // Split name and value.
                *eq = '\0';

                this->bodyParams->addBodyParam(token, eq + 1);
            }

            token = strtok(NULL, "&");
        }

        delete[] copy;
    }
    // Only parse uploaded files if the HTTP method is POST and the
    // Content-Type is multipart/form-data.
    else if (!strcmp(method, "POST")
             && strstr(contentType, "multipart/form-data")
             && boundary)
    {
        boundary += 9; // Skip "boundary=".

        char line[UPLOAD_LINE_SIZE];
        char name[256];
        char type[256];
        char filename[256];
        char* content = NULL;
        size_t contentSize = 0;
        size_t contentCap = 0;
        bool inFile = false;
        bool inField = false;
        FILE* fp = tmpfile();
        fputs(this->getBody()->toString(), fp);
        fseek(fp, 0, SEEK_SET);

        while (readline(fp, line, sizeof(line))) {
            if (!strncmp(line, "--", 2) && strstr(line, boundary)) {
                // End of previous part.
                if (inFile && *filename) {
                    // Save file.
                    Stream* stream = new Stream();
                    stream->write(content);

                    // TODO: Upload error should be variable based on the results of parsing.
                    UploadedFile* uploadedFile = new UploadedFile(stream, contentSize, UPLOAD_ERR_OK, filename, type);
                    this->uploadedFiles->addUploadedFile(name, uploadedFile);
                } else if (inField && *name) {
                    // Save field.
                    this->bodyParams->addBodyParam(name, content);
                }

                // Reset state.
                name[0] = 0;
                type[0] = 0;
                filename[0] = 0;
                free(content);
                content = NULL;
                contentSize = 0;
                contentCap = 0;
                inField = false;
                inFile = false;
                continue;
            }

            if (strncasecmp(line, "Content-Disposition:", 20)) {
                char* p = strstr(line, "name=\"");
                if (p) {
                    sscanf(p + 6, "%255[^\"]", name);
                    inField = true;
                }

                p = strstr(line, "filename=\"");
                if (p) {
                    sscanf(p + 10, "%255[^\"]", filename);
                    inField = false;
                    inFile = true;
                }
            } else if (strncasecmp(line, "Content-Type: ", 14)) {
                strncpy(type, line + 14, sizeof(type));
            } else if (!*line) {
                // Start reading content.
                contentSize = 0;
                contentCap = sizeof(line);

                content = (char*)malloc(contentCap);
                if (!content) {
                    throw new runtime_error(
                        "Failed to allocate upload content buffer.");
                }

                while (fgets(line, sizeof(line), fp)) {
                    if (!strncmp(line, "--", 2) && strstr(line, boundary)) {
                        // Rewind so next loop sees the boundary.
                        fseek(fp, -strlen(line), SEEK_CUR);
                        break;
                    }

                    size_t len = strlen(line);

                    // If we're at the end of the buffer and before the upload
                    // file max size, then increase the buffer size.
                    if (contentSize + len >= contentCap
                        && contentSize < MAX_UPLOAD_FILE_SIZE)
                    {
                        if (contentCap + sizeof(line) > MAX_UPLOAD_FILE_SIZE) {
                            // Remaining bytes.
                            contentCap = MAX_UPLOAD_FILE_SIZE - contentCap;
                        } else contentCap += sizeof(line);

                        content = (char*)realloc(content, contentCap);
                        if (!content) {
                            throw new runtime_error(
                                "Failed to reallocate upload content buffer.");
                        }
                    }

                    // Limit content size to max upload file size for added
                    // security and copy remaining memory.
                    if (contentSize + len >= MAX_UPLOAD_FILE_SIZE) {
                        size_t remainder = MAX_UPLOAD_FILE_SIZE - contentSize;
                        memcpy(content + contentSize, line, remainder);
                        contentSize += remainder;
                        break;
                    }

                    memcpy(content + contentSize, line, len);
                    contentSize += len;
                }

                // Ignore the last '\n' new line character because it's actually
                // a separator, not part of the content.
                if (content[contentSize - 1] == '\n') {
                    content[contentSize - 1] = '\0';
                } else content[contentSize] = '\0';
            }
        }

        fclose(fp);
        if (content) free(content);
    }
}

ServerRequest::ServerRequest(
    const char* method,
    const char* uri,
    char** serverParams) : Request(method, uri)
{
    // NOTE: Even though `serverParams` is not const, typically what will be
    // passed is `environ` which MAY be read-only, so be sure not to change
    // any data in it.
    this->environment = serverParams;

    this->serverParams = new ServerParamList();
    this->cookies = new CookieList();
    this->queryParams = new QueryParamList();
    this->uploadedFiles = new UploadedFileList();
    this->bodyParams = new BodyParamList();
    this->attributes = new AttributeList();

    this->isBodyParsed = false;

    // Parse headers.
    //
    // TODO: Headers should be sanitized for hidden '\0' null-terminators,
    // as well as '\r' and '\n' to prevent CRLF injection attacks.
    //
    // NOTE: Some headers are set in `Request` like `Host`. It might not be
    // a bad idea to add sanitization to `Message` instead of `ServerRequest`
    // to make sure all angles are covered for these vulnerabilities.
    unsigned short count = 0;
    for (char** env = serverParams; *env; env++) {
        // HTTP headers will start with "HTTP_" prefix.
        if (!strncmp(*env, "HTTP_", 5)) {
            // Each header is in the form NAME=VALUE.
            char* eq = strchr(*env, '=');
            if (eq) {
                size_t nameLen = eq - *env;
                char* name = copynstr(*env, nameLen);

                // Normalize header names.
                char* modName = name + 5;
                bool first = true;
                for (char* i = modName; *i; i++) {
                    // Replace '_' underscores with '-' hyphens.
                    if (*i == '_') {
                        *i = '-';
                        first = true;
                    }
                    // Lowercase secondary word characters.
                    else if (!first) *i = tolower(*i);
                    else first = false;
                }

                // Limit header length to maximum.
                if (eq + 1 && strlen(eq + 1) < MAX_HEADER_LENGTH) {
                    this->setHeader(modName, eq + 1);
                } else if (eq + 1) {
                    char* value = new char[MAX_HEADER_LENGTH];
                    memcpy(eq + 1, value, MAX_HEADER_LENGTH);
                    value[MAX_HEADER_LENGTH - 1] = '\0';
                    this->setHeader(modName, value);
                    delete[] value;
                }

                delete[] name;
            }

            if (count++ == MAX_HEADER_COUNT) break;
        }
    }

    // Get protocol version.
    const char* protocol = this->getServerParam("SERVER_PROTOCOL");
    const char* versionStart = strchr(protocol, '/');
    if (versionStart && *++versionStart) this->setProtocolVersion(versionStart);

    // Parse cookies.
    const char* cookie = this->getHeaderLine("Cookie");
    if (*cookie) {
        char* copy = copystr(cookie);

        char* token = strtok(copy, ";");
        while (token) {
            // Cookies typically have a space after ';', so skip them.
            if (*token == ' ') {
                token++;
                continue;
            }

            char* eq = strchr(token, '=');
            if (eq) {
                // Split name and value.
                *eq = '\0';

                this->cookies->addCookie(token, eq + 1);
            }

            token = strtok(NULL, ";");
        }

        delete[] copy;
    }

    // Parse query string arguments.
    const char* query = this->getServerParam("QUERY_STRING");
    if (*query) {
        char* copy = copystr(query);

        char* token = strtok(copy, "&");
        while (token) {
            char* eq = strchr(token, '=');
            if (eq) {
                // Split name and value.
                *eq = '\0';

                this->queryParams->addQueryParam(token, eq + 1);
            }

            token = strtok(NULL, "&");
        }

        delete[] copy;
    }

    return;
}

const char* ServerRequest::getServerParam(const char* name) {
    // TODO: Like with headers, server params, which are typically environment
    // variables, should be sanitized for CRLF injection and DoS attacks. If
    // a malicious user is able to create environment variables, this method
    // could be vulnerable.
    for (char** env = this->environment; *env; env++) {
        // Each environment variable is in the form NAME=VALUE.
        char* eq = strchr(*env, '=');
        if (eq && !strncmp(*env, name, eq - *env)) {
            return eq + 1;
        }
    }

    return "";
}

const char* ServerRequest::getCookieParam(const char* name) {
    return this->cookies->getCookie(name);
}

const char* ServerRequest::getQueryParam(const char* name) {
    return this->queryParams->getQueryParam(name);
}

UploadedFile* ServerRequest::getUploadedFile(const char* name) {
    this->parseBody();
    return this->uploadedFiles->getUploadedFile(name);
}

const char* ServerRequest::getBodyParam(const char* name) {
    this->parseBody();
    return this->bodyParams->getBodyParam(name);
}

const char* ServerRequest::getAttribute(const char* name, const char* def) {
    const char* value = this->attributes->getAttribute(name);
    if (!*value) return def;
    return value;
}

void ServerRequest::setAttribute(const char* name, const char* value) {
    this->attributes->addAttribute(name, value);
}

void ServerRequest::removeAttribute(const char* name) {
    this->attributes->removeAttribute(name);
}

ServerRequest::~ServerRequest() {
    delete this->serverParams;
    delete this->cookies;
    delete this->queryParams;
    delete this->uploadedFiles;
    delete this->bodyParams;
    delete this->attributes;
}

}}} // Csr::Http::Message
