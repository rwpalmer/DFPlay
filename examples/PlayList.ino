/*
    Project:    DFPlay library
    File:       PlayList.ino 
    Version:    0.0.4  August 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3
    
    Description:  
		Demonstrates how playlists can be defined and played with the DFPlay libray & logs to Serial console.
	
		DFPlay uses "Selection Objects" which can be instantiated and defined with commands like:
			Selection SD =   	{2,0,0,0,0};    // selects all of the tracks on the SD media
			Selection SD21 = 	{2,21,0,0,0};   // selects all of the tracks in folder 21 on the SD media
			Selection SD21-4 =	{2,24,4,0,0];   // selects track 4 in folder 21 on the SD media 
			
		As you can see each Selection object has 5 members ...
			The initialization string members are: {media, folder, track, volAdj, equalizer}. When the
			last two members are zero, DFPlay uses the standard volume level and equalizer setting. For
			more details, see the DFPlay Quick Start Guide. 

		A playlist is simply an array or a vector of Selection objects. This sample program selects a
		vector which allows us to add new Selections on-the-fly, via the Particle Console.
		
		A vector can be predefined ... like the following:
		std::vector<Selection> playlist  =  {{2,12,2,+1,0},{2,15,1,0,0},{2,100,1,0,0},{2,12,5,0,0},{2,15,6,+2,3},        
											{2,12,33,0,0},{2,12,49,0,5},{2,12,44,0,0},{2,12,54,0,0},{2,12,55,0,0},      
											{2,12,34,0,0},{2,15,3,0,5},{2,12,41,0,0},{2,12,18,0,0},{2,12,19,0,0},       
											{2,12,18,0,0},{2,12,28,0,0},{2,12,38,0,0},{2,12,48,-1,0},{2,15,7,0,0}};     

		but this sample program starts with an empty vector and has you enter the selections via the Particle
		Console ... since you know where the tracks are on your media ... hopefully. If not, you'll need to find
		out before going further.
		
		This firmware's Particle Function  accepts Solution entry as an initializer string like: {2,81,44,-3,5}, but
		Braces are optional, and trailing member elements can be omitted if they are zero ... 
		so {2,81,0,0,0} can be simply entered as: 2,81 and {2,81,3,0,0} can be entered as: 2,81,3

		
		Also, please make sure your media conforms to DFPlayer folder and file naming standards.
		
			Folders must be named "01" to "99". A folder named "MP3" is also allowed, but DFPlayer can't play it
			as a folder ... meaning there is no DFPlayer command to play "all of the tracks" within the MP3 folder.
			To play tracks within the MP3 folder, select folder 100.
			
			To select individual tracks within Folders "01" to "15" and "MP3", file names must start with a 4-digit
			file name prefix in the range of "0001" to "4095". A name like "0884 Tennessee Bird Walk.mp3" is valid.
			
			To select individual tracks within folders "16" to "99", file names must start with a 3-digit file name 
			prefix in the range of "001" to "255". A name like "002 Beans In My Ears.wav" is valid. 
*/

#include "DFPlay.h"
#include <vector>


    DFPlay dfPlay;
    std::vector<Selection> playlist;   
    uint16_t plx = 0; // <-- playList index                                                                        
    Selection emptySelection = {0,0,0,0,0}; // <-- empty selection object for clearing Selection objects (as used on the next line) 
    Selection selection = emptySelection;   // <-- selection specified by the "setSelection" Particle Function ...


void setup() {
    Particle.function("Selection", setSelection);       // <-- this function is used to enter Selection objects

    dfPlay.begin();           // <-- initializes the DFPlayer library
    dfPlay.setVolume(10);     // <-- sets the default volume level
}

void loop() {
    
    dfPlay.manageDevice(); // <-- manages the DFPlayer device
    
    // -------------------------------------------------  Decides what to play next based on Particle Function dfCommand() input
    if (dfPlay.isIdle()) {
		if (plx < playlist.size()) {
			dfPlay.play(playlist[plx]);
			Serial.printf("\n\r---------------------------------------- Playing Selection %d:{%d,%d,%d,%d,%d}\n\r", plx+1,
				playlist[plx].media,playlist[plx].folder, playlist[plx].track, playlist[plx].volAdj, playlist[plx].equalizer); 
			plx++;
		} else {
		    plx = 0;
        }
    }
}

// PARTICLE FUNCTION TO ENTER SELECTION OBJECTS ...
int setSelection(String sel) {
    selection = emptySelection;
    int i = sel.indexOf('{') + 1;               // <--- Braces are optional ...  White space is ignored
    selection.media = atoi(&sel[i]);            //      Trailing fields can be omitted if zero 
    i = sel.indexOf(',',i);                     //      For example: "2,1,0,0,0" and "2,1" both play media 2, folder 1 
    if (i > -1) { i++; 
        selection.folder = atoi(&sel[i]);
        i = sel.indexOf(',',i);
        if (i > -1) { i++;
            selection.track = atoi(&sel[i]);
            i = sel.indexOf(',',i);
            if (i > -1) { i++;
                selection.volAdj = atoi(&sel[i]);
                i = sel.indexOf(',',i);
                if (i > -1) {i++;
                    selection.equalizer = atoi(&sel[i]);
                }
            }
        }
    }
	playlist.push_back(selection); // add the selection to the end of the playlist
	Serial.printf("Playlist Selection %d = {%d, %d, %d, %d, %d}\r\n", playlist.size(), selection.media, 
	    selection.folder, selection.track, selection.volAdj, selection.equalizer);
    return 0;
}












