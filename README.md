



# ***DFPlay***
 ***An Enhanced Library for the DFPlayer***

## *Overview*
DFPlayers are inexpensive and they have a serial interface. This makes them ideal for use in IOT projects.

While it is true that the DFPlayer can be used without a library, it is also true that a library can make the DFPlayer much easier to use. This library's methods mirror the buttons that you would find on a portable MP3 player.  Methods like"play()", "stop()", "pause()", "resume()", and "skip()"  control the DFPlayer and mask the complexity of the DFPlayer's native command set.  DFPlay also provides enhanced functionality. For example: 
* DFPlay plays all content with one ***play(*** Selection ***)*** command, where "Selection":
	*  defines the content to be played
	* allows the volume setting to be adjusted for the duration of content play.  This addresses a common audio playback problem, where some content plays louder than other content. It also makes it simple to play some content (say an alarm) louder than other content.
	* allows the equalizer setting to be adjusted for the duration of content play
	* enables the use of playlists (demonstrated in the playList.ino example). 

* New mute() and unmute() functionality was added.
* New softStop functionality tells DFPlayer to stop playing when the current track ends.
* Developers have Instant access to DFPlayer state information with methods like:  *isPlaying()*, *isPaused()*, *isIdle()*, and *isMuted()*.
* Low-power "sleep" mode is automatically enabled after the device has been idle for 2 seconds, and seamlessly disabled when the next play()* method is called.

If you do decide to use the DFPlayer without a library, you may still benefit from the information displayed when you execute the playSelection.ino example with logging turned on. See the DFPlay QuickStart Guide for details. 

## *Architecture*
As you can see in the following drawing, DFPlay has one method named **manageDevice()** that handles ALL communication with the DFPlayer device. Every other DFPlay method simply updates and/or returns "state variables" that reside in memory. These methods complete their assigned tasks almost instantly. 
![alt text](https://github.com/rwpalmer/DFPlay/blob/master/images/DFPlayArchitecture.png)


Each time manageDevice() is called, it asks two simple questions: 
1. Have I received any data from the DFPlayer?
2. Do I need to send a command to the DFPlayer?

Over 99.9% of the time, both answers are no, and manageDevice() simply exits. 

When the answer to the first question is yes, manageDevice() updates state variables based on the data it received.

When the answer to the second question is yes, manageDevice() will perform the following tasks:

 1. exit immediately if it is too soon to send another command to the DFPlayer.
 2. evaluate state variables based on a set of rules which compare current and desired states to search for non-compliance.
 3. send a command to the DFPlayer to correct the first non-compliance found.
 4. exit immediately after the command is sent. 
 
If another non-compliance issues exists, each  will be corrected in a subsequent manageDevice() execution cycle. 

DFPlayer needs time to process each command it receives. Most commands complete within 30ms, but some take longer. When manageDevice() sends a command, it calculates when it will be safe to send the next command. That calculation is the basis for step 1 exiting in the above task list. 

All other DFPlay methods simply update state variables in memory, or return data based on state variables. All return instantly. 

## *Usage Information*
The DFPlay QuickStart Guide provides more detailed information about DFPlay commands, and the Selection object that is used to identify the content to be played on the DFPlayer:     [******DFPlay QuickStart*** Guide***](https://docs.google.com/document/d/e/2PACX-1vTxUyPOpk9RFMaxt53oPotWyAa5pTBVzpSS2L23bq2fGhUXK08vAFPSAWQ6gENLNFoum10IWmVFkJ7I/pub)
 
 There are four sample programs in the GitHub "examples" folder. 
#### PlaySD.ino
Plays all of the tracks on SD media without any programming. Simply compile the example, insert the SD media into the DFPlayer, and it will begin to play. If you don't have an SD ready, you can just rip a CD/DVD to the media and insert it into the DFPlayer. You do not need to reorganize the content, rename folders or files.

This is a great program for initial testing of the DFPlayer ... to make sure your wiring is correct.

#### PlaySD-timer.ino
A second version of  PlaySD.ino where the manageDevice() method is run via a timer, and not in loop().

##### For the remaining examples, folders and tracks need to be named based on DFPlayer requirements. For details, refer to the "*How DFPlay plays tracks, and Media Requirements*" topic within the DFPlay QuickStart Guide. 

#### PlaySelection.ino
Plays individual selections, where each selection can play individual tracks, folders, or everything on the media. Again, no programming is involved. Selections are entered via a Particle Function and the Particle Console.  

A second Particle Function that allows you to invoke DFPlay methods by entering commands like "play", "stop", "pause", "resume", etc. All of the commands are documented in comments within the .ino file. 

#### PlayList.ino
Allows you to create and play a "playlist" ... which is an array (or vector) of selections. Once again .,. no programming is required. You simply enter the selections via a Particle Function and the Particle Console. Activity is logged to the Serial Console. 

#### DoorAlarm.ino
Plays one track when the door closes, and another track when the door opens ... and every five minutes while the door is open. Door status is toggled via a Particle Function and the Particle Console. 

In this case, the selected tracks are hard-coded, so you will need to modify the track selections to conform with your media. 
## *Project Status*
The initial release of DFPlay is in final testing ... Planned release in September 2018
RC1 is posted. Please feel free to explore, comment, experiment, test ... whatever. Please post any issues to GitHub.  The author will also field any questions you may have. 

> Written with [StackEdit](https://stackedit.io/).
