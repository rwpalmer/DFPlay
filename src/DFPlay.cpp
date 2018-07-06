/*
    Project:    DFPlay library
    File:       DFPlay.cpp 
    Version:    0.0.1  7/3/18
    Copyright:  2018, Rodney Palmer (rwpalmeribm@gmail.com)
    License:    GNU GPLv3   
*/    
#include "DFPlay.h"

#define LOGGING false;


// ----------------------------------------------------------------------------------------------------------------		
// ------------------------------------------------ Public  METHODS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------------------	


void DFPlay::begin(void) {    //  initialize class members and query DFPlayer to determine what media is attached
    this->dState.selection = {0,0,0,0,0};
    this->dState.playState = IDLE;
    this->dState.equalizer = 0;
    this->dState.volume = 15;
    this->dState.newSelection = false;
    this->dState.muted = false;
    this->dState.softStop = false;
    this->cState.playState = IDLE;
    this->cState.equalizer = 0;
    this->cState.volume = 30;
    this->cState.media = 0;
    this->cState.playType = TBD;
    this->cState.usbAttached = false;
    this->cState.sdAttached = false;
    this->cState.noSubmitsTil = 0;
    this->cState.trackCount = 0;
    Serial1.begin(9600);
    #ifdef LOGGING 
        delay(3000);
        Serial.println("Query Media");
    #endif
    uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0xBC, 0xEF }; // QUERY MEDIA
    submitRequest(request, SUBMIT_INTERVAL);
	return;
}
void DFPlay::play(Selection& sel) { // Play a new selection
    this->dState.selection = sel;
    this->dState.newSelection = true;
    this->dState.playState = PLAYING;
    this->dState.muted = false;
    this->dState.softStop = false;
	this->cState.trackCount = 0;
    return;
}
void DFPlay::pause(void)     			{ if (this->isPlaying()) this->dState.playState = PAUSED;   return; }
void DFPlay::resume(void)       		{ if (this->isPaused()) this->dState.playState = PLAYING;   return; }
void DFPlay::stop(void)         		{ if (this->isPlaying()) this->dState.playState = IDLE;  	return; }
void DFPlay::softStop(void)     		{ if (this->isPlaying()) this->dState.softStop = true;
    #ifdef LOGGING
        Serial.println("softStop requested ..."); // in a media or folder play, softStop stops the DFPlayer when the current track ends
    #endif
    return;
}
uint8_t DFPlay::setVolume(uint8_t vol)  { this->dState.volume = max(min(vol, 30), 0);        		return this->dState.volume; }
uint8_t DFPlay::volumeUp(void)          { this->dState.volume = (min(this->dState.volume +1,30));   return this->dState.volume; }
uint8_t DFPlay::volumeDown(void)        { this->dState.volume = (max(this->dState.volume -1, 0));   return this->dState.volume; }
uint8_t DFPlay::setEqualizer(uint8_t eq){ this->dState.equalizer = eq;      return this->dState.equalizer; }
uint8_t DFPlay::equalizerUp(void)       { this->dState.equalizer = (min(this->dState.equalizer +1,5)); return this->dState.equalizer; }
uint8_t DFPlay::equalizerDown(void)     { this->dState.equalizer = (max(this->dState.equalizer -1,0)); return this->dState.equalizer; }
void DFPlay::mute(void)               	{ this->dState.muted = true;        return; }
void DFPlay::unmute(void)             	{ this->dState.muted = false;       return; }
bool DFPlay::isMuted(void)      		{ return this->dState.muted; }
bool DFPlay::isIdle(void) 				{ if (this->dState.playState == IDLE) 		return true;    else return false; }
bool DFPlay::isPlaying(void)			{ if (this->dState.playState == PLAYING)	return true;  	else return false; }
bool DFPlay::isPaused(void) 			{ if (this->dState.playState == PAUSED) 	return true;    else return false; }


// ----------------------------------------------------------------------------------------------------------------		
// ------------------------------------------------ PRIVATE METHODS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------------------	


