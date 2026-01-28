//Copyright (C) 2010-2025  Henk Jegers
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

//for questions email to info@archeryclock.com  


// ESP32 archeryclock receiver transfers the broadcasted WLAN signal to a usable signal for ArcheryClock outdoor clock
// initially meant for ArcheryClock outdoor full matrix. 

// version ESP32_Fullmatrix_receiver_001 develop version 
// version ESP32_Fullmatrix_receiver_002 Initial version 
// version ESP32_Fullmatrix_receiver_003 added wifi reconnect in case wifi lost during operation. 
// version ESP32_Fullmatrix_receiver_003B Changed sound driver (ledc) since newer ESP version can not use original one in future A and B versions will exist since old version of ESP don't support new 'ledc'usage.
// version ESP32_Fullmatrix_receiver_004 (This one B version for new ESP type) Added the runoutof time with different sound function. Introduced at ArcheryClock 2.6.1.13

//settings

#include "WiFi.h"
#include "AsyncUDP.h"
#include <Wire.h>

//Set the correct SSID and PassWord of your router
const char * ssid = "ArcheryClock";
const char * password = "ArcheryClock94";

//Set the correct port number. Default value used in ArcheryClock software is port 4665
const int port = 4665;


unsigned long previousMillis = 0;
unsigned long interval = 3500;

int serialvalue0;
int serialvalue1;
byte serialvalue2;
byte serialvalue3;
byte serialvalue4;
byte serialvalue5;
byte serialvalue6;
byte serialvalue7;
byte serialvalue8;
byte serialvalue9;
byte serialvalue10;
byte serialvalue11;
byte serialvalue12;
byte serialvalue13;
byte serialvalue14;
byte serialvalue15;

byte ptimesec=0;
byte ntimesec=0;
byte ptimemin=0;
byte ntimemin=0;
byte ptraffic=0;
byte ntraffic=0;
byte psequence=0;
byte nsequence=0;
byte pshoot1=0;
byte pshoot2=0;
byte pshoot3=0;
byte pshoot4=0;
byte pshoot5=0;
byte pshoot6=0;
byte nshoot1=0;
byte nshoot2=0;
byte nshoot3=0;
byte nshoot4=0;
byte nshoot5=0;
byte nshoot6=0;
byte pendr=0;
byte nendr=0;
byte pendl=0;
byte nendl=0;
int leftsec;
int rightsec;
int leftmin;
int rightmin;

int state;
int archerysystem;
int sequenceinfo;
int turnnr=0;
int archersperend;

int freq = 2000;
int channel =0;
int resolution = 8;

int seqrepeatloop=0;
int seqloopcount=0;
int digrepeatloop=0;
int digrepeatminloop=0;
int digloopcount=0;
int maxrepeatloop=12;
int maxrepeatdigloop=80;
int timestart;
int timenow;
bool red;
bool yel;
bool grn;
bool redleft;
bool redright;
bool yelleft;
bool yelright;
bool grnleft;
bool grnright;
bool minsec;
bool blinkdim;
bool buzzer;
bool runouttime;
bool pbuz;
bool leftactive;
bool rightactive;



AsyncUDP udp;

