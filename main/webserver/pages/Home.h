#pragma once

static const char* WEB_HOME_TITLE = "Climate2MQTT Settings";
static const char* WEB_HOME_BODY =
"    <h1>Climate2MQTT Settings</h1>\n"
"    <p>ESP-IDF based climate control system</p>\n"
"    <div class=\"actions actions-center\">\n"
"      <button type=\"button\" onclick=\"window.location='/wifi'\">Configure WiFi</button>\n"
"      <button type=\"button\" onclick=\"window.location='/mqtt'\">Configure MQTT</button>\n"
"      <button type=\"button\" onclick=\"window.location='/climate'\">Configure Climate</button>\n"
"    </div>\n"
"    <h2>Current Configuration</h2>\n"
"    <div class=\"config-item\">\n"
"      <span class=\"config-label\">MQTT Broker URI:</span><span class=\"config-value\">{{mqtt_broker_uri}}</span>\n"
"    </div>\n"
"    <div class=\"config-item\">\n"
"      <span class=\"config-label\">WiFi SSID:</span><span class=\"config-value\">{{wifi_ssid}}</span>\n"
"    </div>\n";
