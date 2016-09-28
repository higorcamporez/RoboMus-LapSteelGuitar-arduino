#include <Servo.h>
 
#define NUMBER_OF_PICKS 6

struct Pick{
  Servo servo;
  int pos;
};

//struct palheta p1,p2,p3,p4,p5;
struct Pick pick[18];

void startPicks(int pin[]){
  

  for(int i; i < NUMBER_OF_PICKS; i++){
    pick[i].servo.attach(pin[i]);
    pick[i].servo.write(5);
    pick[i].pos = 0;
  }
  
}
void setup ()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  pinMode(13, OUTPUT);
  digitalWrite(13,0);
  int pin[NUMBER_OF_PICKS] = {2,3,4,5,6,7};
  startPicks(pin);
  delay(100);
  
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

void loop()
{
   int firstByte;
 
   if(Serial.available() > 0){
    firstByte = Serial.read();    
    switch (firstByte){
      case '1':
        digitalWrite(13,1);
        
        break;
      case '0':
        digitalWrite(13,0);
        break;
      case 30:
        delay(10);
        if(Serial.available() > 0){
          
          int stringNumber = Serial.read();
          delay(10);
          int id = Serial.read();
          playString(&(pick[stringNumber-1]));
         //pick[stringNumber-1].servo.write(25)
          /*digitalWrite(13,1);
          delay(500);
          digitalWrite(13,0);*/
          Serial.write(id);
         
        }
        break;
        
    }
   }
  
    
}
