/*
    Project:    DFPlay library
    File:       PlaySD-timer.ino 
    Version:    0.0.9 - August 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3  
    
    This is a modified version of PlaySD.ino. While PlaySD.ino runs manageDevice() in 
    loop(), this example executes the manageDevice() method from a timer. The timer
    reduces overhead by running manageDevice() less often. In this example, the timer
    is set to call manageDevice() every 20ms. 

    Using this example:
    If you have an SD card with .MP3 and/or .WAV content, this code will play all the tracks regardless of where
	the files are stored, or what they are named. 
		
	LINK TO MORE INFORMATION ABOUT THIS EXAMPLE:
			https://github.com/rwpalmer/DFPlay/blob/master/doc/PlaySD.md
 */

#include <DFPlay.h>

DFPlay dfPlay;
Timer timer(20,timerFunction);      // Runs dfPlay.manageDevice() every 20ms
void setup() {
    Serial1.begin(9600);
    dfPlay.begin(Serial1);					// Prepares DFPlay for execution
    timer.start();
    dfPlay.setVolume(10);			// Sets volume level to 10 (valid range = 0 to 30)
    Selection SDcard = {2,0,0,0,0}; // Selects all tracks on the SD card
    dfPlay.play(SDcard);			// Plays the selection
}

void loop() {
}

void timerFunction() {
    dfPlay.manageDevice();			// Sends requests to DFPlayer & processes responses. 
}







/* Sample DFPlay log file captured while running this program: Test Duration 1 hour, 26 minutes

    Note:   This log was captured during a tier3 test where the selection was changed from 
            {2,0,0,0,0} to {1,0,0,0,0} to select USB media instead of SD media. 
            

Initialize
 Request: 7e ff 06 09 00 00 01 fe f1 ef
 Request: 7e ff 06 09 00 00 02 fe f0 ef
 Request: 7e ff 06 3f 00 00 00 fe bc ef
Response: 7e ff 06 3f 00 00 03 fe b9 ef

Selection: {1,0,0,0,0}
setVolume
 Request: 7e ff 06 06 00 00 0a fe eb ef

Selection: {1,0,0,0,0}
Select Media
 Request: 7e ff 06 09 00 00 01 fe f1 ef

Selection: {1,0,0,0,0}
Query Track Count
 Request: 7e ff 06 47 00 00 00 fe b4 ef
Response: 7e ff 06 47 00 00 18 fe 9c ef

Selection: {1,0,0,0,0}
Play Media: {1,0,0,0,0} ... 24 Tracks
 Request: 7e ff 06 11 00 00 01 fe e9 ef
Response: 7e ff 06 3c 00 00 01 fe be ef
Response: 7e ff 06 3c 00 00 02 fe bd ef
Response: 7e ff 06 3c 00 00 03 fe bc ef
Response: 7e ff 06 3c 00 00 04 fe bb ef
Response: 7e ff 06 3c 00 00 05 fe ba ef
Response: 7e ff 06 3c 00 00 06 fe b9 ef
Response: 7e ff 06 3c 00 00 07 fe b8 ef
Response: 7e ff 06 3c 00 00 08 fe b7 ef
Response: 7e ff 06 3c 00 00 09 fe b6 ef
Response: 7e ff 06 3c 00 00 0a fe b5 ef
Response: 7e ff 06 3c 00 00 0b fe b4 ef
Response: 7e ff 06 3c 00 00 0c fe b3 ef
Response: 7e ff 06 3c 00 00 0d fe b2 ef
Response: 7e ff 06 3c 00 00 0e fe b1 ef
Response: 7e ff 06 3c 00 00 0f fe b0 ef
Response: 7e ff 06 3c 00 00 10 fe af ef
Response: 7e ff 06 3c 00 00 11 fe ae ef
Response: 7e ff 06 3c 00 00 12 fe ad ef
Response: 7e ff 06 3c 00 00 13 fe ac ef
Response: 7e ff 06 3c 00 00 14 fe ab ef
Response: 7e ff 06 3c 00 00 15 fe aa ef
Response: 7e ff 06 3c 00 00 16 fe a9 ef
Response: 7e ff 06 3c 00 00 17 fe a8 ef
Response: 7e ff 06 3c 00 00 18 fe a7 ef
Stop
 Request: 7e ff 06 16 00 00 00 fe e5 ef
Enter Sleep
 Request: 7e ff 06 0a 00 00 00 fe f1 ef
 
*/