void setup()
{
 
  ledcAttach (23, freq, resolution);  
  pinMode(25, INPUT_PULLDOWN); //left or right clock
  pinMode(32, INPUT_PULLDOWN); //contrast
  pinMode(27, INPUT_PULLDOWN); //buzzer on or off
  pinMode(15, OUTPUT);  //buzzer (switching)output
  pinMode(16, OUTPUT);  //buzzer for runoutof time (switching)output

  Wire.begin(); // join i2c bus (address optional for master)
  
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        ntimesec=122;
        Wire.beginTransmission(9); // transmit to device #9
        delay(2);
        Wire.write(char(ntimesec));
        Wire.endTransmission();    // stop transmitting
        while(1) {
            delay(1000);
        }
    }

  
  

     if(udp.listenMulticast(IPAddress(224,25,26,27), port)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            const char rsp[20]={"Reply from ESP-1"};
            if ((packet.data()[0]=='T')and(packet.data()[1]=='S')){  
              //serialvalue=(packet.data);
              serialvalue0=String(packet.data()[0]).toInt();
              serialvalue1=String(packet.data()[1]).toInt();
              serialvalue2=String(packet.data()[2]).toInt();
              serialvalue3=String(packet.data()[3]).toInt();
              serialvalue4=String(packet.data()[4]).toInt();
              serialvalue5=String(packet.data()[5]).toInt();
              serialvalue6=String(packet.data()[6]).toInt();
              serialvalue7=String(packet.data()[7]).toInt();
              serialvalue8=String(packet.data()[8]).toInt();
              serialvalue9=String(packet.data()[9]).toInt();
              serialvalue10=String(packet.data()[10]).toInt();
              serialvalue11=String(packet.data()[11]).toInt();
              serialvalue12=String(packet.data()[12]).toInt();
              serialvalue13=String(packet.data()[13]).toInt();
              serialvalue14=String(packet.data()[14]).toInt();
          //    serialvalue15=String(packet.data()[15]).toInt();
            };
            

       });
        
    }



  
}

byte x = 0;

