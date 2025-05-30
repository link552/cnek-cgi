namespace Csr {
namespace Http {
namespace Message {

void StreamTest();
void MessageTest();
void ResponseTest();
void UriTest();
void RequestTest();
void UploadedFileTest();
void ServerRequestTest();

}}} // Csr::Http::Message

namespace Cnek {

void CnekTest();

} // Cnek

using Csr::Http::Message::StreamTest;
using Csr::Http::Message::MessageTest;
using Csr::Http::Message::ResponseTest;
using Csr::Http::Message::UriTest;
using Csr::Http::Message::RequestTest;
using Csr::Http::Message::UploadedFileTest;
using Csr::Http::Message::ServerRequestTest;
using Cnek::CnekTest;

int main() {
    StreamTest();
    MessageTest();
    ResponseTest();
    UriTest();
    RequestTest();
    UploadedFileTest();
    ServerRequestTest();
    CnekTest();
}
