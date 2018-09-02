/*
    Project:    DFPlay library
    File:       PlaySelections.ino
    Version:    0.0.10 - September 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   

	This example allows you to select media content and to call DFPlay library methods without any coding. 

	Two Particle functions allow you to select media content and to enter DFPlay method names to play, pause, resume ... etc. 

	LINK TO  MORE INFORMATION ABOUT THIS EXAMPLE:
			https://github.com/rwpalmer/DFPlay/blob/master/doc/PlaySolution.md
*/

#include <DFPlay.h>
#include <vector>

    DFPlay dfPlay;
    Selection emptySelection = {0,0,0,0,0}; // <-- empty selection object for clearing Selection objects (as used on the next line) 
    Selection selection = emptySelection;   // <-- selection specified by the "setSelection" Particle Function ...
    
void setup() {
    Particle.function("Selection", setSelection);       // <-- this function updates the Selection object
    Particle.function("dfCommand",dfCommand);           // <-- this function executes DFPlay commands
    Serial1.begin(9600);
    dfPlay.begin(Serial1);           // <-- initializes the DFPlayer library
    dfPlay.setVolume(10);     // <-- sets the default volume level ... (which may be adjusted by each Selection.volAdj)
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
    if (cmd.equalsIgnoreCase("mute"))           { dfPlay.mute(); return 0; }
    if (cmd.equalsIgnoreCase("unmute"))         { dfPlay.unmute(); return 0; }
    if (cmd.equalsIgnoreCase("pause"))          { dfPlay.pause(); return 0; }
    if (cmd.equalsIgnoreCase("resume"))         { dfPlay.resume(); return 0; }
    if (cmd.equalsIgnoreCase("repeat"))         { dfPlay.repeat(); return 0; }
    if (cmd.equalsIgnoreCase("norepeat"))       { dfPlay.norepeat(); return 0; }
    if (cmd.equalsIgnoreCase("stop"))           { dfPlay.stop();  return 0; }
    if (cmd.equalsIgnoreCase("softStop"))       { dfPlay.softStop(); return 0; }
    if (cmd.equalsIgnoreCase("skip"))           { dfPlay.skip(); return 0; }
	if (cmd.equalsIgnoreCase("back"))			{ dfPlay.back(); return 0; }
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
    
   









