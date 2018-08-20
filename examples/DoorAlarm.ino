/*
    Project:    DFPlay library
    File:       DoorAlarm.ino
    Version:    0.0.8 - August 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   

	With this example, you can toggle a door sensor by typing "open" or "closed" into the Particle Function via the Particle Console.
	DFPlay will play one track when the door opens ... and every 5 minutes while the door remains open.
	DFPlay will play a second track when the door closes.
	All toggles go through debounce logic, as if mechanical switches were being used. 
	
	LINK TO MORE INFORMATION ABOUT THIS EXAMPLE:
			https://github.com/rwpalmer/DFPlay/blob/master/doc/DoorAlarm.md
 */
 
#include <Particle.h>
#include "DFPlay.h"

const uint8_t UNKNOWN = 0, OPEN = 1, CLOSED = 2;
const uint32_t DEBOUNCE_MILLIS = 200;
uint8_t doorState = UNKNOWN, doorSensor = UNKNOWN;
uint32_t doorStateMillis = 0;
time_t doorStateTime = 0;

// SELECTION SETTINGS - For sound to play, these SETTINGS must match your media.                 <------------------- PLEASE NOTE
Selection doorOpen   = {2,16,1,0,0};    // <-- translates to --> media 2 (SD), folder 16, track 1, default volume, default equalizer
Selection doorClosed = {2,16,2,0,0};    // <-- translates to --> media 2 (SD), folder 16, track 2, default volume, default equalizer
DFPlay dfPlay;
Timer timer(300000, renotify); // call renotify function every five minutes  (5 minutes * 60 seconds * 1000 = 300000 milliseconds)


void setup() {
    Particle.function("DoorSensor",DoorSensor);
    Serial.begin(115200);
    dfPlay.begin();         // Prepare DFPlay for execution
    dfPlay.setVolume(10);   // Set DFPlayer Volume level
}

void loop() {
 
 // CHECK DOOR SENSOR, DEBOUNCE, AND PROCESS DOOR STATE CHANGES
    if (doorSensor == doorState) { // reset debounce cycle any time the sensor matches doorState
        doorStateMillis = 0;
    } else if (doorStateMillis == 0) { // start debounce cycle any time a new state change is detected
        doorStateMillis = millis();
    } else if (millis() > (doorStateMillis + DEBOUNCE_MILLIS)) { // act when a new steady state has been achieved
        doorStateMillis = 0;
        doorState = doorSensor;
        switch (doorState) {
        case OPEN:      	dfPlay.play(doorOpen);   					// <-- DOOROPEN SOUND PLAYED
							timer.start(); 
							break;        
 
		case CLOSED:    	dfPlay.play(doorClosed);  					// <-- DOORCLOSED SOUND PLAYED
							timer.stop();   
							break;        
        }
    }  // exit Door Sensor logic
	
    // handle all communications with the DFPlayer
    dfPlay.manageDevice();
}

// PARTICLE FUNCTION
int DoorSensor(String s) {
    if (s.equalsIgnoreCase("open"))             { doorSensor = OPEN;
    } else if (s.equalsIgnoreCase("closed"))    { doorSensor = CLOSED;
    }
    return doorSensor;
}

// TIMER FUNCTION
void renotify(void) {
     dfPlay.play(doorOpen);                                                     // <-- DOOROPEN SOUND REPLAYED BY TIMER
     return;
}



