
#if 0
const char html_str[] = {"HTTP/1.1 200 OK\r\n   \
                        Content-Type: text/html\r\n\r\n \
                        <!DOCTYPE html>\r\n \
                        <html>\r\n  \
                        <head>\r\n  \
                            <title>On Button</title>\r\n    \
                            <style>\r\n \
                            .on-button {\r\n    \
                                padding: 100px 80px;\r\n    \
                                font-size: 40px;\r\n    \
                                cursor: pointer;\r\n    \
                                background-color: green;\r\n    \
                                color: white;\r\n   \
                                border: none;\r\n   \
                            }\r\n   \
                            .button-container {\r\n \
                                display: flex;\r\n  \
                                justify-content: center;\r\n    \
                                align-items: center;\r\n    \
                                height: 100vh;\r\n  \
                            }\r\n   \
                            </style>\r\n    \
                        </head>\r\n \
                        <body>\r\n  \
                            <div class=\"button-container\">\r\n    \
                            <button class=\"on-button\" onclick=\"alert(\'Button is ON!\')\">PC On</button>\r\n \
                            </div>\r\n  \
                            <script>\r\n    \
                            function alert(message) {\r\n   \
                                window.alert(message);\r\n  \
                            }\r\n   \
                            </script>\r\n   \
                        </body>\r\n \
                        </html>\r\n\r\n"};

#else

const char html_str[] = {"HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n\r\n"
                        "<!DOCTYPE html>"
                        "<html>"
                        "<body>"
                        "<form action=\"/\" method=\"post\">"
                        "<label for=\"inputStr\">Input Mac Address</label><br>"
                        "<input type=\"text\" id=\"inputStr\" name=\"inputStr\"><br>"
                        "<input type=\"submit\" value=\"Turn on PC\">"
                        "</form>"
                        "</body>"
                        "</html>"};

#endif
