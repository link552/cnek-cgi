#include "Message.hpp"
#include "Shared.hpp"

#include <stdlib.h>
#include <string.h>
#include <stdexcept>

namespace Csr {
namespace Http {
namespace Message {

using std::runtime_error;

/*******************************************************************************
 * ValueNode
 ******************************************************************************/
ValueNode::ValueNode(const char* value) {
    this->value = copystr(value);
    this->next = NULL;
}

ValueNode::~ValueNode() {
    delete[] this->value;
}

/*******************************************************************************
 * ValueList
 ******************************************************************************/
ValueList::ValueList() : head(NULL), tail(NULL), line(NULL) {}

void ValueList::addValue(const char* value) {
    ValueNode* node = new ValueNode(value);

    // Handle first value in list.
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

const char* ValueList::getLine() {
    delete[] this->line;

    // Return null-terminated string if there are no values.
    if (!this->head) {
        this->line = nullstr();
        return this->line;
    }

    // Start at head.
    ValueNode* node = this->head;

    // Get length of line string. It should be the length of the value plus
    // one for each comma separator and the null-terminator.
    size_t length = 0;
    while (node) {
        length += strlen(node->value) + 1;
        node = node->next;
    }

    this->line = new char[length];
    *this->line = '\0';

    // Reset to head.
    node = this->head;

    // Concatenate strings to form line.
    while (node) {
        strcat(this->line, node->value);
        node = node->next;
        if (node) strcat(this->line, ",");
    }

    return this->line;
}

ValueList::~ValueList() {
    // Start at head.
    ValueNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        ValueNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * ValueIterator
 ******************************************************************************/
ValueIterator::ValueIterator(ValueNode* first) {
    this->first = first;
    this->cursor = NULL;
}

bool ValueIterator::next() {
    // If uninitialized, return false.
    if (!this->first && !this->cursor) return false;

    // If first is set, but cursor is null, then we are at the beginning,
    // so set the cursor to the first node.
    if (this->first && !this->cursor) {
        this->cursor = this->first;
        return true;
    }

    // If the cursor's next node is null, then we are at the end.
    if (!this->cursor->next) {
        return false;
    }

    this->cursor = this->cursor->next;
    return true;
}

void ValueIterator::reset() {
    this->cursor = NULL;
}

const char* ValueIterator::getValue() {
    if (!this->cursor) {
        throw std::runtime_error("Tried to call getValue() before calling next().");
    }

    return this->cursor->value;
}

/*******************************************************************************
 * HeaderNode
 ******************************************************************************/
HeaderNode::HeaderNode(const char* name, const char* value) {
    this->name = copystr(name);
    this->values = new ValueList();
    this->values->addValue(value);
    this->next = NULL;
}

HeaderNode::~HeaderNode() {
    delete[] this->name;
    delete this->values;
}

/*******************************************************************************
 * HeaderList
 ******************************************************************************/
HeaderList::HeaderList() : head(NULL), tail(NULL) {}

void HeaderList::addHeader(const char* name, const char* value) {
    HeaderNode* node = new HeaderNode(name, value);

    // Handle first header in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    HeaderNode* prev = NULL;
    HeaderNode* curr = this->head;
    bool exists = false;

    while (curr) {
        // If names are equal, replace the node.
        if (strcasecmp(name, curr->name)) {
            // If it's the only node in the list, replace it.
            if (curr == this->head && curr == this->tail) {
                this->head = node;
                this->tail = node;
            }
            // If it's the head, replace it.
            else if (curr == this->head) {
                node->next = curr->next;
                this->head = node;
            }
            // If it's the tail, replace it and reset prev's next.
            else if (curr == this->tail) {
                prev->next = node;
                this->tail = node;
            }

            // If there is a previous node, reset it's next.
            if (prev) {
                prev->next = node;
                node->next = curr->next;
            }

            delete curr;
            exists = true;
        }

        prev = curr;
        curr = curr->next;
    }

    // If the header doesn't exist, insert it.
    if (!exists) {
        // Append to tail.
        this->tail->next = node;

        // Set next tail.
        this->tail = node;
    }
}

void HeaderList::addHeaderValue(const char* name, const char* value) {
    HeaderNode* node = new HeaderNode(name, value);

    // Handle first header in list.
    if (!this->head && !this->tail) {
        this->head = node;
        this->tail = node;
        return;
    }

    HeaderNode* curr = this->head;
    bool exists = false;

    while (curr) {
        if (strcasecmp(name, curr->name)) {
            curr->values->addValue(value);
            exists = true;
        }

        curr = curr->next;
    }

    // If the header doesn't exist, insert it.
    if (!exists) {
        // Append to tail.
        this->tail->next = node;

        // Set next tail.
        this->tail = node;
    }
}

ValueList* HeaderList::getHeader(const char* name) {
    HeaderNode* curr = this->head;

    while (curr) {
        if (strcasecmp(name, curr->name)) {
            return curr->values;
        }

        curr = curr->next;
    }

    return NULL;
}

bool HeaderList::hasHeader(const char* name) {
    HeaderNode* curr = this->head;

    while (curr) {
        if (strcasecmp(name, curr->name)) {
            return true;
        }

        curr = curr->next;
    }

    return false;
}

void HeaderList::removeHeader(const char* name) {
    HeaderNode* prev = NULL;
    HeaderNode* curr = this->head;

    while (curr) {
        // If names are equal, delete the node.
        if (!strcmp((char*)name, curr->name)) {
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

HeaderList::~HeaderList() {
    // Start at head.
    HeaderNode* node = this->head;

    // Delete all nodes in list.
    while (node) {
        // Save next so we can delete the node.
        HeaderNode* next = node->next;

        delete node;

        // Set next node.
        node = next;
    }
}

/*******************************************************************************
 * HeaderIterator
 ******************************************************************************/
HeaderIterator::HeaderIterator(HeaderNode* first) {
    this->first = first;
    this->cursor = NULL;
}

bool HeaderIterator::next() {
    // If uninitialized, return false.
    if (!this->first && !this->cursor) return false;

    // If first is set, but cursor is null, then we are at the beginning,
    // so set the cursor to the first node.
    if (this->first && !this->cursor) {
        this->cursor = this->first;
        return true;
    }

    // If the cursor's next node is null, then we are at the end.
    if (!this->cursor->next) {
        return false;
    }

    this->cursor = this->cursor->next;
    return true;
}

void HeaderIterator::reset() {
    this->cursor = NULL;
}

const char* HeaderIterator::getName() {
    if (!this->cursor) {
        throw runtime_error("Tried to call getName() before calling next().");
    }

    return this->cursor->name;
}

ValueIterator HeaderIterator::getValues() {
    if (!this->cursor) return ValueIterator(NULL);
    return ValueIterator(this->cursor->values->head);
}

/*******************************************************************************
 * Message
 ******************************************************************************/
Message::Message() : version(nullstr()), headers(), body(new Stream()) {}

const char* Message::getProtocolVersion() {
    return this->version;
}

void Message::setProtocolVersion(const char* version) {
    delete[] this->version;
    this->version = copystr(version);
}

HeaderIterator Message::getHeaders() {
    return HeaderIterator(this->headers.head);
}

bool Message::hasHeader(const char* name) {
    return this->headers.hasHeader(name);
}

ValueIterator Message::getHeader(const char* name) {
    ValueList* values = this->headers.getHeader(name);
    if (!values) return ValueIterator(NULL);
    return ValueIterator(values->head);
}

const char* Message::getHeaderLine(const char* name) {
    ValueList* values = this->headers.getHeader(name);
    if (!values) return "";
    return values->getLine();
}

void Message::setHeader(const char* name, const char* value) {
    this->headers.addHeader(name, value);
}

void Message::setAddedHeader(const char* name, const char* value) {
    this->headers.addHeaderValue(name, value);
}

void Message::removeHeader(const char* name) {
    this->headers.removeHeader(name);
}

Stream* Message::getBody() {
    return this->body;
}

void Message::setBody(Stream* body) {
    this->body->close();
    delete this->body;
    this->body = body;
}

Message::~Message() {
    delete[] this->version;
    if (this->body) this->body->close();
    delete this->body;
}

}}} // Csr::Http::Message
