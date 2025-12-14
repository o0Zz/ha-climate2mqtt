#pragma once

#include <string>
#include "esp_http_server.h"
#include "../config/IConfig.h"

class WebServer
{
public:
    WebServer(IConfig &config, const std::string &appversion = "unknown");
    ~WebServer();

    bool start(uint16_t port = 80);
    void stop();

private:
    esp_err_t serve(httpd_req_t *req, const char *title, const std::string &pageContent);

    static esp_err_t request_handler(httpd_req_t *req);

    IConfig &config;
    httpd_handle_t server;
    std::string appversion;
};
