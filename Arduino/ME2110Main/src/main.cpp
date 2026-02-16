#include <myDuino.h>

void setup(){
    Serial.begin(9600); //consider changing the baud rate for faster serial communication
    //Serial.begin(115200); // Uncomment this line if you set upload_speed to 115200 in platformio.ini
    Serial.println("main robot code running");
}