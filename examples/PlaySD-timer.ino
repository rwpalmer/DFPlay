/* Module: PlaySD-timer.ino
 *	If you have an SD card with .MP3 and/or .WAV content, this code will play it. 
 */

#include "DFPlay.h"

DFPlay dfPlay;
Timer timer(20,timerFunction);      // Runs dfPlay.manageDevice() every 20ms
void setup() {
    dfPlay.begin();					// Prepares DFPlay for execution
    timer.start();
    dfPlay.setVolume(7);			// Sets volume level to 10 (valid range = 0 to 30)
    Selection SDcard = {2,0,0,0,0}; // Selects all tracks on the SD card
    dfPlay.play(SDcard);			// Plays the selection
}

void loop() {
}

void timerFunction() {
    dfPlay.manageDevice();
}