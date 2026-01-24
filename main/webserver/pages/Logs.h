#pragma once

static const char* WEB_LOGS_TITLE = "Logs";
static const char* WEB_LOGS_BODY =
"    <h1>Logs</h1>\n"

"    <pre id=\"logOutput\" style=\"white-space:pre; background:#111; color:#0f0; padding:12px; border-radius:6px; height:360px; overflow-y:auto; overflow-x:auto; font-size:12px;\"></pre>\n"
"    <div class=\"actions\" style=\"flex-direction:row; align-items:center;\">\n"
"      <button type=\"button\" onclick=\"window.location='/'\">< Back</button>\n"
"      <button type=\"button\" onclick=\"fetchLogs()\">Refresh</button>\n"
"    </div>\n"
"    <script>\n"
"      async function fetchLogs(){\n"
"        const res = await fetch('/logs/raw');\n"
"        const el = document.getElementById('logOutput');\n"
"        el.textContent = await res.text();\n"
"        el.scrollTop = el.scrollHeight;\n"
"      }\n"
"      fetchLogs();\n"
"    </script>\n";
