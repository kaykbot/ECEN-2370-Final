# ECEN-2370-Final-Project
EFM32PPG12 + Si7021 Temp &amp; Humidity Sensor + HM10 BLE module

Author: Kay Sho

/////////////////////////////////////////////////////////////////////

Hello!

Thanks for stopping by to take a look at my project! If you're a student in the same course or a similar course working on a similar project, PLEASE continue reading. I promise, this won't take long.


If you're a student: PLEASE don't copy my work. You'll be violating your school's honor code, yes, but you'll be doing something much much worse than that: Robbing yourself of some invaluable, impressive skills as an engineer. While the code you end up with may be very similar in the end, you won't learn anything if you just copy and turn it in. Doing so would hinder your abilities to become a good engineer. Understanding the code behind this project will teach you many skills that you'll find very useful in your engineering career. 

I've done my best to comment my code, but you should still understand why I wrote the code the way I did, and try to improve upon it; make it your own code. I've gone through my code multiple times, but who knows. Maybe my code is choke full of errors and fragile connections, so your goal should be to write your code yourself.


This project utilizes the versatile funtionality of the EFM32 Pearl Gecko and its onboard Si7021 temperature and humidity sensor to read the temperature of the room, convert it to Celcius, and send it through an external BLE device (in my case, the HM10) and into a bluetooth app for you to read. This project utilizes the UART, I2C, and LETIMER peripherals on the PG. Unfortunately, I didn't have time to set up the SPI interface on this. Due to some mishaps, I also don't have the Doxygen for the I2C created. Whenever I set up my Doxygen again, and figure out why Simplicity Studios won't read my project files, I will be sure to upload it ASAP.


The EFM32PG12 has a ton of various functionality. This development board is extremely versatile. You could use it for almost anything you'd like. This could just be my inexperience showing, but I'm sure you could program it like an Arduino or a Raspberry Pi in case you ever get bored of using them and want a bit more of a challenge. If you're a new hobbyist or a student, I highly encourage reading and utilizing the Pearl Gecko's API Documentation, which I have linked below.

https://docs.silabs.com/gecko-platform/latest/emlib/api/efm32xg12/modules

Also helpful was the Datasheets and Reference Manuals for not just the EFM32PG12, but also the HM10 BLE module and the Si7021 Temp & Humidity Sensor.

Thank you for actually reading the README, and happy engineering!
