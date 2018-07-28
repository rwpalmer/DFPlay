/* Module: DoorAlarm.ino
 *  With this example, you can toggle a door sensor by typing "open" or "closed" into the Particle Function via the Particle Console.
 *  DFPlay will play one track when the door opens ... and every 5 minutes while the door remains open.
 *  DFPlay will play a second track when the door closes.
 *  All toggles go through debounce logic, as if mechanical switches were being used. 
 */
 
#include <Particle.h>
#include "DFPlay.h"

const uint8_t UNKNOWN = 0, OPEN = 1, CLOSED = 2;
const uint32_t DEBOUNCE_MILLIS = 200;
uint8_t doorState = UNKNOWN, doorSensor = UNKNOWN;
uint32_t doorStateMillis = 0;
time_t doorStateTime = 0;

// SELECTION SETTINGS - For sound to play, these SETTINGS must match your media.                 <------------------- PLEASE NOTE
Selection doorOpen   = {2,16,1,0,0};    // <-- media 2 (SD), folder 16, track 1, default volume, default equalizer
Selection doorClosed = {2,16,2,0,0};    // <-- media 2 (SD), folder 16, track 2, default volume, default equalizer
DFPlay dfPlay;
Timer timer(300000, renotify); // renotify every five minutes


void setup() {
    Particle.function("DoorSensor",DoorSensor);
    Serial.begin(115200);
    dfPlay.begin();         // Prepare DFPlay for execution
    dfPlay.setVolume(10);   // Set DFPlayer Volume level
}

void loop() {
    // CHECK DOOR SENSOR, DEBOUNCE, AND PROCESS DOOR STATE CHANGES
    if (doorSensor == doorState) { // reset debounce cycle when sensor matches doorState
        doorStateMillis = 0;
    } else if (doorStateMillis == 0) { // start debounce cycle when a state change is first detected
        doorStateMillis = millis();
    } else if (millis() > (doorStateMillis + DEBOUNCE_MILLIS)) { // take action when debounce cycle expires
        // the door sensor has provided a constant reading for DEBOUNCE_MILLIS, so we accept the reading as a valid state change 
        doorState = doorSensor;
        doorStateMillis = 0;
        switch (doorState) {
        case OPEN:      dfPlay.play(doorOpen);   timer.start();   break;
        case CLOSED:    dfPlay.play(doorClosed);  timer.stop();   break;
        }
    }
    // HANDLE ALL COMMUNICATIONS WITH THE DFPLAYER
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
     dfPlay.play(doorOpen);
     return;
}



