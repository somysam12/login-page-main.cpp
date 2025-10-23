# BR MODS EXTERNAL - Login System

## Overview

BR MODS EXTERNAL is a secure C++ desktop application that provides license key validation and authentication for software distribution. The application features a modern OpenGL3/ImGui interface with teal/cyan theming and implements multiple security layers including hardware ID binding, encrypted key transmission, and session management. It communicates with a backend API over HTTPS to validate license keys and prevent unauthorized access and key sharing.

## User Preferences

Preferred communication style: Simple, everyday language.

## System Architecture

### Frontend Architecture

**UI Framework**: ImGui with OpenGL3 backend
- **Rationale**: ImGui provides an immediate-mode GUI that's lightweight, cross-platform, and easy to integrate with OpenGL. Perfect for desktop applications requiring a modern, responsive interface without the overhead of traditional UI frameworks.
- **Rendering**: OpenGL3 handles graphics rendering, providing cross-platform compatibility across Windows, macOS, and Linux.
- **Window Management**: GLFW3 manages window creation, input handling, and event processing.

**Design Pattern**: Single-executable desktop application
- The application follows a straightforward monolithic architecture suitable for client-side authentication tools.
- Main application loop handles rendering, user input, and authentication state management.

### Backend Architecture

**API Communication Pattern**: RESTful HTTP/HTTPS
- **Protocol**: HTTPS with SSL/TLS certificate verification
- **Client**: libcurl handles all HTTP requests with OpenSSL for encryption
- **Rationale**: REST APIs provide a simple, stateless communication pattern. HTTPS ensures encrypted data transmission, protecting license keys and session tokens in transit.

**Authentication Flow**:
1. User enters license key in UI
2. Client generates hardware ID fingerprint
3. License key is hashed using SHA-256 (via OpenSSL)
4. Encrypted credentials sent to backend API over HTTPS
5. Backend validates key, checks hardware binding, and creates session
6. Session token returned and stored client-side

**Security Components**:
- **Key Encryption**: SHA-256 hashing of license keys before transmission (prevents plaintext exposure)
- **Hardware ID Binding**: Machine fingerprinting using system-specific identifiers (Windows: computer name + CPU ID; Linux/Mac: hostname + username)
- **Session Management**: Token-based sessions prevent duplicate logins and session hijacking
- **SSL Verification**: Certificate validation prevents man-in-the-middle attacks
- **Optional Integrity Checking**: Compile-time flag (`ENABLE_INTEGRITY_CHECK`) for binary verification to detect tampering

### Data Storage

**Client-Side**: No persistent storage implementation shown in current codebase
- Authentication state maintained in memory during runtime
- Session tokens handled ephemerally

**Server-Side** (Reference Implementation):
- **Database**: SQL-based (MySQL/PostgreSQL recommended based on example schema)
- **Users Table**: Stores username, hashed license key, hardware ID, activation status, and expiration dates
- **Sessions Table**: Tracks active sessions with tokens, hardware IDs, activity timestamps, and expiration

**Database Schema Design Rationale**:
- Separation of users and sessions allows for multi-device management and session auditing
- Hardware ID binding at database level enforces anti-sharing policies
- Timestamp tracking enables expiration policies and activity monitoring

### Build System

**CMake Configuration**:
- **C++17 Standard**: Modern C++ features for improved code safety and readability
- **Cross-Platform Support**: CMake abstracts platform-specific build configurations
- **Dependency Management**: Uses `find_package()` for system libraries (OpenGL, GLFW3, libcurl, OpenSSL)
- **Conditional Compilation**: `ENABLE_INTEGRITY_CHECK` option allows toggling security features at build time

**Rationale**: CMake provides robust cross-platform build automation, making it easier to maintain builds across Windows, macOS, and Linux without separate build scripts.

## External Dependencies

### Graphics and UI
- **OpenGL**: Cross-platform graphics API for rendering the user interface
- **GLFW3**: Window management, input handling, and OpenGL context creation
- **ImGui** (bundled): Immediate-mode GUI library for creating the login interface

### Networking and Security
- **libcurl**: HTTP/HTTPS client library for API communication
- **OpenSSL**: Provides SSL/TLS encryption and SHA-256 hashing capabilities
  - Used for: Encrypted HTTPS connections, license key hashing, certificate verification

### Backend API Requirements
The application expects a backend REST API with the following capabilities:
- **Authentication Endpoint**: Validates license keys and hardware IDs
- **Session Management**: Creates and validates session tokens
- **Database**: SQL database (MySQL/PostgreSQL) for user and session storage
- **HTTPS Support**: SSL/TLS certificates for secure communication

**Note**: Backend implementation is external to this C++ client. Reference implementation suggests Node.js/Express with SQL database, but any backend technology stack supporting REST APIs and HTTPS is compatible.

### Platform-Specific Dependencies
- **Windows**: Windows API for hardware fingerprinting (computer name, CPU ID)
- **Linux/macOS**: POSIX APIs for system information (hostname, username)