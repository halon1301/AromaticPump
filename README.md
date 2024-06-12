# M5 Blowover Pump

## This device may be used in ways which can be unsafe.  I take no responsibility for unsafe usage, be smart and think with the head on your shoulders, not between your legs.
## DO NOT USE THIS DEVICE ALONE
## DO NOT USE WITH VOLATILE COMPOUNDS
## DO NOT USE WHILE INTOXICATED

Now that I've got the safety stuff done, let's get on to the fun stuff.

### Full BOM

This BOM assumes you're using the provided circuit board designs, if you're going to use strip/protoboard to build this, 
you'll need to account for the differences



| Count | Part                                         | Supplier            | Link                                                                              |
|-------|----------------------------------------------|---------------------|-----------------------------------------------------------------------------------|
| 1     | Main Circuit Board                           | PCBWay              | n/a                                                                               |
| 1     | M5Stack Core 2                               | 	DigiKey            | https://www.digikey.ca/en/products/detail/m5stack-technology-co-ltd/K010/13151126 |
| 1     | Simple RF M4 315Mhz Momentary	               | Adafruit            | https://www.adafruit.com/product/1096                                             |
| 1     | 2 Button Key Fob	                            | Adafruit	           | https://www.adafruit.com/product/1391                                             |
| 1     | 4 Button Key Fob                             | 	Adafruit	          | https://www.adafruit.com/product/1095                                             |
| 1     | 12V Pump	                                    | Amazon	             | https://a.co/d/cortGed                                                            |
| 1     | 12V 2A Power Adapter	                        | Amazon	             | https://a.co/d/3SDBTkm                                                            |
| 1     | DRV8871 DC Motor Driver(Adafruit Brand)	     | Amazon or Adafruit	 | https://www.adafruit.com/product/3190                                             |
| 1     | 7-24V to 5V Buck Converter ( RB-Dfr-956)     | RobotShop	          | https://www.robotshop.com/products/dfrobot-dc-dc-buck-converter-7-24v-to-5v-4a    |
| 1     | Barrel Plug Adapter 5.5mm                    | Amazon              | https://a.co/d/0rlC4Ci                                                            |
| 3     | 10kOhm Resistors                             | Digikey             | https://www.digikey.ca/en/products/detail/yageo/CFR-25JB-52-10K/338               |
| 3     | 22kOhm Resistors                             | DigiKey             | https://www.digikey.ca/en/products/detail/yageo/CFR-25JB-52-22K/1306              |
| 1     | Some kind of physical latching on/off switch | Amazon              | https://a.co/d/hDef00k                                                            |
| 1     | Silicone Tubing                              | Amazon              | https://a.co/d/bUvRTfo                                                            |                                                          |
| 2     | Barb Through fitting                         | Amazon              | https://a.co/d/0NNg51N                                                            |
| 2     | FPC 30 Pin boards                            | Amazon              | https://a.co/d/jerJJ3a                                                                                  |
| 1| 30 Pin ribbon cable | Amazon | https://a.co/d/dCT5ZlU                                                            |

The 4 button fob is entirely optional, as it does the same things the touchscreen on the m5stack already does.  The 2 
button remote is for a sub to use, if you don't want this, consider it optional. Only button 2 is used on the 2 button 
remote (I use button 1 for a lube pump, so it's not wired up).

There are included PCB diagrams made in KiCAD, you can get them printed with PCBWay, but this whole project could easily
be made using proto/strip board and wires.

STLs for a case are also included, these however assume you are using the PCB included with the project, please remix 
and create your own for your use case!

### Assembly
All components are through hole, this is to keep it easy for people to make on their own with basic soldering skills, 
and without requiring special PCBs or other things that may be slightly more esoteric.

The designed PCB uses standard expansion pins for most things, the connectors for the switch and the 12v to the motor 
driver are designed to use a screw block, but you really could just solder wires into the hole and call it done.  Mounting 
points on the board are designed to use M3 screws, I'd recommend a nylon washer between the screw and the board, if you 
use inserts in your case, you should probably put something on the other side, but it's mostly because I'm paranoid 
about shorts.  In my design I use a pair of breakout boards and a 30p ribbon cable to lift the M5 stack off the board 
and give me nice clearance, once again, these are optional, you could do this however you want.

If your using the provided PCB design, you'll want to use some rosin flux, it'll make your life easier.  Solder 
everything to the board where labeled.

Don't connect your M5Stack to the board when you flash it, the USB 5v is not isolated from the 5v bus, and you'll get 
weird results, or fuck your computer up.

Bring up platformIO in your IDE of choice, and run the build, then upload to your board and you are good to plug it in. 
Power up the pump, and you're ready to go.