void loop()
{

    
leftsec=serialvalue4&B01111111;
rightsec=serialvalue5&B01111111;;
leftmin=serialvalue6&B00001111;
rightmin=serialvalue7&B00001111;
minsec=(serialvalue6>>6)&B00000001;
state=serialvalue14&B00000111;
archerysystem=(serialvalue14>>3)&B00000111;

turnnr=(serialvalue7>>4)&B00000111;
leftactive=(serialvalue12>>5)&B00000001;
rightactive=(serialvalue12>>4)&B00000001;
redleft=serialvalue3&B00000001;
redright=(serialvalue3>>3)&B00000001;
yelleft=(serialvalue3>>1)&B00000001;
yelright=(serialvalue3>>4)&B00000001;
grnleft=(serialvalue3>>2)&B00000001;
grnright=(serialvalue3>>5)&B00000001;
if ((digitalRead(27)||(timenow>=600))){buzzer=0;}else{buzzer=(serialvalue3>>6)&B00000001;};
runouttime=(serialvalue3>>7)&B00000001;
if (((serialvalue3>>6)&B00000001)==0){timenow=0;};
if (buzzer!=pbuz){
  pbuz=buzzer;
  timestart=millis();
  
  if (buzzer){
    if (runouttime){
      ledcWriteTone(23, 2200);
      digitalWrite(15, true);
    }else{
      ledcWriteTone(23, 1400);
      digitalWrite(16, true);
    }
  }else{
    ledcWriteTone(23, 0);
    digitalWrite(15, false);
    digitalWrite(16, false);
  };
  

}; 
if(buzzer==1){
  timenow=millis()-timestart;

}
if ((state==1)and((archerysystem!=2)or((archerysystem==2)and(!rightactive)and(!leftactive)))){ntimesec=110;}else{  //110 indicates retrieve arrows state.
 
  if (digitalRead(25)){
    if ((state==1)and(archerysystem==2)and(rightactive)){ntimesec=120;}else{ntimesec=leftsec;};
    ntimemin=leftmin;
    
  }else{
    if ((state==1)and(archerysystem==2)and(leftactive)){ntimesec=120;}else{ntimesec=rightsec;};
    ntimemin=rightmin;
    

  };
  Serial.println(ntimesec);
};  

if (digitalRead(25)){
  red=redleft;
  grn=grnleft;
  yel=yelleft;
}else{
  red=redright;
  grn=grnright;
  yel=yelright;
};

if (((state==2)or((state==1)and(archerysystem==2)))&&(red==0)){blinkdim=0;}else{blinkdim=1;};

ntraffic=ntraffic&B00111111; //set adresbit 001
ntraffic=ntraffic|B00100000; //set adresbit 001
if (buzzer){ntraffic=ntraffic|B00000001;}else{ntraffic=ntraffic&B11111110;};
if (red){ntraffic=ntraffic|B00000010;}else{ntraffic=ntraffic&B11111101;};
if (yel){ntraffic=ntraffic|B00000100;}else{ntraffic=ntraffic&B11111011;};
if (grn){ntraffic=ntraffic|B00001000;}else{ntraffic=ntraffic&B11110111;};
if (digitalRead(32)){ntraffic=ntraffic|B00010000;}else{ntraffic=ntraffic&B11101111;}; //switch pin 26 indicat high contrast (outdoor) or low contrast (indoor)


nsequence=nsequence&B01000000; //set adresbit 01
nsequence=nsequence|B01000000; //set adresbit 01
archersperend=(((serialvalue13&B00000111)+1)*((((serialvalue13)>>3)&B00000011)+1));

if (archerysystem==6){archerysystem=0;};
if (archerysystem==7){archerysystem=1;};
if ((archerysystem==1)&&(archersperend==1)){archerysystem=6;};
if (state==6){archerysystem=5;};

if (archerysystem==0){
  if(((turnnr==1)&&(serialvalue9&B00000111)==1)||((turnnr==2)&&((serialvalue9>>3)&B00000111)==1)){sequenceinfo=1;}else{sequenceinfo=2;};
};
if (archerysystem==1){sequenceinfo=archersperend;};
if (archerysystem==2){
  if (leftactive){sequenceinfo=2;}else{
    if(rightactive){sequenceinfo=1;}else{sequenceinfo=0;};


  };
};

if (archerysystem==3){
  sequenceinfo=(serialvalue12&B00111111);
  if (sequenceinfo==4){sequenceinfo=3;};
  if (sequenceinfo==8){sequenceinfo=4;};
  if (sequenceinfo==16){sequenceinfo=5;};
  if (sequenceinfo==32){sequenceinfo=6;};
};



nsequence=nsequence|(archerysystem);
nsequence=(nsequence|(sequenceinfo<<3));
nshoot1=nshoot1&B10000000; //set adresbit 1000
nshoot1=nshoot1|B10000000; //set adresbit 1000
nshoot1=nshoot1|(serialvalue9&B00000111);
if((serialvalue12>>((serialvalue9&B00000111)-1)&B00000001)==1){nshoot1=nshoot1|B00001000;}else{nshoot1=nshoot1&B11110111;};


nshoot2=nshoot2&B10010000; //set adresbit 1001
nshoot2=nshoot2|B10010000; //set adresbit 1001
nshoot2=nshoot2|((serialvalue9>>3)&B00000111);
if((serialvalue12>>(((serialvalue9>>3)&B00000111)-1)&B00000001)==1){nshoot2=nshoot2|B00001000;}else{nshoot2=nshoot2&B11110111;};
nshoot3=nshoot3&B10100000; //set adresbit 1010
nshoot3=nshoot3|B10100000; //set adresbit 1010
nshoot3=nshoot3|(serialvalue10&B00000111);
if((serialvalue12>>((serialvalue10&B00000111)-1)&B00000001)==1){nshoot3=nshoot3|B00001000;}else{nshoot3=nshoot3&B11110111;};
nshoot4=nshoot4&B10110000; //set adresbit 1011
nshoot4=nshoot4|B10110000; //set adresbit 1011
nshoot4=nshoot4|((serialvalue10>>3)&B00000111);
if((serialvalue12>>(((serialvalue10>>3)&B00000111)-1)&B00000001)==1){nshoot4=nshoot4|B00001000;}else{nshoot4=nshoot4&B11110111;};
nshoot5=nshoot5&B11000000; //set adresbit 1100
nshoot5=nshoot5|B11000000; //set adresbit 1100
nshoot5=nshoot5|(serialvalue11&B00000111);
if((serialvalue12>>((serialvalue11&B00000111)-1)&B00000001)==1){nshoot5=nshoot5|B00001000;}else{nshoot5=nshoot5&B11110111;};
nshoot6=nshoot6&B11010000; //set adresbit 1101
nshoot6=nshoot6|B11010000; //set adresbit 1101
nshoot6=nshoot6|((serialvalue11>>3)&B00000111);
if((serialvalue12>>(((serialvalue11>>3)&B00000111)-1)&B00000001)==1){nshoot6=nshoot6|B00001000;}else{nshoot6=nshoot6&B11110111;};

nendr=nendr&B11100000; //set adresbit 1110
nendr=nendr|B11100000; //set adresbit 1110
nendr=nendr|((serialvalue8&B01111111) %10);
nendl=nendl&B11110000; //set adresbit 1111
nendl=nendl|B11110000; //set adresbit 1111
nendl=nendl|(((serialvalue8&B01111111)-((serialvalue8&B01111111) %10))/10); // example, if 35 seconds and isolate 3, first subbtract 5 result in 30. divide by 10 is 3
if ((serialvalue12>>6)&B00000001){nendl=(nendl|15);}; //practise end


ntimemin=ntimemin|B10000000; //add an 1 as MSB as adress but. So the receiving party recognizes this as minute segment
if (minsec){ntimemin=ntimemin|B01000000;}else{ntimemin=ntimemin&B10111111;}; //2nd bit to indicate seconds or minute notation.
if (blinkdim){ntimemin=ntimemin|B00100000;}else{ntimemin=ntimemin&B11011111;}; //3rd bit to dim segments 50% if red light is blinking in red state
if (digitalRead(32)){ntimemin=ntimemin|B00010000;}else{ntimemin=ntimemin&B11101111;}; //switch pin 26 indicat high contrast (outdoor) or low contrast (indoor)

if ((ntraffic!=ptraffic)|((seqloopcount==1)&&(seqrepeatloop>=maxrepeatloop))){
  ptraffic=ntraffic;
  if (seqloopcount==1){seqloopcount=2;};
  seqrepeatloop=0;
  Wire.beginTransmission(4); // transmit to device #4
  delay(2);
  Wire.write(char(ntraffic));
  Wire.endTransmission();    // stop transmitting
}else{
  if ((nsequence!=psequence)|((seqloopcount==3)&&(seqrepeatloop>=maxrepeatloop))){
    if(nsequence!=psequence){
      pshoot1=0;
      pshoot2=0;
      pshoot3=0;
      pshoot4=0;
      pshoot5=0;
      pshoot6=0;
    };    
    psequence=nsequence;
    if (seqloopcount==3){
      if(archerysystem==1){seqloopcount=4;}else{seqloopcount=16;};
    };
    seqrepeatloop=0;
    Wire.beginTransmission(4); // transmit to device #4
    delay(2);
    Wire.write(char(nsequence));
    Wire.endTransmission();    // stop transmitting    
  }else{
    if (((nshoot1!=pshoot1)&&((archerysystem==1)and(archersperend>1)))|((seqloopcount==5)&&(seqrepeatloop>=maxrepeatloop))){
      pshoot1=nshoot1;
      if (seqloopcount==5){
        seqloopcount=6;
      };
      seqrepeatloop=0;
      
      Wire.beginTransmission(4); // transmit to device #4
      delay(2);
      Wire.write(char(nshoot1));
      Wire.endTransmission();    // stop transmitting        
    }else{
      if (((nshoot2!=pshoot2)&&((archerysystem==1)and(archersperend>1)))|((seqloopcount==7)&&(seqrepeatloop>=maxrepeatloop))){
        pshoot2=nshoot2;
        if (seqloopcount==7){
          if (archersperend>2){seqloopcount=8;}else{seqloopcount=16;};
        };
        seqrepeatloop=0;
        
        Wire.beginTransmission(4); // transmit to device #4
        delay(2);
        Wire.write(char(nshoot2));
        Wire.endTransmission();    // stop transmitting         
      }else{
        if (((nshoot3!=pshoot3)&&((archerysystem==1)and(archersperend>2)))|((seqloopcount==9)&&(seqrepeatloop>=maxrepeatloop))){
          pshoot3=nshoot3;
          if (seqloopcount==9){
            if (archersperend>3){seqloopcount=10;}else{seqloopcount=16;};
          };
          seqrepeatloop=0;
          Wire.beginTransmission(4); // transmit to device #4
          delay(2);
          Wire.write(char(nshoot3));
          Wire.endTransmission();    // stop transmitting                  
        }else{
          if (((nshoot4!=pshoot4)&&((archerysystem==1)and(archersperend>3)))|((seqloopcount==11)&&(seqrepeatloop>=maxrepeatloop))){
            pshoot4=nshoot4;
            if (seqloopcount==11){
              if (archersperend>4){seqloopcount=12;}else{seqloopcount=16;};
            };
            seqrepeatloop=0;
            Wire.beginTransmission(4); // transmit to device #4
            delay(2);
            Wire.write(char(nshoot4));
            Wire.endTransmission();    // stop transmitting                  
          }else{
            if (((nshoot5!=pshoot5)&&((archerysystem==1)and(archersperend>4)))|((seqloopcount==13)&&(seqrepeatloop>=maxrepeatloop))){
              pshoot5=nshoot5;
              if (seqloopcount==13){
                if (archersperend>5){seqloopcount=14;}else{seqloopcount=16;};
              };
              seqrepeatloop=0;
              Wire.beginTransmission(4); // transmit to device #4
              delay(2);
              Wire.write(char(nshoot5));
              Wire.endTransmission();    // stop transmitting                  
            }else{
              if (((nshoot6!=pshoot6)&&((archerysystem==1)and(archersperend>5)))|((seqloopcount==15)&&(seqrepeatloop>=maxrepeatloop))){
                pshoot6=nshoot6;
                if (seqloopcount==15){
                  seqloopcount=16;
                };
                seqrepeatloop=0;
                Wire.beginTransmission(4); // transmit to device #4
                delay(2);
                Wire.write(char(nshoot6));
                Wire.endTransmission();    // stop transmitting                  
              }else{
                if ((nendl!=pendl)|((seqloopcount==17)&&(seqrepeatloop>=maxrepeatloop))){
                  pendl=nendl;
                  if (seqloopcount==17){
                    seqloopcount=18;
                  };
                  seqrepeatloop=0;
                  Wire.beginTransmission(4); // transmit to device #4
                  delay(2);
                  Wire.write(char(nendl));
                  Wire.endTransmission();    // stop transmitting                  
                }else{
                  if ((nendr!=pendr)|((seqloopcount==19)&&(seqrepeatloop>=maxrepeatloop))){
                    pendr=nendr;
                    if (seqloopcount>=19){
                      seqloopcount=0;
                    };
                    seqrepeatloop=0;
                    
                    Wire.beginTransmission(4); // transmit to device #4
                    delay(2);
                    Wire.write(char(nendr));
                    Wire.endTransmission();    // stop transmitting                  
                  }else{
                    if (seqrepeatloop>=maxrepeatloop){
                      seqloopcount++;
                    }else{
                      seqrepeatloop++;
                    }                    
                  }
                }
              }
            }
          }
        }
      }
    }
  }

}



if ((ntimesec!=ptimesec)|((digloopcount==1)&&(digrepeatloop>=maxrepeatdigloop))){
  ptimesec=ntimesec;
  digloopcount=2;
  digrepeatloop=0;
  Wire.beginTransmission(9); // transmit to device #9
  delay(2);
  Wire.write(char(ntimesec));
  Wire.endTransmission();    // stop transmitting
}else{
  if ((ntimemin!=ptimemin)|((digloopcount==3)&&(digrepeatminloop>=maxrepeatdigloop))){
    ptimemin=ntimemin;
    if (digloopcount>=3){digloopcount=0;};
    digrepeatminloop=0;
    Wire.beginTransmission(9); // transmit to device #9
    delay(2);
    Wire.write(char(ntimemin));
    
    Wire.endTransmission();    // stop transmitting
  }else{
    if (digrepeatminloop<maxrepeatdigloop){  //(digrepeatloop>=maxrepeatdigloop)||
      digrepeatminloop++;
    }else{
      digloopcount=3; 
      digrepeatloop=(maxrepeatdigloop/2); 
    }
    if (digrepeatloop<maxrepeatdigloop){
      digrepeatloop++;  
    }else{
      digloopcount=1;
    }
    
  }
}
  

  

  x++;
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
  delay(60);
}
