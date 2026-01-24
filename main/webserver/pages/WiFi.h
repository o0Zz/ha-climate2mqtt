#pragma once

static const char* WEB_WIFI_TITLE = "WiFi Configuration";
static const char* WEB_WIFI_BODY =
"    <h1>WiFi Configuration</h1>\n"
"    <form method=\"POST\" action=\"/save\">\n"
"      <label for=\"wifi_ssid\">SSID</label>\n"
"      <input id=\"wifi_ssid\" name=\"wifi_ssid\" type=\"text\" value=\"{{wifi_ssid}}\">\n"
"      <label for=\"wifi_password\">Password</label>\n"
"      <input id=\"wifi_password\" name=\"wifi_password\" type=\"password\" value=\"{{wifi_password}}\">\n"
"      <label for=\"wifi_hostname\">Hostname</label>\n"
"      <input id=\"wifi_hostname\" name=\"wifi_hostname\" type=\"text\" value=\"{{wifi_hostname}}\">\n"
"      <label for=\"wifi_security\">Security</label>\n"
"      <select id=\"wifi_security\" name=\"wifi_security\"></select>\n"
"      <div class=\"actions\" style=\"flex-direction:row; align-items:center;\">\n"
"        <button type=\"submit\">Save</button>\n"
"        <button type=\"button\" onclick=\"window.location='/'\">Cancel</button>\n"
"      </div>\n"
"    </form>\n"
"    <script> init_select(\"wifi_security\", \"{{wifi_security_list}}\", {{wifi_security}}); </script>\n";