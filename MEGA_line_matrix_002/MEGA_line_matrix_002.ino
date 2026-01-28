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

//settings
String language = "Eng"; // "Eng" for English, "Dut" for Dutch, "Ita" for Italian. 
char TLconf = 'R'; // R is traffic lights on right side of digits. L is traffic lights on left side of digits. 

#include <Wire.h>
#include <SPI.h>
#include <DMD2.h>


#include <fonts/ACFont8x13.h>
#include <fonts/ACFont11x16.h>
#include <fonts/ACFont16x16.h>
#include <fonts/ACFont20x16.h>
char shtr1val; //value shooter 1 (ABC...)
char shtr2val; //value shooter 2 (ABC...)
char shtr3val; //value shooter 3 (ABC...)
char shtr4val; //value shooter 4 (ABC...)
char shtr5val; //value shooter 5 (ABC...)
char shtr6val; //value shooter 6 (ABC...)
int outdoorcontrast=255;
int indoorcontrast=50;
int blinkdownpercentage=10; 
int traffic=0;
int sequence=6;
int nshooter1=0;
int nshooter2=0;
int nshooter3=0;
int nshooter4=0;
int nshooter5=0;
int nshooter6=0;
int pshooter1=0;
int pshooter2=0;
int pshooter3=0;
int pshooter4=0;
int pshooter5=0;
int pshooter6=0;
int endr=0;
int endl=0;
int nendr=0;
int nendl=0;
int pendr=0;
int pendl=0;
int natype=9; // type of match (fita, 25m1p, etc.)
int patype=9; // type of match (fita, 25m1p, etc.)
int nseqval=0; 
int pseqval=0; 
int redx;
int yelx;
int grnx;
int linex25m;
int linex;
int endx;
int soundcount=0;
int timestart;
int timenow;
bool ncontrast;
bool pcontrast;
bool nbuz;
bool pbuz;
bool nred;
bool pred;
bool nyel;
bool pyel;
bool ngrn;
bool pgrn;


char stopbrightup='N';
int stopbrightval=255;
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
bool blinkdim;
bool minsec;



SoftDMD dmd(5,1);  // DMD controls the entire display
void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(115200);           // start serial for output
  dmd.setBrightness(outdoorcontrast);
  dmd.selectFont(ACFont16x16);
  dmd.begin();
  if (TLconf=='R'){
    redx=145;
    yelx=113;
    grnx=81;
    linex=0;
    linex25m=0;
    endx=97;
  }else{
    redx=0;
    yelx=32;
    grnx=64;
    linex=96;
    linex25m=80;
    endx=48;    
  };



}

