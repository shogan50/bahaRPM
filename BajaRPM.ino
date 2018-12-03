//Reads curtis output on Can0 and responds to ODBII RPM and a few other requests.
//By Scott Hogan
//10/31/2018

#include "variant.h"
#include <due_can.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

int rpm = 100;
int amps = 0;
byte motorTemp = 0;
byte controlTemp = 0;
byte throttle = 0;
byte brake = 0;
byte TPS = 0;
long hb = 100;
long hb1 = 20000;

///////////////////////////Set this to your minimum desired RPM
int min_RPM = 650;


byte PID04 = 0b00010000; //Calculated engine load
byte PID05 = 0b00001000;  //Engine Coolant Temp
byte PID12 = 0b00010000;  //RPM
byte PID17 = 0b10000000;  //TPS
byte PID0_0 = PID04 & PID05;
byte PID0_1 = PID12;
byte PID0_2 = PID17;
byte PID0_3 = 0b00000000;


void setup()
{

  Serial.begin(115200);
  Serial.print("made it to setup");
  
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can0.begin(CAN_BPS_125K);
  //Can1.begin(CAN_BPS_500K);
  Can1.begin(CAN_BPS_500K);
  
  
  //By default there are 7 mailboxes for each device that are RX boxes
  //This sets each mailbox to have an open filter that will accept extended
  //or standard frames
  int filter;
  //extended
  for (filter = 0; filter < 3; filter++) {
	Can0.setRXFilter(filter, 0, 0, false);
	Can1.setRXFilter(filter, 0, 0, false);
  }  
  //standard
  //for (int filter = 3; filter < 7; filter++) {
	//Can0.setRXFilter(filter, 0, 0, false);
	//Can1.setRXFilter(filter, 0, 0, false);
  //}  
  Serial.print("finished setup");
  
}

void printFrame(CAN_FRAME &frame) {
   Serial.print("ID: 0x");
   Serial.print(frame.id, HEX);
   Serial.print(" Len: ");
   Serial.print(frame.length);
   Serial.print(" Data: 0x");
   for (int count = 0; count < frame.length; count++) {
       Serial.print(frame.data.bytes[count], HEX);
       Serial.print(" ");
   }
   Serial.print("\r\n");
}

void loop(){

  
  CAN_FRAME incoming;
  CAN_FRAME outgoing;


// read incoming from the curtis
  if (Can0.available() > 0) {
  	Can0.read(incoming);
    //printFrame(incoming);
    //Serial.print("CAN0 incoming\n");
    if (incoming.id == 0x601){
      //uncomment the following line for straight RPM with min_RPM set to 0
      //or set min_RPM to say 650 to not permit the RPM to be reported below 650.
      //rpm = max(min_RPM,incoming.data.bytes[1]+incoming.data.bytes[0]*256);

      // thuncomment this line to simply add min_RPM to the actual RPM
      rpm = incoming.data.bytes[1]+incoming.data.bytes[0]*256 + min_RPM;

      
      motorTemp = incoming.data.bytes[1] + incoming.data.bytes[0] * 256;
      amps = incoming.data.bytes[4]/10 + 256*incoming.data.bytes[5]/10;
    }
    if (incoming.id == 0x602){
      TPS = incoming.data.bytes[4];
      brake = incoming.data.bytes[5];
    }
  }

   // read ODB II query  
  if (Can1.available() > 0) {
  	Serial.print("CAN1 incoming\n");
  	Can1.read(incoming);
    printFrame(incoming);
    
    if (incoming.id == 0x7df){
      printFrame(incoming);
      outgoing.id = 0x7E8;
      outgoing.length = 8;
      outgoing.extended = false;
      outgoing.data.byte[0] = 0x03;  // additional bytes.  
      outgoing.data.byte[1] = 0x41;  // add 40h to the service, which is 1
      outgoing.data.byte[4] = 0x55;
      outgoing.data.byte[5] = 0x55;
      outgoing.data.byte[6] = 0x55;
      outgoing.data.byte[7] = 0x55;
      outgoing.priority = 4; //0-15 lower is higher priority
      // if this is a PID 0 request (looking for capabilities)
        if (incoming.data.bytes[1] == 1 && incoming.data.bytes[2] == 0 ){
            outgoing.data.byte[0] = 0x06;  // additional bytes.  No idea what this is
            outgoing.data.byte[2] = 0x00;  // PID code
            outgoing.data.byte[3] = PID0_0;
            outgoing.data.byte[4] = PID0_1;
            outgoing.data.byte[5] = PID0_2;
            outgoing.data.byte[6] = PID0_3;
         
        }
        // if this is an engine load request
        if (incoming.data.bytes[1] == 1 && incoming.data.bytes[2] == 0x4) {
            outgoing.data.byte[2] = 0x4;  // PID code
            outgoing.data.byte[3] = TPS;  //use TPS for now.  Need more info about system to calc
  
        }
        // if this is a coolant temperature request
        if (incoming.data.bytes[1] == 1 && incoming.data.bytes[2] == 5) {
            outgoing.data.byte[2] = 0x5;  // PID code
            outgoing.data.byte[3] = motorTemp; 
    
        }
        if (incoming.data.bytes[1] == 1 && incoming.data.bytes[2] == 0xc) {
            //rpm = (650 + millis()/10) % 5000;
            outgoing.data.byte[0] = 0x04;  // additional bytes.  
            outgoing.data.byte[2] = 0xc;  // PID code
            outgoing.data.byte[3] = rpm * 4 / 256;  
            outgoing.data.byte[4] = (rpm * 4) % 256;
  
        }
        // if this is a throttle position request
        if (incoming.data.bytes[1] == 1 && incoming.data.bytes[2] == 0x11) {
            //TPS = (millis()/100)%255;
            outgoing.data.byte[2] = 0x11;  // PID code
            outgoing.data.byte[3] = TPS; 
  
        }
        Can1.sendFrame(outgoing);
        Serial.print("responding\n");
        printFrame(outgoing);
  
    }
  }
}


