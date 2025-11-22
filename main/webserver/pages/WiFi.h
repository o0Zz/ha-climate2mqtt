#pragma once

static const char* WEB_WIFI_TITLE = "WiFi Configuration";
static const char* WEB_WIFI_BODY =
"    <h1>WiFi Configuration</h1>\n"
"    <form method=\"POST\" action=\"/save\">\n"
"      <label for=\"wifi_ssid\">SSID</label>\n"
"      <input id=\"wifi_ssid\" name=\"wifi_ssid\" type=\"text\" value=\"{{wifi_ssid}}\">\n"
"      <label for=\"wifi_password\">Password</label>\n"
"      <input id=\"wifi_password\" name=\"wifi_password\" type=\"password\" value=\"{{wifi_password}}\">\n"
"      <label for=\"wifi_security\">Security</label>\n"
"      <select id=\"wifi_security\" name=\"wifi_security\"></select>\n"
"      <div class=\"actions\">\n"
"        <button type=\"submit\">Save</button>\n"
"        <button type=\"button\" onclick=\"window.location='/'\">Cancel</button>\n"
"      </div>\n"
"    </form>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n"
"    <script> init_select(\"wifi_security\", \"{{wifi_security_list}}\", {{wifi_security}}); </script>\n";