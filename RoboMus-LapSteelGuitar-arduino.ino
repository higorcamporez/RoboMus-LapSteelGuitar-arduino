#include <Servo.h>
#include <Ultrasonic.h>
 
#define NUMBER_OF_PICKS 9
/*
  struct que representa uma palheta.
  servo -> servo que contrala a palheta
  pos -> qual posicao o servo esta, ou seja, 0 acima da corda e 1 abaixo da corda
*/
struct Pick{
  Servo servo;
  int pos;
};


struct Pick pick[NUMBER_OF_PICKS]; //array que representa as palhetas

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
/* inicia o vetor 'fret' com a distancia da respectiva do barra posicionada na casa até o sensor para cada casa. (fret[numero_da_casa] = distancia)*/
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
/*Define as portas de controle dos motores que movimentam a barra no braco do instrumento como portas de saida*/
void startEngine(){
  pinMode(enginePin1, OUTPUT);
  pinMode(enginePin2, OUTPUT);

}

/*
 * Inicia o vetor de palhetas com o respectivo servo e uma posicao inicial
 * @paran pin[] contem o numero da porta que cada motor foi conectado
*/
void startPicks(int pin[]){
  

  for(int i=0; i < NUMBER_OF_PICKS; i++){
    pick[i].servo.attach(pin[i]);
    pick[i].servo.write(5);
    pick[i].pos = 0;
  }
  
}
/*
 * Funcao para tocar uma determinar corda
 * @paran *p ponteiro para a palheta que deve ser acionada
*/
void playString(struct Pick *p){
    
    if(p->pos == 0){
      p->servo.write(25);
      p->pos=1;
    }else{
      p->servo.write(5);
      p->pos = 0;
    }   
  
}
/*Funcao que para os motores responsaveis por mover a barra no braco*/
void engineStop(){
  analogWrite(enginePin1, 0); 
  analogWrite(enginePin2, 0); 
}
/*Funcao que a barra para frente. Obs: nao lembre se 'pra frente' eh no sentido do corpo do violao*/
void moveEngineFoward(int engineSpeed){
  analogWrite(enginePin1, engineSpeed);
  analogWrite(enginePin2, 0); 
}
/*Funcao que a barra para tras. Obs: nao lembre se 'pra tras' eh no sentido das tarraxas do violao*/
void moveEngineBackard(int engineSpeed){

  analogWrite(enginePin1, 0); // set pin 2 on L293D low
  analogWrite(enginePin2, engineSpeed); // set pin 7 on L293D high

}

/*Retorna a distancia que o sensor esta lendo*/
float getSensorDistance(){
  float cmMsec, inMsec;
  long microsec = ultrasonic.timing();

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  
  return cmMsec;
}
/*
 * Funcao que posiciona a barra em uma determinada casa com uma certa velocidade
 * @paran numberFret casa para posicionar a barra
 * @paran engineSpeed velocidade do motor 0-255
*/
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
/*Funcao para mover a barra para baixo(encostar nas cordas) ou para cima (desencostar das cordas)*/
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

  int pin[NUMBER_OF_PICKS] = {2,3,4,5,6,7,8,9,10}; // valores das portas em que os servos da palhetas sao conectados
  startPicks(pin); 
  startEngine();
  startFrets();
  startBar();
  
  //teste
  delay(100);
  Serial.print("apsas");
  positionBar(4, 200);
 
  
}
int cont = 0;
void loop()
{
   int firstByte;
   /*apenas para teste*/
   if(Serial.available() > 0){
    int x = Serial.read();
    if(x == '+'){
      cont++;
    }else{
      cont--;
    }
    Serial.println(cont);
    moveBar(cont);
  }//fim teste
  
   if(Serial.available() > 0){
    firstByte = Serial.read();    
    switch (firstByte){
      
      case 0: // teste piscar led
        digitalWrite(13,1);
        delay(200);
        digitalWrite(13,0);
        break;
      case 30: // codigo para tocar corda
        
        digitalWrite(13,1);
        delay(5);
        digitalWrite(13,0);
        if(Serial.available() > 0){
          
          int stringNumber = Serial.read(); // numero da corda
          
          int id = Serial.read(); //id msg
          playString(&(pick[stringNumber-1])); //tocar codar
          Serial.write(id); // envia o id da msg para confirmacao
         
        }
        break;
      case 50: //codigo para mover barra para cima ou para baixo

        if(Serial.available() > 0){
          
          int pos = Serial.read(); //posicao
          
          int id = Serial.read(); //id da msg
          moveBar(pos); //mover barra
          Serial.write(id);//enviar id de confirmacao
         
        }
        break;
        
      case 60: //codigo para posicionar barra
        
        delay(10);
        if(Serial.available() > 0){
          
          int fret = Serial.read(); // casa para posicionamento
         
          int id = Serial.read();       //id da msg
          positionBar(fret, 200); //posicionar a barra
          Serial.write(id); //enviar id de confirmacao
          
        }
        break;
        
    }

   }
   //teste
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
