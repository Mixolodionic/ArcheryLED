//Copyright (C) 2010-2020  Henk Jegers
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


// MEGA_segments_matrix Prints the time on a 2x2 matrix which is part of a full matrix. 
// Uses input from ESP32_Fullmatrix_receiver via I2C    
// initially meant for ArcheryClock outdoor full matrix. 

// version MEGA_segments_matrix_001: Initial version


#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>

#include <fonts/ACFont20x32.h>
#include <fonts/ACFont20x16.h>
int outdoorcontrast=255;
int indoorcontrast=100;
int blinkdownpercentage=10; 
int timesec=0;
int ptimesec=0;
int timemin=0;
int ptimemin=0;

int segments1=0;
int segments2=0;
int segments3=0;
int seconds=0;
int Brightness=255;
bool retrievestate;
bool wifierrorstate;
bool blinkdim;
bool minsec;
bool contrast;

SoftDMD dmd(2,2);  // DMD controls the entire display
void setup()
{
  Wire.begin(9);                // join i2c bus with address #9
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  dmd.setBrightness(outdoorcontrast);
  dmd.selectFont(ACFont20x32);
  dmd.begin();



}

void loop()
{
  if(timesec==110){retrievestate=true;}else{retrievestate=false;};
  if(timesec==122){wifierrorstate=true;}else{wifierrorstate=false;};
  segments1=(timemin&B00001111)%10;
  seconds=timesec&B01111111;
  segments3=seconds %10;
  segments2=(seconds-segments3)/10;
  minsec=(timemin>>6)&B00000001;
  blinkdim=(timemin>>5)&B00000001;
  contrast=(timemin>>4)&B00000001;
  if (contrast){Brightness=indoorcontrast;}else{Brightness=outdoorcontrast;};
  if (!blinkdim){Brightness=Brightness*blinkdownpercentage/100;};
  dmd.setBrightness(Brightness);
  
if ((timesec!=ptimesec)||(timemin!=ptimemin)){
  ptimesec=timesec;
  ptimemin=timemin;
  if ((retrievestate)||(wifierrorstate)){
    dmd.selectFont(ACFont20x16);
    dmd.drawFilledBox(0,0,63,7, GRAPHICS_OFF); 
    dmd.drawFilledBox(0,0,0,31, GRAPHICS_OFF);         
    if(retrievestate){dmd.drawString(2,8,":;?");};
    if(wifierrorstate){dmd.drawString(2,8,"ABC");};
    dmd.drawFilledBox(0,24,63,31, GRAPHICS_OFF);
           

  }else{
    if (timesec==120){
      dmd.drawFilledBox(0,0,63,31, GRAPHICS_OFF);
      }else{
    dmd.selectFont(ACFont20x32);
    if (minsec){
      dmd.drawString(-1,-2,String(segments1));
      dmd.drawFilledBox(20,0,22,6, GRAPHICS_OFF); 
      dmd.drawFilledBox(20,7,22,9); 
      dmd.drawFilledBox(20,10,22,18, GRAPHICS_OFF); 
      dmd.drawFilledBox(20,19,22,21); 
      dmd.drawFilledBox(20,22,22,31, GRAPHICS_OFF); 
      dmd.drawString(24,-2,String(segments2));
      dmd.drawString(45,-2,String(segments3));
    }else{
      if(segments1!=0){
        dmd.drawFilledBox(0,0,0,31, GRAPHICS_OFF); 
        dmd.drawString(1,-2,String(segments1));
        dmd.drawString(22,-2,String(segments2));
        dmd.drawString(43,-2,String(segments3));
      }else{
        if(segments2!=0){
          dmd.drawFilledBox(0,0,11,31, GRAPHICS_OFF); 
          dmd.drawString(12,-2,String(segments2));
          dmd.drawString(33,-2,String(segments3));
          dmd.drawFilledBox(54,0,63,31, GRAPHICS_OFF); 
        }else{
          dmd.drawFilledBox(0,0,21,31, GRAPHICS_OFF); 
          dmd.drawString(22,-2,String(segments3));
          dmd.drawFilledBox(43,0,63,31, GRAPHICS_OFF); 
        }
      }
    }; 
    };   
  };
  
 }; 


}


void receiveEvent(int howMany)
{

  int x = Wire.read();    // receive byte as an integer
  delay(1);
  Serial.println(x);         // print the integer
  delay(1);
  if ((x>>7)&B00000001){timemin=x;}else{timesec=x;}
}
