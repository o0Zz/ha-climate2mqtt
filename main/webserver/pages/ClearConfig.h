#pragma once

static const char* WEB_CLEAR_CONFIG_TITLE = "Configuration Cleared";
static const char* WEB_CLEAR_CONFIG_BODY =
"    <h1>Configuration Cleared</h1>\n"
"    <p>All stored settings have been erased. You can now reconfigure the device.</p>\n"
"    <p style=\"margin-top:20px;color:#555;\">Redirecting to Home in 5 seconds...</p>\n"
"    <script>setTimeout(()=>window.location='/',5000);</script>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n";
