#include "HttpUtils.h"
#include <iostream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <cstdio>
#include <cstring>
#endif



namespace HiveMnd::Network
{
#ifdef _WIN32

    std::string HttpGet(const std::string& url, int)
    {
        std::wstring wurl(url.begin(), url.end());
        URL_COMPONENTS comps{};
        wchar_t host[256];
        wchar_t path[1024];
        comps.dwStructSize = sizeof(comps);
        comps.lpszHostName = host;
        comps.dwHostNameLength = _countof(host);
        comps.lpszUrlPath = path;
        comps.dwUrlPathLength = _countof(path);

        if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &comps))
            return "";

        HINTERNET hSession = WinHttpOpen(L"HiveMnd/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);

        if (!hSession)
            return "";

        HINTERNET hConnect = WinHttpConnect(hSession, comps.lpszHostName, comps.nPort, 0);
        if (!hConnect)
        {
            WinHttpCloseHandle(hSession);
            return "";
        }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET",
            comps.lpszUrlPath,
            nullptr, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            (comps.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);

        if (!hRequest)
        {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "";
        }

        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
        {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return "";
        }

        WinHttpReceiveResponse(hRequest, nullptr);
        std::string response;
        DWORD bytesAvailable = 0;
        do
        {
            if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable) || bytesAvailable == 0)
                break;

            std::vector<char> buffer(bytesAvailable + 1);
            DWORD bytesRead = 0;
            if (WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead))
                response.append(buffer.data(), bytesRead);
        } while (bytesAvailable > 0);

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return response;
    }

    std::string HttpPost(const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>&,
        int)
    {
        std::ostringstream ss;
        ss << "[POST placeholder] URL: " << url << " Data: " << body;
        return ss.str();
    }

#else
    std::string HttpGet(const std::string& url, int)
    {
        std::ostringstream ss;
        ss << "[HttpGet stub] " << url;
        return ss.str();
    }

    std::string HttpPost(const std::string& url,
        const std::string& body,
        const std::map<std::string, std::string>&,
        int)
    {
        std::ostringstream ss;
        ss << "[HttpPost stub] " << url << " Data: " << body;
        return ss.str();
    }
#endif
}
