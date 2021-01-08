
#include <Servo.h>


#define IN1 22
#define IN2 24
#define IN3 26
#define IN4 28
#define rightENA 3
#define leftENA 4
#define blade 9
#define rightIR 52
#define leftIR 53
#define centerTrigger 34
#define centerEcho 35
#define rightTrigger 38
#define rightEcho 39
#define leftTrigger 42
#define leftEcho 43
#define pot A0

int i, j;
float Accum;
const int InputNodes = 5;
const int HiddenNodes = 6;
const int OutputNodes = 3;

float Output[OutputNodes];
float Hidden[HiddenNodes];

byte Input[InputNodes];

float HiddenWeights[InputNodes+1][HiddenNodes] = {
{-1.34, 3.08, -0.27, 1.15, 0.59, 0.08, },
{-4.77, 5.80, -0.77, -1.17, 3.26, -0.82, },
{-0.27, -2.27, 0.13, -2.96, 3.34, 0.22, },
{1.64, 5.91, -4.52, 7.28, -8.49, -4.09, },
{3.92, -7.65, -6.28, -2.90, 3.77, -4.89, },
{-1.64, -3.25, 2.98, -1.58, -0.46, 2.16 }
};

float OutputWeights[HiddenNodes+1][OutputNodes] = {
{7.10, 3.96, 1.60, },
{-10.97, 1.37, -0.66, },
{-7.98, -1.64, -7.83, },
{-4.30, 6.06, 2.97, },
{2.43, -10.29, 0.46, },
{-4.77, -2.47, -5.53, },
{3.87, 0.12, 5.47 }
};


int rightSpeed;
int leftSpeed;
int bladeSpeed;
int centerDuration;
int leftDuration;
int rightDuration;
float centerDistance;
float leftDistance;
float rightDistance;
int minDistance = 45;

void FeedForward();
Servo ESC;


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(rightENA, OUTPUT);
  pinMode(leftENA, OUTPUT);
  pinMode(blade, OUTPUT);
  pinMode(rightIR, INPUT);
  pinMode(leftIR, INPUT);
  pinMode(centerTrigger, OUTPUT);
  pinMode(leftTrigger, OUTPUT);
  pinMode(rightTrigger, OUTPUT);
  pinMode(centerEcho, INPUT);
  pinMode(leftEcho, INPUT);
  pinMode(rightEcho, INPUT);
  pinMode(pot, INPUT);


  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.begin(9600);
  ESC.attach(9,1000,2000); // (pin, min pulse width, max pulse width in milleseconds)
  ESC.writeMicroseconds(1000);
  FeedForward();
}

void FeedForward(){
  /******************************************************************
* Compute hidden layer activations
******************************************************************/

      for( i = 0 ; i < HiddenNodes ; i++ ) {    
        Accum = HiddenWeights[InputNodes][i] ;
        for( j = 0 ; j < InputNodes ; j++ ) {
          Accum += Input[j] * HiddenWeights[j][i] ;
        }
        Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
      }

/******************************************************************
* Compute output layer activations and calculate errors
******************************************************************/

      for( i = 0 ; i < OutputNodes ; i++ ) {    
        Accum = OutputWeights[HiddenNodes][i] ;
        for( j = 0 ; j < HiddenNodes ; j++ ) {
          Accum += Hidden[j] * OutputWeights[j][i] ;
        }
        Output[i] = 1.0/(1.0 + exp(-Accum)) ;   
      }
//      for(i = 0; i < OutputNodes; i++) Serial.println(Output[i]);
}

void loop() {
  digitalWrite(centerTrigger, HIGH);
  delayMicroseconds(100);
  digitalWrite(centerTrigger, LOW);
  centerDuration = pulseIn(centerEcho, HIGH);
  centerDistance = centerDuration * 0.017;

  digitalWrite(rightTrigger, HIGH);
  delayMicroseconds(100);
  digitalWrite(rightTrigger, LOW);
  rightDuration = pulseIn(rightEcho, HIGH);
  rightDistance = rightDuration * 0.017;
  
  digitalWrite(leftTrigger, HIGH);
  delayMicroseconds(100);
  digitalWrite(leftTrigger, LOW);
  leftDuration = pulseIn(leftEcho, HIGH);
  leftDistance = leftDuration * 0.017;

  Input[0] = leftDistance < minDistance ? 1 : 0;
  Input[1] = centerDistance < minDistance ? 1 : 0;
  Input[2] = rightDistance < minDistance ? 1 : 0;
  Input[3] = digitalRead(leftIR) == LOW ? 1 : 0;
  Input[4] = digitalRead(rightIR) == LOW ? 1 : 0;

  FeedForward();

  rightSpeed = Output[0] > 0.5 ? 200 : 0;
  leftSpeed = Output[1] > 0.5 ? 200 : 0;

  analogWrite(rightENA, rightSpeed);
  analogWrite(leftENA, leftSpeed);


int potValue;

  // put your main code here, to run repeatedly:
  potValue = analogRead(pot);
  potValue = map(potValue, 0, 1023, 0, 180);
// for ( int bladeSpeed =0; bladeSpeed <= 35; bladeSpeed +=5){
    ESC.write(Output[2] > 0.5 ? potValue : 0);
}
