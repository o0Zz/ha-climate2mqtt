#pragma once

#include <string>
#include <memory>
#include "esp_http_server.h"
#include "config/IConfig.h"
#include "systeminfo/ISystemInfo.h"

class WebServer
{
public:
    WebServer(IConfig &config, std::unique_ptr<systeminfo::ISystemInfo> &&systemInfo);
    ~WebServer();

    bool start(uint16_t port = 80);
    void stop();

private:
    esp_err_t serve(httpd_req_t *req, const char *title, const std::string &pageContent);
    esp_err_t serveLogsRaw(httpd_req_t *req);

    static esp_err_t request_handler(httpd_req_t *req);

    IConfig &config;
    httpd_handle_t server;
    std::unique_ptr<systeminfo::ISystemInfo> systemInfo;
};
