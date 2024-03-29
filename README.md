# Mini-Mobile-Mission-Control
International Space Station Tracker using a QT Py board, GPS and OLED

This project uses a QT Py board from Adafruit, A battery backpack board from Oak Dev Tech, A BN-280 GPS board and a 128x128 pixel SSD1327 OLED screen and a 1000mAh Li-Ion battery.

The QT Py checks your current location then calculates  the location of the International Space Station (ISS) using a recent Two Line Element.
The ISS location is shown on a World map along with the orbital path for the previous and next 50 minutes.

The neopixel on the QT Py board lights up when the ISS is above the horizon.

There is also a version that runs on a smaller (cheaper) SSD1306 128x64 pixel OLED.

Total cost to make this project should be less than $25.

The updated version uses a QtPy ESP32 S2 which downloads the TLE from the web on startup so that the data is always fairly accurate and uses two SSD1306 OLEDs and a BE-280 GPS board.

![Mini Mobile Mission Control](https://github.com/OkuboHeavyIndustries/Mini-Mobile-Mission-Control/blob/74784d850107647306428a71729298652873720b/IMG_3743.png)

If you make this or use the code please let me know - I'd love to see a picture! 

Twitter @okuboheavyind, reddit okuboheavyindustries and Instagram @okuboheavyindustries

If you like this project and can afford it please send me a cup of coffee!

Bitcoin: 19H3zFF4W3zUZ3jAdjmiDNNLs8Ja46M6AD

ETH: 0xD656DB37b61ac30Fa1e16a3162719FE417b231C8
