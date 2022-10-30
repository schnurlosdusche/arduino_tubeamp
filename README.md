# arduino_tubeamp

<p align="center">
  <img src="https://github.com/schnurlosdusche/arduino_tubeamp/blob/main/PRE_1.png" width="640" title="PRE6H1N">
</p>

--------------------------------------------------------------------------------------------------
                                                 DISCLAIMER

!! Attention : Working with mains power or high DC voltages, like in tube amps used,
serious injury might occur or death. Always make sure, the tube amp is free of any
remaining voltage/capacity stored in the circuit (capacitors). You need exactly to know,
what you do when working with mains or high voltages in general !

!! I do not give any warranty, that the information given here is free of errors.
Use these information on your own risk. 

--------------------------------------------------------------------------------------------------

I installed an arduino nano in my selfmade tube-preamp to monitor and control the voltages for 
the tubes. My preamp uses a 6H1N and an EAA91 for each channel. 

The best way to start a tube amp, is to preheat the tubes for a recent time before applying the
higher anode voltage. In my case, a voltage of around 90VDC is used as anode voltage.
Nowerdays (and I guess the future in general), tubes are getting more and more expensive and
sometimes almost impossible to get/buy (especially some kinds). If there is a way, to maximize
their lifespan, this project seems for me to be a logical way. So why not use a technology
that is simple, cheap and easy to build on your own ?

The Nano measures the voltages of 6.3VDC and the 90VDC in the first step after applying mains
power to the preamp. Within 5 seconds, both power supplies should stabilize. Are both voltages within
a certain range, preheating starts. In this time, a connected NeoPixel (WS2812B) LED indicates the
preheating process. After approximately 40 seconds, preheating is finished and the anode voltage
will be switched on and applied to the circuit via a relay.

From that point on, the sketch will measure continously the voltages and if one of them leaves
the predefined range, the automation will switch off heater and anode voltages.

The Neopixel LED indicates the actual status of the preamp. Additionally, a 20x4 Display can be 
connected to read the voltages and the status. If an error occurs like a voltage not in defined range,
a message is shown on the display telling you, which voltage fails and the corresponding value.

The 20x4 display sits in a 3D printed case, that can be mounted when needed. For this, I designed
an easy mount connection, that includes the 4-pin I2C connection. See the provided STL files within this
project. Ths U-shaped part will be fitted to the bottom of the preamp, so the other part can slide 
in and provide a safe connection beside a hold for the display. The rectangular space on both parts
give enough room for a 4-pin "Dupont" connector. On the display side male and the preamp-side female.

<p align="center">
  <img src="https://github.com/schnurlosdusche/arduino_tubeamp/blob/main/I2C_Displaymount_3.png" width="640" title="PRE6H1N">
</p>


In the meantime, I printed a few of these parts and they all fit very well and provide a strong and
simple connection in both ways, electrically and mechanically. There is just no possibility to 
connect the I2C bus in a wrong way... 

I do not provide the case for the display as .stl file, as there are different types of displays
and so, the build volume / size might differ from the one, I use. I simply glued my case for the 
display to the arm, so you can do, too. 

The only requirement regarding the display is the ability, to use an I2C connection for the
communication. You may adjust the code to use a 16x2 character display...

Future plans :

- Switching the pre out ports by relay. Shorten them to ground during preheating and in case of
an error that is detected by the arduino...

- Monitoring the current for the heater and defining the duration of the preheating process depending
on the current (current will be higher when tubes are "cold" and decreases during warm up)
The arduino nano can communicate with a Nodemcu (ESP8266) via the I2C bus and report the voltages 
and the current for long term statistics. I expect to see a slight change in the values over time...
The nodemcu could forward these information via MQTT to a server, where the values can be stored.
The only downside of using a nodemcu so near or inside a tube preamp is the radio frequency from the
WiFi/Bluetooth module. I guess, I have to test that... 

- Finding relays to handle high voltages of appr 360VDC for my other tubeamps... so they can also be 
monitored and controlled.

This project is still a work in progress...

----------------------------------------------------------------------------------------------------

More information will soon be available on my webiste : schnurlosdusche (dot) de

Feel free to get in contact with me : tubes (at) schnurlosdusche (dot) de
