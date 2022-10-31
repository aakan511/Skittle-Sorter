#include <Wire.h>
#include <Servo.h>
#include "Adafruit_TCS34725.h"
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725();

//Declare pin functions for feed stepper motor
#define stp 4
#define dir 3
#define EN  2

//Declare pins for caurosel motor
#define stp1 5
#define dir1 6
#define EN1 7

#define vibRelay 8 //vibration motor relay

#define mixerPin 9 //mixing servo pin
Servo mixerServo;

float startTime = 0; //amt sorted variables
float timer;
float count = 0;

int steps = 0;

int timer1=0; //jam timer

int lastMixTime = 0;//mixer timer
boolean turnDirection = false;

char user_input;
int x;
int y;
int state;

int switchState = 0; //for feeder switich

int nextColor=0;
int currentColor = 0;//caurosel starts directly over red hopper

//normal skittles
float minMaxH[5][2] = {{0.00, 30.00}, {25.00, 50.00}, {85.00, 109.00}, {5.00, 20.00}, {110.00, 130.00}}; //normal skittles stuff
float minMaxS[5][2] = {{15.00, 64.99}, {45.00, 125.00}, {70.00, 95.00}, {65.00, 125.00}, {35.00, 75.00}};
float minMaxV[5][2] = {{75.00, 89.99}, {80.00, 105.00}, {85.00, 110.00}, {90.00, 120.00}, {80.00, 95.00}};
String colors[5] = {"red", "yellow", "green", "orange", "purple"};

//skittles wildberry 
float WBminMaxH[5][2] = {{0.00, 9.00}, {0.00, 20.00}, {116.00, 135.00}, {90.00, 115.00}, {130.00,145.00}}; //wild berry skittles stuff
float WBminMaxS[5][2] = {{30.00, 130.00}, {10.00, 70.00}, {40.00, 80.00}, {77.00, 105.00}, {90.00, 130.00}};
float WBminMaxV[5][2] = {{87.00, 127.00}, {75.00, 100.00}, {80.00, 95.00}, {87.00, 105.00}, {85.00, 110.00}};
String WBcolors[5] = {"red", "orange", "purple", "green", "blue"};


float rgb[3][3];
float hsv[3][3];

void setup(void) {
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(stp1, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(EN1, OUTPUT); 
  resetEDPins(); //Set step, direction, microstep and enable pins to default states

  pinMode(vibRelay, OUTPUT);//relay setup
  
  Serial.begin(9600);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
  senseRGB(rgb);
  digitalWrite(EN, LOW);
  digitalWrite(EN1 , LOW);
  nextCandy();
  mixerServo.attach(mixerPin);
  delay(2000);
  startTime = millis();
  timer1 = millis();
  lastMixTime = millis();
}

void loop(void) {
  while(true){
  steps=0;
  if(millis()-lastMixTime>=1200L){
    lastMixTime=millis();
    mix();
  }
  
  
if(analogRead(A2) >1000){
        
    //digitalWrite(vibRelay, LOW);
    senseRGB(rgb);
    for(int i=0; i<3; i++){
      RGBtoHSV(rgb[i], hsv[i]);
    }
      Serial.print(hsv[2][0]);
      Serial.print("\t");
      Serial.print(hsv[2][1]);
      Serial.print("\t");
      Serial.print(hsv[2][2]);
      Serial.print("\n");
    currentColor = nextColor;
    count++;
    timer = millis()-startTime;
    if(analogRead(A3) > 100){
      nextColor=WBskittleColor(hsv);
      //Serial.println(WBcolors[nextColor]);
    }else if(analogRead(A3)<100){
      nextColor=skittleColor(hsv);
      //Serial.println(colors[nextColor]);
    }
    //float avg = count/(timer/1000.0);
    //Serial.println(avg);
    delay(150);
    //digitalWrite(vibRelay, HIGH);
    carousel();
   }else{
    
    while(analogRead(A2) < 1000){
      nextCandy();
      steps++;
      if(steps>4000){while(true){delay(1);}}
   }
   }
    //digitalWrite(vibRelay, HIGH);
   
    
    //Serial.println("=");
    //eject skittle
    
}
}

void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(EN, LOW);
  digitalWrite(stp1, LOW);
  digitalWrite(dir1, LOW);
  digitalWrite(EN1, HIGH);
}



