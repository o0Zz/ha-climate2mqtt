#pragma once

static const char* WEB_HOME_TITLE = "Climate2MQTT Settings";
static const char* WEB_HOME_BODY =
"    <h1>Climate2MQTT Settings</h1>\n"
"    <p>ESP-IDF based climate control system <b>v{{app_version}}</b></p>\n"
"    <div class=\"actions actions-centered\">\n"

"      <button type=\"button\" onclick=\"window.location='/wifi'\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M320 160C229.1 160 146.8 196 86.3 254.6C73.6 266.9 53.3 266.6 41.1 253.9C28.9 241.2 29.1 220.9 41.8 208.7C113.7 138.9 211.9 96 320 96C428.1 96 526.3 138.9 598.3 208.7C611 221 611.3 241.3 599 253.9C586.7 266.5 566.4 266.9 553.8 254.6C493.2 196 410.9 160 320 160zM272 496C272 469.5 293.5 448 320 448C346.5 448 368 469.5 368 496C368 522.5 346.5 544 320 544C293.5 544 272 522.5 272 496zM200 390.2C188.3 403.5 168.1 404.7 154.8 393C141.5 381.3 140.3 361.1 152 347.8C193 301.4 253.1 272 320 272C386.9 272 447 301.4 488 347.8C499.7 361.1 498.4 381.3 485.2 393C472 404.7 451.7 403.4 440 390.2C410.6 356.9 367.8 336 320 336C272.2 336 229.4 356.9 200 390.2z\" fill=\"white\"/>\n"
"       </svg></span>Configure WiFi</button>\n"

"      <button type=\"button\" onclick=\"window.location='/mqtt'\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M176 544C96.5 544 32 479.5 32 400C32 336.6 73 282.8 129.9 263.5C128.6 255.8 128 248 128 240C128 160.5 192.5 96 272 96C327.4 96 375.5 127.3 399.6 173.1C413.8 164.8 430.4 160 448 160C501 160 544 203 544 256C544 271.7 540.2 286.6 533.5 299.7C577.5 320 608 364.4 608 416C608 486.7 550.7 544 480 544L176 544zM337 255C327.6 245.6 312.4 245.6 303.1 255L231.1 327C221.7 336.4 221.7 351.6 231.1 360.9C240.5 370.2 255.7 370.3 265 360.9L296 329.9L296 432C296 445.3 306.7 456 320 456C333.3 456 344 445.3 344 432L344 329.9L375 360.9C384.4 370.3 399.6 370.3 408.9 360.9C418.2 351.5 418.3 336.3 408.9 327L336.9 255z\" fill=\"white\"/>\n"
"       </svg></span>Configure MQTT</button>\n"

"      <button type=\"button\" onclick=\"window.location='/climate'\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M259.1 73.5C262.1 58.7 275.2 48 290.4 48L350.2 48C365.4 48 378.5 58.7 381.5 73.5L396 143.5C410.1 149.5 423.3 157.2 435.3 166.3L503.1 143.8C517.5 139 533.3 145 540.9 158.2L570.8 210C578.4 223.2 575.7 239.8 564.3 249.9L511 297.3C511.9 304.7 512.3 312.3 512.3 320C512.3 327.7 511.8 335.3 511 342.7L564.4 390.2C575.8 400.3 578.4 417 570.9 430.1L541 481.9C533.4 495 517.6 501.1 503.2 496.3L435.4 473.8C423.3 482.9 410.1 490.5 396.1 496.6L381.7 566.5C378.6 581.4 365.5 592 350.4 592L290.6 592C275.4 592 262.3 581.3 259.3 566.5L244.9 496.6C230.8 490.6 217.7 482.9 205.6 473.8L137.5 496.3C123.1 501.1 107.3 495.1 99.7 481.9L69.8 430.1C62.2 416.9 64.9 400.3 76.3 390.2L129.7 342.7C128.8 335.3 128.4 327.7 128.4 320C128.4 312.3 128.9 304.7 129.7 297.3L76.3 249.8C64.9 239.7 62.3 223 69.8 209.9L99.7 158.1C107.3 144.9 123.1 138.9 137.5 143.7L205.3 166.2C217.4 157.1 230.6 149.5 244.6 143.4L259.1 73.5zM320.3 400C364.5 399.8 400.2 363.9 400 319.7C399.8 275.5 363.9 239.8 319.7 240C275.5 240.2 239.8 276.1 240 320.3C240.2 364.5 276.1 400.2 320.3 400z\" fill=\"white\"/>\n"
"       </svg></span>Configure Climate</button>\n"

