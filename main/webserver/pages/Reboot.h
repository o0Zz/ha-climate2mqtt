#pragma once

static const char* WEB_REBOOT_TITLE = "Rebooting";
static const char* WEB_REBOOT_BODY =
"    <h1>Rebooting...</h1>\n"
"    <p style=\"margin-top:20px;color:#555;\">Redirecting to Home in 5 seconds...</p>\n"
"    <script>setTimeout(()=>window.location='/',5000);</script>\n"
"    <a href=\"/\" class=\"back\">&larr; Back to Home</a>\n";