#include "WebServer.h"
#include "esp_log.h"
#include <sstream>
#include <map>
#include <cctype>

#include "webserver/JinjaLikeTemplate.h"
#include "webserver/UrlUtils.h"

#include "config/ConfigConsts.h"

#include "pages/HeaderFooter.h"
#include "pages/Home.h"
#include "pages/WiFi.h"
#include "pages/MQTT.h"
#include "pages/Climate.h"
#include "pages/Saved.h"

#define TAG "WebServer"

struct RouteDef {
    const char* uri;
    httpd_method_t method;
    const char *title;
    const char *body;
};

    //All routes definition
const RouteDef routes[] = {
    {"/",        HTTP_GET,  WEB_HOME_TITLE, WEB_HOME_BODY},
    {"/wifi",    HTTP_GET,  WEB_WIFI_TITLE, WEB_WIFI_BODY},
    {"/mqtt",    HTTP_GET,  WEB_MQTT_TITLE, WEB_MQTT_BODY},
    {"/climate", HTTP_GET,  WEB_CLIMATE_TITLE, WEB_CLIMATE_BODY},
    {"/save",    HTTP_POST, WEB_SAVED_TITLE, WEB_SAVED_BODY},
};

WebServer::WebServer(IConfig &config) : 
    config(config), 
    server(nullptr)
{
}

WebServer::~WebServer()
{
    stop();
}

esp_err_t WebServer::serve(httpd_req_t *req, const char *title, const std::string &pageContent)
{
    JinjaLikeTemplate::VarMap vars;

    for (const char* key : ConfigList) 
    {
        if (key == nullptr)
            break;
        vars[key] = config.getString(key, "");
    }

    vars["title"] = title;

    std::string htmlStr = JinjaLikeTemplate::render(std::string(WEB_COMMON_HEAD) + pageContent + WEB_COMMON_FOOT, vars);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, htmlStr.c_str(), htmlStr.length());
    return ESP_OK;
}

esp_err_t WebServer::request_handler(httpd_req_t *req)
{
    //Generic get handler that serves all pages
    WebServer* webServer = static_cast<WebServer*>(req->user_ctx);

    if (req->method == HTTP_POST) 
    {
            //Save posted settings
        auto form = UrlUtils::parse_urlencoded_form(req);
        for (const auto& kv : form) 
        {
            for (const char* key : ConfigList) 
            {
                if (key == nullptr) 
                    break;

                if (kv.first == key) {
                    webServer->config.setString(kv.first.c_str(), kv.second.c_str());
                }
            }
        }
    }

    for (const auto& route : routes) {
        if (req->uri == std::string(route.uri)) {
            return webServer->serve(req, route.title, route.body);
        }
    }

    // Default to home page
    return webServer->serve(req, WEB_HOME_TITLE, WEB_HOME_BODY);
}

bool WebServer::start(uint16_t port)
{
    if (server != nullptr) {
        ESP_LOGW(TAG, "Server already started");
        return true;
    }
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;
    config.ctrl_port = 32768;
    config.max_open_sockets = 7;
    config.lru_purge_enable = true;
    
    ESP_LOGI(TAG, "Starting HTTP server on port %d", port);
    
    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
        return false;
    }

    for (const auto& route : routes) {
        httpd_uri_t uri_def = {
            .uri = route.uri,
            .method = route.method,
            .handler = request_handler,
            .user_ctx = this
        };
        err = httpd_register_uri_handler(server, &uri_def);
        if (err != ESP_OK)
            ESP_LOGE(TAG, "Failed to register handler for %s: %s", route.uri, esp_err_to_name(err));
    }
    ESP_LOGI(TAG, "HTTP server started successfully");
    return true;
}

void WebServer::stop()
{
    if (server != nullptr) {
        ESP_LOGI(TAG, "Stopping HTTP server");
        httpd_stop(server);
        server = nullptr;
    }
}
