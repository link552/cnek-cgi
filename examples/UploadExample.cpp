#include "Cnek.hpp"

#include <string.h>

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
using Csr::Http::Message::UploadedFile;

int main() {
    // Setup.
    Cnek::Cnek cnek;
    ServerRequest* serverRequest = cnek.getServerRequest(environ, stdin);
    Response* response = new Response();
    Stream* body = response->getBody();

    response->setHeader("Content-Type", "text/html");

    // Handle request.
    if (!strcmp(serverRequest->getMethod(), "GET")) {
        body->write("<form method=\"POST\" enctype=\"multipart/form-data\"><input type=\"text\" name=\"field\"/><br/><input type=\"file\" name=\"fileONE\"><br/><input type=\"file\" name=\"fileTWO\"><br/><input type=\"file\" name=\"fileTHREE\"><br/><input type=\"submit\" value=\"Upload\"></form>");
    } else {
        const char* field = serverRequest->getBodyParam("field");
        if (*field) {
            body->write("Field: ");
            body->write(field);
            body->write("<br/><br/>");
        }

        UploadedFile* uploadedFile1 = serverRequest->getUploadedFile("fileONE");
        if (uploadedFile1) {
            uploadedFile1->moveTo("uploads/file1.txt");
            body->write("fileONE upload successful!<br/>");
            body->write("name=");
            body->write(uploadedFile1->getClientFileName());
            body->write("</br>type=");
            body->write(uploadedFile1->getClientMediaType());
            body->write("<br/><br/>");
        }

        UploadedFile* uploadedFile2 = serverRequest->getUploadedFile("fileTWO");
        if (uploadedFile2) {
            uploadedFile2->moveTo("uploads/file2.txt");
            body->write("fileTWO upload successful!<br/>");
            body->write("name=");
            body->write(uploadedFile2->getClientFileName());
            body->write("</br>type=");
            body->write(uploadedFile2->getClientMediaType());
            body->write("<br/><br/>");
        }

        UploadedFile* uploadedFile3 = serverRequest->getUploadedFile("fileTHREE");
        if (uploadedFile3) {
            uploadedFile3->moveTo("uploads/file3.txt");
            body->write("fileTHREE upload successful!<br/>");
            body->write("name=");
            body->write(uploadedFile3->getClientFileName());
            body->write("</br>type=");
            body->write(uploadedFile3->getClientMediaType());
        }
    }

    // Emit response.
    cnek.emitResponse(response, stdout);
}
