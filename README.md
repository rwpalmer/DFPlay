# ***DFPlay***
## *An Enhanced Library for the DFPlayer*

### *Overview*
DFPlayers are inexpensive and they have a serial interface. This makes them ideal for use in IOT projects.

While it is true that the DFPlayer can be used without a library, it is also true that a library can make the DFPlayer much easier to use. This library lets developers use intuitive commands to control the DFPlayer. *If you have ever used an MP3 player, you already know how to use the DFPlay library* ... but DFPlay also provides enhanced functionality. For example: 
* DFPlay uses one "play()" command. That command plays a "Selection" which identifies the content to be played, and permits a volume adjustment and equalizer setting to be customized for the the selected content. Selections can also be organized into playlists for those who want to take things to that level.  
* Mute and unMute functionality was added.
* New softStop functionality tells DFPlayer to stop playing when the current track ends 
* Developers have Instant access to DFPlayer state information with methods like:  *isPlaying()*, *isPaused()*, *isIdle()*, and *isMuted()*
* DFPlay seamlessly shifts the DFPlayer  to low-power "sleep" state when the device has been idle for 2 seconds, and seamlessly shifts back to the normal power state when the next play()* method is called
### *Architecture*
##### DFPlay provides high-performance device control, but consumes minimal resources.
DFPlay manages the DFPlayer based on ***state*** variables which are stored in memory. Some of these variables describe a "desired state" , and some describe the "current state" of the DFPlayer.

DFPlay has one method ***manageDevice()*** that handles ALL of the communication with the DFPlayer. Each time it runs, it does the following:
* Updates state variables based on data received from the DFPlayer
* Evaluates the state variables based on a set of rules which are designed to:
	* Exit as quickly as possible when DFPlayer current state is incompliance with the desired state, or if it is too soon to send another command to the DFPlayer. 
	* Send one command to the DFPlayer and exit as soon as non-compliance has been detected. 

When multiple commands are required to bring DFPlayer in compliance, manageDevice() nudges the DFPlayer into compliance each time it is executed in loop() or by a software timer. The code path taken to perform each nudge is short, and the DFPlayer is not very chatty, so manageDevice() has very little impact on applications that use this library.

DFPlay has many other methods. These methods merely update or return state variables in memory, so all return instantly. 

Please follow these links for additional information:

 - DFPlay QuickStart Guide
 - DFPlay Code Samples
 - DFPlay Usage Examples
