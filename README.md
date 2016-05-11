# Socket.io-v1.x-Library
Socket.io Library for Arduino

Works with W5100 & ENC28J60 as well as ESP8266

Library will not work when the ESP8266 is being driven by a Uno

Support for JSON was added using https://github.com/bblanchon/ArduinoJson
Users will need to download the library directly from this link and install it.

Instead of using client.send, you can now use client.sendJSON


thanks to funkyguy4000, users can now use #define ESP8266, #define W5100 or #define ENC28J60
no need to edit socketio.h anymore

REST API added : getREST(path), postREST(path, type, data), putREST(path, type, data), deleteREST(path)
headers are not yet handled, soon

incoming messages can now be longer than 125 bytes (at least up to 255 bytes)
will do more testing and edit if necessary

thank you all for your patience
