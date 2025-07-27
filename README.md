# wemos_Controller_MQTT_SmartHome

this is a smart home controller for my bathroom, balcony and home office made specifically to my needs but you can edit it to suit your own needs.
The exact controller that I'm using is `Wemos NodeMCU V3` which has many different names out there, here is some links to the board: [like 1](https://www.elecbee.com/en-24443-3Pcs-NodeMCU-V3-340G-Lua-WIFI-Module-Integration-Of-ESP8266-Extra-Memory-32M-Flash), [link 2](https://www.amazon.eg/dp/B0B6SWKRZN?ref_=cm_sw_r_cp_ud_dp_RTM9NTGJZEV2W1SRYX2V), [link 3](https://ar.aliexpress.com/item/32815804751.html?gatewayAdapt=glo2ara)

wemos front                |  wemos back
:-------------------------:|:-------------------------:
![wemos front](https://cdn1.botland.store/67475-pdt_540/wifi-module-esp8266-wemos-nodemcu-v3-32mb-11-gpio-adc-pwm.jpg)  |  ![wemos back](https://cdn2.botland.store/67477-pdt_540/wifi-module-esp8266-wemos-nodemcu-v3-32mb-11-gpio-adc-pwm.jpg)

## How to setup environment for build

1. install [python3](https://python.org/)
2. install [arduino](https://www.arduino.cc)

    - on windows use url: <https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE>  
  or type `python` in cmd which will take you to windows store to download [python3](https://www.microsoft.com/store/productId/9PJPW5LDXLZ5)
    - on Ubuntu: use the same link above or (much better) run the following command: `sudo apt install arduino`

3. install the [esp board](https://github.com/esp8266/Arduino#installing-with-boards-manager) from [arduino board manager](https://github.com/esp8266/Arduino#installing-with-boards-manager)  

    - on arduino ide, follow the [instructions](https://github.com/esp8266/Arduino#installing-with-boards-manager).  
    :fire: Tip: I highly recommend installing arduino IDE first as it't the native IDE and installing it provide a lot of support for vs-code extension if you intend to use it.
    - from vs code [arduino extension](https://marketplace.visualstudio.com/items?itemName=vsciot-vscode.vscode-arduino):
        - hit F1 then select "Arduino: Board Manager"
        - on the down right corner of the shown window you'll see "Additional URLs" click it then add the following url from the [esp board](https://github.com/esp8266/Arduino#installing-with-boards-manager) instructions page(the current url is <https://arduino.esp8266.com/stable/package_esp8266com_index.json>).
        - press "Refresh Package Indexes" in the top left coroner of "Arduino Board Manager window"

4. using your preferred Arduino Board manager, search for "esp" and install "esp8266 by ESP8266 Community". choosing any version should be fine but I'm using 3.0.0

5. now you have to install the libraries used:

    - open arduino library manager from your preferred IDE(F1 then Arduino: library manager from vs code)
    - install "WiFiManager" by [tzapu](https://github.com/tzapu/WiFiManager) version 2.0.3-alpha
    - install "PubSubClient" by [Nick O'Leary](https://github.com/knolleary/pubsubclient) version V2.8.0
    - install "arduino-timer" by [Michael Contreras](https://github.com/contrem/arduino-timer) version V2.3.0
    - install "RemoteDebug" by [Joao Lopes](https://github.com/JoaoLopesF/RemoteDebug) version V2.1.2 --> the one over telnet for Arduino ESP8266 and ESP32
    - install "async-mqtt-client" [download](https://github.com/marvinroger/async-mqtt-client/tree/89bf46485d5b60ce1e8e5e4d265a9c1570de3dc5) and install it to (C:\Users\Anas-Khedr-PC\Documents\Arduino\libraries) where ***Anas-Khedr-PC*** is your user, extract and make sure name of folder in "async-mqtt-client" and this folder should have the same structure as the github rebo, meaning no other subdirectory to get to the actual repo files.
    ![folder_structure](https://i.imgur.com/QhWOZAC.png)
    - ESPAsyncTCP by [Me-No-Dev](https://github.com/dvarrel/ESPAsyncTCP) V1.2.2
    - Adafruit ADS1X15 by [Adafruit](https://github.com/adafruit/Adafruit_ADS1X15) V1.1.1
    - DHT sensor library for ESPx by [beegee_tokyo](https://github.com/beegee-tokyo/DHTesp) V1.18.0

---

And that's it. :tada: :tada:  
after you finish building you'll see at the end all the libraries that Arduino used to build the project like this:
> Using library ESP8266WiFi at version 1.0 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\ESP8266WiFi
Using library DNSServer at version 1.1.1 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\DNSServer
Using library ESP8266WebServer at version 1.0 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\ESP8266WebServer
Using library WiFiManager at version 2.0.3-alpha in folder: C:\Users\zero_\Documents\Arduino\libraries\WiFiManager
Using library PubSubClient at version 2.8 in folder: C:\Users\zero_\Documents\Arduino\libraries\PubSubClient
Using library RemoteDebug at version 2.1.2 in folder: C:\Users\zero_\Documents\Arduino\libraries\RemoteDebug
Using library AsyncMqttClient at version 0.9.0 in folder: C:\Users\zero_\Documents\Arduino\libraries\async-mqtt-client
Using library ESPAsyncTCP at version 1.2.2 in folder: C:\Users\zero_\Documents\Arduino\libraries\ESPAsyncTCP
Using library arduino-timer at version 2.3.0 in folder: C:\Users\zero_\Documents\Arduino\libraries\arduino-timer
Using library Adafruit ADS1X15 at version 1.1.1 in folder: C:\Users\zero_\Documents\Arduino\libraries\Adafruit_ADS1X15
Using library Wire at version 1.0 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\Wire
Using library DHT sensor library for ESPx at version 1.18 in folder: C:\Users\zero_\Documents\Arduino\libraries\DHT_sensor_library_for_ESPx
Using library EEPROM at version 1.0 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\EEPROM
Using library ESP8266mDNS at version 1.2 in folder: C:\Users\zero_\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.0\libraries\ESP8266mDNS
"C:\\Users\\zero_\\AppData\\Local\\Arduino15\\packages\\esp8266\\tools\\xtensa-lx106-elf-gcc\\3.0.0-newlib4.0.0-gnu23-48f7b08/bin/xtensa-lx106-elf-size" -A "E:\\Smart Home\\wemos_Controller_MQTT_SmartHome\\build/pub_sub.ino.elf"
Sketch uses 415681 bytes (39%) of program storage space. Maximum is 1044464 bytes.
Global variables use 38936 bytes (47%) of dynamic memory, leaving 42984 bytes for local variables. Maximum is 81920 bytes.

you can see that arduino mentioned each library that I used with its location and version.

## use AI support

you can try and use this [codetxt](https://github.com/HassanHassanKerdash/codetxt) repo to generate some input file for your AI model and request assistance.
