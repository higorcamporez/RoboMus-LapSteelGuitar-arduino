#include <Servo.h>
#include <Ultrasonic.h>
 
#define NUMBER_OF_PICKS 9

struct Pick{
  Servo servo;
  int pos;
};


struct Pick pick[18];
//o indece é o numero da casa
// e o valor é a distancia do sensor
float fret[12];

//engine ports
int enginePin1 = 20; 
int enginePin2 = 19; 


//ultrassonic ports
#define TRIGGER_PIN  14
#define ECHO_PIN     15
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
int fretBarPosition= 1; 

//bar ports
Servo servoBar1;
Servo servoBar2;
#define SERVO_BAR_1 24
#define SERVO_BAR_2 22
 
void startBar(){
  servoBar1.attach(SERVO_BAR_1);
  servoBar1.write(0);

  servoBar2.attach(SERVO_BAR_2);
  servoBar2.write(180);

}
void startFrets(){
  fret[0] = 32.49; //fret one
  fret[1] = 30.77; //fret two
  fret[2] = 27.4; //fret three
  fret[3] = 24.83; 
  fret[4] = 22.77; 
  fret[5] = 20.69; 
  fret[6] = 19.60; 
  fret[7] = 10.00;
  fret[8] = 10.00; 
  fret[9] = 10.00; 
  fret[10] = 10.00; 
  fret[11] = 10.00; 
  
}
void startEngine(){
  pinMode(enginePin1, OUTPUT);
  pinMode(enginePin2, OUTPUT);

}


void startPicks(int pin[]){
  

  for(int i=0; i < NUMBER_OF_PICKS; i++){
    pick[i].servo.attach(pin[i]);
    pick[i].servo.write(5);
    pick[i].pos = 5;
  }
  
}

void playString(struct Pick *p){
  
    if(p->pos == 0){
      
      p->servo.write(25);
      p->pos=1;
      
    }else{
      p->servo.write(5);
      p->pos = 0;
    }   
  
}
void engineStop(){
  analogWrite(enginePin1, 0); // set pin 2 on L293D high
  analogWrite(enginePin2, 0); // set pin 7 on L293D low
}
void moveEngineFoward(int engineSpeed){
  analogWrite(enginePin1, engineSpeed);
  analogWrite(enginePin2, 0); 
}
void moveEngineBackard(int engineSpeed){

  analogWrite(enginePin1, 0); // set pin 2 on L293D low
  analogWrite(enginePin2, engineSpeed); // set pin 7 on L293D high

}

float getSensorDistance(){
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  return cmMsec;
}

void positionBar(int numberFret, int engineSpeed){
   
   float distance = getSensorDistance();
   
   while(abs(distance  - fret[numberFret-1]) > 0.5){
      delay(20);
     if(distance < fret[numberFret-1]){
        moveEngineFoward(engineSpeed);
     }else{
        moveEngineBackard(engineSpeed);
     }
    
     distance = getSensorDistance();
   }
   engineStop();
}
void moveBar(int pos){
  servoBar1.write(pos);
  servoBar2.write(180-pos);
}
void setup ()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //pinMode(13, OUTPUT);
  //digitalWrite(13,0);
  int pin[NUMBER_OF_PICKS] = {2,3,4,5,6,7,8,9,10};
  startPicks(pin);
  startEngine();
  startFrets();
  startBar();
  delay(100);
  Serial.print("apsas");
  positionBar(4, 200);
 
  
}
int cont = 0;
void loop()
{
   int firstByte;
   if(Serial.available() > 0){
    int x = Serial.read();
    if(x == '+'){
      cont++;
    }else{
      cont--;
    }
    Serial.println(cont);
    moveBar(cont);
  }
   if(Serial.available() > 0){
    firstByte = Serial.read();    
    switch (firstByte){
      
      case 0:
        digitalWrite(13,1);
        delay(200);
        digitalWrite(13,0);
        break;
      case 30:
        
        digitalWrite(13,1);
        delay(5);
        digitalWrite(13,0);
        if(Serial.available() > 0){
          
          int stringNumber = Serial.read();
          
          int id = Serial.read();
          playString(&(pick[stringNumber-1]));
         //pick[stringNumber-1].servo.write(25)
          /*digitalWrite(13,1);
          delay(500);
          digitalWrite(13,0);*/
          Serial.write(id);
         
        }
        break;
      case 50:

        if(Serial.available() > 0){
          
          int pos = Serial.read();
          
          int id = Serial.read();
          moveBar(pos);
          Serial.write(id);
         
        }
        break;
        
      case 60:
        
        delay(10);
        if(Serial.available() > 0){
          int i;
          int fret = Serial.read();
         
          int id = Serial.read();       
          positionBar(fret, 200);
          
        }
        break;
        
    }

   }
   delay(400);
  Serial.print("dist: ");
  Serial.println(getSensorDistance());
  /*
  positionBar(4, 200);
  delay(400);
  positionBar(10, 200);
  delay(400);
  */

  
  
  
}
