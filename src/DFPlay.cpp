/*
    Project:    DFPlay library
    File:       DFPlay.cpp 
    Version:    1.0.0 - September 2018
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   
*/    
#include "DFPlay.h"

//#define DFPLAY_DEBUG_SERIAL Serial // <-- uncomment this #define statement to enable logging to a Serial console. 


// ----------------------------------------------------------------------------------------------------------------		
// ------------------------------------------------ Public  METHODS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------------------	

void DFPlay::begin(Stream& s) {    //  initialize class members and query DFPlayer to determine what media is attached
    this->stream = &s;
    this->dState.selection = {0,0,0,0,0};
    this->dState.playState = IDLE;
    this->dState.equalizer = 0;
    this->dState.volume = 15;
    this->dState.newSelection = false;
    this->dState.muted = false;
    this->dState.repeat = false;
    this->dState.softStop = false;
    this->dState.skip = false;
	this->dState.back = false;
    this->cState.playState = INITIALIZE; // <-- tells manageDevice() to perform the DFPlayer initialization sequence
    this->cState.equalizer = 0;
    this->cState.volume = 30;
    this->cState.media = 0;
    this->cState.playType = TBD;
	this->cState.playFailure = false;
    this->cState.usbAttached = false;
    this->cState.sdAttached = false;
    this->cState.tracks = 0;
    this->cState.trackCount = 0;
    this->cState.sleeping = false;
    this->cState.changePending = true;
	this->cState.idleMillis = 0;
	this->cState.firstEot = true;
    #if defined(DFPLAY_DEBUG_SERIAL) && defined(SPARK) 
		this->cState.noSubmitsTil = millis() + 3000; // allow time for the Serial connection to be established before initializing the DFPlayer
	#else 
		this->cState.noSubmitsTil = 0; 
	#endif
	return;
}
void DFPlay::play(Selection& sel) {
    this->dState.selection = sel;
    this->dState.newSelection = true;
    this->dState.playState = PLAYING;
    this->dState.softStop = false;
    this->dState.skip = false;
	this->dState.back = false;
	this->cState.tracks = 0;
	this->cState.changePending = true;
	this->cState.playFailure = false;
    return;
}
void DFPlay::play(uint8_t m, uint8_t f, uint16_t t, int8_t  v, uint8_t e) {
	Selection s = {m,f,t,v,e};
	this->play(s);
}

void DFPlay::pause(void)  {
    if (this->isPlaying()) {
        this->dState.playState = PAUSED;
        this->cState.changePending = true;
    }
    return; 
}
void DFPlay::resume(void)  {
    if (this->isPaused()) {
        this->dState.playState = PLAYING;
        this->cState.changePending = true;
    }
    return; 
}
void DFPlay::stop(void)	{
    if (this->isPlaying()) {
        this->dState.playState = IDLE;
        this->cState.changePending = true;
    }
    return;
}
void DFPlay::softStop(void)	{ 
    if (this->isPlaying()) {
        this->dState.softStop = true;
        #ifdef DFPLAY_DEBUG_SERIAL
            DFPLAY_DEBUG_SERIAL.println("softStop requested ..."); // in a media or folder play, softStop stops the DFPlayer when the current track ends
        #endif
    }
    return;
}
void DFPlay::skip(void) {
    if (this->cState.playState == PLAYING) {
        this->dState.skip = true;
        this->cState.changePending = true;
    }
    return;
}
void DFPlay::back(void) {
    if (this->cState.playState == PLAYING) {
        this->dState.back = true;
        this->cState.changePending = true;
    }
    return;
}
uint8_t DFPlay::setVolume(uint8_t vol)  { 
    this->dState.volume = max(min(vol, (uint8_t)30), (uint8_t)0);   
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.volume;
}
uint8_t DFPlay::volumeUp(void)  {
    this->dState.volume = (min(this->dState.volume +1,30));
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.volume;
}
uint8_t DFPlay::volumeDown(void)  {
    this->dState.volume = (max(this->dState.volume -1, 0));
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.volume;
}
uint8_t DFPlay::setEqualizer(uint8_t eq) {
    this->dState.equalizer = eq;
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.equalizer;
}
uint8_t DFPlay::equalizerUp(void) {
    this->dState.equalizer = (min(this->dState.equalizer +1,5));
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.equalizer;
}
uint8_t DFPlay::equalizerDown(void) {
    this->dState.equalizer = (max(this->dState.equalizer -1,0));
    if (this->isPlaying()) this->cState.changePending = true;
    return this->dState.equalizer;
}
void DFPlay::mute(void) {
    this->dState.muted = true;
    if (this->isPlaying()) this->cState.changePending = true;
    return;
}
void DFPlay::unmute(void) {
    this->dState.muted = false;
    if (this->isPlaying()) this->cState.changePending = true;
    return;
}
void DFPlay::repeat (void)            		{ this->dState.repeat = true; return; }
void DFPlay::norepeat(void)				{ this->dState.repeat = false; return; }
bool DFPlay::isMuted(void)      			{ return this->dState.muted; }
bool DFPlay::isIdle(void) 					{ if (this->dState.playState == IDLE) 		return true;    else return false; }
bool DFPlay::isPlaying(void)				{ if (this->dState.playState == PLAYING)	return true;  	else return false; }
bool DFPlay::isPaused(void) 				{ if (this->dState.playState == PAUSED) 	return true;    else return false; }
bool DFPlay::isRepeating(void)       	{ if (this->dState.repeat) return true; else return false; }
bool DFPlay::isSleeping(void)         	{ return this->cState.sleeping; }
bool DFPlay::playFailure(void)			{ return this->cState.playFailure; }
uint32_t DFPlay::getTrackCount(void)	{ return cState.trackCount; }
uint32_t DFPlay::getTracks(void)		{ return cState.tracks; }


