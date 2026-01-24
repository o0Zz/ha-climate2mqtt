#pragma once

static const char* WEB_CLIMATE_TITLE = "Climate Configuration";
static const char* WEB_CLIMATE_BODY =
"    <h1>Climate Configuration</h1>\n"
"    <form method=\"POST\" action=\"/save\">\n"
"      <label for=\"climate_type\">Climate Type</label>\n"
"      <select id=\"climate_type\" name=\"climate_type\"></select>\n"
"      <label for=\"climate_poll_ms\">Polling Interval (ms)</label>\n"
"      <input id=\"climate_poll_ms\" name=\"climate_poll_ms\" type=\"text\" value=\"{{climate_poll_ms}}\">\n"
"      <label for=\"climate_tx_pin\">UART TX Pin</label>\n"
"      <input id=\"climate_tx_pin\" name=\"climate_tx_pin\" type=\"text\" value=\"{{climate_tx_pin}}\">\n"
"      <label for=\"climate_rx_pin\">UART RX Pin</label>\n"
"      <input id=\"climate_rx_pin\" name=\"climate_rx_pin\" type=\"text\" value=\"{{climate_rx_pin}}\">\n"
"      <div class=\"actions\" style=\"flex-direction:row; align-items:center;\">\n"
"        <button type=\"submit\">Save</button>\n"
"        <button type=\"button\" onclick=\"window.location='/'\">Cancel</button>\n"
"      </div>\n"
"    </form>\n"
"    <script> init_select(\"climate_type\", \"{{climate_type_list}}\", {{climate_type}}); </script>\n";
