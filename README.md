
# ***DFPlay***
## *An Enhanced Library for the DFPlayer*

### *Overview*
DFPlayers are inexpensive and they have a serial interface. This makes them ideal for use in IOT projects.

While it is true that the DFPlayer can be used without a library, it is also true that a library can make the DFPlayer much easier to use. This library lets developers use intuitive commands to control the DFPlayer. *If you have ever used an MP3 player, you already know how to use the DFPlay library*. DFPlay also provides enhanced functionality. For example: 
* DFPlay plays all content with one ***play(*** Selecton ***)*** command, where "Selection" defines the content to be played and allows volume and equalizers settings to be adjusted before the content plays.  This addresses a common audio playback problem, where some content plays louder than other content. It also makes it possible to change equalizer settings automatically as content changes. Those who might want to take things to the next level can create playlists as arrays / vectors of selections. (example: DFPlaylist.ini). 
* Mute and unMute functionality was added.
* New softStop functionality tells DFPlayer to stop playing when the current track ends 
* Developers have Instant access to DFPlayer state information with methods like:  *isPlaying()*, *isPaused()*, *isIdle()*, and *isMuted()*.
* DFPlay seamlessly shifts the DFPlayer  to low-power "sleep" state when the device has been idle for 2 seconds, and seamlessly shifts back to the normal power state when the next play()* method is called.
### *Architecture*
##### DFPlay provides high-performance device control, but consumes minimal resources.
DFPlay manages the DFPlayer based on ***state*** variables which are stored in memory. Some of these variables describe a "desired state" , and some describe the "current state" of the DFPlayer.


DFPlay has one method ***manageDevice()*** that handles ALL of the communication with the DFPlayer. Each time manageDevice() is called, it asks two simple questions: 
1. Have I received any data from the DFPlayer?
2. Do I need to send a command to the DFPlayer?

Over 99.9% of the time, both answers are no, and manageDevice() simply exits. 

When the answer to the first question is yes, manageDevice() updates state variables based on the data it received.

When the answer to the second question is yes, manageDevice() will perform the following tasks:

 1. exit if it is too soon to send another command to the DFPlayer.
 2. evaluate state variables based on a set of rules which compare current and desired states to search for non-compliance.
 3. send a command to the DFPlayer to correct the first non-compliance found.
 4. exit immediately after the first command is sent. 
 
If another non-compliance issues exists, each  will be corrected in a subsequent manageDevice() execution cycle. 

DFPlayer needs time to process each command it receives. Most commands complete within 30ms, but some take longer. When manageDevice() sends a command, it calculates when it will be safe to send the next command. That calculation is the basis for step 1 exiting in the above task list. 

All other DFPlay methods simply update state variables in memory, or return data based on state variables. All return instantly. 

Please follow these links for additional information:

 - [DFPlay QuickStart Guide](https://docs.google.com/document/d/e/2PACX-1vTxUyPOpk9RFMaxt53oPotWyAa5pTBVzpSS2L23bq2fGhUXK08vAFPSAWQ6gENLNFoum10IWmVFkJ7I/pub)
 - DFPlay Examples

> Written with [StackEdit](https://stackedit.io/).