// -----------------------------------------------------------------------------
// HiveMnd Utility Library : HttpUtils
// -----------------------------------------------------------------------------
// Overview:
//  Provides simple HTTP client functions for performing GET and POST requests.
//  Supports basic text and JSON endpoints. Automatically selects appropriate
//  backend depending on platform (WinHTTP on Windows, libcurl-compatible stub
//  elsewhere).
//
//  Designed for diagnostics, updates, and API integrations.
//
// Key Features:
//  • Simple blocking GET and POST requests
//  • Optional timeout parameter
//  • Returns full response body as std::string
//  • Minimal dependencies (WinHTTP or stubs)
//
// Usage:
//  #include "Network/HttpUtils.h"
//  std::string body = HiveMnd::Network::HttpGet("https://api.ipify.org");
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <map>

namespace HiveMnd::Network
{
    // Perform a simple HTTP GET request.
    // Returns response body or an empty string on failure.
    std::string HttpGet(const std::string& url, int timeoutMs = 5000);

    // Perform a simple HTTP POST request.
    // Accepts headers and a data payload (e.g. JSON).
    std::string HttpPost(
        const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>& headers = {},
        int timeoutMs = 5000);
}

