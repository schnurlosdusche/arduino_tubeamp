# arduino_tubeamp
control tube heater and anode supply with arduino nano

--------------------------------------------------------------------------------------------------
                                        DISCLAIMER
--------------------------------------------------------------------------------------------------

!! Attention : Working with mains power or high DC voltages, like in tube amps used,
serious injury might occur or death. Always make sure, the tube amp is free of any
remaining voltage/capacity stored in the circuit (capacitors). You need exactly to know,
what you do when working with mains or high voltages in general !

!! I do not give any warranty, that the information given here is free of errors.
Use these information on your own risk. 

--------------------------------------------------------------------------------------------------

I installed an arduino nano in my selfmade tube-preamp to monitor and control the voltages for 
the tubes.

The best way to start a tube amp, is to preheat the tubes for a recent time before applying the
higher anode voltage. In my case, a voltage of around 90VDC is used as anode voltage.

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
project.

This project is still a work in progress...

----------------------------------------------------------------------------------------------------

Feel free to get in contact with me : github (at) schnurlosdusche (dot) de
