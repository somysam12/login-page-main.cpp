#include "http_client.h"
#include <curl/curl.h>
#include <iostream>

HTTPClient::HTTPClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HTTPClient::~HTTPClient() {
    curl_global_cleanup();
}

size_t HTTPClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool HTTPClient::VerifySSL(const std::string& url) {
    return url.substr(0, 5) == "https";
}

HTTPResponse HTTPClient::Get(const std::string& url) {
    HTTPResponse response;
    response.success = false;
    response.statusCode = 0;
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        response.error = "Failed to initialize CURL";
        return response;
    }
    
    std::string readBuffer;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BR-MODS-Client/1.0");
    
    if (VerifySSL(url)) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    }
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        response.statusCode = static_cast<int>(http_code);
        response.body = readBuffer;
        response.success = true;
    } else {
        response.error = curl_easy_strerror(res);
    }
    
    curl_easy_cleanup(curl);
    return response;
}

HTTPResponse HTTPClient::Post(const std::string& url, const json& data) {
    HTTPResponse response;
    response.success = false;
    response.statusCode = 0;
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        response.error = "Failed to initialize CURL";
        return response;
    }
    
    std::string readBuffer;
    std::string postData = data.dump();
    
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "BR-MODS-Client/1.0");
    
    if (VerifySSL(url)) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    }
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        response.statusCode = static_cast<int>(http_code);
        response.body = readBuffer;
        response.success = true;
    } else {
        response.error = curl_easy_strerror(res);
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

void HTTPClient::SetTimeout(long timeout) {
}

void HTTPClient::SetUserAgent(const std::string& userAgent) {
}
