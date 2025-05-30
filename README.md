<h1 align="center">Cnek-CGI</h1>

<div align="center"><img src="https://github.com/user-attachments/assets/a60745bb-90e4-41f9-8494-9ce813162b44" alt="Cnek"/></div>

> **Cnek-CGI** is a C++98 implementation of the [CSR (C++ Standard Recommendations)](https://github.com/CPP-SRG), providing a standard interface for developing CGI (Common Gateway Interface) web applications. Cnek-CGI aims to make server-side C++ web development more consistent and interoperable by adhering to the CSR specification.

---

## About

Cnek-CGI implements the CSR interface specifically for CGI environments, enabling developers to build portable, standards-compliant web applications in C++98. By using Cnek-CGI, projects can benefit from a well-defined interface while targeting traditional CGI deployment scenarios.

---

## Features

* Implements the CSR specification for CGI.
* Written in portable C++98.
* Designed for extensibility and integration with other CSR-based projects.
* Promotes code reuse and standardization in server-side C++ web development.

### Interfaces Implemented

* [HTTP Message CSR v1.0.0](https://github.com/CPP-SRG/http-message/releases/tag/v1.0.0)

---

## Getting Started

### Prerequisites

* C++98 compatible compiler.
* Familiarity with CGI and server-side C++ development.

### Installation

1. Clone this repository.
2. Include the Cnek-CGI header files in `include/` to your project.
3. Add the source files in `src/` to your compilation step.

### Settings

Override the following settings with `#define`.

```cpp
// Size of buffer to parse upload file body lines.
#define UPLOAD_LINE_SIZE 4096 // Default 4KB.

// Max number of bytes to allocate for uploaded files.
#define MAX_UPLOAD_FILE_SIZE 4194304 // Default 4MB.

// Maximum number of headers to read from the request.
#define MAX_HEADER_COUNT 32 // Default 32B.

// Maximum number of bytes per header.
#define MAX_HEADER_LENGTH 1024 // Default 1KB.

// Size of buffer used in read().
#define STREAM_BUFFER_SIZE 4096 // Default 4KB.

// Max number of bytes to allocate for read(), toString(), and getContents().
#define MAX_STREAM_READ_SIZE 4194304 // Default 4MB.

// Size of buffer used in read().
#define STREAM_BUFFER_SIZE 4096 // Default 4KB.

// Max number of bytes to allocate for read(), toString(), and getContents().
#define MAX_STREAM_READ_SIZE 4194304 // Default 4MB.

// Size of buffer used to read the request body.
#define REQUEST_BODY_BUFFER_SIZE 4096 // Default 4KB.

// Max number of bytes to read from the request body.
#define MAX_REQUEST_BODY_SIZE 4194304 // Default 4MB.
```

---

## Usage

Include the relevant Cnek-CGI headers in your application:

```cpp
#include "Cnek.hpp"
// ...other headers as needed
```

Follow the CSR documentation for interface details and refer to the `examples/` directory for usage patterns specific to CGI.

### Compiling Examples

```cmd
g++ -std=c++98 -o index.cgi ^
-I include/Csr/Http/Message ^
-I include/Cnek ^
src/Csr/Http/Message/*.cpp ^
src/Cnek/*.cpp ^
examples/HelloExample.cpp
```

---

## Status \& Security

#### IMPORTANT!

Cnek-CGI is in an early _ALPHA_ stage and is still under active development.  
**It has not yet been thoroughly tested and hardened for security vulnerabilities.**  
Before using in a public-facing or production environment, please wait for version `1.0.0` release.  

---

## Contributing

Contributions are welcome! To contribute:

1. Fork this repository.
2. Create a feature or bug branch.
3. Add a failing test for your feature or bug.
3. Write your changes to the source.
4. Run the tests and make sure they pass.
5. Submit a pull request with a clear description.

Please follow the current coding styles when making your changes.

### Compiling Tests

```cmd
g++ -std=c++98 -o test.exe ^
-I include/Csr/Http/Message ^
-I include/Cnek ^
src/Csr/Http/Message/*.cpp ^
tests/Csr/Http/Message/*.cpp ^
src/Cnek/*.cpp ^
tests/Cnek/*.cpp ^
tests/Test.cpp
```

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contact

For questions, suggestions, or support, please open an issue or contact the maintainers.

---

> Cnek-CGI is part of an effort to standardize and modernize C++ server-side web development. Thank you for trying it out and helping it grow!
