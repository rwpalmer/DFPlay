/*
    Project:    DFPlay library
    File:       PlaySD.ino 
    Version:    0.0.9 - August 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   

  	If you have an SD card with .MP3 and/or .WAV content, this code will play all the tracks regardless of where
	the files are stored, or what they are named. 
		
	LINK TO MORE INFORMATION ABOUT THIS EXAMPLE:
			https://github.com/rwpalmer/DFPlay/blob/master/doc/PlaySD.md
 */

#include <DFPlay.h>


DFPlay dfPlay;
void setup() {
    Serial1.begin(9600);
    dfPlay.begin(Serial1);					// Prepares DFPlay for execution
    dfPlay.setVolume(15);			// Sets volume level to 10 (valid range = 0 to 30)
    Selection SDcard = {2,0,0,0,0}; // Selects all tracks on the SD card
    dfPlay.play(SDcard);			// Plays the selection
}

void loop() {
    dfPlay.manageDevice();			// Sends commands to DFPlayer & processes returned data. 
}



/* Sample DFPlay log file captured while running this program: Test Duration 3 hours, 43 minutes
Initialize
 Request: 7e ff 06 09 00 00 01 fe f1 ef 
 Request: 7e ff 06 09 00 00 02 fe f0 ef 
 Request: 7e ff 06 3f 00 00 00 fe bc ef 
Response: 7e ff 06 3f 00 00 03 fe b9 ef
Selection: {2,0,0,0,0}
setVolume
 Request: 7e ff 06 06 00 00 0a fe eb ef
Selection: {2,0,0,0,0}
Select Media
 Request: 7e ff 06 09 00 00 02 fe f0 ef
Selection: {2,0,0,0,0}
Query Track Count
 Request: 7e ff 06 48 00 00 00 fe b3 ef
Response: 7e ff 06 48 00 00 4e fe 65 ef
Selection: {2,0,0,0,0}
Play Media: {2,0,0,0,0} ... 78 Tracks
 Request: 7e ff 06 11 00 00 02 fe e8 ef
Response: 7e ff 06 3d 00 00 01 fe bd ef
Response: 7e ff 06 3d 00 00 02 fe bc ef
Response: 7e ff 06 3d 00 00 03 fe bb ef
Response: 7e ff 06 3d 00 00 04 fe ba ef
Response: 7e ff 06 3d 00 00 05 fe b9 ef
Response: 7e ff 06 3d 00 00 06 fe b8 ef
Response: 7e ff 06 3d 00 00 07 fe b7 ef
Response: 7e ff 06 3d 00 00 08 fe b6 ef
Response: 7e ff 06 3d 00 00 09 fe b5 ef
Response: 7e ff 06 3d 00 00 0a fe b4 ef
Response: 7e ff 06 3d 00 00 0b fe b3 ef
Response: 7e ff 06 3d 00 00 0c fe b2 ef
Response: 7e ff 06 3d 00 00 0d fe b1 ef
Response: 7e ff 06 3d 00 00 0e fe b0 ef
Response: 7e ff 06 3d 00 00 0f fe af ef
Response: 7e ff 06 3d 00 00 10 fe ae ef
Response: 7e ff 06 3d 00 00 11 fe ad ef
Response: 7e ff 06 3d 00 00 12 fe ac ef
Response: 7e ff 06 3d 00 00 13 fe ab ef
Response: 7e ff 06 3d 00 00 14 fe aa ef
Response: 7e ff 06 3d 00 00 15 fe a9 ef
Response: 7e ff 06 3d 00 00 16 fe a8 ef
Response: 7e ff 06 3d 00 00 17 fe a7 ef
Response: 7e ff 06 3d 00 00 18 fe a6 ef
Response: 7e ff 06 3d 00 00 19 fe a5 ef
Response: 7e ff 06 3d 00 00 1a fe a4 ef
Response: 7e ff 06 3d 00 00 1b fe a3 ef
Response: 7e ff 06 3d 00 00 1c fe a2 ef
Response: 7e ff 06 3d 00 00 1d fe a1 ef
Response: 7e ff 06 3d 00 00 1e fe a0 ef
Response: 7e ff 06 3d 00 00 1f fe 9f ef
Response: 7e ff 06 3d 00 00 20 fe 9e ef
Response: 7e ff 06 3d 00 00 21 fe 9d ef
Response: 7e ff 06 3d 00 00 22 fe 9c ef
Response: 7e ff 06 3d 00 00 23 fe 9b ef
Response: 7e ff 06 3d 00 00 24 fe 9a ef
Response: 7e ff 06 3d 00 00 25 fe 99 ef
Response: 7e ff 06 3d 00 00 26 fe 98 ef
Response: 7e ff 06 3d 00 00 27 fe 97 ef
Response: 7e ff 06 3d 00 00 28 fe 96 ef
Response: 7e ff 06 3d 00 00 29 fe 95 ef
Response: 7e ff 06 3d 00 00 2a fe 94 ef
Response: 7e ff 06 3d 00 00 2b fe 93 ef
Response: 7e ff 06 3d 00 00 2c fe 92 ef
Response: 7e ff 06 3d 00 00 2d fe 91 ef
Response: 7e ff 06 3d 00 00 2e fe 90 ef
Response: 7e ff 06 3d 00 00 2f fe 8f ef
Response: 7e ff 06 3d 00 00 30 fe 8e ef
Response: 7e ff 06 3d 00 00 31 fe 8d ef
Response: 7e ff 06 3d 00 00 32 fe 8c ef
Response: 7e ff 06 3d 00 00 33 fe 8b ef
Response: 7e ff 06 3d 00 00 34 fe 8a ef
Response: 7e ff 06 3d 00 00 35 fe 89 ef
Response: 7e ff 06 3d 00 00 36 fe 88 ef
Response: 7e ff 06 3d 00 00 37 fe 87 ef
Response: 7e ff 06 3d 00 00 38 fe 86 ef
Response: 7e ff 06 3d 00 00 39 fe 85 ef
Response: 7e ff 06 3d 00 00 3a fe 84 ef
Response: 7e ff 06 3d 00 00 3b fe 83 ef
Response: 7e ff 06 3d 00 00 3c fe 82 ef
Response: 7e ff 06 3d 00 00 3d fe 81 ef
Response: 7e ff 06 3d 00 00 3e fe 80 ef
Response: 7e ff 06 3d 00 00 3f fe 7f ef
Response: 7e ff 06 3d 00 00 40 fe 7e ef
Response: 7e ff 06 3d 00 00 41 fe 7d ef
Response: 7e ff 06 3d 00 00 42 fe 7c ef
Response: 7e ff 06 3d 00 00 43 fe 7b ef
Response: 7e ff 06 3d 00 00 44 fe 7a ef
Response: 7e ff 06 3d 00 00 45 fe 79 ef
Response: 7e ff 06 3d 00 00 46 fe 78 ef
Response: 7e ff 06 3d 00 00 47 fe 77 ef
Response: 7e ff 06 3d 00 00 48 fe 76 ef
Response: 7e ff 06 3d 00 00 49 fe 75 ef
Response: 7e ff 06 3d 00 00 4a fe 74 ef
Response: 7e ff 06 3d 00 00 4b fe 73 ef
Response: 7e ff 06 3d 00 00 4c fe 72 ef
Response: 7e ff 06 3d 00 00 4d fe 71 ef
Response: 7e ff 06 3d 00 00 4e fe 70 ef
Stop
 Request: 7e ff 06 16 00 00 00 fe e5 ef
Enter Sleep
 Request: 7e ff 06 0a 00 00 00 fe f1 ef
*/ 








