# Stepper Motors

Table of Contents
- [Stepper Motors](#stepper-motors)
  - [Part 0 - Background](#part-0---background)
    - [Motor](#motor)
    - [Driver](#driver)
  - [Part 1 - Wiring Everything Up](#part-1---wiring-everything-up)
  - [Part 2 - Driving the stepper motor](#part-2---driving-the-stepper-motor)
  - [Part 3 - Driving the stepper motor with interrupts](#part-3---driving-the-stepper-motor-with-interrupts)
  - [Part 4 - other stuff](#part-4---other-stuff)
  - [todo:](#todo)

## Part 0 - Background

### Motor
Today we're working with a NEMA 23 stepper motor. NEMA is an association which creates standards. The NEMA 23 standard means that our motor is approximately 2.3 inches wide. The larger the stepper motor, the more torque and more current we need to power it. Our motor has a max torque of 1.26 Nm. Interesting fact: there are NEMA 54 stepper motors out there. These are a lot bigger in person than they look in pictures. These can produce 36 Nm of force.

You might notice that your motor has four wires coming out of it. As you might have guessed, these are how we control our motor.

<img src="images/NEMA23_wires.png" alt="Nema 23 Wires" width="200"/>

Internally, these wires are connected up to coils (A1, A2, B1, B2 are our wires) which surround a magnet:

<img src="images/stepper_coils.png" alt="stepper coils" width="200"/>

So if we run current from wire A1 towards wire A2, through the coil that connects the two, then we create a magnetic field that may, we'll attract the north end of the magnet towards the coil.

And if we drive current from A2 to A1, in the opposite direction, we'll attract the south end of the magnet towards the coil. 

Similarity, if we run current through the B1/B2 coil, we'll attract / repel the magnet. 

Here's a descriptive image:
<img src="images/stepper_coils_steps.png" alt="stepper coils" width="500"/>

As you can see in the above image, if we drive wires A1, A2, A3 and A4 in the right order, we can cause the magnet inside to spin! Furthermore, this internal magnet is connected up to a shaft. So spinning the magnet = spinning the motor.

A cool physics thing that you can try out right now (which idk how works--one of our physics legends can probably explain :)) is if you press the wire ends of the right two wires (find w/ trial and error) and then try to spin the motor shaft, you'll feel some resistance and maybe clicking!

Our motor has 200 steps in a full revolution.

### Driver
So connect up four gpio pins to the four wires coming out of the stepper and we're set, right? Unfortunately, no. Our pins output a max of 16 mA of current at 3.3v which isn't nearly enough for our stepper. Instead, we need external hardware. 

You might imagine that we can hook up some transistors to the Pi and go from there. That's a perfectly valid approach. In fact, a dual h Bridge packages up these transistors up for us into a single easy to use board. However, most cheap dual h bridges that I found weren't rated for high enough current for our stepper, and would get unbearably hot. 

Another way to drive the stepper is to use a dedicated stepper motor driver. In this lab, we'll use the a4988 for this. The a4988 abstracts away a lot of work for us--instead of having to control wires A1/A2 and B1/B2 directly, the a4988 will take care of this. **With the a4988, we just use the DIR and STEP pins. DIR changes the direction of stepping, and STEP steps the motor forward by one step.**

## Part 1 - Wiring Everything Up
<img src="images/a4988_pinout.png" alt="stepper coils" width="500"/>

This image describes how to wire everything up. Note: ignore the capacitor (weird thing labelled 100 uF)

As a step by step guide, connecting directly to the a4988 (you can also use a breadboard if you want):
1. With your power supply unplugged, Take two male to female jumper wires and stick the male ends into the power supply, then screw them in. 
  <img src="images/power_supply_wires.JPG" alt="stepper coils" width="200"/>
2. With the power supply still unplugged, connect the female ends of the jumper wires to VMOT and the adjacent GND. 
3. Connect RESET and SLEEP together on the a4988 with a female to female jumper
4. With the Pi off, connect to GPIO outputs to STEP and DIR pins. 
5. With the Pi off, connect 3v3 and a ground pin to VDD and the adjacent GND on the a4988 respectively
6. Connect the stepper motor's wires to A1/A2/B1/B2
   1. You may find it useful to solder male dupont connector ends to the motor's wires. Highly recommend.
   2. It does matter which wire goes to which pin, to a certain degree
   3. The key is to find wire "pairings" and then ensure you connect each pair to a pin with the same letter (i.e. if I find that the green and red wire are a pair, then I should connect the green wire to either A1 or A2 and then the red wire to the other).
   4. You can find wire pairings with trial and error. Take any two wires, then touch their metal ends together. While their ends are held together, try spinning the motor shaft. If you feel more resistance than if the wire ends aren't contacted then you've found your pair. Otherwise, try one of the other two possible pairings. 

## Part 2 - Driving the stepper motor
This should be pretty easy, assuming everything is wired up correctly. All we need to do is write a 0 or a 1 to DIR, depending on which direction we want to run the motor in. And then we need to write a 1 to STEP to step.

You can now write a nice interface to step the motor and change directions. Some starter code that you can modify add to or ignore as you'd like is provided in `stepper.c`. Also some music data for the national anthem is provided in `national_anthem.c`. To play it, you'll have to build your own "play notes" function. 

Some general notes:
- Our motor has 200 steps per full revolution.
- To do multiple motor steps, you'll need to write a 0 to STEP between any two 1-writes to STEP. 
- You'll need to delay for some amount of time between any two motor steps, otherwise you'll "skip steps." You should find out what the min delay time you can have between any two motor steps, before your motor begins to skip. 
  - Challenge: try to spin the motor as fast as you can! Try adding accelerations with well timed delays to get even more speed. 
- You should keep track of the motor's position (based on the number of steps you've stepped it and the direction you stepped it in).
- It may be fun to write functions which step the motor for a certain number of steps, or until it's reached a certain number of steps.
- You can also write functions to step the motor at a specific speed or frequency, and then generate tones from there (you can download a tuner app on your phone to find precise frequencies, and then use ratios and stuff to extrapolate more notes/pitches from that single, tuned note).

## Part 3 - Driving the stepper motor with interrupts

Telling the motor explicitly to step is annoying. Let's use interrupts so that we can just tell the stepper to run at a speed, and let the timer interrupts deal with actually stepping the motor! The main gist is that we want to step the motor either forward or backward at a certain time, based on whatever speed we set the motor to run at. There's some basic starter code provided for this. But as usual, you can do this however you'd like. Feel free to modify, add or ignore the starter code. 

Notes:
- Right now, the timer interrupts are set to occur every 100 usec. I found that this time quanta gave me enough resolution for the motor, but feel free to change it however you'd like.
- One idea is to add on a Queue field (see `Q.h` from 140e) to the `stepper_t` struct and then create the ability to enqueue positions (and maybe speeds / times for those positions), and then modify the interrupt handler so that it reads from this queue and steps the motor accordingly. 
- You may find division and mod functions useful. I've provided a mod function which uses binary long division in the starter code. 

## Part 4 - other stuff
incorporate microphone, hall effect sensor, gyro/accel, other sensors to make something cool?

## todo:
- part 4  
- microstepping?  
- acceleration?  