// ----------------------------------------------------------------------------------------------------------------		
// ------------------------------------------------ manageDevice() -----------------------------------------------
// ----------------------------------------------------------------------------------------------------------------	

void DFPlay::manageDevice(void) {
    
// PROCESS DATA FRAMES RECEIVED FROM THE DFPLAYER
    while (stream->available()) {
		if(fx < RESPONSE_FRAME_SIZE) {
			frame[fx] = stream->read();
			fx++;
		}
		if (fx == RESPONSE_FRAME_SIZE) {
        	// COMPUTE CHECKSUM AND PERFORM VALIDATIONS
        	uint16_t cs = 0xFFFF - (frame[VER] + frame[LEN] + frame[CMD] + frame[FBK] + frame[MSB] + frame[LSB]) +1;
        	bool frameError = false, checksumError = false;
        	for (int i = 0; i < RESPONSE_FRAME_SIZE; i++) {
            	switch (i) {
            		case  FST:  if (frame[FST]  != 0x7E)     { frameError = true; }     break; 
            		case  VER:  if (frame[VER]  != 0xFF)     { frameError = true; }     break;
            		case  LEN:  if (frame[LEN]  != 0x06)     { frameError = true; }     break;
            		case  CMSB: if (frame[CMSB] != cs / 256) { checksumError = true; }  break;
            		case  CLSB: if (frame[CLSB] != cs % 256) { checksumError = true; }  break;
            		case  LST:  if (frame[LST]  != 0xEF)     { frameError = true; }     break;
            	}
        	}
        	#ifdef DFPLAY_DEBUG_SERIAL
        	    DFPLAY_DEBUG_SERIAL.printf("Response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", 
        	    frame[0],frame[1],frame[2],frame[3],frame[4],frame[5],frame[6],frame[7],frame[8],frame[9]);
        	#endif
        	if (frameError) { 
        	    // HANDLE FRAMING ERRORS ... RESYNC TO NEXT POSSIBLE FIRST BYTE CHARACTER
        		fx = 0;
        		for (uint8_t i = 1; i < RESPONSE_FRAME_SIZE; i++) { 
        			if (frame[i] == 0x7E) { // found next possible FST in current frame
        				for ( ; i < RESPONSE_FRAME_SIZE; fx++, i++) { // align frame with new FST
        					frame[fx] = frame[i];
        				}
        				for (i = fx; i < RESPONSE_FRAME_SIZE; i++) { // clear remaining bytes
        					frame[fx] = 0x00;
        				}
        			}
        		}	
            } else if (! checksumError) { 
        	
                // POST DATA FRAME INFORMATION TO STATE VARIABLES
                
                // FRAME 0x3A - unsolicited announcement of USB or SD media attachment
                //            - this frame wakes the player and makes attached media the selected media
                //            - If other media is playing, play will stop, so we set the states accordingly
                if (frame[CMD] == 0x3a) {
                    this->cState.sleeping = false;
					this->cState.playState = IDLE;
					this->cState.idleMillis = millis();
					this->dState.playState = IDLE;
     			    this->cState.changePending = true;
                    if (frame[LSB] == USB) {
                        this->cState.usbAttached = true;
                        this->cState.media = USB;
                   }
                    else if (frame[LSB] == SD) {
                        this->cState.sdAttached = true;
                        this->cState.media = SD;
                    }
                }
                
                // FRAME 0x3B - unsolicited announcment of USB or SD media removal
                else if (frame[CMD] == 0x3b) {
                    if (frame[LSB] == USB) 	this->cState.usbAttached = false;
                    else if (frame[LSB] == SD)  this->cState.sdAttached = false;

					if ((isPlaying()) && (this->dState.selection.media == frame[LSB])) {
						this->cState.playState = IDLE;
						this->cState.idleMillis = millis();
						this->dState.playState = IDLE;
     			        this->cState.changePending = true;
					}
                }
                
                // FRAME 0x3C & 0x3D - unsolicited "end of track" announcements
                else if ((frame[CMD] == 0x3c) || (frame[CMD] == 0x3d)) {
                    // For MEDIA and FOLDER plays ...
    			    if ((this->cState.playType == FOLDER) || (this->cState.playType == MEDIA)) {
    			        this->cState.trackCount++;
        			    // when softStop is desired, signal a hard-stop when the current track ends
        			    if (this->dState.softStop) {
        			        this->dState.playState = IDLE;
        			        this->cState.changePending = true;
     			        } else if (this->cState.trackCount >= this->cState.tracks) { // last track was played
         			        this->cState.trackCount = 0;
         			        if ( !this->dState.repeat) {
         			            this->dState.playState = IDLE; // signal a hard stop
         			            this->cState.changePending = true;
         			            // DFPlayer requires extra time to process when stopping after the last track in a folder or on media
         			            this->cState.noSubmitsTil = millis() + 100; // see note 3 at the bottom of this file for more detail
         			        }
         			    }
    			    }
					// For Individual Track Plays ...
    				else {
    				    if (this->cState.firstEot) { // When playing individual tracks, DFPlayer returns duplicate End-Of-Track frames
    				        this->cState.firstEot = false;  // DFPlay sets this flag so it knows to act when the second frame arives.
        				} else if (this->dState.repeat) {           // The second EOF HAS arrived and repeat is TRUE
        				    this->dState.newSelection = true;
        				    this->cState.changePending = true;
        				} else {                                    // The second EOF HAS arrived and repeat is FALSE
                            this->dState.playState = IDLE;
        				    this->cState.changePending = true;
        				    #ifdef DFPLAY_DEBUG_SERIAL
            				    DFPLAY_DEBUG_SERIAL.println("Track Play Complete ...");
            			    #endif
        				}
    				}
                }
                
                // FRAME 0x3F - solicited or unsolicited media status frame
        		else if (frame[CMD] == 0x3f) {
        		    if (frame[LSB] == USB) {                // USB only
        		        this->cState.usbAttached = true; 
        		        this->cState.sdAttached = false;
        		    } else if (frame[LSB] == SD) {          // SD only
        		        this->cState.usbAttached = false;
        		        this->cState.sdAttached = true;
        		    } else if (frame[LSB] == 0x03) {         // USB and SD
        		        this->cState.usbAttached = true;
        		        this->cState.sdAttached = true;
    				}
        		}
        		
        		/* FRAME 0x40 - unsolicited DFPlayer error notification ... Error descriptions listed below for reference
                    0x01:    NO MEDIA / IN STANDBY MODE / MODULE BUSY   |   0x05:    SPECIFIED FOLDER OR TRACK OUT OF SCOPE  
                    0x02:    DEVICE IN SLEEP MODE                       |   0x06:    SPECIFIED FOLDER OR TRACK NOT FOUND
                    0x03:    SERIAL RECEIVING ERROR                     |   0x07:    TRACK MUST BE PLAYING TO INTERCUT AD   
                    0x04:    REQUEST CHECKSUM INCORRECT                 |   0x08:    MEDIA READ FAILURE
                */
        		else if (frame[CMD] == 0x40) {
        		    // Reset to IDLE state when the error code indicates a track play failure ...
        		    if ((frame[LSB] == 0x05) || (frame[LSB] == 0x06) || (frame[LSB] == 0x08)) {
						this->cState.playState = IDLE;
						this->cState.idleMillis = millis();
						this->dState.playState = IDLE;
                        this->cState.changePending = true;
						this->cState.playFailure = true;
			            #ifdef DFPLAY_DEBUG_SERIAL
			                DFPLAY_DEBUG_SERIAL.println("Play Failure ...");
			            #endif
                     }
        		}
        		
                // FRAME 0x47, 0x48, or 0x4E - solicited frames that return trackcount on USB, on SD, or in a folder
        		else if ((frame[CMD] == 0x47) || (frame[CMD] == 0x48) || (frame[CMD] == 0x4e)) {
        		    this->cState.tracks = ((frame[MSB] * 256) + frame[LSB]);
        		    this->cState.trackCount = 0;
        			if (this->cState.tracks == 0) {
    			        this->cState.playState = IDLE;
						this->cState.idleMillis = millis();
                        this->dState.playState = IDLE;
                        this->cState.changePending = true;
			            #ifdef DFPLAY_DEBUG_SERIAL
			                DFPLAY_DEBUG_SERIAL.println("No Tracks to Play ...");
			            #endif
    			    }
        		}
            } // FRAME PROCESSING COMPLETE
            // When this point is reached, a valid frame has been processed OR the frame failed validation ...
            // In either case, we clear the frame, reset the frame index, and look for another frame ...
        	for (fx = 0; fx < RESPONSE_FRAME_SIZE; fx++) { 
        	    frame[fx] = 0x00; 
            }
        	fx = 0;
        }
    } // end of while (stream->available()) ...
    
    /* --------------------------------------------------- R U L E S ------------------------------------------------------
                - DFPlay Rules are executed in sequence.
                - Each rule looks at specific state variables to determine if it should take action.
                    - When the answer is "NO", the next rule will be executed.
                    - When the answer is "YES", the rule will take action and return to loop(). This cedes the processor
                      to the the primary application.
                - Some rules merely return to loop() because state variables tell them that DFPlayer is in compliance with
                  the desired state or because they detect a reason that prevents subsequent rules from taking any action. 
     */    

// ASSESS STATE VARIABLES TO DETERMINE IF THERE IS ANY VALUE IN GOING FURTHER
    //  Each loop cycle will reach this point, but need not go further if one or more of the following cases is true ...

    // RULE A1 - Exit when no changes are pending (This rule returns over 99.9% of the time. )
    if ( !this->cState.changePending) return;
    
	// RULE A2 - Return to loop if it is too soon to submit another command to the DFPlayer
    if (millis() < this->cState.noSubmitsTil) return;
    
    // RULE A3 - When DFPlayer is IDLE and IDLE is the desired state, check sleep mode and return to loop
	if ((this->dState.playState == IDLE) && (this->cState.playState == IDLE)) {
	    // Enter sleep mode if player has been inactive for 2 seconds or more
	    if (( !this->cState.sleeping) && ((millis() - this->cState.idleMillis) > 2000)) {
	       #ifdef DFPLAY_DEBUG_SERIAL
                DFPLAY_DEBUG_SERIAL.println("Enter Sleep");
            #endif
            uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0A, 0x00, 0x00, 0x00, 0xFE, 0xF1, 0xEF };
            submitRequest(request,SUBMIT_INTERVAL);
            this->cState.sleeping = true;
            this->cState.changePending = false;
	    }
	    return;
	}
	
    // RULE A4 - When the MPU boots, make sure DFPlayer is not sleeping and query the media status
    if (this->cState.playState == INITIALIZE) { // this state is set by begin()
        if (this->cState.media == 0) {          // this will wake DFPlayer if USB media is attached
			#ifdef DFPLAY_DEBUG_SERIAL	       
				DFPLAY_DEBUG_SERIAL.println("Initialize");
			#endif
		    uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x01, 0xFE, 0xF1, 0xEF };
		    submitRequest(request,SUBMIT_INTERVAL*3);
		    this->cState.media++;
        } else if (this->cState.media == 1) {   // this will wake DFPlayer if SD media is attached
		    uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xFE, 0xF0, 0xEF };
		    submitRequest(request,SUBMIT_INTERVAL*3);
		    this->cState.media++;
        } else if (this->cState.media == 2) {  // query DFPlayer media and reset playState
            uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0xBC, 0xEF };
		    submitRequest(request,SUBMIT_INTERVAL);
		    this->cState.playState = IDLE;
			this->cState.idleMillis = millis();
		    this->cState.media = 0;
        }
        return; // exit DFPlayer initialization procedure
    }

    // RULE A5 - Make sure DFPlayer is awake and ready to accept commands
    if (this->cState.sleeping) {
        #ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.println("Exit Sleep");
		#endif
		if (this->cState.usbAttached) {
		    uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x01, 0xFE, 0xF1, 0xEF };
		    submitRequest(request,SUBMIT_INTERVAL*3);
		    this->cState.media = USB;
		} else {
		    uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, 0x02, 0xFE, 0xF0, 0xEF};
		    submitRequest(request,SUBMIT_INTERVAL*3);
		    this->cState.media = SD;
		} 
		this->cState.sleeping = false;
		return;
    }
	#ifdef DFPLAY_DEBUG_SERIAL
		if (this->dState.newSelection == true) {
			DFPLAY_DEBUG_SERIAL.printf("\nSelection: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		}
	#endif

// ASSESS STATE VARIABLES AND ADJUST STOP, PAUSE/RESUME, VOLUME, AND EQUALIZER SETTINGS AS REQUIRED TO ALIGN WITH DESIRED STATE
    // Trigger actions affecting current play or in preparation to play a new selection -----------------------------------------------

    // RULE B1 - Skip to next track
    if ((this->dState.skip) && (this->cState.playState == PLAYING)) {
		if ((this->cState.playType == FOLDER) || (this->cState.playType == MEDIA)) {
			// if playing last track and not repeating, stop
			if ((this->cState.trackCount == (cState.tracks -1)) &&  ( !this->dState.repeat)) {
				this->dState.playState = IDLE;
				this->cState.idleMillis = millis();
 		        this->cState.noSubmitsTil = millis() + 100; // see note 3 at the bottom of this file for more detail
			} else { // otherwise, skip to the next track
				#ifdef DFPLAY_DEBUG_SERIAL
					DFPLAY_DEBUG_SERIAL.println("Skip");
				#endif
				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0xFE, 0xFA, 0xEF };
				submitRequest(request,SUBMIT_INTERVAL);
				this->cState.changePending = false;
				// adjust trackCount
				this->cState.trackCount++;
				if (this->cState.trackCount >= this->cState.tracks) {
					this->cState.trackCount = 0;
				}
			}
 		} else { // playing an individual track
		    this->dState.playState = IDLE; // signal a hard stop (If a playlist is used, going IDLE will trigger the next play command)
			this->cState.idleMillis = millis();
 		}
    	this->dState.skip = false;
		return;
    }
    // RULE B2 - Back to the prior track
    if ((this->dState.back) && (this->cState.playState == PLAYING)) {
		if ((this->cState.playType == FOLDER) || (this->cState.playType == MEDIA)) {
			// if playing first track and not repeating, stop
			if ((this->cState.trackCount == 0) &&  ( !this->dState.repeat)) {
				this->dState.playState = IDLE;
				this->cState.idleMillis = millis();
			} else { // otherwise, skip to the prior track
				#ifdef DFPLAY_DEBUG_SERIAL
					DFPLAY_DEBUG_SERIAL.println("Back");
				#endif
				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x02, 0x00, 0x00, 0x00, 0xFE, 0xF9, 0xEF };
				submitRequest(request,SUBMIT_INTERVAL);
				this->cState.changePending = false;
				// adjust trackCount
				if (this->cState.trackCount == 0) {
					this->cState.trackCount = (this->cState.tracks);
				} 
				this->cState.trackCount--;
			}
 		} else { // playing an individual track
		    this->dState.playState = IDLE; // signal a hard stop (If a playlist is used, going IDLE will trigger the next play command)
			this->cState.idleMillis = millis();
		}
    	this->dState.back = false;
		return;
    }
      
    // RULE B3 - Make sure DFPlayer is stopped when IDLE is the desired playState
	if ((this->dState.playState == IDLE) && (this->cState.playState != IDLE)) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.println("Stop");
		#endif
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x16, 0x00, 0x00, 0x00, 0xFE, 0xE5, 0xEF };
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = IDLE;
		this->cState.idleMillis = millis();
		this->cState.changePending = true;
		return;
    }

   // RULE B4 - PAUSE when PLAYING and desired state is PAUSED
    if ((this->dState.playState == PAUSED) && (this->cState.playState == PLAYING)) {
        #ifdef DFPLAY_DEBUG_SERIAL
            DFPLAY_DEBUG_SERIAL.println("Pause");
        #endif
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xFE, 0xED, 0xEF };
        submitRequest(request, SUBMIT_INTERVAL);
        this->cState.playState = PAUSED;
		this->cState.changePending = false;
 return;
    }

    // RULE B5 - Assure DFPlayer volume is set to the desired volume
	uint8_t desiredVolume = 0; // this setting will prevail when muted
    if ( !this->dState.muted) {
        desiredVolume = max(min((this->dState.volume + this->dState.selection.volAdj), 30), 0);
	}
	if (desiredVolume != this->cState.volume) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.println("setVolume");
		#endif
		uint16_t cs = 0xFEFB - (0x06 + desiredVolume); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, desiredVolume, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.volume = desiredVolume;
		if( !this->dState.newSelection) this->cState.changePending = false;
		return;
	}

    // RULE B6 - Assure DFPlayer equalizer is set to the desired state
    uint8_t desiredEqualizer = 0;
    if (this->dState.selection.equalizer == 0) desiredEqualizer = this->dState.equalizer;
	else desiredEqualizer = this->dState.selection.equalizer;
    if (this->cState.equalizer != desiredEqualizer) {
        #ifdef DFPLAY_DEBUG_SERIAL
            DFPLAY_DEBUG_SERIAL.println("setEqualizer");
        #endif
        uint16_t cs = 0xFEFB - (0x07 + desiredEqualizer); // compute checksum
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x07, 0x00, 0x00, desiredEqualizer, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF };
        submitRequest(request,SUBMIT_INTERVAL);
        this->cState.equalizer = desiredEqualizer;
        if( !this->dState.newSelection) this->cState.changePending = false;
        return;
    }

    // RULE B7 - Resume when DFPlayer is PAUSED and desired state is PLAYING
    if ((this->cState.playState == PAUSED) && (this->dState.playState == PLAYING)) {
        #ifdef DFPLAY_DEBUG_SERIAL
            DFPLAY_DEBUG_SERIAL.println("Resume");
        #endif
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xFE, 0xEE, 0xEF };
        submitRequest(request,SUBMIT_INTERVAL);
        this->cState.playState = PLAYING;
		this->cState.changePending = false;
        return;
    }
