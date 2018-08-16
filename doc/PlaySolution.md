## PlaySelection.ino

This example allows you to select media content and to call DFPlay library methods without any coding. 

Two Particle Functions are used:
1. The first Particle Function is named "Selection", and it permits you to select the media content that you wish to play.
2. The second Particle Function is named "Method", and it permits you to enter the DFPlay method name that you wish to call. 

When you bring up your device's Particle Console, the functions look like this:
**![](https://lh3.googleusercontent.com/0Gls6G3F1G4DW30lso9ujectEat7KesjveMFdjGr5D7qpZcT2lfeTZRZMc2Ux3VghcY_4MICUCccMsivwwdWSVT6WqFuvn6VBBCoCy8EL_M8CT6j1OnaZ2gNKcDPob68Oydo5HTV)**

### To use this example
* Flash your Particle device with the PlaySelection.ino code.
* Bring up your device's Particle Console
* In the "Selection" function:
	* Enter a Selection and click the CALL button.
	* If you're not sure what to enter, the following will play all of the tracks on the media:
		* Type 1 and click the CALL button if you have USB media
		* Type 2 and click the CALL button if you have SD media
		* or, Consult the DFPlay QuickStart Guide for more details about Selections. 
		
* In the "Method" function:
	* Type "play" (without quotes) and click the CALL button.
	* Type other method names, clicking the CALL button after each.

**Method Names:** (may be entered in upper, lower, or mixed case)
* "***play***",  "***stop***"
* "***pause***", " ***resume***"
* "***mute***", "***unmute***"
* "***repeat***", "***norepeat***"
* "***volumeUp***", "***volumeDown***"
* "***equalizerUp***", "***equalizerDown***"

While you play all of the tracks in a folder, or all of the tracks on the media, you can also use:
* "***skip***", to skip to the next track
* "***softStop***", to stop automatically when the current track ends

When LOGGING is enabled, this method can be used to display DFPlay's dialog with the DFPlayer on a Serial console. You will find information about how to enable  LOGGING in the DFPlay QuickStart Guide.


> Written with [StackEdit](https://stackedit.io/).