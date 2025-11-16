#pragma once

static const char* WEB_CLIMATE_TITLE = "Climate Configuration";
static const char* WEB_CLIMATE_BODY =
"    <h1>Climate Configuration</h1>\n"
"    <form method=\"POST\" action=\"/save\">\n"
"      <label for=\"climate_type\">Climate Type</label>\n"
"      <input id=\"climate_type\" name=\"climate_type\" type=\"text\" value=\"{{climate_type}}\">\n"
"      <label for=\"climate_polling_ms\">Polling Interval (ms)</label>\n"
"      <input id=\"climate_polling_ms\" name=\"climate_polling_ms\" type=\"text\" value=\"{{climate_polling_ms}}\">\n"
"      <label for=\"climate_uart_tx_pin\">UART TX Pin</label>\n"
"      <input id=\"climate_uart_tx_pin\" name=\"climate_uart_tx_pin\" type=\"text\" value=\"{{climate_uart_tx_pin}}\">\n"
"      <label for=\"climate_uart_rx_pin\">UART RX Pin</label>\n"
"      <input id=\"climate_uart_rx_pin\" name=\"climate_uart_rx_pin\" type=\"text\" value=\"{{climate_uart_rx_pin}}\">\n"
"      <div class=\"actions\">\n"
"        <button type=\"submit\">Save</button>\n"
"        <button type=\"button\" onclick=\"window.location='/'\">Cancel</button>\n"
"      </div>\n"
"    </form>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n";