"      <button type=\"button\" onclick=\"window.location='/logs'\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M128 144C128 130.7 138.7 120 152 120L488 120C501.3 120 512 130.7 512 144C512 157.3 501.3 168 488 168L152 168C138.7 168 128 157.3 128 144zM128 288C128 274.7 138.7 264 152 264L488 264C501.3 264 512 274.7 512 288C512 301.3 501.3 312 488 312L152 312C138.7 312 128 301.3 128 288zM128 432C128 418.7 138.7 408 152 408L488 408C501.3 408 512 418.7 512 432C512 445.3 501.3 456 488 456L152 456C138.7 456 128 445.3 128 432z\" fill=\"white\"/>\n"
"       </svg></span>View Logs</button>\n"

"    </div>\n"
"    <h2>Current Configuration</h2>\n"
"    <div class=\"config-item\">\n"
"      <div class=\"config-line\"><span class=\"config-label\">WiFi SSID:</span><span class=\"config-value\">{{wifi_ssid}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">Hostname:</span><span class=\"config-value\">{{hostname}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">MAC Address:</span><span class=\"config-value\">{{mac_address}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">IP Address:</span><span class=\"config-value\">{{ip_address}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">MQTT Broker:</span><span class=\"config-value\">{{mqtt_broker_uri}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">HomeAssistant Unique ID:</span><span class=\"config-value\">{{mqtt_unique_id}}</span></div>\n"
"      <div class=\"config-line\"><span class=\"config-label\">UpTime:</span><span class=\"config-value\">{{uptime_hours}} hour(s)</span></div>\n"

"    </div>\n"

"    <div class=\"actions actions-centered\" style=\"flex-direction:row; align-items:center;\">\n"
"       <button type=\"button\" onclick=\"window.location='/upgrade'\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M342.6 73.4C330.1 60.9 309.8 60.9 297.3 73.4L169.3 201.4C156.8 213.9 156.8 234.2 169.3 246.7C181.8 259.2 202.1 259.2 214.6 246.7L288 173.3L288 384C288 401.7 302.3 416 320 416C337.7 416 352 401.7 352 384L352 173.3L425.4 246.7C437.9 259.2 458.2 259.2 470.7 246.7C483.2 234.2 483.2 213.9 470.7 201.4L342.7 73.4zM160 416C160 398.3 145.7 384 128 384C110.3 384 96 398.3 96 416L96 480C96 533 139 576 192 576L448 576C501 576 544 533 544 480L544 416C544 398.3 529.7 384 512 384C494.3 384 480 398.3 480 416L480 480C480 497.7 465.7 512 448 512L192 512C174.3 512 160 497.7 160 480L160 416z\" fill=\"white\"/>\n"
"       </svg></span>Upgrade Firmware</button>\n"

"      <form id=\"clearConfigForm\" method=\"POST\" action=\"/clear_config\" style=\"display:inline;\"></form>\n"
"      <button type=\"button\" onclick=\"if(confirm('Clear all configuration?')) document.getElementById('clearConfigForm').submit();\"><span class=\"icon\">\n"
"       <svg viewBox=\"0 0 640 640\" width=\"24\" height=\"24\">\n"
"           <path d=\"M224 112C224 94.3 238.3 80 256 80L384 80C401.7 80 416 94.3 416 112L416 128L512 128C529.7 128 544 142.3 544 160C544 177.7 529.7 192 512 192L496 192L474.2 520.6C472.5 545 452.2 564 427.7 564L212.3 564C187.8 564 167.5 545 165.8 520.6L144 192L128 192C110.3 192 96 177.7 96 160C96 142.3 110.3 128 128 128L224 128L224 112zM256 128L384 128L384 112L256 112L256 128zM216 248C216 230.3 230.3 216 248 216C265.7 216 280 230.3 280 248L280 496C280 513.7 265.7 528 248 528C230.3 528 216 513.7 216 496L216 248zM360 248C360 230.3 374.3 216 392 216C409.7 216 424 230.3 424 248L424 496C424 513.7 409.7 528 392 528C374.3 528 360 513.7 360 496L360 248z\" fill=\"white\"/>\n"
"       </svg></span>Clear Configuration</button>\n"

"      <form id=\"rebootForm\" method=\"POST\" action=\"/reboot\" style=\"display:inline;\"></form>\n"
"      <button type=\"button\" onclick=\"if(confirm('Reboot to apply configuration?')) document.getElementById('rebootForm').submit();\"><span class=\"icon\">\n"
"      <svg viewBox=\"0 0 24 24\" width=\"24\" height=\"24\" fill=\"none\" stroke=\"white\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" aria-hidden=\"true\"><line x1=\"12\" y1=\"2\" x2=\"12\" y2=\"12\" />"
"         <path d=\"M5.5 5.5a9 9 0 1 0 13 0\" />"
"      </svg></span>Reboot to Apply</button>\n"
"  </div>\n";