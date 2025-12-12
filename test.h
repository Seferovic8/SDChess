#pragma once

#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "winhttp.lib")

static std::pair<std::string, int>  testiram(int from, int to) {
    HINTERNET hSession = WinHttpOpen(L"SimpleGET/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

   // if (!hSession) return 1;

    HINTERNET hConnect = WinHttpConnect(hSession,
        L"127.0.0.1",  // IP as string
        8000,           // port separate
        0);


    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST", L"/calculate", NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0  // <-- plain HTTP, not secure
    );


    std::string body = "{ \"from\": " + std::to_string(from) +
        ", \"to\": " + std::to_string(to) + " }";

    // Convert body to UTF-16
    BOOL result = WinHttpSendRequest(
        hRequest,
        L"Content-Type: application/json\r\n",
        -1,
        (LPVOID)body.c_str(),
        body.size(),
        body.size(),
        0
    );

    if (result)
        result = WinHttpReceiveResponse(hRequest, NULL);
    std::string response="";
    if (result) {
        DWORD size = 0;

        do {
            WinHttpQueryDataAvailable(hRequest, &size);
            if (!size) break;

            std::vector<char> buffer(size);
            DWORD downloaded = 0;

            if (WinHttpReadData(hRequest, buffer.data(), size, &downloaded))
                for (size_t i = 0; i < buffer.size(); ++i) {
                    response += buffer[i];
                }
                //std::cout.write(buffer.data(), downloaded);

        } while (size > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    std::pair<std::string, int> resultPair;

    auto posFen = response.find("\"fen\":");
    auto posNum = response.find("\"numPositions\":");

    if (posFen != std::string::npos && posNum != std::string::npos) {
        // extract fen string
        std::string fen = "";
        int i = 9;
        while (true) {

            if (response[i] == '"') {

            break;
            }
            fen += response[i];
            i += 1;
        }

        resultPair.first = fen;

        // extract integer
        size_t colon = response.find(":", posNum);
        size_t end = response.find_first_of(",}", colon);
        resultPair.second = std::stoi(response.substr(colon + 1, end - colon - 1));
    }


    return resultPair;

}
