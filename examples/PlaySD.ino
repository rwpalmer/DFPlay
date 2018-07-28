/* Module: PlaySD.ino
 *	If you have an SD card with .MP3 and/or .WAV content, this code will play it. 
 */

#include "DFPlay.h"


DFPlay dfPlay;
void setup() {
    dfPlay.begin();					// Prepares DFPlay for execution
    dfPlay.setVolume(15);			// Sets volume level to 10 (valid range = 0 to 30)
    Selection SDcard = {2,0,0,0,0}; // Selects all tracks on the SD card
    dfPlay.play(SDcard);			// Plays the selection
}

void loop() {
    dfPlay.manageDevice();			// Sends commands to DFPlayer & processes returned data. 
}