// ASSESS STATE VARIABLES AND START PLAYING A NEW SELECTION

	// RULE C1 - Return to loop() if there is nothing new to play
	if (! this->dState.newSelection) return;

    // RULE C2 - Return to loop() if selection.media is not attached to the DFPlayer
	if (((this->dState.selection.media == USB) && (this->cState.usbAttached)) 
	||  ((this->dState.selection.media == SD) && (this->cState.sdAttached))) { // media ok
	} else {
		this->dState.playState = IDLE;
		this->dState.newSelection = false;
		this->cState.changePending = false;
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.println("Invalid Media");
		#endif
		return;
	}

	// RULE C3 - Make sure selection.media is the selected media
	if (this->cState.media != this->dState.selection.media) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.println("Select Media");
		#endif
		uint16_t cs = 0xFEFB - (0x09 + this->dState.selection.media); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, this->dState.selection.media, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL*5);
		this->cState.media = this->dState.selection.media;
		return;
    }
	
    // RULE C4 - Play tracks on the media
	if (this->dState.selection.folder == 0) {
	    if (this->dState.selection.track == 0) {
    		if (this->cState.tracks == 0) { // Issue the DFPlayer command to query the number of files on the media
    			#ifdef DFPLAY_DEBUG_SERIAL
    			    DFPLAY_DEBUG_SERIAL.println("Query Track Count");
    		    #endif
    			if (this->dState.selection.media == USB)  {
    				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x47, 0x00, 0x00, 0x00, 0xFE, 0xB4, 0xEF};
    				submitRequest(request,SUBMIT_INTERVAL);
    			} else { // selection media == SD
    				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x48, 0x00, 0x00, 0x00, 0xFE, 0xB3, 0xEF};
    				submitRequest(request,SUBMIT_INTERVAL);
    			}
    			cState.tracks--;
    			return;   
    		} else if (cState.tracks > 0) {
    		// Issue the DFPlayer command to play the media
        		#ifdef DFPLAY_DEBUG_SERIAL
        			DFPLAY_DEBUG_SERIAL.printf("Play Media: {%d,%d,%d,%d,%d} ... %d Tracks\n\r", 
        			this->dState.selection.media, this->dState.selection.folder, this->dState.selection.track, 
        			this->dState.selection.volAdj, this->dState.selection.equalizer, this->cState.tracks);
        		#endif
        		uint16_t cs = 0xFEFB - (0x11 + this->dState.selection.media); // compute checksum
        		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x11, 0x00, 0x00,  this->dState.selection.media, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
        		submitRequest(request,SUBMIT_INTERVAL, MEDIA);
        		return;
    		} else if (cState.tracks == -8) { // timeout after 40 SUBMIT_INTERVALS
    		    this->dState.playState = IDLE;
    		    this->cState.idleMillis = millis();
    		    this->cState.playState = IDLE;
    		    this->cState.changePending = true;
    		    return;
    		} else { 
    		    this->cState.tracks--;
    		    this->cState.noSubmitsTil = (millis() + (5* (int)SUBMIT_INTERVAL));
            #ifdef DFPLAY_DEBUG_SERIAL
              DFPLAY_DEBUG_SERIAL.println("waiting for Query ...");
            #endif
    		    return;
    	    }
	    } else { // dState.selection.track is not 0, so we play a track based on a track number.
    		#ifdef DFPLAY_DEBUG_SERIAL
    			DFPLAY_DEBUG_SERIAL.printf("Play Track#: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
    			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
    		#endif
    		uint16_t cs = 0xFEFB - (0x08 + (this->dState.selection.track / 256) + (this->dState.selection.track % 256)); // compute checksum
    		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x08, 0x00, (uint8_t)(this->dState.selection.track / 256), 
    		    (uint8_t)(this->dState.selection.track % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
    		submitRequest(request,SUBMIT_INTERVAL,TRACK);
    		return;
	    }
	}   

	// RULE C5 - Play all the tracks in a folder
	if (this->dState.selection.track == 0) {
		if (this->cState.tracks == 0) { // Issue the DFPlayer command to query the number of files in a folder
			#ifdef DFPLAY_DEBUG_SERIAL
			    DFPLAY_DEBUG_SERIAL.println("Query Track Count");
		    #endif
			uint16_t cs = 0xFEFB - (0x4E + this->dState.selection.folder); // compute checksum
			uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x4E, 0x00, 0x00,  this->dState.selection.folder, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
			submitRequest(request,( 5* (int)SUBMIT_INTERVAL));
			cState.tracks--;
			return;
		} else if (cState.tracks > 0) {
        	// Issue the DFPlayer command to play the folder
        	#ifdef DFPLAY_DEBUG_SERIAL
        		DFPLAY_DEBUG_SERIAL.printf("Play Folder: {%d,%d,%d,%d,%d} ... %d Tracks\n\r", 
        		this->dState.selection.media, this->dState.selection.folder, this->dState.selection.track, 
        		this->dState.selection.volAdj, this->dState.selection.equalizer, this->cState.tracks);
        	#endif
        	uint16_t cs = 0xFEFB - (0x17 + this->dState.selection.folder); // compute checksum
        	uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x17, 0x00, 0x00,  this->dState.selection.folder, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
        	submitRequest(request,SUBMIT_INTERVAL, FOLDER);
        	return;
		} else if (cState.tracks == -8) { // timeout after 40 SUBMIT_INTERVALS
		    this->dState.playState = IDLE;
		    this->cState.idleMillis = millis();
		    this->cState.playState = IDLE;
		    this->cState.changePending = true;
		    return;
		} else { 
		    this->cState.tracks--;
		    this->cState.noSubmitsTil = (millis() + (5* (int)SUBMIT_INTERVAL));
        #ifdef DFPLAY_DEBUG_SERIAL
		      DFPLAY_DEBUG_SERIAL.println("waiting for Query ...");
        #endif
		    return;
	    }
	}

	// RULE C6 - Play an individual track track from folders 01 to 15 ... Track must have a 4-digit file name prefix
	if (this->dState.selection.folder < 16) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.printf("Play 4-digit prefix: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t dbyte = ((this->dState.selection.folder * 4096) + this->dState.selection.track);
		uint16_t cs = 0xFEFB - (0x14 + (dbyte / 256) + (dbyte % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x14, 0x00, (uint8_t)(dbyte / 256), (uint8_t)(dbyte % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL, TRACK);
		return;
	}

	// RULE C7 - Play an individual track track from folders 16 to 99 ... Track must have a 3-digit file name prefix
	if ((this->dState.selection.folder > 15) && (this->dState.selection.folder < 100)) {
    	#ifdef DFPLAY_DEBUG_SERIAL
    		DFPLAY_DEBUG_SERIAL.printf("Play 3-digit prefix: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
    		this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
    	#endif
    	uint16_t cs = 0xFEFB - (0x0F + this->dState.selection.folder + this->dState.selection.track); // compute checksum
    	uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0F, 0x00, this->dState.selection.folder, (uint8_t)this->dState.selection.track, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
    	submitRequest(request,SUBMIT_INTERVAL, TRACK);
    	return;
    }
	
	// RULE C8 - Play a track from the root folder ... Track must have a 4-digit file name prefix
    if (this->dState.selection.folder == 100) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.printf("Play root: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t cs = 0xFEFB - (0x03 + (this->dState.selection.track / 256) + (this->dState.selection.track % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x03, 0x00, (uint8_t)(this->dState.selection.track / 256), 
		    (uint8_t)(this->dState.selection.track % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL, TRACK);
		return;
    }
    
	// RULE C9 - Play a track from the MP3 folder ... Track must have a 4-digit file name prefix
    if (this->dState.selection.folder == 101) {
		#ifdef DFPLAY_DEBUG_SERIAL
			DFPLAY_DEBUG_SERIAL.printf("Play MP3: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t cs = 0xFEFB - (0x12 + (this->dState.selection.track / 256) + (this->dState.selection.track % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x12, 0x00, (uint8_t)(this->dState.selection.track / 256), 
		    (uint8_t)(this->dState.selection.track % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL,TRACK);
		return;
    }
    
	return;
} // end of manageDevice()


// ----------------------------------------------------------------------------------------------------------------		
// ------------------------------------------------ PRIVATE METHODS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------------------	
   
// Submit PLAY requests to the DFPlayer
void DFPlay::submitRequest(uint8_t request[], uint16_t dlay, uint8_t ptype) {
	this->cState.playState = PLAYING;
	this->cState.playType = ptype;
	this->dState.newSelection = false;
	this->cState.changePending = false;
	if(ptype == TRACK) {
	    if (dState.selection.folder == 0) this->cState.firstEot = false; // DFPlayer command 0x08 returns one End-Of-Track notification
	    else this->cState.firstEot = true; // ALL other DFPlayer "track play" commands return two ... 
	}
	submitRequest(request, dlay);
}
// Submit OTHER requests to DFPlayer
void DFPlay::submitRequest(uint8_t request[], uint16_t dlay) {
    uint8_t requestLength = request[LEN] + 4;
    #ifdef DFPLAY_DEBUG_SERIAL
        DFPLAY_DEBUG_SERIAL.print(" Request:");
        for (int i = 0; i<requestLength; i++) {
            DFPLAY_DEBUG_SERIAL.printf(" %02x", request[i]);
        }
        DFPLAY_DEBUG_SERIAL.println();
    #endif
    stream->write(request,requestLength);
    this->cState.noSubmitsTil = millis() + dlay;
}

    
/*  DFPlayer Notes:

	    TIMING ADJUSTMENTS MAY BE REQUIRED IN SCENDARIOS WHERE VERY LARGE TRACK COUNTS ARE USED. 

    1. This program uses the "noSubmitTil" variable to manage the timing of commands sent to the DFPlayer. The 
        minimum interval is defined by the SUBMIT_INTERVAL constant ... 30ms by default. (DFPlay.h CLASS CONSTANT)
		
    2. When querying the number of tracks in a folder or on the media DFPlayer can take MUCH MUCH longer. This program 
		checks for completion every 5 SUBMIT_INTERVALs up to eight times before timing out. (Rules C4 and C5)
		
    3. In most circumstances, a 0x16 (stop) command can be issued immediately after a 0x3c/d/e (end-of-play) frame  
        is received, but there is a timing problem when the 0x17 command is playing a folder and we want to stop
        after the last track in the folder. Based on testing, we need to insert a 100-250ms delay before the 0x16 (stop)
        command is issued. (When processing frames 3C and 3D, and Rule B1)
		
	4. When playing single tracks,  DFPlayer returns duplicate end-of-track frames (except for the 0x08 command). 
		You will see this if DFPLAY_DEBUG_SERIAL is enabled. This library ignores the first eot frame and acts on the second. 
		(cState.firstEot exists to manage this odd behavior). 
*/    
    
    
    
    
