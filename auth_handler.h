#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H

#include <string>
#include <ctime>

struct AuthResult {
    bool success;
    std::string message;
    std::string sessionToken;
    std::time_t expiresAt;
};

class AuthHandler {
private:
    std::string currentSessionToken;
    std::string currentUsername;
    bool isAuthenticated;
    
    std::string GenerateHWID();
    std::string EncryptKey(const std::string& key);
    bool VerifyIntegrity();

public:
    AuthHandler();
    ~AuthHandler();
    
    AuthResult ValidateKey(const std::string& username, const std::string& key);
    bool CheckSession();
    void Logout();
    bool IsAuthenticated() const;
    std::string GetUsername() const;
};

#endif
