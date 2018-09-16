/*
    Project:    DFPlay library
    File:       PlayList.ino 
    Version:    1,0,0  September 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3
    
  		DFPlay uses a "Selection" object to select the media content to play. A playlist is merely an array (or vector)  of "Selection" objects.  This example demonstrates how playlists can be defined and played using the DFPlay library. 

	    PlayList.ino creates an empty vector and allows you to populate the vector by entering selections via a Particle Function and the Particle Console. The DFPlayer starts to play after you enter the first selection. 
		
		LINK TO MORE INFORMATION ABOUT THIS EXAMPLE:
				https://github.com/rwpalmer/DFPlay/blob/master/doc/PlayList.md
*/
#include <DFPlay.h>
#include <vector>


    DFPlay dfPlay;
    std::vector<Selection> playlist;   
    uint16_t plx = 0; // <-- playList index                                                                        
    Selection emptySelection = {0,0,0,0,0}; // <-- empty selection object for clearing Selection objects (as used on the next line) 
    Selection selection = emptySelection;   // <-- selection specified by the "setSelection" Particle Function ...
    bool playFailureHandled = false;

void setup() {
    Particle.function("Selection", setSelection);       // <-- this function is used to enter Selection objects
    Serial1.begin(9600);
    dfPlay.begin(Serial1);           // <-- initializes the DFPlayer library
    dfPlay.setVolume(10);     // <-- sets the default volume level
}

void loop() {
    
    dfPlay.manageDevice(); // <-- manages the DFPlayer device
    
    // -------------------------------------------------  Decides what to play next based on Particle Function dfCommand() input
    if (dfPlay.isIdle()) {
        // if the last play failed, delete the Selection from the playlist
        if ((dfPlay.playFailure()) && (!playFailureHandled)) {
            playlist.erase(playlist.begin() + --plx);
            Serial.printf("                                         playFailure is true: Selection %d deleted\n\r", plx+1);
            playFailureHandled = true;
        }
        // Play the next Selection in the playlist
		if (plx < playlist.size()) {
			dfPlay.play(playlist[plx]);
			playFailureHandled = false;
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












