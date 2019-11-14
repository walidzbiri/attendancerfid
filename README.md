# Attendance using Rfid
RFID attendance system realized using MFRC522,MOSQUITTO MQTT (On Raspberry PI) ,GREEN RED LEDS and ESP8266 


## Overview

When user apply RFID card on the RC522 module, ESP8266 will detect presence of card and read the serial number. ESP is subscribed to the LEDS topics and will publish serial number and access(1 or 0) to the MQTT broker running on the Raspberry PI. The broker will receive uid and access from the ESP8266 then send them to the Flask web server which will render uid and access to html page and send two get requests to the broker using ajax to control LEDS depending on the access (authorized or not).

## Demo

[![RFID attendance system using RASPBERRY PI MQTT ESP8266 RC522 and FLASK](http://i3.ytimg.com/vi/HSq7vPbVM2U/maxresdefault.jpg)](https://youtu.be/HSq7vPbVM2U)

## Schematics
To complete this project you need these next components:

1x ESP8266 
1x RC522 Sensor 
1x 4700 Ohm Resistor
2x 470 Ohm resistors
2x LEDs (GREEN and RED)

![alt text](https://github.com/walidzbiri/attendancerfid/blob/master/Capture.PNG)

## Usage

Run and install Mosquitto broker on your Raspberry

```
pi@raspberry:~ $ mosquitto -d
```

Installing Flask\
We’re going to use a Python microframework called Flask to turn the PC into web server.
To install Flask, you’ll need to have python and pip installed.

```
pip install flask
pip install paho-mqtt
```

Installing SocketIO

```
pip install flask-socketio
```

## Programming the ESP8266

For the ESP8266 to interact with the web server, you need to install PubSubClient library. This library provides a client for doing simple publish/subscribe messaging with a server that supports MQTT (basically allows your ESP8266 to talk with Python web server).

Installing the PubSubClient library
1) ([Click here to download the PubSubClient library](https://github.com/knolleary/pubsubclient/archive/master.zip)) You should have a .zip folder in your Downloads folder

2) Unzip the .zip folder and you should get pubsubclient-master folder

3) Rename your folder from pubsubclient-master to pubsubclient

4) Move the pubsubclient folder to your Arduino IDE installation libraries folder

The library comes with a number of example sketches. See File > Examples > PubSubClient within the Arduino IDE software.

Installing the MFRC522 sensor library\
The MFRC522 sensor library provides an easy way of using MFRC522 sensor to read tags with your ESP8266 or Arduino boards.

1)  ([Click here to download the MFRC255 sensor library](https://github.com/miguelbalboa/rfid)) You should have a .zip folder in your Downloads folder

2) Open Arduino IDE. Then go to and select Sketch->Include Library->Add .ZIP Library.

3) Choose rfid-master.zip file and click to Open.

## Uploading sketch
Finally, you can upload the full sketch to your ESP8266 (replace with your SSID, password and RPi IP address) which is in the arduino folder of the project


## Launching the Web Server
Go to the root of the project folder and run this command
```
python app.py
```
Your web server should start immediately on port :8181!


contributors to this project : Joudari Alaeddine, Essemlali Oussama,Jaafar Ouadoudi
