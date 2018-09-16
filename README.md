# ***DFPlay***
 ***Enhanced Library for DFPlayer / FN-M16P***

## *Overview*
DFPlayers are inexpensive and they have a serial interface. This makes them a useful resource for IOT projects.

While it is true that the DFPlayer can be used without a library, it is also true that a library can make the DFPlayer much easier to use. This library's methods mirror the buttons that you would find on a portable MP3 player.  Methods like"play()", "stop()", "pause()", "resume()", and "skip()"  control the DFPlayer and mask the complexity of DFPlayer's native command set.  DFPlay also provides enhanced functionality. For example: 
* DFPlay plays all content with one ***play(*** Selection ***)*** command, where "Selection":
	*  defines the content to be played
	* allows the volume setting to be adjusted for the duration of content play.  This addresses a common audio playback problem, where some content plays louder than other content. It also makes it simple to play some content (say an alarm) louder than other content.
	* allows the equalizer setting to be adjusted for the duration of content play
	* enables the use of playlists (demonstrated in the library's playList.ino example). 

* New mute() and unmute() functionality was added.
* New softStop() functionality tells DFPlayer to stop playing when the current track ends.
* Developers have Instant access to DFPlayer state information with methods like:  *isPlaying()*, *isPaused()*, *isIdle()*, *isMuted()*.
* Low-power "sleep" mode is automatically enabled after the device has been idle for 2 seconds, and seamlessly disabled when the next play()* method is called.

So DFPlay offers ease-of-use and enhanced functionality. In the following "Architecture" section, you will see how it can manage the DFPlayer with minimal impact on system resources. 

The final point in this "overview" is to mention that there are times when a library can not or should not be used. DFPlay's QuickStart Guide, PlaySolution.ino example, and LOGGING functions  were developed with the hope that they can assist those who use the library and those who are simply looking for to build their understanding of the DFPlayer and its native command set.  

## *Architecture*
As you can see in the following drawing, DFPlay has one method named **manageDevice()** that handles ALL communication with the DFPlayer device. Every other DFPlay method simply updates and/or returns "state variables" that reside in memory. These methods complete their assigned tasks and return instantly. 

![alt text](https://github.com/rwpalmer/DFPlay/blob/master/images/DFPlayArchitecture.png)


Each time manageDevice() is called, it asks two simple questions: 
1. Have I received any data from the DFPlayer?
2. Do I need to send a command to the DFPlayer?

The vast majority of the time (over 99.9% in testing), both answers are no, and manageDevice() simply exits. 

When the answer to the first question is yes, manageDevice() updates state variables based on the data it received from the DFPlayer. 

When the answer to the second question is yes, manageDevice() will perform the following tasks:

 1. exit immediately if it is too soon to send another command to the DFPlayer.
 2. evaluate state variables based on a set of rules which compare current and desired states to search for non-compliance.
 3. send a command to the DFPlayer to correct the first non-compliance found.
 4. exit immediately after the command is sent. 
 
If another non-compliance issues exists, each  will be corrected in a subsequent manageDevice() execution cycle. 

DFPlayer needs time to process each command it receives. Most commands complete within 30ms, but some take longer. When manageDevice() sends a command, it calculates when it will be safe to send the next command. That calculation is the basis for step 1 exiting in the above task list. 

DFPlay's design allows it to manage the DFPlayer with minimal impact to the host application, and gives developers the option of letting manageDevice() run in loop() or triggering it with a timer that can be started and stopped based on the applications need for the DFPlayer.  

## *Usage Information*
The DFPlay QuickStart Guide provides more detailed information about DFPlay commands, and the Selection object that is used to identify the content to be played on the DFPlayer:     [******DFPlay QuickStart*** Guide***](https://docs.google.com/document/d/e/2PACX-1vTxUyPOpk9RFMaxt53oPotWyAa5pTBVzpSS2L23bq2fGhUXK08vAFPSAWQ6gENLNFoum10IWmVFkJ7I/pub)
 
The DFPlay library is published with five sample applications.  All require a 
DFPlayer (like the FN-M16P / DFPlayer-Mini) wired to a Particle device and a speaker or earphones. Details are provided

#### PlaySD.ino and PlaySD-timer.ino
Insert any SD media that contains .mp3 or .wav files into the DFPlayer, flash the Particle device, and  listen to the music ... or whatever. 
[documentation](https://github.com/rwpalmer/DFPlay/blob/master/doc/PlaySD.md)

#### PlaySelection.ino
Use DFPlay library methods to control your DFPlayer ... without any coding. This example allows you to select media content and execute DFPlay methods via two Particle functions. 
[documentation](https://github.com/rwpalmer/DFPlay/blob/master/doc/PlaySolution.md)

#### PlayList.ino
Create a "playlist" and listen to your DFPlayer play it ... without any coding.  This example allows you to select media content and add it to a playlist. The playlist automatically starts playing when the first selection is added. Add as many selections as you like. 
[documentation](https://github.com/rwpalmer/DFPlay/blob/master/doc/PlayList.md)

#### DoorAlarm.ino
Monitor a virtual door. This example Plays one track when the door closes, and another track when the door opens, and replays it every five minutes while the door remains open. Door status is toggled via a Particle function.

In this case, the selected tracks are hard-coded, so you will need to modify the track selections to conform with your media.
[documentation](https://github.com/rwpalmer/DFPlay/blob/master/doc/DoorAlarm.md)

## *Project Status*
* v 0.0.9 (prerelease) was released for Particle field testing
* v 0.0.10
	- includes some enhancements and fixes
	- was released as a (one-time) Arduino port 
* v 1.0.0 is a work in progress ...  
	* will include additional enhancements and fixes
	* Particle release planned  for September 30, 2018
* v 1.1.0 is planned for early 2019
	* Plan to support FN-M10P ( DFPlayer-Micro )
	* Plan to support FN-S10P ( w/ flash memory & secondary amp)

The author does not plan to release additional Arduino versions, but would welcome the opportunity to work with anyone who is so inclined. 

Please feel free to explore, comment, experiment, test ... whatever. Please post any issues to GitHub.  The author will also field any questions you may have. 

> Written with [StackEdit](https://stackedit.io/).