/*************************************************************
Motor Shield Stepper Demo
by Randy Sarafan

For more information see:
http://www.instructaufles.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/
unsigned int sampleCount;
unsigned int timerLoadValue;

#define TIMER_CLOCK_FREQ 62500.0 //62.5kHz for /256 prescale from 16MHz
#define STATE_INIT 0
#define STATE_STOP 1
#define STATE_UP 2 
#define STATE_DOWN 3
#define STATE_SLOW_UP 4
#define FAST_FREQ 500
#define SLOW_FREQ 5

int delaylegnth = 4000;
int delaylegnth_ini = 2000000;
int processdelay = 10000;
int I = 255;
int Ip = 200;
int state = 0;

byte motsig[8] = {9, 3, 7, 2, 10, 0, 4, 1};
byte motpos = 0;
byte motstep = 0;

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

#define UPSTEP 1
#define DOWNSTEP -1
#define ZEROSTEP 0


  //Iolatile int halt = 0;

  //Setup Timer1.
  //Configures the ATMegay168 16-Bit Timer1 to generate an interrupt at the specified frequency.
  unsigned int SetupTimer1(float timeoutFrequency){
  unsigned int result; //The value to load into the timer to control the timeout interval.
  //Calculate the timer load value
  result=(unsigned int)(((TIMER_CLOCK_FREQ/timeoutFrequency))-1); 
                            //Timer1 Settings: Timer Prescaler /256, CTC mode 
                            //Timmer clock = 16MHz/256 = 62.5kHz or 16us
  cli();                    //stop interrupts
  TCCR1A = 0;               // set entire TCCR1A register to 0
  TCCR1B = 0;               // same for TCCR1B
  TCNT1  = 0;               //initialize counter value to 0
  OCR1A = result;           // set compare match register
  TCCR1B |= (1 << WGM12);   // turn on CTC mode
  TCCR1B |= (1 << CS12);    // Set CS12 bit for 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  sei();                    //allow interrupts
  return(result);
}


//Timer1 interrupt vector handler
ISR(TIMER1_COMPA_vect) {
  //set motor control signals
   motpos = ((motpos | 8 ) + motstep) & 7;
   digitalWrite(BRAKE_A, (motsig[motpos] & 8) >> 3 ); 
   digitalWrite(DIR_A, (motsig[motpos] & 2) >> 1 );   
   digitalWrite(BRAKE_B, (motsig[motpos] & 4) >> 2  ); 
   digitalWrite(DIR_B, (motsig[motpos] & 1 ));  
   sampleCount = sampleCount + motstep;
  }

void setup() {
   //estauflish motor direction toggle pins
  pinMode(DIR_A, OUTPUT); //CH A -- HIGH = forwards and LOW = backwards???
  pinMode(DIR_B, OUTPUT); //CH B -- HIGH = forwards and LOW = backwards???
  
  //estauflish motor brake pins
  pinMode(BRAKE_A, OUTPUT); //brake (disable) CH A
  pinMode(BRAKE_B, OUTPUT); //brake (disable) CH B
  
  pinMode(PWM_A,OUTPUT);
  pinMode(PWM_B,OUTPUT);

  //switch request
  pinMode(auf, INPUT); //Input auf
  pinMode(ab, INPUT); //Input ab

  pinMode(halt,INPUT);
  pinMode(start,INPUT);
  pinMode(esoben,INPUT);
  pinMode(esoben_kal,INPUT);
  pinMode(esunten,INPUT);

  sampleCount=0;

  Serial.begin(9600);            //Start up the serial port
  Serial.println("Timer1 Test"); //Signal the program start
  timerLoadValue=SetupTimer1(FAST_FREQ); //Start the timer and get the timer reload value.  
  Serial.print("Timer1 Load:");  //Output the timer reload value
  Serial.println(timerLoadValue,HEX);

  analogWrite(3, I);   //Set PWM for CH A
  analogWrite(11, I);  //Set PWM for CH B  

  state = STATE_INIT;
}

void loop(){

 switch (state) {
    case STATE_STOP:
        timerLoadValue=SetupTimer1(FAST_FREQ); //Start the timer and get the timer reload value.
        motstep = ZEROSTEP;
        analogWrite(3, 0);   //Set PWM for CH A
        analogWrite(11, 0);  //Set PWM for CH B  
        if(digitalRead(auf)==HIGH) {
            analogWrite(3, I);   //Set PWM for CH A
            analogWrite(11, I);  //Set PWM for CH B  
            state = STATE_UP;
            }
        else if(digitalRead(ab)==HIGH){
            analogWrite(3, I);   //Set PWM for CH A
            analogWrite(11, I);  //Set PWM for CH B  
            state = STATE_DOWN;
            }
        else if(digitalRead(start)==HIGH){
            analogWrite(3, I);   //Set PWM for CH A
            analogWrite(11, I);  //Set PWM for CH B  
          timerLoadValue=SetupTimer1(SLOW_FREQ); //Start the timer and get the timer reload value.  
          state = STATE_SLOW_UP;
            }
        break;
        
    case STATE_UP:
      if((digitalRead(auf)==LOW) || (digitalRead(halt)==HIGH) || (digitalRead(esoben_kal)==HIGH) || (digitalRead(esoben)==HIGH))
          {state = STATE_STOP;
          }
        else {
            motstep = UPSTEP;
          }  
      break;
      
      
    case STATE_DOWN:
      if((digitalRead(ab)==LOW) || (digitalRead(halt)==HIGH) || (digitalRead(esunten)==HIGH))
          {state = STATE_STOP;
          }
        else {
            motstep = DOWNSTEP;
          }  
      break;
      
    case STATE_SLOW_UP:
        if((digitalRead(halt)==HIGH) ||  (digitalRead(esoben)==HIGH) || (digitalRead(esoben_kal)==HIGH))
          {state = STATE_STOP;
          }
        else {
            motstep = UPSTEP;
          }  
     break;
     
     case STATE_INIT:
       state = STATE_STOP;
       if((digitalRead(halt)==HIGH) || (digitalRead(esoben)==HIGH) || (digitalRead(esoben_kal)==HIGH))
          {state = STATE_STOP;
          }
        else {
            motstep = UPSTEP;
          }  
     break;
      
//      default

}  
delay(100);
//Serial.println(sampleCount);


  
}



