# BR MODS EXTERNAL - Login System

A secure C++ login system with OpenGL3/ImGui interface for validating license keys against your website.

## Features

- **Modern UI**: Clean, themed interface matching BR MODS design (teal/cyan theme)
- **Secure Authentication**: HTTPS-based key validation with your backend
- **Hardware ID Binding**: Prevents key sharing across different machines
- **Session Management**: Detects and prevents duplicate logins ("User already in use")
- **Anti-Bypass Protection**: 
  - Integrity checks
  - Encrypted key transmission
  - Hardware fingerprinting
  - SSL certificate validation

## Dependencies

### Required Libraries

1. **ImGui** (included in project)
   - Download from: https://github.com/ocornut/imgui
   - Place in `imgui/` folder

2. **GLFW3**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libglfw3-dev
   
   # macOS
   brew install glfw
   
   # Windows
   vcpkg install glfw3
   ```

3. **OpenGL**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libgl1-mesa-dev
   
   # macOS (built-in)
   
   # Windows (included with graphics drivers)
   ```

4. **libcurl**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libcurl4-openssl-dev
   
   # macOS
   brew install curl
   
   # Windows
   vcpkg install curl
   ```

5. **OpenSSL**
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libssl-dev
   
   # macOS
   brew install openssl
   
   # Windows
   vcpkg install openssl
   ```

6. **nlohmann/json**
   ```bash
   # Header-only library
   # Download json.hpp from: https://github.com/nlohmann/json
   # Place in /usr/local/include/ or project include directory
   
   # Or via package manager:
   sudo apt-get install nlohmann-json3-dev  # Ubuntu
   brew install nlohmann-json                # macOS
   vcpkg install nlohmann-json               # Windows
   ```

## Project Structure

```
.
├── main.cpp              # Main application with ImGui UI
├── auth_handler.h/cpp    # Authentication logic
├── http_client.h/cpp     # HTTPS client for API calls
├── config.h              # API configuration
├── CMakeLists.txt        # Build configuration
├── README.md             # This file
└── imgui/                # ImGui library (you need to download)
    ├── imgui.h
    ├── imgui.cpp
    ├── imgui_*.cpp
    └── backends/
        ├── imgui_impl_glfw.h/cpp
        └── imgui_impl_opengl3.h/cpp
```

## Setup Instructions

### 1. Download ImGui

```bash
git clone https://github.com/ocornut/imgui.git
```

### 2. Configure Your API

Edit `config.h` and update the API endpoint:

```cpp
const std::string API_BASE_URL = "https://your-website.com/api";
```

### 3. Build the Project

```bash
mkdir build
cd build
cmake ..
make
```

For Windows with Visual Studio:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 4. Run

```bash
./BR_MODS_EXTERNAL
```

## Backend API Requirements

Your website needs to implement these endpoints:

### 1. POST `/api/validate`
Validate user credentials and create session.

**Request:**
```json
{
  "username": "user123",
  "key": "hashed_license_key",
  "hwid": "hardware_id_hash",
  "app_version": "1.0.0"
}
```

**Response (Success):**
```json
{
  "success": true,
  "session_token": "unique_session_token",
  "expires_at": 1234567890,
  "message": "Login successful"
}
```

**Response (Failure):**
```json
{
  "success": false,
  "message": "Invalid key",
  "error": "Key not found or expired"
}
```

**Response (Already in use):**
```json
{
  "success": false,
  "message": "User already in use!"
}
```

### 2. POST `/api/check-session`
Verify active session.

**Request:**
```json
{
  "session_token": "token",
  "username": "user123"
}
```

**Response:**
```json
{
  "valid": true
}
```

### 3. POST `/api/logout`
End session.

**Request:**
```json
{
  "session_token": "token",
  "username": "user123"
}
```

## Security Features

1. **Key Encryption**: License keys are hashed using SHA-256 before transmission
2. **Hardware ID**: Unique hardware fingerprint prevents key sharing
3. **SSL/TLS**: All communications use HTTPS with certificate verification
4. **Integrity Checks**: Optional binary integrity verification (see SECURITY.md)
5. **Session Validation**: Server-side session management prevents bypass
6. **Thread Safety**: Atomic variables and mutex locks prevent race conditions

For detailed security configuration, see [SECURITY.md](SECURITY.md)

## Customization

### Change Theme Colors

Edit `main.cpp` in the `Render()` function:

```cpp
// Title color (cyan)
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.9f, 0.9f, 1.0f));

// Input background (teal)
ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.5f, 0.5f, 0.8f));

// Button color (cyan)
ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.9f, 0.9f, 1.0f));
```

### Modify Window Size

Edit constants in `main.cpp`:

```cpp
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 500;
```

## Troubleshooting

### Build Errors

1. **ImGui not found**: Make sure ImGui is in the `imgui/` folder
2. **GLFW not found**: Install GLFW development libraries
3. **OpenSSL errors**: Install OpenSSL development package
4. **JSON errors**: Install nlohmann/json library

### Runtime Errors

1. **"Failed to connect to server"**: Check API_BASE_URL in config.h
2. **SSL errors**: Ensure your server has valid SSL certificate
3. **"Integrity check failed"**: Normal warning, doesn't affect functionality

## License

This code is provided for use in your BR MODS project. Modify as needed.

## Support

For issues with your backend API integration, ensure:
- CORS is properly configured
- Endpoints return valid JSON
- SSL certificates are valid
- Server accepts POST requests with JSON body
