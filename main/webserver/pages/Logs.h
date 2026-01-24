#pragma once

static const char* WEB_LOGS_TITLE = "Logs";
static const char* WEB_LOGS_BODY =
"    <h1>Logs</h1>\n"
"    <p>Recent device logs</p>\n"
"    <div class=\"actions\" style=\"flex-direction:row; gap:8px;\">\n"
"      <button type=\"button\" onclick=\"fetchLogs()\">Refresh</button>\n"
"    </div>\n"
"    <pre id=\"logOutput\" style=\"white-space:pre; background:#111; color:#0f0; padding:12px; border-radius:6px; height:360px; overflow-y:auto; overflow-x:auto; font-size:12px;\"></pre>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n"
"    <script>\n"
"      async function fetchLogs(){\n"
"        const res = await fetch('/logs/raw');\n"
"        const text = await res.text();\n"
"        const el = document.getElementById('logOutput');\n"
"        el.textContent = text;\n"
"        el.scrollTop = el.scrollHeight;\n"
"      }\n"
"      fetchLogs();\n"
"    </script>\n";
