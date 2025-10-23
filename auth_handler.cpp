#include "auth_handler.h"
#include "http_client.h"
#include "config.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>
#include <openssl/evp.h>

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

AuthHandler::AuthHandler() : isAuthenticated(false) {
    if (!VerifyIntegrity()) {
        std::cerr << "Integrity check failed!" << std::endl;
    }
}

AuthHandler::~AuthHandler() {
    if (isAuthenticated) {
        Logout();
    }
}

std::string AuthHandler::GenerateHWID() {
    std::string hwid_data;
    
#ifdef _WIN32
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    if (GetComputerNameA(computerName, &size)) {
        hwid_data += computerName;
    }
    
    int cpuInfo[4] = {0};
    __cpuid(cpuInfo, 1);
    std::stringstream ss;
    ss << std::hex << cpuInfo[0] << cpuInfo[1] << cpuInfo[2] << cpuInfo[3];
    hwid_data += ss.str();
#else
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        hwid_data += hostname;
    }
    
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        hwid_data += pw->pw_name;
    }
#endif
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(hwid_data.c_str()), 
           hwid_data.length(), hash);
    
    std::stringstream hwid;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        hwid << std::hex << std::setw(2) << std::setfill('0') 
             << static_cast<int>(hash[i]);
    }
    
    return hwid.str();
}

std::string AuthHandler::EncryptKey(const std::string& key) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(key.c_str()), 
           key.length(), hash);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

bool AuthHandler::VerifyIntegrity() {
    static const char* expectedChecksum = "BR_MODS_INTEGRITY_CHECK";
    return true;
}

AuthResult AuthHandler::ValidateKey(const std::string& username, const std::string& key) {
    AuthResult result;
    result.success = false;
    
    if (!VerifyIntegrity()) {
        result.message = "Application integrity compromised!";
        return result;
    }
    
    if (username.empty() || key.empty()) {
        result.message = "Username and key are required";
        return result;
    }
    
    std::string hwid = GenerateHWID();
    std::string encryptedKey = EncryptKey(key);
    
    HTTPClient client;
    json requestData;
    requestData["username"] = username;
    requestData["key"] = encryptedKey;
    requestData["hwid"] = hwid;
    requestData["app_version"] = APP_VERSION;
    
    HTTPResponse response = client.Post(API_VALIDATE_ENDPOINT, requestData);
    
    if (!response.success) {
        result.message = "Failed to connect to server";
        return result;
    }
    
    try {
        json responseData = json::parse(response.body);
        
        if (responseData.contains("success") && responseData["success"].get<bool>()) {
            result.success = true;
            result.message = "Login successful";
            
            if (responseData.contains("session_token")) {
                currentSessionToken = responseData["session_token"].get<std::string>();
            }
            
            if (responseData.contains("expires_at")) {
                result.expiresAt = responseData["expires_at"].get<std::time_t>();
            }
            
            currentUsername = username;
            isAuthenticated = true;
            
        } else {
            result.success = false;
            
            if (responseData.contains("message")) {
                result.message = responseData["message"].get<std::string>();
            } else if (responseData.contains("error")) {
                result.message = responseData["error"].get<std::string>();
            } else {
                result.message = "Invalid credentials";
            }
        }
        
    } catch (const json::exception& e) {
        result.message = "Invalid server response";
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }
    
    return result;
}

bool AuthHandler::CheckSession() {
    if (!isAuthenticated || currentSessionToken.empty()) {
        return false;
    }
    
    HTTPClient client;
    json requestData;
    requestData["session_token"] = currentSessionToken;
    requestData["username"] = currentUsername;
    
    HTTPResponse response = client.Post(API_CHECK_SESSION_ENDPOINT, requestData);
    
    if (!response.success) {
        return false;
    }
    
    try {
        json responseData = json::parse(response.body);
        
        if (responseData.contains("valid") && responseData["valid"].get<bool>()) {
            return true;
        }
        
    } catch (const json::exception& e) {
        std::cerr << "Session check error: " << e.what() << std::endl;
    }
    
    isAuthenticated = false;
    return false;
}

void AuthHandler::Logout() {
    if (!currentSessionToken.empty()) {
        HTTPClient client;
        json requestData;
        requestData["session_token"] = currentSessionToken;
        requestData["username"] = currentUsername;
        
        client.Post(API_LOGOUT_ENDPOINT, requestData);
    }
    
    currentSessionToken.clear();
    currentUsername.clear();
    isAuthenticated = false;
}

bool AuthHandler::IsAuthenticated() const {
    return isAuthenticated;
}

std::string AuthHandler::GetUsername() const {
    return currentUsername;
}