int skittleColor(float a[3][3]){
  for(int i=0; i<5; i++){
    if(a[2][0] > minMaxH[i][0] && a[2][0] < minMaxH[i][1]){
      if(a[2][1] > minMaxS[i][0] && a[2][1] < minMaxS[i][1]){
        if(a[2][2] > minMaxV[i][0] && a[2][2] < minMaxV[i][1]){
          return i;
      }
      }
    }
  }
  return 0;


//  
//  if(a[2][0] <=5.0 || a[2][0] > 240.0){
//    return 0;
//  }
//  if(a[2][0] > minMaxH[1][0] && a[2][0] < minMaxH[1][1]){
//    return 1;
//    
//  }if(a[2][0] > minMaxH[2][0] && a[2][0] < minMaxH[2][1]){
//    if(a[2][1] > minMaxS[2][0] && a[2][1] < minMaxS[2][1]){
//      return 2;
//    }
//  }if(a[2][0] > minMaxH[4][0] && a[2][0] < minMaxH[4][1]){
//    if(a[2][1] > minMaxS[4][0] && a[2][1] < minMaxS[4][1]){
//      return 4;
//    }
//  }
//  if(a[2][0] > minMaxH[3][0] && a[2][0] < minMaxH[3][1]){
//    if(a[2][1] > minMaxS[3][0] && a[2][1] < minMaxS[3][1]){
//      if(a[2][2] > minMaxV[3][0] && a[2][2] < minMaxV[3][1]){
//          return 3;
//      }
//    }
//  }else{
//    return 0;
//   
//  }
}

int WBskittleColor(float a[3][3]){
  for(int i=0; i<2; i++){
    if(a[2][0] > WBminMaxH[3][0] && a[2][0] < WBminMaxH[3][1]){
      return 3;
      
    } else if(a[2][0] > WBminMaxH[4][0] && a[2][0] < WBminMaxH[4][1]){
      if(a[2][1] > WBminMaxS[4][0] && a[2][1] < WBminMaxS[4][1]){
        return 4;
      }
    }else if(a[2][0] > WBminMaxH[2][0] && a[2][0] < WBminMaxH[2][1]){
      if(a[2][1] > WBminMaxS[2][0] && a[2][1] < WBminMaxS[2][1]){
        return 2;
      }
    
    }else if(a[2][0] > WBminMaxH[1][0] && a[2][0] < WBminMaxH[1][1]){
      if(a[2][1] > WBminMaxS[1][0] && a[2][1] < WBminMaxS[1][1]){
        if(a[2][2] > WBminMaxV[1][0] && a[2][2] < WBminMaxV[1][1]){
            return 1;
        }
      }
    }else if(a[2][0] > 230 || a[2][0] < 90){
      return 0;
    }
    senseRGB(a);
  }
  return 0;  
}




void senseRGB(float a[3][3]){
  for(int i=0; i<3; i++){
    getData(a[i]);
    delay(1);       
  }
  return;
}
void getData(float b[3]){
  float r1=0, g1=0, b1=0;
  tcs.getRGB(&r1, &g1, &b1);
  b[0]=r1; b[1]=g1; b[2]=b1;
}

void RGBtoHSV(float RGBColor[3], float HSVColor[3]){
  float color[3];
  float Cmax, Cmin, delta;
  float hue, sat, val;
  float hueAngle, satPerc, valPerc;

  color[0] = RGBColor[0]; color[1] = RGBColor[1]; color[2] = RGBColor[2];
  
  color[0] /= 255; color[1] /= 255; color[2] /= 255;
  
  Cmax = arrayMax(color, 3);
  Cmin = arrayMin(color, 3);
  delta = Cmax - Cmin;

  if(delta == 0) {
    hue = 0;
  }
  if(Cmax == color[0]) {
    hue = (60 * fmod(((color[1] - color[2]) / delta), 6)) * 0.7083;
    if(color[1] - color[2] < 0) {
      hue += 255;
    }
  }
  if(Cmax == color[1]) {
    hue = (60 * (((color[2] - color[0]) / delta) + 2)) * 0.7083;
  }
  if(Cmax == color[2]) {
    hue = (60 * (((color[0] - color[1]) / delta) + 4)) * 0.7083;
  }

  if(Cmax == 0) {
    sat = 0;
  } else  {
    sat = (delta / Cmax) * 255;
  }
  
  val = Cmax * 255;

  HSVColor[0] = hue; HSVColor[1] = sat; HSVColor[2] = val;
  
}

