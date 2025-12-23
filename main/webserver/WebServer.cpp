#include "WebServer.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"

#include <sstream>
#include <map>
#include <cctype>

#include "webserver/JinjaLikeTemplate.h"
#include "webserver/UrlUtils.h"

#include "pages/HeaderFooter.h"
#include "pages/Home.h"
#include "pages/WiFi.h"
#include "pages/MQTT.h"
#include "pages/Climate.h"
#include "pages/Saved.h"
#include "pages/Upgrade.h"
#include "pages/Reboot.h"
#include "pages/ClearConfig.h"

#define TAG "WebServer"

#define HTTP_CHUNK_SIZE 1024
struct RouteDef {
    const char* uri;
    httpd_method_t method;
    const char *title;
    const char *body;
};

    //All routes definition
const RouteDef routes[] = {
    {"/",               HTTP_GET,  WEB_HOME_TITLE, WEB_HOME_BODY},
    {"/wifi",           HTTP_GET,  WEB_WIFI_TITLE, WEB_WIFI_BODY},
    {"/mqtt",           HTTP_GET,  WEB_MQTT_TITLE, WEB_MQTT_BODY},
    {"/climate",        HTTP_GET,  WEB_CLIMATE_TITLE, WEB_CLIMATE_BODY},
    {"/save",           HTTP_POST, WEB_SAVED_TITLE, WEB_SAVED_BODY},
    {"/reboot",         HTTP_POST, WEB_REBOOT_TITLE, WEB_REBOOT_BODY},
    {"/clear_config",   HTTP_POST, WEB_CLEAR_CONFIG_TITLE, WEB_CLEAR_CONFIG_BODY},
    {"/upgrade",        HTTP_GET,  WEB_UPGRADE_TITLE, WEB_UPGRADE_BODY},
    {"/upgrade",        HTTP_POST, nullptr, nullptr},
};

WebServer::WebServer(IConfig &config, std::unique_ptr<systeminfo::ISystemInfo> &&systemInfo) : 
    config(config), 
    server(nullptr),
    systemInfo(std::move(systemInfo))
{
}

WebServer::~WebServer()
{
    stop();
}

esp_err_t WebServer::serve(httpd_req_t *req, const char *title, const std::string &pageContent)
{
    JinjaLikeTemplate::VarMap vars = config.getAll();

    for (const auto& kv : config.getConfigValueList()) 
    {
        const char* const* list = kv.second;
        if (list != nullptr) 
        {
            std::string options;
            for (int i = 0; list[i] != nullptr; ++i) 
            {
                options += list[i];
                if (list[i + 1] != nullptr)
                    options += ",";
            }
            vars[kv.first + "_list"] = options;
        }
    }
    
    vars["app_version"] = systemInfo->getVersion();
    vars["mac_address"] = systemInfo->getMacAddress();
    vars["ip_address"] = systemInfo->getIPAddress();
    vars["hostname"] = systemInfo->getHostname();
    vars["uptime_seconds"] = std::to_string(systemInfo->getUptimeSeconds());
    vars["free_heap_bytes"] =  std::to_string(systemInfo->getFreeHeapBytes());
    vars["total_heap_bytes"] = std::to_string(systemInfo->getTotalHeapBytes());
    vars["chip_model"] = systemInfo->getChipModel();
    vars["cpu_frequency_mhz"] = std::to_string(systemInfo->getCpuFrequencyMHz());
    vars["cpu_core_count"] = std::to_string(systemInfo->getCpuCoreCount());
    vars["chip_revision"] = systemInfo->getChipRevision();
    vars["flash_size_bytes"] = std::to_string(systemInfo->getFlashSizeBytes());
    
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

    ESP_LOGI(TAG, "HTTP Method: %d Uri: %s", req->method, req->uri);
    if (req->method == HTTP_POST) {
        if (std::string(req->uri) == "/upgrade") 
        {
            char buf[HTTP_CHUNK_SIZE];

            ESP_LOGI(TAG, "OTA: Searching for the next upgrade partition ...");
            const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
            if (!update_partition) {
                ESP_LOGE(TAG, "No OTA partition found !");
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition");
                return ESP_FAIL;
            }

            ESP_LOGI(TAG, "OTA: Beginning ...");
            size_t max_fw_size = update_partition->size;
            esp_ota_handle_t ota_handle = 0;
            esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "OTA begin failed: %s", esp_err_to_name(err));
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA begin failed");
                return err;
            }

            size_t total = 0;
            int received = 0;
            while (total < max_fw_size) 
            {
                received = httpd_req_recv(req, buf, HTTP_CHUNK_SIZE);
                if (received <= 0) 
                    break;

                err = esp_ota_write(ota_handle, buf, received);
                if (err != ESP_OK) {
                    esp_ota_end(ota_handle);
                    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA write failed");
                    return err;
                }

                ESP_LOGI(TAG, "OTA: Wrote %d bytes", received);
                total += received;
            }

            ESP_LOGI(TAG, "OTA: Finished to received data, total: %d bytes", total);
            if (total == 0) 
            {
                esp_ota_end(ota_handle);
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "No firmware received");
                return ESP_FAIL;
            }
            
            ESP_LOGI(TAG, "OTA: Ending...");
            err = esp_ota_end(ota_handle);
            if (err != ESP_OK) 
            {
                ESP_LOGE(TAG, "OTA end failed: %s", esp_err_to_name(err));
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA end failed");
                return err;
            }

            ESP_LOGI(TAG, "OTA: Switching boot partition...");
            err = esp_ota_set_boot_partition(update_partition);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "OTA set boot partition successful");
                httpd_resp_sendstr(req, "Upgrade successful. Rebooting...");
                vTaskDelay(pdMS_TO_TICKS(2000));
                esp_restart();
            } else {
                ESP_LOGE(TAG, "OTA set boot partition failed: %s", esp_err_to_name(err));
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA set boot failed");
            }
            return err;

        } 
        else if (std::string(req->uri) == "/save") 
        {
            //Save posted settings
            auto form = UrlUtils::parse_urlencoded_form(req);
            webServer->config.set(form);
            
        }
        else if (std::string(req->uri) == "/reboot") 
        {
            (void)webServer->serve(req, WEB_REBOOT_TITLE, WEB_REBOOT_BODY);
            vTaskDelay(pdMS_TO_TICKS(2000));
            esp_restart();
            return ESP_OK;
        }
        else if (std::string(req->uri) == "/clear_config")
        {
            if (!webServer->config.clear())
            {
                ESP_LOGE(TAG, "Failed to clear configuration");
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to clear configuration");
                return ESP_FAIL;
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
