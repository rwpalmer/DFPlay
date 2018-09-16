/*
    Project:    DFPlay library
    File:       DFPlay.h
    Version:    1.0.0 - September 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   
*/    
#ifndef __DFPLAY_H_
#define __DFPLAY_H_

#ifdef SPARK
	#include <Particle.h>
#else
	#include <Arduino.h>
#endif

using namespace std;
class Selection {
	// this class defines the variables related to playing a track on the DFPlayer
	public:
		uint8_t media;      	// selects the media
		uint8_t folder;     	// selects the folder
		uint16_t track;     	// selects the track
		int8_t  volAdj;     	// adjusts the volume for the duration the Selection content (+/-)
		uint8_t equalizer;  	// sets the equalizer setting for the duration of the Selection content

		bool operator==(const Selection& b) { // NOTE: == only compares media, folder, and track. 
			if ((this->media == b.media) && (this->folder == b.folder) && (this->track == b.track))  return true;
			else return false;
		}
        
		bool operator!=(const Selection& b) { // NOTE: != only compares media, folder and track.
			if ((this->media == b.media) && (this->folder == b.folder) && (this->track == b.track))  return false;
			else  return true;
		}
		Selection operator=(const Selection &rt) { // NOTE: = copies all data elements.
			if (this != &rt) {
				this->media = rt.media;
				this->folder = rt.folder;
				this->track = rt.track;
				this->volAdj = rt.volAdj;
				this->equalizer = rt.equalizer;
			}
			return *this;
		}
};
	
class DesiredState {
	// this object is maintained to reflect the calling program's desired state for the DFPlayer
	public:
		Selection	selection;      	// the selected Selection object
		uint8_t		playState;      	// the desired play State (IDLE, PLAYING, PAUSED)
		uint8_t		equalizer;  		// the default equalizer state (0 to 5) ... may be overridden by selection.equalizer
		uint8_t		volume;     		// the default volume state (0 to 30) ... may be adjusted by selection.volAdj
		bool        	newSelection;	// when true, the selection has been updated via the play command
		bool			muted;				// when true, desired state is muted
		bool			softStop;       	// when true, play will stop when the current track ends
		bool        	repeat;         		// when true, selection play will repeat
		bool        	skip;           		// when true, skip to next track
		bool			back;				// when true, skip to the prior track
	};

class CurrentState {
	// this object is maintained to reflect the DFPlayer's current state
	public:
		uint8_t		playState;			// the current play state (IDLE, PLAYING, or PAUSED)
		uint8_t		equalizer;      	// the current equalizer state (0 to 5)
		uint8_t		volume;         	// the current volume state (0 to 30)
		uint8_t		media;          	// identifies the currently selected media (USB, SD, SLEEP)  
		uint8_t		playType; 			// the current play type (TBD, MEDIA, FOLDER, or TRACK)      
		bool			playFailure;		// true indicates that the last play command returned an error
		bool	    	usbAttached;	    // true indicates that a USB key is attached to the DFPlayer
		bool			sdAttached;	    // true indicates that an SD card is attached to the DFPlayer
		bool        	sleeping;       	// true indicates that the DFPlayer is in low-power mode
		bool        	changePending;	// true tells manageDevice() that a state change is pending
		bool        	firstEot;       		// true identifies first End-Of-Track frame 
		uint32_t		noSubmitsTil;		// the time when the DFPlayer will be ready to accept the next command
		uint32_t		trackCount;     	// the number of tracks that have been played in a folder or media selection
		uint32_t    	idleMillis;     		// millis when cState.playState was set to IDLE
		int    	    	tracks;         		// the number of tracks in a folder or media selection
};		

class DFPlay {
	private:
		//  CLASS CONSTANTS
		static const uint8_t USB = 1, SD = 2, SLEEP = 10; // Media state
		static const uint8_t IDLE = 0, PLAYING = 1, PAUSED = 2, INITIALIZE = 3;  // Play state
		static const uint16_t SUBMIT_INTERVAL = 30; // standard interval between frames submitted to DFPlayer (milliseconds)
		static const uint8_t TBD = 0, MEDIA = 1, FOLDER = 2, TRACK = 3; // PlayType
		static const uint8_t RESPONSE_FRAME_SIZE = 10;
		static const uint8_t FST=0, VER=1, LEN=2, CMD=3, FBK=4, MSB=5, LSB=6, CMSB=7, CLSB=8, LST=9; // Response frame byte names 

		//	CLASS MEMBERS
		Stream *stream;
		DesiredState dState;
		CurrentState cState;
		uint8_t frame[RESPONSE_FRAME_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};  // Response frame
		uint8_t fx = 0;                                                         // Response frame byte index

		// PRIVATE METHODS
		void submitRequest(uint8_t[] ,uint16_t);
		void submitRequest(uint8_t[] ,uint16_t, uint8_t);
        
	public:
		// PUBLIC METHODS        
		void begin(Stream& s);
		void play(Selection&);
		void play(uint8_t m, uint8_t f=0, uint16_t t=0, int8_t  v=0, uint8_t e=0);
		void pause(void);
		void resume(void);
		void repeat(void);
		void norepeat(void);
		void stop(void);
		void softStop(void);
		void skip(void);
		void back(void);
		uint8_t setVolume(uint8_t);
		uint8_t volumeUp(void);
		uint8_t volumeDown(void);
		uint8_t setEqualizer(uint8_t);
		uint8_t equalizerUp(void);
		uint8_t equalizerDown(void);
		void mute(void);
		void unmute(void);
		bool isMuted(void);
		bool isIdle(void);
		bool isPlaying(void);
		bool isPaused(void);
		bool isRepeating(void);
		bool isSleeping(void);
		bool playFailure(void);
		void manageDevice(void);
		uint32_t getTrackCount(void);
		uint32_t getTracks(void);
};
#endif
