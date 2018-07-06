#include "DFPlay.h"


DFPlay dfPlay;
void setup() {
    Serial.begin(115200);
    dfPlay.begin();
    dfPlay.setVolume(15);
    Selection SDcard = {2,0,0,0,0}; // Selects all tracks on the SD card
    dfPlay.play(SDcard);
}

void loop() {
    dfPlay.manageDevice();
}












