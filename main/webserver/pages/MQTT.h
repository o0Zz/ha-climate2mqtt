#pragma once

static const char* WEB_MQTT_TITLE = "MQTT Configuration";
static const char* WEB_MQTT_BODY =
"    <h1>MQTT Configuration</h1>\n"
"    <form method=\"POST\" action=\"/save\">\n"
"      <label for=\"mqtt_broker_uri\">Broker URI</label>\n"
"      <input id=\"mqtt_broker_uri\" name=\"mqtt_broker_uri\" type=\"text\" value=\"{{mqtt_broker_uri}}\">\n"
"      <label for=\"mqtt_user\">User</label>\n"
"      <input id=\"mqtt_user\" name=\"mqtt_user\" type=\"text\" value=\"{{mqtt_user}}\">\n"
"      <label for=\"mqtt_password\">Password</label>\n"
"      <input id=\"mqtt_password\" name=\"mqtt_password\" type=\"password\" value=\"{{mqtt_password}}\">\n"
"      <label for=\"mqtt_unique_id\">Unique ID</label>\n"
"      <input id=\"mqtt_unique_id\" name=\"mqtt_unique_id\" type=\"text\" value=\"{{mqtt_unique_id}}\">\n"
"      <div class=\"actions\">\n"
"        <button type=\"submit\">Save</button>\n"
"        <button type=\"button\" onclick=\"window.location='/'\">Cancel</button>\n"
"      </div>\n"
"    </form>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n";
