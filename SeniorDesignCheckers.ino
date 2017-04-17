#include <Servo.h>

//Position (in tile coordinates) of the z magnet
int currentXPos = 0;
int currentYPos = 0;

//Time it takes for stepper to step the distance of 1 tile in ms
const int stepsToTravelOneTile = 500;

//Motor step and direction pins (stepper motors)
const int xMotorStepPin = 0;
const int xMotorDirectionPin = 0;
const int yMotorStepPin = 0;
const int yMotorDirectionPin = 0;

enum stepperDirection{
  LEFT = 0,
  RIGHT = 1,
  UP = 1,
  DOWN = 0
};

class zMotor{
  public:
  zMotor(int motorPin){
    servo.attach(motorPin);
  }

  //Raises the zMotor and waits half a second
  void raise(){
    servo.write(180);
    delay(500);
  }

  //Lowers the zMotor and waits half a second
  void lower(){
    servo.write(0);
    delay(500);
  }
  private:
    Servo servo;
};

//Servo motor pin
const int zMotorPin = 0;

zMotor zMotor(zMotorPin);

//Reed switch pins 
//**Note: switch positions names are named as : switch[column][row], with rows
//and columns starting from the bottom left corner from the perspective of
//the player.  User with board is player 1.
const int switch10 = 6;
const int switch30 = A1;
const int switch01 = 4;
const int switch21 = 8;
const int switch12 = A5;
const int switch32 = A3;
const int switch03 = 5;
const int switch23 = 7;
const int switch14 = 9;
const int switch34 = A0;
const int switch05 = 2;
const int switch25 = 11;
const int switch16 = A4;
const int switch36 = A2;
const int switch07 = 3;
const int switch27 = 10;


//Set up the motors' step and direction pins
void setupMotors(){
  pinMode(xMotorStepPin, OUTPUT);
  pinMode(xMotorDirectionPin, OUTPUT);
  pinMode(yMotorStepPin, OUTPUT);
  pinMode(yMotorDirectionPin, OUTPUT);

}

//Set up the reed switch pins as inputs and pull them up;
void setupReedSwitchPins(){
  pinMode(switch10,INPUT_PULLUP);
  pinMode(switch30,INPUT_PULLUP);
  pinMode(switch01,INPUT_PULLUP);
  pinMode(switch21,INPUT_PULLUP);
  pinMode(switch12,INPUT_PULLUP);
  pinMode(switch32,INPUT_PULLUP);
  pinMode(switch03,INPUT_PULLUP);
  pinMode(switch23,INPUT_PULLUP);
  pinMode(switch14,INPUT_PULLUP);
  pinMode(switch34,INPUT_PULLUP);
  pinMode(switch05,INPUT_PULLUP);
  pinMode(switch25,INPUT_PULLUP);
  pinMode(switch16,INPUT_PULLUP);
  pinMode(switch36,INPUT_PULLUP);
  pinMode(switch07,INPUT_PULLUP);
  pinMode(switch27,INPUT_PULLUP);
}

void setup() {
  setupMotors();
  

  
  setupReedSwitchPins();
  Serial.begin(9600);
}

//Resets the position of the magnet back to the bottom left corner
void resetMagnetPosition(){
  //Lower the z motor
  zMotor.lower();
  
  //Set direction to move z-axis magnet to lower left corner 
  //of the board
  digitalWrite(xMotorDirectionPin,LEFT);
  digitalWrite(yMotorDirectionPin,LEFT);
  //Sufficient steps to bring the magnet to the lower left position
  for(int steps = 0; steps < 30000; ++steps)
  {
    digitalWrite(xMotorStepPin,HIGH);
    digitalWrite(yMotorStepPin,HIGH);
    delayMicroseconds(50);
    digitalWrite(xMotorStepPin,LOW);
    digitalWrite(yMotorStepPin,LOW);
    delayMicroseconds(50);
  }
}

//Moves the Z Magnet to the given position and update the current position
//of the zMagnet
//param xPos: The x tile of the the game board (should be an int from 0-3 inclusive).
//param yPos: The y tile of the game board (should be an int from 0-7 includsive).
void moveZMagnet(int xPos, int yPos){
  //Calculate how many tiles from the destination
  int xOffset = currentXPos - xPos;
  int yOffset = currentYPos - yPos;

  //Calculate direction to the destination
  int xDirection = xOffset < 0 ? RIGHT: LEFT;
  int yDirection = yOffset < 0 ? UP : DOWN;

  xOffset =  abs(xOffset);
  yOffset = abs(yOffset);

  //Set direction of motors
  digitalWrite(xMotorDirectionPin, xDirection);
  digitalWrite(yMotorDirectionPin, yDirection);

  //Intentionally move in 1 axis at a time to avoid moving diagonally
  //Move x axis
  for(int steps = 0; steps < xOffset * stepsToTravelOneTile; ++steps)
  {
    digitalWrite(xMotorStepPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(xMotorStepPin,LOW);
    delayMicroseconds(50);
  }

  //Move y axis
  for(int steps = 0; steps < yOffset * stepsToTravelOneTile; ++steps)
  {
    digitalWrite(yMotorStepPin, HIGH);
    delayMicroseconds(50);
    digitalWrite(yMotorStepPin,LOW);
    delayMicroseconds(50);
  }

  //Update the current position
  currentXPos = xPos;
  currentYPos = yPos;
}



//Moves a checkers piece 
//param startXPos: initial x tile position of the piece to be moved
//param startYPos: initial y tile position of the piece to be moved
//param destXPos: x tile position for the piece to be moved to
//param destYPos: y tile position for the piece to be moved to
void movePiece(int startXPos, int startYPos, int destXPos, int destYPos){
  moveZMagnet(startXPos,startYPos);
  //Raise the zMotor to grab the piece
  zMotor.raise();

  moveZMagnet(destXPos,destYPos);

  //Lower the motor to release hold of piece
  zMotor.lower();
}
void loop() {
  
  
}



