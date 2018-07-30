/* Module: DFPlaySelections.ino
        
    This example exposes two Particle functions via the Particle Console:
	
		The first function is named "Selection", and lets you:

		  *	select a single	track, all of the tracks in a folder, or all of the tracks on the media. 
          * optionally, adjust volume and equalizer settings
            
		The second function is named "dfCommand", and permits you to execute DFPlay methods via the
		following self-explanatory commands: (in upper, lower, or mixed case)

		  * "play", "pause", "stop", "next"
		  * "volumeUp", "volumeDown"
		  * "equalizerUp", "equalizerDown"
		  * "mute", "muteOff"
		  * "repeat", "repeatOff"

		  While you play all of the tracks in a folder, or all of the tracks on the media, you can
		  also use:
		  * "softStop" when you want play to stop when the current track ends

	PLAYING ALL OF THE TRACKS ON THE MEDIA:
		  * You can play any SD or USB media that contains .MP3 and/or .WAV file.
		  * Tracks may be stored anywhere on the media ... in the root, in nested folders, anywhere.
		  * Folder and track names are ignored, so anything goes.
		  * To play all of the tracks on USB media, set "Selection" to: 1
		  * To play all of the tracks on SD media, set :Selection" to: 2
		  
	PLAYING ALL OF THE TRACKS IN A FOLDER:
		  * Folders must be named "01" to "99", and all folders must be stored at the media root.
		  * Track names are ignored, so anything goes.
		  * to play all of the tracks in folder "7" on USB media, set "Selection" to: 1,7
		  * to play all of the tracks in folder "59" on SD media, set "Selection" to: 2,59
		  
	PLAYING INDIVIDUAL TRACKS:
	
		  * TRACKS LOCATED IN FOLDERS NAMED "01" to "15", or "MP3
			  * Track names must have a 4-digit file name prefix like: "0012 Tennessee Bird Walk.MP3"
			  * To play the above track located in folder 11 on USB media, set "Selection" to: 1,11,12
			  * To play the above track located in folder MP3 on SD media, set "Selection" to: 2,100,12 
			  
		  * TRACKS LOCATED IN FOLDERS NAMED "16" to "99"
			  * Track names must have a 3-digit file name prefix like: "044 Beans In Your Ears.WAV"
			  * To play the above track located in a folder 81 on SD media, set "Selection" to: 2,81,44
			  
	MORE ABOUT THE "SELECTION" OBJECT:
		Each Selection object contains 5 member variables. 
			1.  Selection.media identifies the media ... 1 = USB KEY, 2 = SD CARD
			2.  Selection.folder identifies a folder on the media ... 1 to 100, where 100 selects the folder named "MP3"
			3.  Selection.track identifies a specific track within a folder based on the file name prefix
					3-digit prefixes can range from 1 to 255
					4-digit prefixes can range from 1 to 4095
					For performance reasons, folders should not contain over 3000 tracks
			4.  Selection.volAdj specifies a track volume adjustment ... plus or minus volume increment 
			5.  Selection.equalizer specifies a track equalizer setting ... this overrides the default equalizer setting
			
		So, the initialization string for a Selection is {media,folder,track,volAdj,equalizer}
		
		This sample program accepts an initializer string like: {2,81,44,-3,5}. Braces are optional, and trailing member
		elements can be omitted if they are zero ... so {2,81,0,0,0} can be simply entered as: 2,81
*/


#include "DFPlay.h"
#include <vector>

    DFPlay dfPlay;
    Selection emptySelection = {0,0,0,0,0}; // <-- empty selection object for clearing Selection objects (as used on the next line) 
    Selection selection = emptySelection;   // <-- selection specified by the "setSelection" Particle Function ...
    
void setup() {
    Particle.function("Selection", setSelection);       // <-- this function updates the Selection object
    Particle.function("dfCommand",dfCommand);           // <-- this function executes DFPlay commands
    dfPlay.begin();           // <-- initializes the DFPlayer library
    dfPlay.setVolume(7);     // <-- sets the default volume level ... (which may be adjusted by each Selection.volAdj)
}

void loop() {
    dfPlay.manageDevice(); // <-- manages the DFPlayer device
}

//  -------------------------------------------------------------------------------- Use Particle Function to control play

// PARTICLE FUNCTION TO EXECUTE DFPLAYER COMMANDS
int dfCommand(String cmd) {
    if (cmd.equalsIgnoreCase("volumeUp"))   	return (dfPlay.volumeUp());
    if (cmd.equalsIgnoreCase("volumeDown"))   	return (dfPlay.volumeDown());
    if (cmd.equalsIgnoreCase("equalizerUp"))	return (dfPlay.equalizerUp());
    if (cmd.equalsIgnoreCase("equalizerDown"))	return (dfPlay.equalizerDown());
    if (cmd.equalsIgnoreCase("mute"))           { dfPlay.muteOn(); return 0; }
    if (cmd.equalsIgnoreCase("muteOff"))        { dfPlay.muteOff(); return 0; }
    if (cmd.equalsIgnoreCase("pause"))          { dfPlay.pause(); return 0; }
    if (cmd.equalsIgnoreCase("resume"))         { dfPlay.resume(); return 0; }
    if (cmd.equalsIgnoreCase("repeat"))         { dfPlay.repeatOn(); return 0; }
    if (cmd.equalsIgnoreCase("repeatOff"))      { dfPlay.repeatOff(); return 0; }
    if (cmd.equalsIgnoreCase("stop"))           { dfPlay.stop();  return 0; }
    if (cmd.equalsIgnoreCase("softStop"))       { dfPlay.softStop(); return 0; }
    if (cmd.equalsIgnoreCase("skip"))           { dfPlay.skip(); return 0; }
    if (cmd.equalsIgnoreCase("play"))           { dfPlay.play(selection); return 0; }
    return -1;
}


// PARTICLE FUNCTION TO SET the Selection object named "selection" ...
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
    return 0;
}
    
   









