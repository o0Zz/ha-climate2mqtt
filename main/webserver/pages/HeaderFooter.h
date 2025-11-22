#pragma once

static const char* WEB_COMMON_HEAD =
"<!DOCTYPE html>\n"
"<html>\n"
"<head>\n"
"  <meta charset=\"UTF-8\">\n"
"  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
"  <title>{{title}}</title>\n"
"  <style>\n"
"    body { font-family: Arial, sans-serif; font-size: 15px; margin: 40px; background: #f5f5f5; }\n"
"    .container { max-width: 620px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
"    h1 { color: #333; border-bottom: 2px solid #007bff; padding-bottom: 10px; margin-top:0; }\n"
"    h2 { color: #555; margin-top: 30px; }\n"
"    .config-item { margin: 15px 0; padding: 10px; background: #f8f9fa; border-left: 3px solid #007bff; }\n"
"    .config-label { font-weight: bold; color: #007bff; }\n"
"    .config-value { color: #333; margin-left: 10px; }\n"
"    label { display:block; margin:12px 0 4px; font-weight:bold; }\n"
"    input[type=text], input[type=password] { width:100%; padding:8px; box-sizing:border-box; border:1px solid #ccc; border-radius:4px; }\n"
"    .actions { margin-top:20px; display:flex; flex-direction:column; gap:10px; }\n"
"    .actions-center { justify-content:center; }\n"
"    .actions-center button { width:auto; min-width:140px; }\n"
"    button { background:#007bff; color:#fff; border:none; padding:10px 16px; border-radius:4px; cursor:pointer; font-size: 1em; }\n"
"    button:hover { background:#0069d9; }\n"
"    a.back { display:inline-block; margin-top:15px; }\n"
"    .icon { color: #fff; font-size: 1.5em; vertical-align: middle; margin-right: 8px; }\n"
"    .icon svg { vertical-align: middle; margin-right: 8px; }\n"
"  </style>\n"
"</head>\n"
"<body>\n"
"  <div class=\"container\">\n";

static const char* WEB_COMMON_FOOT =
"  </div>\n"
"</body>\n"
"</html>";
