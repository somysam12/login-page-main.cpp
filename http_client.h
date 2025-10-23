#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct HTTPResponse {
    bool success;
    int statusCode;
    std::string body;
    std::string error;
};

class HTTPClient {
private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    bool VerifySSL(const std::string& url);

public:
    HTTPClient();
    ~HTTPClient();
    
    HTTPResponse Get(const std::string& url);
    HTTPResponse Post(const std::string& url, const json& data);
    void SetTimeout(long timeout);
    void SetUserAgent(const std::string& userAgent);
};

#endif