void DFPlay::manageDevice(void) {
    
// PROCESS DATA FRAMES RECEIVED FROM THE DFPLAYER
    while (Serial1.available()) {
		if(fx < RESPONSE_FRAME_SIZE) {
			frame[fx] = Serial1.read();
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
        	#ifdef LOGGING
        	    Serial.printf("Response: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\r", 
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
                if (frame[CMD] == 0x3a) {
                    if (frame[LSB] == USB) this->cState.usbAttached = true;
                    else if (frame[LSB] == SD)  this->cState.sdAttached = true;
                }
                
                // FRAME 0x3B - unsolicited announcment of USB or SD media removal
                else if (frame[CMD] == 0x3b) {
                    if (frame[LSB] == USB) this->cState.usbAttached = false;
                    else if (frame[LSB] == SD)  this->cState.sdAttached = false;
                }
                
                // FRAME 0x3C & 0x3D - unsolicited "end of track" announcements
                else if ((frame[CMD] == 0x3c) || (frame[CMD] == 0x3d)) {
                    // For MEDIA and FOLDER plays ...
    			    if ((this->cState.playType == FOLDER) || (this->cState.playType == MEDIA)) {
        			    // when softStop is desired, signal a hard-stop when the current track ends
        			    if (this->dState.softStop) {
        			        this->dState.playState = IDLE;
     			        } else if (this->cState.trackCount > 1) { 
         			        this->cState.trackCount--; // decrement file counter
         			    } else { // last track was played
         			        this->dState.playState = IDLE; // signal a hard stop
         			        this->cState.trackCount = 0;
         			        // DFPlayer requires extra time to process when stopping after the last track in a folder or on media
         			        this->cState.noSubmitsTil = millis() + 100; // see note 3 at the bottom of this file for more detail
         			    }
    			    }
					// For STANDARD, HUGE, and MP3 plays ...
    				else {
    				    this->cState.playState = IDLE;
                        this->dState.playState = IDLE;
    				    #ifdef LOGGING
        				    Serial.println("Track Play Complete ...");
        			    #endif
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
        		    // When the error code indicates a track play failure ...
        		    if ((frame[LSB] == 0x05) || (frame[LSB] == 0x06) || (frame[LSB] == 0x08)) {
        		        // When a STANDARD play fails, and the folder supports HUGE plays ... 
						if ((this->cState.playType == STANDARD) && (this->dState.selection.folder < 16)) {
						    // set the playState to FAILED ... to tell RULE B1 to trigger a HUGE play. 
							this->cState.playState = FAILED;
						} else { // reset everything to IDLE state
							this->cState.playState = IDLE;
							this->dState.playState = IDLE;
						}
			            #ifdef LOGGING
			                Serial.println("Track Play Failure ...");
			            #endif
                     }
        		}
        		
                // FRAME 0x47, 0x48, or 0x4E - solicited frames that return trackcount on USB, on SD, or in a folder
        		else if ((frame[CMD] == 0x47) || (frame[CMD] == 0x48) || (frame[CMD] == 0x4e)) {
        		    this->cState.trackCount = ((frame[MSB] * 256) + frame[LSB]);
        			if (this->cState.trackCount == 0) {
    			        this->cState.playState = IDLE;
                        this->dState.playState = IDLE;
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
    } // end of while (Serial1.available()) ...
    
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

    // RULE A1 - Return to loop() when DFPlayer is IDLE and IDLE is the desired state
	if ((this->cState.playState == IDLE) && (this->dState.playState == IDLE)) return;
    
	// RULE A2 - Return to loop() if DFPlayer has no media to play
	if ((! this->cState.usbAttached) && (! this->cState.sdAttached)) return;
	
	// RULE A3 - Return to loop() if it is too soon to submit another command to the DFPlayer
    if (millis() < this->cState.noSubmitsTil) return;

	#ifdef LOGGING
		if (this->dState.newSelection == true) {
			Serial.printf("\nSelection: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		}
	#endif

// ASSESS STATE VARIABLES AND ADJUST STOP, PAUSE/RESUME, VOLUME, AND EQUALIZER SETTINGS AS REQUIRED TO ALIGN WITH DESIRED STATE
    // Trigger actions affecting current play or in preparation to play a new selection -----------------------------------------------

	// RULE B1 - Play HUGE when a STANDARD play fails and the folder supports 4-digit file name prefixes 
	if (this->cState.playState == FAILED) { // FAILED flag is set when 0x40 frame was received from DFPlayer
		#ifdef LOGGING
			Serial.printf("Play Huge: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
							this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t dbyte = ((this->dState.selection.folder * 4096) + this->dState.selection.track);
		uint16_t cs = 0xFEFB - (0x14 + (dbyte / 256) + (dbyte % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x14, 0x00, (uint8_t)(dbyte / 256), (uint8_t)(dbyte % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playType = HUGE;
		this->cState.playState = PLAYING;
		return;
	}
 
    // RULE B2 - Make sure DFPlayer is stopped when IDLE is the desired playState
	if ((this->dState.playState == IDLE) && (this->cState.playState != IDLE)) {
		#ifdef LOGGING
			Serial.println("Stop");
		#endif
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x16, 0x00, 0x00, 0x00, 0xFE, 0xE5, 0xEF };
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = IDLE;
		return;
   }

   // RULE B3 - PAUSE when PLAYING and desired state is PAUSED
    if ((this->dState.playState == PAUSED) && (this->cState.playState == PLAYING)) {
        #ifdef LOGGING
            Serial.println("Pause");
        #endif
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0E, 0x00, 0x00, 0x00, 0xFE, 0xED, 0xEF };
        submitRequest(request, SUBMIT_INTERVAL);
        this->cState.playState = PAUSED;
        return;
    }

    // RULE B4 - Assure DFPlayer volume is set to the desired volume
	uint8_t desiredVolume = 0;
    if ( !this->dState.muted) {
        desiredVolume = max(min((this->dState.volume + this->dState.selection.volAdj), 30), 0);
	}
	if (desiredVolume != this->cState.volume) {
		#ifdef LOGGING
			Serial.println("setVolume");
		#endif
		uint16_t cs = 0xFEFB - (0x06 + desiredVolume); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x06, 0x00, 0x00, desiredVolume, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.volume = desiredVolume;
		return;
	}

    // RULE B5 - Assure DFPlayer equalizer is set to the desired state
    uint8_t desiredEqualizer = 0;
    if (this->dState.selection.equalizer == 0) desiredEqualizer = this->dState.equalizer;
	else desiredEqualizer = this->dState.selection.equalizer;
    if (this->cState.equalizer != desiredEqualizer) {
        #ifdef LOGGING
            Serial.println("setEqualizer");
        #endif
        uint16_t cs = 0xFEFB - (0x07 + desiredEqualizer); // compute checksum
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x07, 0x00, 0x00, desiredEqualizer, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF };
        submitRequest(request,SUBMIT_INTERVAL);
        this->cState.equalizer = desiredEqualizer;
        return;
    }

    // RULE B6 - Resume when DFPlayer is PAUSED and desired state is PLAYING
    if ((this->cState.playState == PAUSED) && (this->dState.playState == PLAYING)) {
        #ifdef LOGGING
            Serial.println("Resume");
        #endif
        uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0D, 0x00, 0x00, 0x00, 0xFE, 0xEE, 0xEF };
        submitRequest(request,SUBMIT_INTERVAL);
        this->cState.playState = PLAYING; 
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
		#ifdef LOGGING
			Serial.println("Invalid Media");
		#endif
		return;
	}

	// RULE C3 - Make sure selection.media is the selected media
	if (this->cState.media != this->dState.selection.media) {
		#ifdef LOGGING
			Serial.println("SelectMedia");
		#endif
		uint16_t cs = 0xFEFB - (0x09 + this->dState.selection.media); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x09, 0x00, 0x00, this->dState.selection.media, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.media = this->dState.selection.media;
		return;
    }
    // RULE C4 - Play all tracks on the media
	if (this->dState.selection.folder == 0) {
		if (this->cState.trackCount == 0) {
			#ifdef LOGGING
			    Serial.println("Query Track Count");
		    #endif
			if (this->dState.selection.media == USB)  {
				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x47, 0x00, 0x00, 0x00, 0xFE, 0xB4, 0xEF};
				submitRequest(request,SUBMIT_INTERVAL);
			} else { // selection media == SD
				uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x48, 0x00, 0x00, 0x00, 0xFE, 0xB3, 0xEF};
				submitRequest(request,SUBMIT_INTERVAL);
			}
			return;   
		}
		#ifdef LOGGING
			Serial.printf("Play Media: {%d,%d,%d,%d,%d} ... %d Tracks\n\r", 
			this->dState.selection.media, this->dState.selection.folder, this->dState.selection.track, 
			this->dState.selection.volAdj, this->dState.selection.equalizer, this->cState.trackCount);
		#endif
		uint16_t cs = 0xFEFB - (0x11 + this->dState.selection.media); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x11, 0x00, 0x00,  this->dState.selection.media, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = PLAYING;
		this->cState.playType = MEDIA;
		this->dState.newSelection = false;
		return;
	}   

	// RULE C5 - Play all tracks in a folder
	if (this->dState.selection.track == 0) {
		if (this->cState.trackCount == 0) { // query the number of files in the folder
			#ifdef LOGGING
			    Serial.println("Query Track Count");
		    #endif
			uint16_t cs = 0xFEFB - (0x4E + this->dState.selection.folder); // compute checksum
			uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x4E, 0x00, 0x00,  this->dState.selection.folder, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
			submitRequest(request,(int)SUBMIT_INTERVAL *5);
			return;
		}
		#ifdef LOGGING
			Serial.printf("Play Folder: {%d,%d,%d,%d,%d} ... %d Tracks\n\r", 
			this->dState.selection.media, this->dState.selection.folder, this->dState.selection.track, 
			this->dState.selection.volAdj, this->dState.selection.equalizer, this->cState.trackCount);
		#endif
		uint16_t cs = 0xFEFB - (0x17 + this->dState.selection.folder); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x17, 0x00, 0x00,  this->dState.selection.folder, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = PLAYING;
		this->cState.playType = FOLDER;
		this->dState.newSelection = false;
		return;
	}

	// RULE C6 - Play a HUGE track (HUGE tracks have a 4-digit file-name prefix)
	if (this->dState.selection.track > 255) {
		#ifdef LOGGING
			Serial.printf("Play Huge: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t dbyte = ((this->dState.selection.folder * 4096) + this->dState.selection.track);
		uint16_t cs = 0xFEFB - (0x14 + (dbyte / 256) + (dbyte % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x14, 0x00, (uint8_t)(dbyte / 256), (uint8_t)(dbyte % 256), (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = PLAYING;
		this->cState.playType = HUGE;
		this->dState.newSelection = false;
		return;
	}

	// RULE C7 - Play a track from the MP3 folder
    if (this->dState.selection.folder == 100) {
		#ifdef LOGGING
			Serial.printf("Play MP3: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
			this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
		#endif
		uint16_t cs = 0xFEFB - (0x12 + (this->dState.selection.track / 256) + (this->dState.selection.track % 256)); // compute checksum
		uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x12, 0x00, (uint8_t)this->dState.selection.track / 256, 
		    (uint8_t)this->dState.selection.track % 256, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
		submitRequest(request,SUBMIT_INTERVAL);
		this->cState.playState = PLAYING;
		this->cState.playType = MP3;
		this->dState.newSelection = false;
		return;
    }

	// RULE C8 - Play a STANDARD track (STANDARD tracks have a 3-digit file-name prefix)
	#ifdef LOGGING
		Serial.printf("Play Standard: {%d,%d,%d,%d,%d}\n\r", this->dState.selection.media, this->dState.selection.folder, 
		this->dState.selection.track, this->dState.selection.volAdj, this->dState.selection.equalizer);
	#endif
	uint16_t cs = 0xFEFB - (0x0F + this->dState.selection.folder + this->dState.selection.track); // compute checksum
	uint8_t request[] = { 0x7E, 0xFF, 0x06, 0x0F, 0x00, this->dState.selection.folder, this->dState.selection.track, (uint8_t)(cs / 256), (uint8_t)(cs % 256), 0xEF};
	submitRequest(request,SUBMIT_INTERVAL);
	this->cState.playState = PLAYING;
	this->cState.playType = STANDARD;
	this->dState.newSelection = false;
	return;
}  
    
/* ================================================================================================ submitRequest()
                                    Write Request Frames to Serial 1
   ================================================================================================================
*/
void DFPlay::submitRequest(uint8_t request[] ,uint8_t dlay) {
    uint8_t requestLength = request[LEN] + 4;
    #ifdef LOGGING
        Serial.print(" Request:");
        for (int i = 0; i<requestLength; i++) {
            Serial.printf(" %02x", request[i]);
        }
        Serial.println();
    #endif
    Serial1.write(request,requestLength);
    this->cState.noSubmitsTil = millis() + dlay;
} 

    
/*  DFPlayer Notes:
    1.  This program uses the "noSubmitTil" variable to manage the timing of commands sent to the DFPlayer. The 
        minimum interval is defined by the SUBMIT_INTERVAL constant ... 30ms by default. 
    2.  When querying the number of tracks in a folder, an extra-long interval is required (5 times the normal)
    3.  In most circumstances, a 0x16 (stop) command can be issued immediately after a 0x3c/d/e (end-of-play) frame  
        is received, but there is a timing problem when the 0x17 command is playing a folder and we want to stop
        after the last track in the folder. In this case, we need to insert a 100-250ms delay before the 0x16 (stop)
        command is issued.
	    TIMING ADJUSTMENTS MAY BE REQUIRED IN SCENDARIOS WHERE VERY LARGE TRACK COUNTS ARE USED. 
	4.  When playing single tracks, DFPlayer returns duplicate end-of-track frames. You will see this if LOGGING is
	    enabled. This library does not filter out the duplicates since that would take more resources than just
	    processing them. 
*/    
    
    
    
    