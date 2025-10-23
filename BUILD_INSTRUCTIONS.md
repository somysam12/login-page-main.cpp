# Build Instructions for BR MODS EXTERNAL

## Important Note

**This is a C++ desktop GUI application that must be compiled and run on your LOCAL MACHINE.**

This application uses:
- ImGui (immediate-mode GUI)
- OpenGL3 (graphics rendering)
- GLFW3 (window management)  
- libcurl (HTTPS communication)
- OpenSSL (encryption)

It **cannot** run on cloud environments like Replit - it requires a desktop environment with graphics support.

## Quick Start

### 1. Download ImGui

```bash
# Clone ImGui library (required dependency)
git clone https://github.com/ocornut/imgui.git
```

### 2. Configure Your API Endpoint

Edit `include/config.h` and set your website's API URL:

```cpp
const std::string API_BASE_URL = "https://your-website.com/api";
```

### 3. Build the Application

**Linux/Mac:**
```bash
mkdir build
cd build
cmake ..
make
```

**Windows (Visual Studio):**
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 4. Run

```bash
# Linux/Mac
./BR_MODS_EXTERNAL

# Windows
BR_MODS_EXTERNAL.exe
```

## Project Structure

```
.
├── src/                  # Source files
│   ├── main.cpp
│   ├── auth_handler.cpp
│   └── http_client.cpp
├── include/              # Header files
│   ├── auth_handler.h
│   ├── http_client.h
│   └── config.h
├── docs/                 # Documentation
│   ├── README.md
│   ├── SECURITY.md
│   └── backend_example.md
├── imgui/                # ImGui library (download separately)
├── CMakeLists.txt        # Build configuration
└── .gitignore

```

## Dependencies Installation

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libglfw3-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    nlohmann-json3-dev
```

### macOS  
```bash
brew install cmake glfw curl openssl nlohmann-json
```

### Windows
Use [vcpkg](https://github.com/microsoft/vcpkg):
```bash
vcpkg install glfw3 curl openssl nlohmann-json
```

## Configuration

### API Endpoints
Update `include/config.h`:
```cpp
const std::string API_BASE_URL = "https://your-website.com/api";
```

### Enable Integrity Check (Optional)
```bash
# Build with integrity check
cmake -DENABLE_INTEGRITY_CHECK=ON ..
make

# Generate checksum
sha256sum BR_MODS_EXTERNAL

# Add checksum to include/config.h
const std::string EXPECTED_BINARY_CHECKSUM = "your_sha256_here";

# Rebuild
make
```

## Backend Setup

Your website needs to implement these API endpoints:

1. **POST /api/validate** - Validate license key
2. **POST /api/check-session** - Check active session  
3. **POST /api/logout** - End session

See `docs/backend_example.md` for implementation examples in Node.js, PHP, and Python.

## Documentation

- **README.md** - Complete documentation and features
- **SECURITY.md** - Security configuration and best practices
- **backend_example.md** - Backend API implementation examples

## Troubleshooting

### "ImGui not found"
Download ImGui from https://github.com/ocornut/imgui and place in project root:
```bash
git clone https://github.com/ocornut/imgui.git
```

### "GLFW not found"  
Install GLFW development libraries:
```bash
# Ubuntu/Debian
sudo apt-get install libglfw3-dev

# macOS
brew install glfw

# Windows
vcpkg install glfw3
```

### "Cannot open display"
This error occurs if you try to run on a headless server. This application requires a desktop environment with graphics support.

## For Git Repository

This code is ready to be committed to your Git repository:

```bash
git add .
git commit -m "Add BR MODS EXTERNAL login system"
git push origin main
```

Remember to:
- Update `include/config.h` with your API URL before committing
- Never commit your API keys or secrets
- Add `build/` and `imgui/` to `.gitignore` (already done)

## Support

For questions:
- Check `docs/README.md` for features and setup
- See `docs/SECURITY.md` for security configuration
- Review `docs/backend_example.md` for API implementation

## License

This code is provided for use in your BR MODS project.
