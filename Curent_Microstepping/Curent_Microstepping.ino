#define STOP 0
#define UP 1
#define DOWN -1
#define RUN 8
#define M_PI 3.1415926

const int halt = 2; 
const int auf = 4;
const int ab = 5;
const int start = 6;
const int esoben = A0;
const int esunten = 10;
const int esoben_kal = 7;
const int DIR_A = 12;
const int DIR_B = 13;
const int BRAKE_B = 8;
const int BRAKE_A = 9;
const int PWM_A = 3;
const int PWM_B = 11;

unsigned char pos;
unsigned char motpos;
int state;
unsigned int steps;
int i;
int I;
int delay_;
byte motsig[4] = {0,1,3,2}; 
char buffer[10];

void step(int steps, int delay_){
   if(state!=RUN){
     Serial.println("noope");
     exit;
   }
   //start
   digitalWrite(BRAKE_A, 0); 
   digitalWrite(BRAKE_B, 0); 
   //run
   for(i=0;i<steps;i++){
     if(state==STOP || digitalRead(halt) || digitalRead(esoben) || (Serial.available()>0 && Serial.read()=='s') ){
       state=STOP;
       break;
     }
     for(pos=0;pos<32;pos++){
       digitalWrite(DIR_A,  (pos & 16) >> 4 );   
       digitalWrite(DIR_B, (((pos & 8) >> 3) | ((pos & 16) >> 4)) - (((pos & 8) >> 3) & ((pos & 16) >> 4)));  
       I=255*sin(pos*2*M_PI/32.);
       analogWrite(PWM_A, abs(I));   //Set PWM for CH A
       I=255*cos(pos*2*M_PI/32.);
       analogWrite(PWM_B, abs(I));  //Set PWM for CH B  
       delay(delay_);
     }
   }
   digitalWrite(BRAKE_A, 1 ); 
   digitalWrite(BRAKE_B, 1 ); 
}

void fast_step(int steps,int dir){
   if(state==STOP || state==RUN){
     Serial.println("noope");
     exit;
   }
   //start
   digitalWrite(BRAKE_A, 0); 
   digitalWrite(BRAKE_B, 0); 
   analogWrite(PWM_A, 255);
   analogWrite(PWM_B, 255);
   //run
   for(i=0;i<steps;i++){
     if(state==STOP || digitalRead(halt) || (digitalRead(esoben) && dir==UP) || (digitalRead(esunten) && dir==DOWN) || (Serial.available()>0 && Serial.read()=='s') ){
       state=STOP;
       break;
     }
     motpos = ((motpos | 4 ) + dir) & 3;
     digitalWrite(DIR_A, (motsig[motpos] & 2) >> 1 );   
     digitalWrite(DIR_B, (motsig[motpos] & 1 ));  
     delay(3);
   }
   digitalWrite(BRAKE_A, 1 ); 
   digitalWrite(BRAKE_B, 1 );
   analogWrite(PWM_A, 0);
   analogWrite(PWM_B, 0);
}

void stop(){
  state=STOP;
}

void setup(){
  pinMode(DIR_A, OUTPUT); //CH A -- HIGH = forwards and LOW = backwards???
  pinMode(DIR_B, OUTPUT); //CH B -- HIGH = forwards and LOW = backwards???
  pinMode(BRAKE_A, OUTPUT); //brake (disable) CH A
  pinMode(BRAKE_B, OUTPUT); //brake (disable) CH B
  pinMode(PWM_A,OUTPUT);
  pinMode(PWM_B,OUTPUT);
  digitalWrite(BRAKE_A, HIGH); //(0=no_break)
  digitalWrite(BRAKE_B, HIGH); 
  pinMode(auf, INPUT); //Input auf
  pinMode(ab, INPUT); //Input ab
  pinMode(halt,INPUT);
  pinMode(start,INPUT);
  pinMode(esoben,INPUT);
  pinMode(esoben_kal,INPUT);
  pinMode(esunten,INPUT);
  Serial.begin(9600);
  Serial.println("Start");
  analogWrite(PWM_A, 0);   //Set PWM for CH A
  analogWrite(PWM_B, 0);  //Set PWM for CH B  
  attachInterrupt(0, stop, CHANGE);
  state=RUN;
  steps=1;
  delay_=5;
}

void loop(){
  if(Serial.available()>0){
    buffer[0]=Serial.read();
    switch (buffer[0]){
      case 's':
        state=STOP;
        break;
      case 'r':
        state=RUN;
        step(steps, delay_);
        break;
      case 'u':
        state=UP;
        fast_step(steps,UP);
        break;
      case 'd':
        state=DOWN;
        fast_step(steps,DOWN);
        break;
      case 'S':
        steps=Serial.parseInt();
        break;
      case 'D':
        delay_=Serial.parseInt();
        break;
      case 't':
        Serial.println("talking...");
        Serial.print("Steps: ");
        Serial.println(steps);
        Serial.print("Delay: ");
        Serial.println(delay_);
        break;
      default:
        while(Serial.available()>0){
           buffer[0]=Serial.read(); 
        }
        Serial.println("HELP:");
        Serial.println("u: UP");
        Serial.println("d: DOWN");
        Serial.println("r: RUN");
        Serial.println("s: STOP");
        Serial.println("SXXX: XXX Steps (30000 recomended)");
        Serial.println("DX: Delay X Millisconds (Default 5)");
        Serial.println("t: Talk eg. give out current values");
    }
  }else if(digitalRead(halt)){
    state=STOP;
  }else if(digitalRead(auf)){
    state=UP;
    fast_step(steps,UP);
  }else if(digitalRead(ab)){
    state=DOWN;
    fast_step(steps,DOWN);   
  }else if(digitalRead(start)){
    state=RUN;
    step(steps, delay_);
  }else{
   delay(500); 
  }
}

