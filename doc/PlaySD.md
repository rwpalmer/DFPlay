## PlaySD.ino and PlaySD-timer.ino

**PlaySD.ino** is an extremely simple  example of an application using the DFPlay library. Once the Particle device boots, it simply tells the DFPlayer to play all of the mp3 and wav on the SD media.

If you don't have an SD ready, you can just copy mp3 or wav files to an SD, or rip an audio CD to the media and insert it into the DFPlayer. You do not need to reorganize the content or rename folders and files to play "all of the tracks on the media" on a DFPlayer.

**PlaySD-timer.ino** provides identical functionality, but the code demonstrates how the DFPlay manageDevice() method can be run via a timer ... instead of running it within the firmware loop().

When manageDevice() is run by a timer, manageDevice() can be run less often. The timer can also be started and stopped based on an application's need to use the DFPlayer.  Recommendation: Always run DFPlay's begin() method after a timer start to make sure that the DFPlay library has a fresh understanding of the DFPlayer's current state before it starts issuing commands to the device. 

*Either of these applications would be great for  initial testing to make sure DFPlayer wiring is correct.*

**Usage:**
1.  Insert media that contains at least one mp3 or wav file.
2.  Flash your Particle device’s firmware.  
3.  Listen.  
      
-   To stop play, unseat and reseat the media.
-   To restart play, restart the Particle device
    
-   To adjust volume change the following line in firmware setup() and reflash the Particle device. ( Valid settings are 0 to 30 )
```
		dfPlay.setVolume(10);
```
		
		
- To use USB media instead of SD media, just change "2" to "1" in the following line of code:
```
		Selection SDcard = {2,0,0,0,0};
```
> Written with [StackEdit](https://stackedit.io/).