float arrayMin(float ar[], int s){
  float minVal = 1000000000;
  for(int i=0; i<s; i++){
    if(ar[i] < minVal){
      minVal = ar[i];
    }
  }
  return minVal;
}
float arrayMax(float ar[], int s){
  float maxVal = -100000000;
  for(int i=0; i<s; i++){
    if(ar[i] > maxVal){
      maxVal = ar[i];
    }
  }
  return maxVal;
}

void nextCandy()
{
  digitalWrite(dir, LOW); //Pull direction pin high to move in "reverse"
  for(int x= 0; x<1; x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step
    delayMicroseconds(200);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(200);
  }
}

void ejectCandy(){
  digitalWrite(dir, LOW);
  for(int i=0; i<400; i++){
    digitalWrite(stp,HIGH); //Trigger one step
    delayMicroseconds(400);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(400);
  }
}

void prevBucket()
{
  digitalWrite(dir1, HIGH); //Pull direction pin high to move in "reverse"
  for(int x= 0; x<320; x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp1,HIGH); //Trigger one step
    delayMicroseconds(200);
    digitalWrite(stp1,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(200);
  }
}

void nextBucket()
{
  digitalWrite(dir1, LOW); //Pull direction pin high to move in "reverse"
  for(int x= 0; x<320; x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp1,HIGH); //Trigger one step
    delayMicroseconds(200);
    digitalWrite(stp1,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(200);
  }
}

void carousel(){//rotates carousel to eject skittle
  int diff = nextColor-currentColor;
  if(diff>2){
    diff=diff-5;

  }if(diff<-2){
    diff=5+diff;

  }
  if(diff==1){
    digitalWrite(dir, LOW);
    digitalWrite(dir1, LOW);
    for(int i=0; i<400; i++){
      digitalWrite(stp, HIGH);
      if(i<320){
        digitalWrite(stp1, HIGH);
      }
      delayMicroseconds(400);
      if(i<320){
        digitalWrite(stp1, LOW);
      }
      digitalWrite(stp, LOW);
      delayMicroseconds(400);
    }
  }if(diff==2){
    digitalWrite(dir, LOW);
    digitalWrite(dir1, LOW);
    for(int i=0; i<400; i++){
      digitalWrite(stp, HIGH);
      digitalWrite(stp1, HIGH);
      delayMicroseconds(200);
      if(i<320){
        digitalWrite(stp1,LOW);
        delayMicroseconds(200);
        digitalWrite(stp1, HIGH);
        delayMicroseconds(200);
      }else{
        delayMicroseconds(200);
      }
      digitalWrite(stp, LOW);
      digitalWrite(stp1, LOW);
      delayMicroseconds(400);
    }
  }

  if(diff==-1){
    digitalWrite(dir, LOW);
    digitalWrite(dir1, HIGH);
    for(int i=0; i<400; i++){
      digitalWrite(stp, HIGH);
      if(i<320){
        digitalWrite(stp1, HIGH);
      }
      delayMicroseconds(400);
      if(i<320){
        digitalWrite(stp1, LOW);
      }
      digitalWrite(stp, LOW);
      delayMicroseconds(400);
    }
  }if(diff==-2){
    digitalWrite(dir, LOW);
    digitalWrite(dir1, HIGH);
    for(int i=0; i<400; i++){
      digitalWrite(stp, HIGH);
      digitalWrite(stp1, HIGH);
      delayMicroseconds(200);
      if(i<240){
        digitalWrite(stp1,LOW);
        delayMicroseconds(200);
        digitalWrite(stp1, HIGH);
        delayMicroseconds(200);
      }else{
        delayMicroseconds(200);
      }
      digitalWrite(stp, LOW);
      digitalWrite(stp1, LOW);
      delayMicroseconds(400);
    }
  }
if(diff==0){
    ejectCandy();        
  }
}

void mix(){
  if(turnDirection){
    mixerServo.write(180);
  }else if(!turnDirection){
    mixerServo.write(0);
  }
  turnDirection = !turnDirection;
}





