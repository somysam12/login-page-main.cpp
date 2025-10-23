#ifndef CONFIG_H
#define CONFIG_H

#include <string>

const std::string APP_VERSION = "1.0.0";
const std::string APP_NAME = "Login Sys By @Tgshaitaan";

const std::string API_BASE_URL = "https://your-website.com/api";

const std::string API_VALIDATE_ENDPOINT = API_BASE_URL + "/validate";
const std::string API_CHECK_SESSION_ENDPOINT = API_BASE_URL + "/check-session";
const std::string API_LOGOUT_ENDPOINT = API_BASE_URL + "/logout";

const long HTTP_TIMEOUT = 30;

#ifdef ENABLE_INTEGRITY_CHECK
const std::string EXPECTED_BINARY_CHECKSUM = "REPLACE_WITH_YOUR_BINARY_SHA256_HASH";
#endif

#endif