void loop()
{
  nbuz=traffic&B00000001;
  nred=(traffic>>1)&B00000001;
  nyel=(traffic>>2)&B00000001;
  ngrn=(traffic>>3)&B00000001;
  ncontrast=(traffic>>4)&B00000001;
  natype=sequence&B00000111;
  nseqval=(sequence>>3)&B00000111;
  nendr=(endr&B00001111);  
  nendl=(endl&B00001111); 
  if (nbuz!=pbuz){
    pbuz=nbuz;
  }; 

  if (ncontrast){Brightness=indoorcontrast;}else{Brightness=outdoorcontrast;};
  dmd.setBrightness(Brightness);
  
if (nred!=pred){
  pred=nred;
 
  if (nred){
    dmd.selectFont(ACFont16x16);
    dmd.drawString(redx,-1,"@");
  }else{
    dmd.selectFont(ACFont16x16);
    dmd.drawFilledBox(redx,0,redx+15,15, GRAPHICS_OFF);

  };  
 }; 

if (nyel!=pyel){
  pyel=nyel;
 
  if (nyel==true){
    dmd.selectFont(ACFont16x16);
    dmd.drawString(yelx,-1,"@");
  }else{
    dmd.selectFont(ACFont16x16);
    dmd.drawFilledBox(yelx,0,yelx+15,15, GRAPHICS_OFF);

  };  
 }; 

if (ngrn!=pgrn){
  pgrn=ngrn;
 
  if (ngrn==true){
    dmd.selectFont(ACFont16x16);
    dmd.drawString(grnx,-1,"@");
  }else{
    dmd.selectFont(ACFont16x16);
    dmd.drawFilledBox(grnx,0,grnx+15,15, GRAPHICS_OFF);
  };  
 }; 

if ((natype!=patype)or(nseqval!=pseqval)){
  if (((natype==1)&&(patype!=1))||(nseqval!=pseqval)){
    nshooter1=0;
    nshooter2=0;
    nshooter3=0;
    nshooter4=0;
    nshooter5=0;
    nshooter6=0;
  }
  if (natype==0){ //TOP-Bottom modus
    dmd.selectFont(ACFont11x16);
    if (nseqval==1){
      if (language=="Eng"){dmd.drawString(linex-1,0,"BOTTOM");};
      if (language=="Dut"){
        dmd.drawFilledBox(linex,0,linex+3,16, GRAPHICS_OFF);
        dmd.drawFilledBox(linex+60,0,linex+64,16, GRAPHICS_OFF);          
        dmd.drawString(linex+5,0,"ONDER");
      }; 
      if (language=="Ita"){
        dmd.drawFilledBox(linex,0,linex+3,16, GRAPHICS_OFF);
        dmd.drawFilledBox(linex+60,0,linex+64,16, GRAPHICS_OFF);          
        dmd.drawString(linex+5,0,"BASSO");
      }; 
    }else{
      if (language=="Eng"){
        dmd.drawFilledBox(linex,0,linex+15,16, GRAPHICS_OFF);
        dmd.drawFilledBox(linex+49,0,linex+64,16, GRAPHICS_OFF);
        dmd.drawString(linex+16,0,"TOP");
      };
      if (language=="Dut"){
        dmd.drawFilledBox(linex,0,linex+3,16, GRAPHICS_OFF);
        dmd.drawFilledBox(linex+60,0,linex+64,16, GRAPHICS_OFF);          
        dmd.drawString(linex+5,0,"BOVEN");
      };  
      if (language=="Ita"){
        dmd.drawFilledBox(linex,0,linex+8,16, GRAPHICS_OFF);
        dmd.drawFilledBox(linex+54,0,linex+64,16, GRAPHICS_OFF);          
        dmd.drawString(linex+10,0,"ALTO");
      };  
    }
  }
 
  if (natype==1){  //indoor outdoor
    if (nseqval<5){dmd.selectFont(ACFont16x16);}else{dmd.selectFont(ACFont11x16);};    //size of font depend on number of characters to print
    if (nseqval==1){dmd.drawFilledBox(linex,0,linex+63,15, GRAPHICS_OFF);};                                      //print nothing (remove existing text) since there is only one archer per end
    if (nseqval==2){
      dmd.drawFilledBox(linex,0,linex+15,15, GRAPHICS_OFF);
      dmd.drawFilledBox(linex+48,0,linex+63,15, GRAPHICS_OFF);
    };    
    if (nseqval==3){
      dmd.drawFilledBox(linex,0,linex+8,15, GRAPHICS_OFF);
      dmd.drawFilledBox(linex+57,0,linex+63,15, GRAPHICS_OFF);
    };    
    if (nseqval==5){
      dmd.drawFilledBox(linex,0,linex+4,15, GRAPHICS_OFF);
      dmd.drawFilledBox(linex+60,0,linex+63,15, GRAPHICS_OFF);
    };    
  }
  if (natype==2){ //alternating
    dmd.selectFont(ACFont20x16);
    if(nseqval==0){  //remove arrows
      dmd.drawString(linex,0,"=");  
      dmd.drawString(linex+44,0,"=");
    }
    if(nseqval==2){ //left
      dmd.drawString(linex,0,"<");  //print left arrow
      dmd.drawString(linex+44,0,"="); //remove right arrow
    }
    if(nseqval==1){ //right
      dmd.drawString(linex,0,"=");  //remove left arrow
      dmd.drawString(linex+44,0,">"); //print right arrow
    }
    dmd.selectFont(ACFont11x16);
  }
  if (natype==3){ // 25m1p 
    dmd.selectFont(ACFont8x13);
    dmd.drawFilledBox(linex25m,0,linex25m+63,2, GRAPHICS_OFF);
    if (language=="Eng"){
      dmd.drawString(linex25m,3," ARCHER ");
    }
    if (language=="Dut"){
      dmd.drawString(linex25m,3,"SCHUTTER");
    }
    if (language=="Ita"){
      dmd.drawString(linex25m,3," ARCIERE");
    }
    dmd.selectFont(ACFont11x16);
    dmd.drawString(linex25m+66,0,String(nseqval));
  }else{
    if (TLconf=='R'){
      dmd.drawFilledBox(linex25m+64,0,linex25m+79,15, GRAPHICS_OFF);
    }else{
      dmd.drawFilledBox(linex25m,0,linex25m+15,15, GRAPHICS_OFF);
    }
    
  }
  if (natype==5){  //alarm state
    dmd.selectFont(ACFont16x16);


    dmd.drawString(linex,0,"STOP");
    
  }else{
    dmd.setBrightness(Brightness);
  }
  if (natype==6){ 
    dmd.drawFilledBox(linex,0,linex+63,15, GRAPHICS_OFF);
  }
  
   
  patype=natype;
  pseqval=nseqval;
  
}

if (natype==5){  //alarm state
  dmd.selectFont(ACFont16x16);
  timenow=millis()-timestart;
  if (stopbrightup=='Y'){
    if (timenow>500){      
      timestart=millis();
      stopbrightup='N';      
    };
  }else{
    if (timenow>500){      
      timestart=millis();
      stopbrightup='Y';      
    };
  };
  if (stopbrightup=='N'){dmd.drawFilledBox(linex,0,linex+64,15, GRAPHICS_OFF);}else{dmd.drawString(linex,0,"STOP");}
  }


if ((nendl!=pendl)or(nendr!=pendr)){
  if (natype==2){ //alternating, endnr at bottom
    dmd.selectFont(ACFont11x16);
    dmd.drawFilledBox(endx,0,endx+15,15, GRAPHICS_OFF);
    if (nendl==0){
      dmd.drawFilledBox(linex+21,0,linex+25,15, GRAPHICS_OFF);
      dmd.drawFilledBox(linex+38,0,linex+42,15, GRAPHICS_OFF);
      dmd.drawString(linex+27,0,String(nendr));
    }else{
      dmd.drawString(linex+21,0,String(nendl));
      dmd.drawString(linex+32,0,String(nendr));
    }
  }else{
    
    if (nendl==0){
      dmd.selectFont(ACFont11x16);
      dmd.drawFilledBox(endx,0,endx+2,15, GRAPHICS_OFF);
      dmd.drawFilledBox(endx+13,0,endx+15,15, GRAPHICS_OFF);
      dmd.drawString(endx+3,0,String(nendr));
    }else{
      
      dmd.selectFont(ACFont8x13);
      dmd.drawFilledBox(endx,0,endx+15,2, GRAPHICS_OFF);
      if (nendl==15){dmd.drawString(endx,3,"P");}else{dmd.drawString(endx,3,String(nendl));};
      
      dmd.drawString(endx+8,3,String(nendr));
    }
  }
}


if ((natype==1)and(nseqval>1)){
  if(nseqval<5){dmd.selectFont(ACFont16x16);}else{dmd.selectFont(ACFont11x16);};    //size of font depend on number of characters to print
  if(nshooter1!=pshooter1){
    pshooter1=nshooter1;
    if ((nshooter1>>3)&B00000001){shtr1val=((nshooter1)&B00000111)+64;}else{shtr1val=((nshooter1)&B00000111)+96;}; 
    if (nseqval==2){dmd.drawString(linex+16,0,String(shtr1val));};
    if (nseqval==3){dmd.drawString(linex+9,0,String(shtr1val));};
    if (nseqval==4){dmd.drawString(linex,0,String(shtr1val));};
    if (nseqval==5){dmd.drawString(linex+5,0,String(shtr1val));};
    if (nseqval==6){dmd.drawString(linex-1,0,String(shtr1val));};
  };
  if(nshooter2!=pshooter2){
    pshooter2=nshooter2;
    if ((nshooter2>>3)&B00000001){shtr2val=((nshooter2)&B00000111)+64;}else{shtr2val=((nshooter2)&B00000111)+96;}; 
    if (nseqval==2){dmd.drawString(linex+32,0,String(shtr2val));};
    if (nseqval==3){dmd.drawString(linex+25,0,String(shtr2val));};
    if (nseqval==4){dmd.drawString(linex+16,0,String(shtr2val));};
    if (nseqval==5){dmd.drawString(linex+16,0,String(shtr2val));};
    if (nseqval==6){dmd.drawString(linex+10,0,String(shtr2val));};
  };
  if(nshooter3!=pshooter3){
    pshooter3=nshooter3;
    if ((nshooter3>>3)&B00000001){shtr3val=((nshooter3)&B00000111)+64;}else{shtr3val=((nshooter3)&B00000111)+96;}; 
    if (nseqval==3){dmd.drawString(linex+41,0,String(shtr3val));};
    if (nseqval==4){dmd.drawString(linex+32,0,String(shtr3val));};
    if (nseqval==5){dmd.drawString(linex+27,0,String(shtr3val));};
    if (nseqval==6){dmd.drawString(linex+21,0,String(shtr3val));};
  //  Serial.println(shtr3val);
  };
  if(nshooter4!=pshooter4){
    pshooter4=nshooter4;
    if ((nshooter4>>3)&B00000001){shtr4val=((nshooter4)&B00000111)+64;}else{shtr4val=((nshooter4)&B00000111)+96;}; 
    if (nseqval==4){dmd.drawString(linex+48,0,String(shtr4val));};
    if (nseqval==5){dmd.drawString(linex+38,0,String(shtr4val));};
    if (nseqval==6){dmd.drawString(linex+32,0,String(shtr4val));};
  };
  if(nshooter5!=pshooter5){
    pshooter5=nshooter5;
    if ((nshooter5>>3)&B00000001){shtr5val=((nshooter5)&B00000111)+64;}else{shtr5val=((nshooter5)&B00000111)+96;}; 
    if (nseqval==5){dmd.drawString(linex+49,0,String(shtr5val));};
    if (nseqval==6){dmd.drawString(linex+43,0,String(shtr5val));};
  };
  if(nshooter6!=pshooter6){
    pshooter6=nshooter6;
    if ((nshooter6>>3)&B00000001){shtr6val=((nshooter6)&B00000111)+64;}else{shtr6val=((nshooter6)&B00000111)+96;}; 
    if (nseqval==6){dmd.drawString(linex+54,0,String(shtr6val));};
  };
}

}


void receiveEvent(int howMany)
{

  int x = Wire.read();    // receive byte as an integer
  delay(1);
  Serial.println(x);         // print the integer
  delay(1);
  
if (((x>>5)&B00000111)==1){traffic=x;};
if (((x>>6)&B00000011)==1){sequence=x;};
if (((x>>4)&B00001111)==8){nshooter1=x;};
if (((x>>4)&B00001111)==9){nshooter2=x;};
if (((x>>4)&B00001111)==10){nshooter3=x;};
if (((x>>4)&B00001111)==11){nshooter4=x;};
if (((x>>4)&B00001111)==12){nshooter5=x;};
if (((x>>4)&B00001111)==13){nshooter6=x;};
if (((x>>4)&B00001111)==14){endr=x;};
if (((x>>4)&B00001111)==15){endl=x;};



}
