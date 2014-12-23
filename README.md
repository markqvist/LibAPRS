LibAPRS
==========

LibAPRS is an Arduino IDE library that makes it easy to send and receive APRS packets with a [MicroModem](http://unsigned.io/micromodem)-compatible modem.

You can buy a complete modem from [my shop](http://unsigned.io/shop), or you can build one yourself pretty easily. Take a look at the documentation in the [MicroModem](https://github.com/markqvist/MicroModem) repository for information and getting started guides!

See the example included in the library for info on how to use it!

## Some features

- Send and receive AX.25 APRS packets
- Full modulation and demodulation in software
- Easy configuration of callsign and path settings
- Easily process incoming packets
- Shorthand functions for sending location updates and messages, so you don't need to manually create the packets
- Ability to send raw packets
- Support for settings APRS symbols
- Support for power/height/gain info in location updates
- Can run with open squelch

## Installation

Place the "LibAPRS" folder (the one in the same folder as this readme file) inside your Arduino "libraries" folder. That's all!

## Getting started

You should read through the "Basic_usage" example included with the library. It contains an explanation of all the functions and a basic sketch to get you up and running with sending and receiving packets.

## Got bugs?

This library is very early, and being actively developed all the time. This means you should expect to find bugs. If you do, please report them here, so I can fix them! It also means I might have to make changes that will break code, and that you will have to rewrite your sketch. If you don't think that sounds good, wait a little while for a stable release of the library :)