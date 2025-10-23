# Security Features and Implementation Guide

## Overview

This document explains the security features implemented in the BR MODS EXTERNAL login system and how to enable/configure them.

## Features

### 1. Key Encryption (SHA-256)
**Status**: ✅ Enabled by default

License keys are hashed using SHA-256 before being sent to your server. This prevents:
- Key interception in transit
- Plain-text key storage
- Man-in-the-middle attacks (when combined with HTTPS)

**Implementation**: Automatic - no configuration needed.

### 2. Hardware ID Binding
**Status**: ✅ Enabled by default

Each login generates a unique hardware fingerprint based on:
- **Windows**: Computer name + CPU ID
- **Linux/Mac**: Hostname + username

This prevents:
- Key sharing across multiple machines
- Account sharing
- Unauthorized access

**Implementation**: Automatic - no configuration needed.

### 3. SSL/TLS Certificate Verification
**Status**: ✅ Enabled by default

All HTTPS communications verify SSL certificates to prevent:
- Man-in-the-middle attacks
- Connection hijacking
- Data interception

**Implementation**: Automatic when using HTTPS URLs.

### 4. Session Management
**Status**: ✅ Enabled by default

Server-side session tokens prevent:
- Duplicate logins ("User already in use")
- Session hijacking
- Unauthorized access

**Implementation**: Requires backend API support (see backend_example.md).

### 5. Binary Integrity Check
**Status**: ⚠️ Optional (disabled by default)

Verifies the application binary hasn't been modified or tampered with.

**How to Enable:**

1. **Build your application first**:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

2. **Generate the checksum** of your compiled binary:
   ```bash
   # Linux/Mac
   sha256sum BR_MODS_EXTERNAL
   
   # Windows
   certUtil -hashfile BR_MODS_EXTERNAL.exe SHA256
   ```

3. **Update config.h** with the checksum:
   ```cpp
   #ifdef ENABLE_INTEGRITY_CHECK
   const std::string EXPECTED_BINARY_CHECKSUM = "your_actual_sha256_hash_here";
   #endif
   ```

4. **Rebuild with integrity check enabled**:
   ```bash
   cmake -DENABLE_INTEGRITY_CHECK=ON ..
   make
   ```

5. **Update CMakeLists.txt** to support the flag:
   ```cmake
   option(ENABLE_INTEGRITY_CHECK "Enable binary integrity verification" OFF)
   
   if(ENABLE_INTEGRITY_CHECK)
       add_definitions(-DENABLE_INTEGRITY_CHECK)
   endif()
   ```

**Warning**: 
- The checksum changes every time you recompile
- You must regenerate the checksum after each build
- This feature is best used for final release builds only

## Thread Safety

### Login Process
The login UI uses thread-safe mechanisms:
- **Atomic variables** for boolean flags (`loginInProgress`, `isLoggedIn`)
- **Mutex locks** for string messages (`errorMessage`, `statusMessage`)

This prevents:
- Race conditions
- Data corruption
- UI freezing during login

## Best Practices

### For Development
1. **Disable integrity check** during development:
   ```bash
   cmake ..  # Without -DENABLE_INTEGRITY_CHECK
   ```

2. **Use test API endpoints** in `config.h`:
   ```cpp
   const std::string API_BASE_URL = "https://test-api.your-website.com/api";
   ```

### For Production
1. **Enable integrity check**:
   ```bash
   cmake -DENABLE_INTEGRITY_CHECK=ON -DCMAKE_BUILD_TYPE=Release ..
   make
   sha256sum BR_MODS_EXTERNAL > checksum.txt
   ```

2. **Use production API**:
   ```cpp
   const std::string API_BASE_URL = "https://api.your-website.com/api";
   ```

3. **Strip debug symbols**:
   ```bash
   strip BR_MODS_EXTERNAL
   ```

4. **Sign your binary** (Windows):
   ```bash
   signtool sign /f certificate.pfx /p password BR_MODS_EXTERNAL.exe
   ```

## Additional Security Recommendations

### Code Obfuscation
Consider using C++ obfuscators like:
- **Themida** (Windows)
- **VMProtect** (Windows/Mac)
- **Enigma Protector** (Windows)

### Anti-Debugging
Add anti-debugging techniques:
```cpp
#ifdef _WIN32
if (IsDebuggerPresent()) {
    exit(1);
}
#endif
```

### Network Security
1. **Use strong SSL/TLS** (TLS 1.2+)
2. **Implement rate limiting** on your backend
3. **Log all login attempts**
4. **Monitor for suspicious activity**

### Backend Security
See `backend_example.md` for:
- Database schema
- Session management
- Key validation
- "User already in use" detection

## Penetration Testing

Before release, test your application against:
1. **Memory dumping** tools
2. **Network sniffing** (Wireshark)
3. **Debuggers** (x64dbg, GDB)
4. **Reverse engineering** tools (IDA, Ghidra)

## Vulnerability Reporting

If you discover a vulnerability:
1. **DO NOT** disclose publicly
2. Document the issue
3. Test the fix
4. Update all deployed versions

## Updates

Keep your dependencies updated:
```bash
# Check for OpenSSL updates
openssl version

# Update libcurl
curl --version

# Update ImGui
git -C imgui pull
```

## Compliance

Ensure your implementation complies with:
- **GDPR** (if collecting user data)
- **Local privacy laws**
- **License agreements** for all libraries used

## License Key Best Practices

### On Your Backend
1. **Hash keys** before storing in database
2. **Add expiration dates**
3. **Implement key revocation**
4. **Track usage patterns**
5. **Limit activation count**

### Key Generation
Generate secure keys:
```python
import secrets
import hashlib

def generate_license_key():
    raw_key = secrets.token_hex(16)
    hashed = hashlib.sha256(raw_key.encode()).hexdigest()
    return raw_key, hashed  # Store hashed in DB, give raw to user
```

## Incident Response

If keys are compromised:
1. **Revoke affected keys** immediately
2. **Log all sessions** with those keys
3. **Notify users** (if required by law)
4. **Generate new keys**
5. **Investigate** the breach source

## Monitoring

Track on your backend:
- Failed login attempts
- Multiple HWID changes
- Unusual login times
- Geolocation anomalies
- Session duration patterns

## Support

For security questions:
- Review the code in `auth_handler.cpp`
- Check backend examples in `backend_example.md`
- Test with tools like Postman
- Monitor server logs
