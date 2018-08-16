## PlayList.ino

DFPlay uses a "Selection" object to select the media content to play. A playlist is merely an array (or vector)  of "Selection" objects.  This example demonstrates how playlists can be defined and played using the DFPlay library. 

PlayList.ino creates an empty vector and allows you to populate the vector by entering selections into a Particle function that looks like this on the Particle Console:

![](https://github.com/rwpalmer/DFPlay/blob/master/images/PlayListFunctions.png)

The DFPlayer will start playing after you enter the first selection. Additional selections may then be added while the DFPlayer is playing. Each new selection will be added to the end of the playlist, and each will play in turn. When the end of the playlist is reached, Playlist.ino will play it again. 

### Prerequisites:
USB or SD Media with file and folder names that conform to DFPlayer requirements as documented in the DFPlay QuickStart Guide.

Unless you have memorized the track locations that you want to play, you might want to jot them down before you go further. Three or four selections is enough to get a sense for how this works.  Don't get carried away ... because the list will be lost when the Particle device reboots. 

If you want a longer run-time, remember you can include track selections, folder selections and even media selections within the playlist. If you have both SD and USB media attached, you can enter selections from either or both. 

Usage:

1. Flash the Particle Device   
2. Bring up a serial console if you wish to see the log that is created.   
3. Enter selections (details below)
4. Listen

### Selections:
This section provides a list of selection objects and describes what they select. Hopefully, this will be enough to get you going. For more detail, please consult the DFPlay QuickStart Guide. 

The following selections are presented as you might see them initialized in code:
(Initialization string members are: **{media, folder, track, volAdj, equalizer}**) 
```
     Selection USB ={1,0,0,0,0}; // selects all tracks on the USB media
     Selection SD ={2,0,0,0,0}; // selects all tracks on the SD media
     Selection SD21 ={2,21,0,0,0}; // selects all tracks in folder 21 on the SD
     Selection SD214 ={2,21,4,0,0}; // selects track 4 in folder 21 on the SD
     Selection SD214a ={2,21,4,+3,0}; // same selection played at default_volume + 3
     Selection SD214a ={2,21,4,-2,0}; // same selection played at default_volume - 2
     Selection SD214b ={2,21,4,0,5}; // same selection played with equalizer 5 (bass)
```
When volAdj and equalizer are zero, DFPlay uses the default volume level and equalizer. 

### Entering Selections into the Particle Function:
The Particle function used in this example accepts an initialization string with the following caveats:
* braces are optional
* trailing members with a zero value can be omitted

**Therefore:**
{1,0,0,0,0} can be entered as: 1
{2,0,0,0,0} can be entered as: 2
{2,21,0,0,0} can be entered as: 2,21
{2,21,4,0,0} can be entered as: 2,21,4
{2,21,4,+3,0} can be entered as: 2,21,4,3
{2,21,4,-2,0} can be entered as: 2,21,4,-2
{2,21,4,0,5} can be entered as: 2,21,4,0,5	

> Written with [StackEdit](https://stackedit.io/).