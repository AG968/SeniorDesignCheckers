#include <Servo.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 23   // can be a digital pin
#define XP 22   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define TS_Y 820
#define TS_X 770


TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

struct tempMovedPiece{
   
   int startXPos;
   int destXPos;
 
   //yPos won't be changing for temporarily moved pieces, so we can just store 1 yPos as a general one. 
   int yPos;
 
 };
 
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


const int BOARD_WIDTH = 4;
const int BOARD_HEIGHT = 8;

//Default to 1.  If we join a game, it will change to 2, if we create a game it will stay as 1.
int gamePlayer = 1;
int currentPlayerTurn;
int numOfPlayers;

//Position (in tile coordinates) of the z magnet - reset positon
//of the magnet is [3,7]
int currentXPos = 3;
int currentYPos = 7;

//Servo is flipped, so our max will be our min value
const int MAX_Z_HEIGHT = 90;
const int MIN_Z_HEIGHT = 180;

//Time it takes for stepper to step the distance of 1 tile in ms
const int STEPS_PER_TILE_X = 625;
const int STEPS_PER_TILE_Y = 550;

//Motor step and direction pins (stepper motors)
const int xMotorStepPin = 0;
const int xMotorDirectionPin = 0;
const int xMotorSleepPin = 0;

const int yMotorStepPin = 0;
const int yMotorDirectionPin = 0;
const int yMotorSleepPin = 0;

enum stepperDirection{
  LEFT = 0,
  RIGHT = 1,
  UP = 0,
  DOWN = 1
};
enum ServerResponseType{
  LIST_OF_GAMES,
  GAME_STATUS
};


//Game parameters to be sent to database when move is made
int sourceCol = 0;
int sourceRow = 0;
int destCol = 0;
int destRow = 0;
int gameID = 0;
int gameStatus = 1;


//Game board tiles to calculate which move was made.
//Initialize 1 for tiles with player 1, 2 for player 2, 
//-1 for empty tiles 
int currentGameTileStatus[4][8] = {{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1}};
int previousGameTileStatus[4][8] = {{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1}};
int gameTiles[4][8] = {{-1,1,-1,-1,-1,-1,-1,2},{1,-1,-1,-1,-1,-1,2,-1},{-1,1,-1,-1,-1,-1,-1,2},{1,-1,-1,-1,-1,-1,2,-1}};

//TFTparameters
String currentPage = "main menu";
String ssid[100];
int ssidTracker = 0;
int games[100];
int gameTracker = 0;
bool previous = false;
String text = "";
int gameListSize = 0;
int wifiListSize = 0;
String ssidText = "";


class zMotor{
  public:

  void init(int motorPin){
    servo.attach(motorPin);
    lower();
  }

  //Raises the zMotor and waits half a second
  void raise(){
    int i;
    //Control the speed at which the zMotor is raised
    for(i = MIN_Z_HEIGHT; i > MAX_Z_HEIGHT; --i){
      servo.write(i);
      delay(5);
    }
    delay(500);
  }

  //Lowers the zMotor and waits half a second
  void lower(){
    int i;
    //Control the speed at which the zMotor is lowered
    for(i = MAX_Z_HEIGHT; i < MIN_Z_HEIGHT; ++i){
      servo.write(i);
      delay(5);
    }
    delay(500);
  }
  
  private:
    Servo servo;
};

//Servo motor pin
const int zMotorPin = 9;
zMotor zMotor;





//Reed switch pins 
//**Note: switch positions names are named as : switch[column][row], with rows
//and columns starting from the bottom left corner from the perspective of
//player 1
const int switch10 = 50;
const int switch30 = 42;
const int switch01 = 40;
const int switch21 = 52;
const int switch12 = 43;
const int switch32 = 48;
const int switch03 = 38;
const int switch23 = 39;
const int switch14 = 35;
const int switch34 = 46;
const int switch05 = 34;
const int switch25 = 41;
const int switch16 = 45;
const int switch36 = 44;
const int switch07 = 36;
const int switch27 = 37;


//Set up the motors' step and direction pins
void setupMotors(){
  pinMode(xMotorStepPin, OUTPUT);
  pinMode(xMotorDirectionPin, OUTPUT);
  pinMode(yMotorStepPin, OUTPUT);
  pinMode(yMotorDirectionPin, OUTPUT);

  zMotor.init(zMotorPin);
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

//Resets the position of the magnet back to the bottom left corner
void resetMagnetPosition(){
  //Lower the z motor
  zMotor.lower();
  
  //Set direction to move z-axis magnet to upper right corner 
  //of the board
  moveZMagnet(3,7);
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

  //Set direction of motors
  digitalWrite(xMotorDirectionPin, xDirection);
  digitalWrite(yMotorDirectionPin, yDirection);

  xOffset =  abs(xOffset);
  yOffset = abs(yOffset);

  Serial.println(xOffset);
  Serial.println(yOffset);
  digitalWrite(xMotorSleepPin,HIGH);
  digitalWrite(yMotorSleepPin,HIGH);
  //delay 1 ms after waking up as per datasheet
  delay(3);
  if((xOffset*STEPS_PER_TILE_X) > (yOffset * STEPS_PER_TILE_Y))
  {
      for(int steps = 0; steps < xOffset * STEPS_PER_TILE_X; ++steps)
      {
        digitalWrite(xMotorStepPin, HIGH);
        if(steps < yOffset * STEPS_PER_TILE_Y){
          digitalWrite(yMotorStepPin,HIGH);
        }
        
        delayMicroseconds(400);
        
        digitalWrite(xMotorStepPin,LOW);
        if(steps < yOffset * STEPS_PER_TILE_Y){
          digitalWrite(yMotorStepPin,LOW);
        }
        
        delayMicroseconds(400);
      }
  
  }else{
      for(int steps = 0; steps < yOffset * STEPS_PER_TILE_Y; ++steps)
      {
        digitalWrite(yMotorStepPin, HIGH);
        if(steps < xOffset * STEPS_PER_TILE_X)
        {
          digitalWrite(xMotorStepPin,HIGH);
        }
        
        delayMicroseconds(400);
        
        digitalWrite(yMotorStepPin,LOW);
        if(steps < xOffset * STEPS_PER_TILE_X){
          digitalWrite(xMotorStepPin,LOW);
        }
        
        delayMicroseconds(400);
      }
  }
  
  //Update the current position
  currentXPos = xPos;
  currentYPos = yPos;

  //Set drivers to sleep after moving is done
  digitalWrite(xMotorSleepPin,LOW);
  digitalWrite(yMotorSleepPin,LOW);
}


//Moves a checkers piece 
//param startXPos: initial x tile position of the piece to be moved
//param startYPos: initial y tile position of the piece to be moved
//param destXPos: x tile position for the piece to be moved to
//param destYPos: y tile position for the piece to be moved to
 void movePiece(int startXPos, int startYPos, int destXPos, int destYPos){
   int xOffset = abs(startXPos - destXPos);
   //if the xOffset is > 1, that means we are eating a piece, so we must move that piece to the dead zone before making our final move
   //IF PIECE IS BEING EATEN
   if(xOffset > 1)
   {
     //Victim is the piece that has been eaten
     int xPosOfVictim = (startXPos + destXPos)/2;
     int yPosOfVictim = (startYPos + destYPos)/2;
 
     tempMovedPiece temporarilyMovedPieces[10];
     int temporarilyMovedPiecesSize = 0;
     
     //Before moving our victim to the dead zone, move the pieces that are in the same column as the victim out of the way, so that we have a path
     //to move our victim to the deadzone
     for(int row = 0; row < yPosOfVictim; ++row){
       if(gameTiles[xPosOfVictim][row] != -1){
         if(xPosOfVictim < BOARD_WIDTH - 1)
         {
           //Move the piece to the tile to the right
           movePiece(xPosOfVictim,row,xPosOfVictim + 1, row);
           //Store the location of the temporarily moved piece so that we could move it back when we're done
           temporarilyMovedPieces[temporarilyMovedPiecesSize].startXPos = xPosOfVictim;
           temporarilyMovedPieces[temporarilyMovedPiecesSize].destXPos = xPosOfVictim + 1;
           temporarilyMovedPieces[temporarilyMovedPiecesSize].yPos = row;
           ++temporarilyMovedPiecesSize;
           
         } else
         {
           //Move the piece to the tile to the left
           movePiece(xPosOfVictim,row,xPosOfVictim - 1, row);
           //Store the location of the temporarily moved piece so that we could move it back when we're done
           temporarilyMovedPieces[temporarilyMovedPiecesSize].startXPos = xPosOfVictim;
           temporarilyMovedPieces[temporarilyMovedPiecesSize].destXPos = xPosOfVictim - 1;
           temporarilyMovedPieces[temporarilyMovedPiecesSize].yPos = row;
           ++temporarilyMovedPiecesSize;
         }
       }
     } 
 
     //Move the piece to the deadzone
     moveZMagnet(xPosOfVictim,yPosOfVictim);
     zMotor.raise();
     //-1 is a row outside of the game board playable area, which is the deadzone
     moveZMagnet(xPosOfVictim, -1);
     zMotor.lower();
 
     //Move the pieces that were temporarily moved back to their original location
     for(int i = 0; i < temporarilyMovedPiecesSize; ++i){
       movePiece(temporarilyMovedPieces[i].destXPos, temporarilyMovedPieces[i].yPos, temporarilyMovedPieces[i].startXPos, temporarilyMovedPieces[i].yPos);
     }
   } // END IF PIECE IS BEING EATEN
   
   moveZMagnet(startXPos,startYPos);
   //Raise the zMotor to grab the piece
   zMotor.raise();
   moveZMagnet(destXPos,destYPos);
 
   //Lower the motor to release hold of piece
   zMotor.lower();
 
   //Update gameTiles status
   gameTiles[destXPos][destYPos] = gameTiles[startXPos][startYPos];
   gameTiles[startXPos][startYPos] = -1;
  }

//Update the status of the previous game status to the current,
//so that we could update the current  game status with the values
//from the digital pins
void transferGameTileStatus(){
  previousGameTileStatus[1][0] = currentGameTileStatus[1][0];
  previousGameTileStatus[3][0] = currentGameTileStatus[3][0];
  previousGameTileStatus[0][1] = currentGameTileStatus[0][1];
  previousGameTileStatus[2][1] = currentGameTileStatus[2][1];
  previousGameTileStatus[1][2] = currentGameTileStatus[1][2];
  previousGameTileStatus[3][2] = currentGameTileStatus[3][2];
  previousGameTileStatus[0][3] = currentGameTileStatus[0][3];
  previousGameTileStatus[2][3] = currentGameTileStatus[2][3];
  previousGameTileStatus[1][4] = currentGameTileStatus[1][4];
  previousGameTileStatus[3][4] = currentGameTileStatus[3][4];
  previousGameTileStatus[0][5] = currentGameTileStatus[0][5];
  previousGameTileStatus[2][5] = currentGameTileStatus[2][5];
  previousGameTileStatus[1][6] = currentGameTileStatus[1][6];
  previousGameTileStatus[3][6] = currentGameTileStatus[3][6];
  previousGameTileStatus[0][7] = currentGameTileStatus[0][7];
  previousGameTileStatus[2][7] = currentGameTileStatus[2][7];
}

//Reads the digital pins to get the state of the game
//and updates the previousGameTileStatus and currentGameTileStatus.
void getGameTileStatus(){
  transferGameTileStatus();
  
  currentGameTileStatus[1][0] = digitalRead(switch10);
  currentGameTileStatus[3][0] = digitalRead(switch30);
  currentGameTileStatus[0][1] = digitalRead(switch01);
  currentGameTileStatus[2][1] = digitalRead(switch21);
  currentGameTileStatus[1][2] = digitalRead(switch12);
  currentGameTileStatus[3][2] = digitalRead(switch32);
  currentGameTileStatus[0][3] = digitalRead(switch03);
  currentGameTileStatus[2][3] = digitalRead(switch23);
  currentGameTileStatus[1][4] = digitalRead(switch14);
  currentGameTileStatus[3][4] = digitalRead(switch34);
  currentGameTileStatus[0][5] = digitalRead(switch05);
  currentGameTileStatus[2][5] = digitalRead(switch25);
  currentGameTileStatus[1][6] = digitalRead(switch16);
  currentGameTileStatus[3][6] = digitalRead(switch36);
  currentGameTileStatus[0][7] = digitalRead(switch07);
  currentGameTileStatus[2][7] = digitalRead(switch27);
}

//////////////WEB POST/GET SECTION
///Checks to see which move the player made, and posts that move to the game server
void endPlayerTurn(){
  //Find which piece moved
  for (int column = 0; column < BOARD_WIDTH; ++column){
    for (int row = 0; row < BOARD_HEIGHT; ++row){
      // If the tile state changed
      if(currentGameTileStatus[column][row] != previousGameTileStatus[column][row])
      {
        //The tile moved FROM this tile
        if(currentGameTileStatus[column][row] > previousGameTileStatus[column][row])
        {
          //In the case we eat a piece, we will have two tiles whose state
          //were changed.  e.g the piece that ate the other piece moved, and the
          //piece that was eaten was removed from the board.
          if(gameTiles[column][row] == gamePlayer)
          { 
            sourceCol = column;
            sourceRow = row;
          }
          
        }
        else
        {
            destCol = column;
            destRow = row;
            gameTiles[column][row] = gamePlayer;
        }
      }
    }
  }

  postToGameServer("0");
}

//Sends a GET requeset to HTTP Client to get the status of the game
//Takes in either "getListOfGames" to get the list of existing games,
//or "0" to check the status of the game for the current game ID
void getGameStatus(String request){
 //Send a 'g' to the ESP8266 to let it know that we want to do a GET
Serial.println ("Sending g");
 Serial1.print('g');

  while(Serial1.readStringUntil('\n') != "*")
  {
  //Wait for wifi module to request the gameID param for the GET
  }
  
  Serial.println ("Got *");
  //ESP8266 wifi module is expecting a comma separated string with the
  //following parameters:
  // Request,gameID,numOfPlayers,currentPlayerTurn,sourceCol,sourceRow,destCol,destRow
  // We only need gameID, so we can pass in 0 as the other parameters
  String command = request + "," + (String)gameID + ",2,0,0,0,0,0";
  Serial.print(command);
  Serial1.print(command);
  /*
  Serial1.print(request);
  Serial1.print(',');
  Serial1.print(gameID);
  Serial1.println(",0,0,0,0,0,0");
*/
  while(Serial1.available() < 1)
  {
    //Wait for GET response from ESP8266
  }
  Serial.println ("GOT ESP RESPONSE");

  //GET Request is complete, so read the JSON response from the server
  //and start parsing
  String x;
  String responseString = "";
  while((x = Serial1.readStringUntil('\n')) != "COMPLETE\r")
  {
    responseString +=x;
     // J-SON!!!!! Parse the incoming string
  }

  if(request  == "getListOfGames")
  {
    parseServerResponse(responseString, LIST_OF_GAMES);
  }else{
    parseServerResponse(responseString, GAME_STATUS);
  }

 }


//Takes in a request (either "createGame", "joinGame", "deleteGame", or "0");
//"0" means it will post the game status of the current gameID to the server.
void postToGameServer(String request)
{
    //Send a 'p' to let the ESP8266 know that we want to do a POST
    Serial.println("Sending p");
  Serial1.println('p');

  String x;
  while((x = Serial1.readStringUntil('\n')) != "*\r")
  {
    Serial.println(x);
    Serial.println("Waiting...");
    //Wait for the ESP8266 to request the POST parameters
  }

Serial.println("Got *");
  //ESP8266 wifi module is expecting a comma separated string with the
  //following parameters:
  // Request,gameID,numOfPlayers,currentPlayerTurn,sourceCol,sourceRow,destCol,destRow
  // If the passed in parameters are not needed for the specified request,
  // then they are ignored
  int nextPlayer = gamePlayer == 1 ? 2 : 1;
  
  Serial1.print(request); //Request
  Serial1.print(',');
  Serial1.print(gameID);
  Serial1.print(",2,"); //numOfPlayers
  Serial1.print(nextPlayer);
  Serial1.print(',');
  Serial1.print(sourceCol);
  Serial1.print(',');
  Serial1.print(sourceRow);
  Serial1.print(',');
  Serial1.print(destCol);
  Serial1.print(',');
  Serial1.print(destRow);
  
String responseString = "";
  
  while((x = Serial1.readStringUntil('\n')) != "COMPLETE\r")
  {
    responseString += x;
  }

Serial.println("Parameters sent:");
Serial.print(request); Serial.print(','); Serial.print(gameID); Serial.print(",2,"); Serial.print(nextPlayer); Serial.print(","); Serial.print(sourceCol);
Serial.print(','); Serial.print(sourceRow); Serial.print(','); Serial.print(destCol); Serial.print(','); Serial.println(destRow);
Serial.println("Sent parameters");
 



  //Parse the header out of the HTTP response until we get our PHP response
  parseServerResponse(responseString, GAME_STATUS);
 
}

void parseServerResponse(String response, ServerResponseType type){
  response.remove(0,response.indexOf('{'));
  
  switch(type)
  {
    case GAME_STATUS:
      parseGameStatusResponse(response);
      break;
    case LIST_OF_GAMES:
      parseListOfGamesResponse(response);
      break;
    default:
      break;
  }
}

void parseListOfWifiNetworks(String responseString){
  Serial.print("Response:");
  Serial.println(responseString);
  int endIndex = responseString.indexOf(',');
  wifiListSize = responseString.substring(0,endIndex).toInt();
  responseString.remove(0,endIndex+1);
  
  
  Serial.print("Num of networks:");
  Serial.println(wifiListSize);
  for(int i = 0; i < wifiListSize; ++i){
    if(i != wifiListSize - 1){
      endIndex = responseString.indexOf(',');
    }
    else{
      endIndex = responseString.indexOf('*');
    }
    String network = responseString.substring(0,endIndex);
    ssid[i] = network;
    Serial.print("Network: ");
    Serial.println(ssid[i]);
    responseString.remove(0,endIndex+1);
    Serial.println(ssid[i]);
  }

    Serial.println("Parsed networks");
    for(int i = 0; i < wifiListSize; ++i){
      Serial.println(ssid[i]);
   }
}

//List of games response is {numOfGames,gameId1, gameId2,gameIdN}
void parseListOfGamesResponse(String responseString){
  Serial.println("Before parsing:");
  Serial.println(responseString);
  int endIndex = responseString.indexOf(',');
  gameListSize = responseString.substring(1,endIndex ).toInt();
  responseString.remove(0, endIndex + 1 );

  gameID = 0;
  int foundGameID = 0;
  //int games[100];
  //memset(games,0,sizeof(games));
  Serial.println("Parsing games");
  for(int i = 0; i < gameListSize; ++i)
  {
    endIndex = responseString.indexOf(',');
    Serial.print("Converting to int:");
    Serial.println(responseString.substring(0,endIndex));
    foundGameID = responseString.substring(0,endIndex).toInt();
    Serial.println(foundGameID);
    responseString.remove(0,endIndex + 1);
    //Serial.println(responseString);
    games[i] = foundGameID;
  }
  Serial.println("Games IDs:");
  for(int i = 0; i < gameListSize; ++i){
    Serial.println(games[i]);
  }
}

void parseGameStatusResponse(String responseString){
  int endIndex = responseString.indexOf(',');
  gameStatus = responseString.substring(1,endIndex ).toInt();
   Serial.println(responseString);
  responseString.remove(0, endIndex + 1 );
  Serial.println(responseString);
  //Parse gameID
  endIndex  = responseString.indexOf(',');
  gameID = responseString.substring(0,endIndex).toInt();;
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse numOfPlayers
  endIndex  = responseString.indexOf(',');
  numOfPlayers = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse currentPlayerTurn
  endIndex  = responseString.indexOf(',');
  currentPlayerTurn = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse sourceCol
  endIndex  = responseString.indexOf(',');
  sourceCol = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse sourceRow
  endIndex  = responseString.indexOf(',');
  sourceRow = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse destCol
  endIndex  = responseString.indexOf(',');
  destCol = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse destRow
  endIndex  = responseString.indexOf('}');
  destRow = responseString.substring(0,endIndex).toInt();

  Serial.println(gameStatus);
  Serial.println(gameID);
  Serial.println(numOfPlayers);
  Serial.println(currentPlayerTurn);
  Serial.println(sourceCol);
  Serial.println(sourceRow);
  Serial.println(destCol);
  Serial.println(destRow);
  
}

void getWifiNetworks(){
  Serial.println("Sending s");
  Serial1.print('s');
  //Serial.println("Sent s");
  
  while(Serial1.readStringUntil('\n') != "*\r"){
      //Wait
    
  }

  String response;
  String x;
  Serial.println("Getting networks...");
  while((x = Serial1.readStringUntil('\n')) != "COMPLETE\r"){
    response += x;
  }
  Serial.println("Done");
  
  parseListOfWifiNetworks(response);
  
  
}

void connectToWifi(String ssid, String password)
{
  Serial.println("Sending n");
  Serial1.print('n'); 
  
  while(Serial1.readStringUntil('\n') != "*"){
 
    Serial.println("Waiting.");
  }
  Serial.print("Sending network ssid:");
  Serial.println(ssid);
  Serial1.print(ssid);

   while(Serial1.readStringUntil('\n') != "*"){
    //Wait
  }

  Serial.print("Sending network password:");
  Serial.println(password);
  Serial1.print(password);
}

void disconnectFromWifi(){
  Serial.println("Sending d");
  Serial1.print('d');
  String disconnectStatus = Serial1.readStringUntil('\n');

  Serial.println("Disconnected");
}
/////////END WEB POST/GET

/////////Begin TFT page setup

//create button
unsigned long createButton(String text, int x, int y, int w, int h, int btnColor, int txtColor, int txtSize) {
  //Draw Button UX
 
  tft.fillRect(x, y, w, h, btnColor);
  tft.drawRect(x, y, w, h, txtColor);
  //Write Button UX
  tft.setCursor(x+ (w/4), y +(h/4));
 
  tft.setTextColor(txtColor);
  tft.setTextSize(txtSize);
  tft.println(text);


  return 1;
  }

  //create button
unsigned long createButton(int text, int x, int y, int w, int h, int btnColor, int txtColor, int txtSize) {
  //Draw Button UX
 
  tft.fillRect(x, y, w, h, btnColor);
  tft.drawRect(x, y, w, h, txtColor);
  //Write Button UX
  tft.setCursor(x+ (w/4), y +(h/4));
 
  tft.setTextColor(txtColor);
  tft.setTextSize(txtSize);
  tft.println(text);


  return 1;
  }

  //create the waiting page
  void waitingPage(){
    currentPage = "waiting page";
    tft.fillScreen(BLACK);
    createButton("Main Menu",80,280,80,40, BLUE, WHITE, 1);

     tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("Waiting for opponent.");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Waiting for opponent..");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Waiting for opponent...");
}
void connectingPage(){
    currentPage = "connecting page";
    tft.fillScreen(BLACK);
   

     tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("Connecting to wifi.");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Connecting to wifi..");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Connecting to wifi...");
}
void lookingForWifiPage(){
    currentPage = "looking for wifi page";
    tft.fillScreen(BLACK);
   

     tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("Looking for wifi...");
  
}
void gettingListOfGamesPage(){
    currentPage = "getting list of games page";
    tft.fillScreen(BLACK);
   

     tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("Getting list of games...");
 
}
void disconnectedPage(){
    currentPage = "disconnected page";
    tft.fillScreen(BLACK);
   

  tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setCursor(60, 70);  
  tft.println("Disconnected from the game server.");
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("redirecting to Main Menu.");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("redirecting to Main Menu..");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("redirecting to Main Menu...");
}
//create main menu
void mainMenu(){
     currentPage = "main menu";
     tft.fillScreen(BLACK);
     createButton("Connect to Wifi",50,40,140,60, BLUE, WHITE, 1);
     createButton("Join Game",50,130,140,60, BLUE, WHITE, 1);
     createButton("Create Game",50,220,140,60, BLUE, WHITE, 1);
     
  }

  // create wifi page
   void wifiPage(){
       currentPage = "wifi page";
       tft.fillScreen(BLACK);
       if(ssidTracker < wifiListSize){
       createButton(ssid[ssidTracker],25,40,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
        if(ssidTracker < wifiListSize){
       createButton(ssid[ssidTracker],135,40,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
        if(ssidTracker < wifiListSize){ 
       createButton(ssid[ssidTracker],25,120,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
        if(ssidTracker < wifiListSize){
       createButton(ssid[ssidTracker],135,120,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
        if(ssidTracker < wifiListSize){
       createButton(ssid[ssidTracker],25,200,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
       if(ssidTracker < wifiListSize){
       createButton(ssid[ssidTracker],135,200,80,40, BLUE, WHITE, 1);
       
       }
       ssidTracker++;
       createButton("Menu",0,280,80,40, BLUE, WHITE, 1);
       if(ssidTracker < wifiListSize){
       createButton("Next",160,280,80,40, BLUE, WHITE, 1);
       }
       if(previous){
        createButton("Previous",80,280,80,40, BLUE, WHITE, 1);
       }
  }

  //create page of list of games
   void gameListPage(){
       currentPage = "game list page";
       tft.fillScreen(BLACK);
       if(gameTracker < gameListSize){
       createButton(games[gameTracker],25,40,80,40, BLUE, WHITE, 1);
       
       }
       gameTracker++;
        if(gameTracker < gameListSize){
       createButton(games[gameTracker],135,40,80,40, BLUE, WHITE, 1);
      
       }
        gameTracker++;
        if(gameTracker < gameListSize){ 
       createButton(games[gameTracker],25,120,80,40, BLUE, WHITE, 1);
       
       }
       gameTracker++;
        if(gameTracker < gameListSize){
       createButton(games[gameTracker],135,120,80,40, BLUE, WHITE, 1);
       
       }
       gameTracker++;
        if(gameTracker < gameListSize){
       createButton(games[gameTracker],25,200,80,40, BLUE, WHITE, 1);
       
       }
       gameTracker++;
       if(gameTracker < gameListSize){
       createButton(games[gameTracker],135,200,80,40, BLUE, WHITE, 1);
       
       }
       gameTracker++;
       createButton("Menu",0,280,80,40, BLUE, WHITE, 1);
       if( gameTracker < gameListSize){
       createButton("Next",160,280,80,40, BLUE, WHITE, 1);
       }
       if(previous){
        createButton("Previous",80,280,80,40, BLUE, WHITE, 1);
       }
       
       
  }

  //create players move waiting page
   void yourMovePage(){
    
        currentPage = "your move page";
       tft.fillScreen(BLACK);

        
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 20);
  tft.print("You are in game: ");
  tft.println(gameID);
  tft.setCursor(60, 100);  
  tft.println("Your move.");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Your move..");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("Your move...");
       createButton("Quit",0,280,80,40, BLUE, WHITE, 1);
 
       createButton("Done",160,280,80,40, BLUE, WHITE, 1);

       
  }

  //create opponents move waiting page
   void opponentMovePage(){
    currentPage = "opponent move page";
    tft.fillScreen(BLACK);
    createButton("Quit",80,280,80,40, BLUE, WHITE, 1);

     tft.setTextColor(WHITE);
  tft.setTextSize(1.5);
  tft.setTextColor(WHITE);
  tft.setCursor(60, 100);  
  tft.println("opponent move.");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("opponent move..");
  delay(700);
  tft.setCursor(60, 100);
  tft.println("opponent move...");



  }

//create password input with keyboard page
void passwordPage(){

    


    currentPage = "password page";
    tft.fillScreen(BLACK);
     createButton("Back",10,20,50,40, BLUE, WHITE, 1);
     tft.drawRect(60, 20, 180, 40, WHITE);

     createButton("Done",10,80,70,20, BLUE, WHITE, 1);
    createButton("Space",85,80,70,20, BLUE, WHITE, 1);
    createButton("Delete",160,80,70,20, BLUE, WHITE, 1);

     createButton("A",10,110,20,20, BLUE, WHITE, 1);
    createButton("B",35,110,20,20, BLUE, WHITE, 1);
    createButton("C",60,110,20,20, BLUE, WHITE, 1);
    createButton("D",85,110,20,20, BLUE, WHITE, 1);
    createButton("E",110,110,20,20, BLUE, WHITE, 1);
    createButton("F",135,110,20,20, BLUE, WHITE, 1);
    createButton("G",160,110,20,20, BLUE, WHITE, 1);
    createButton("H",185,110,20,20, BLUE, WHITE, 1);
    createButton("I",210,110,20,20, BLUE, WHITE, 1);

    createButton("J",10,140,20,20, BLUE, WHITE, 1);
    createButton("K",35,140,20,20, BLUE, WHITE, 1);
    createButton("L",60,140,20,20, BLUE, WHITE, 1);
    createButton("M",85,140,20,20, BLUE, WHITE, 1);
    createButton("N",110,140,20,20, BLUE, WHITE, 1);
    createButton("O",135,140,20,20, BLUE, WHITE, 1);
    createButton("P",160,140,20,20, BLUE, WHITE, 1);
    createButton("Q",185,140,20,20, BLUE, WHITE, 1);
    createButton("R",210,140,20,20, BLUE, WHITE, 1);

    createButton("S",10,170,20,20, BLUE, WHITE, 1);
    createButton("T",35,170,20,20, BLUE, WHITE, 1);
    createButton("U",60,170,20,20, BLUE, WHITE, 1);
    createButton("V",85,170,20,20, BLUE, WHITE, 1);
    createButton("W",110,170,20,20, BLUE, WHITE, 1);
    createButton("X",135,170,20,20, BLUE, WHITE, 1);
    createButton("Y",160,170,20,20, BLUE, WHITE, 1);
    createButton("Z",185,170,20,20, BLUE, WHITE, 1);
    createButton("a",210,170,20,20, BLUE, WHITE, 1);


    createButton("b",10,200,20,20, BLUE, WHITE, 1);
    createButton("c",35,200,20,20, BLUE, WHITE, 1);
    createButton("d",60,200,20,20, BLUE, WHITE, 1);
    createButton("e",85,200,20,20, BLUE, WHITE, 1);
    createButton("f",110,200,20,20, BLUE, WHITE, 1);
    createButton("g",135,200,20,20, BLUE, WHITE, 1);
    createButton("h",160,200,20,20, BLUE, WHITE, 1);
    createButton("i",185,200,20,20, BLUE, WHITE, 1);
    createButton("j",210,200,20,20, BLUE, WHITE, 1);

    

    createButton("k",10,230,20,20, BLUE, WHITE, 1);
    createButton("l",35,230,20,20, BLUE, WHITE, 1);
    createButton("m",60,230,20,20, BLUE, WHITE, 1);
    createButton("n",85,230,20,20, BLUE, WHITE, 1);
    createButton("o",110,230,20,20, BLUE, WHITE, 1);
    createButton("p",135,230,20,20, BLUE, WHITE, 1);
    createButton("q",160,230,20,20, BLUE, WHITE, 1);
    createButton("r",185,230,20,20, BLUE, WHITE, 1);
    createButton("s",210,230,20,20, BLUE, WHITE, 1);

    createButton("t",10,260,20,20, BLUE, WHITE, 1);
    createButton("u",35,260,20,20, BLUE, WHITE, 1);
    createButton("v",60,260,20,20, BLUE, WHITE, 1);
    createButton("w",85,260,20,20, BLUE, WHITE, 1);
    createButton("x",110,260,20,20, BLUE, WHITE, 1);
    createButton("y",135,260,20,20, BLUE, WHITE, 1);
    createButton("z",160,260,20,20, BLUE, WHITE, 1);
    createButton(".",185,260,20,20, BLUE, WHITE, 1);
    createButton(",",210,260,20,20, BLUE, WHITE, 1);
    
    createButton("!",10,290,20,20, BLUE, WHITE, 1);
    createButton("@",35,290,20,20, BLUE, WHITE, 1);
    createButton("#",60,290,20,20, BLUE, WHITE, 1);
    createButton("$",85,290,20,20, BLUE, WHITE, 1);
    createButton("%",110,290,20,20, BLUE, WHITE, 1);
    createButton("^",135,290,20,20, BLUE, WHITE, 1);
    createButton("&",160,290,20,20, BLUE, WHITE, 1);
    createButton("*",185,290,20,20, BLUE, WHITE, 1);
    createButton(";",210,290,20,20, BLUE, WHITE, 1);
 
    
  }

  void reset(){
    for(int x = 0; x < 4; x++){
      for (int y =0; y < 8; y++){
        currentGameTileStatus[x][y] = -1;
        previousGameTileStatus[x][y] = -1;
        gameTiles[x][y] = -1;
      }
    }
        gameTiles[0][1] = 1;
        gameTiles[0][7] = 2;
        gameTiles[1][0] = 1;
        gameTiles[1][6] = 2;
        gameTiles[2][1] = 1;
        gameTiles[2][7] = 2;
        gameTiles[3][0] = 1;
        gameTiles[3][6] = 2;
        
        
  }


void setup() {
  setupMotors();
  setupReedSwitchPins();
  memset(games,0,sizeof(games));
  //memset(ssid,'n',sizeof(ssid));
  
  Serial.begin(115200);

  //ESP8266 serial
  Serial1.begin(115200);





  
  tft.reset();

   uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));



      
    return;
  }

  tft.begin(identifier);

  tft.fillScreen(BLACK);


  pinMode(13, OUTPUT);

  mainMenu();

}
// define the min and max pressure for the tft
#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop() {
   digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
 


  pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(YM, OUTPUT);

if(gameID != 0 && p.z < MINPRESSURE){
  Serial.print("Game ID is:");
  Serial.println(gameID);
 getGameStatus("0");
}
 if(gameStatus == -1 && gameID != 0){
  disconnectedPage();
  delay(2000);
  
  mainMenu();
 }
 //If you're waiting for an opponent move and they just made a move
 if(gamePlayer  == currentPlayerTurn && currentPage == "opponent move page"){
 
  movePiece(sourceCol, sourceRow, destCol, destRow); //update game tiles array
  yourMovePage();
 }
 //If you're on the waiting page and someone joins your game
 if(numOfPlayers  == 2 && currentPage == "waiting page"){
  yourMovePage();
 }
 
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    Serial.print("pressure : ");
    Serial.print(p.z);
    Serial.print(" x : ");
    Serial.print(p.x);
    Serial.print(" y : ");
    Serial.println(p.y);

   if(currentPage == "waiting page" || currentPage == "opponent move page"){
   
      if(p.x > 80 && p.x < 160){
        //main menu button
      if(p.y > 280 && p.y < 320){
        postToGameServer("deleteGame");
        gameID = 0;
        mainMenu();
      }
      }
    }
   else if(currentPage == "your move page"){
      if(p.x > 160 && p.x < 240){
            if(p.y > 280 && p.y < 320){
            endPlayerTurn();  
            opponentMovePage();
            }
            }
      else  if(p.x > 0 && p.x < 80){
        if(p.y > 280 && p.y < 320){
              postToGameServer("deleteGame");
              gameID = 0;
              mainMenu();
        }
          }
   }
    else if(currentPage == "main menu"){
      if(p.x > 50 && p.x < 190){
        //connect to wifi button
        if(p.y > 40 && p.y < 100){
          lookingForWifiPage();
          getWifiNetworks();
          wifiPage();
        }
        //join game button
        else if(p.y > 130 && p.y < 190){
          gettingListOfGamesPage();
          getGameStatus("getListOfGames");
          gamePlayer = 2;
          gameListPage();
        }
        //create game button
        else if(p.y > 220 && p.y < 280){
          postToGameServer("createGame");
          reset();
          gamePlayer = 1; 
          waitingPage();
        }
      }
    }

    else if(currentPage == "game list page"){
      if(p.x > 25 && p.x < 105){
        if(p.y > 40 && p.y < 80){
      
          gameID = games[gameTracker-6];
          postToGameServer("joinGame");
          gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
        else if(p.y > 120 && p.y < 160){
         
          gameID = games[gameTracker-4];
          postToGameServer("joinGame");
          gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
        else if(p.y > 200 && p.y < 240){
          
          gameID = games[gameTracker-2];
           postToGameServer("joinGame");
           gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
      }
       if(p.x > 135 && p.x < 215){
        if(p.y > 40 && p.y < 80){
         
          gameID = games[gameTracker-5];
           postToGameServer("joinGame");
           gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
        else if(p.y > 120 && p.y < 160){
          
          gameID = games[gameTracker-3];
          postToGameServer("joinGame");
          gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
        else if(p.y > 200 && p.y < 240){
          
          gameID = games[gameTracker-1];
          postToGameServer("joinGame");
          gamePlayer = 2;
          opponentMovePage();
          gameTracker = 0;
        }
      }
       if(p.x > 0 && p.x < 80){
        //'previous' button
        if(p.y > 280 && p.y < 320){
          previous = false;
          gameTracker = 0;
          mainMenu();
        }
          }

         if(p.x > 160 && p.x < 240){
            if(p.y > 280 && p.y < 320){
              Serial.println("next");
              previous = true;
              gameListPage();
            }
            }
        if(p.x > 80 && p.x < 160){
          if(p.y > 280 && p.y < 320){
          gameTracker = gameTracker - 12;
          Serial.println("ONE");
          if(previous && gameTracker < 7){
            Serial.println("TWO");
            previous = false;
            gameListPage();
             Serial.println(previous);
          }
          else{
            Serial.println("THREE");
            gameListPage();
          }
          
        }
      }

    }
    else if(currentPage == "wifi page"){
      if(p.x > 25 && p.x < 105){
        if(p.y > 40 && p.y < 80){
       
          ssidText = ssid[ssidTracker-6];
          passwordPage();
          
        }
        else if(p.y > 120 && p.y < 160){
        
          ssidText = ssid[ssidTracker-4];
          passwordPage();
        }
        else if(p.y > 200 && p.y < 240){
          
          ssidText = ssid[ssidTracker-2];
          passwordPage();
        }
      }
       if(p.x > 135 && p.x < 215){
        if(p.y > 40 && p.y < 80){
    
          ssidText = ssid[ssidTracker-5];
          passwordPage();
        }
        else if(p.y > 120 && p.y < 160){
          
          ssidText = ssid[ssidTracker-3];
          passwordPage();
        }
        else if(p.y > 200 && p.y < 240){
          
          ssidText = ssid[ssidTracker-1];
          passwordPage();
        }
      }
       if(p.x > 0 && p.x < 80){
        if(p.y > 280 && p.y < 320){
          ssidTracker = 0;
          previous = false;
          mainMenu();
          Serial.println(previous);
        }
       }
         if(p.x > 160 && p.x < 240){
            if(p.y > 280 && p.y < 320){
              Serial.println("next");
              previous = true;
              wifiPage();
            }
            }
        if(p.x > 80 && p.x < 160){
          if(p.y > 280 && p.y < 320){
          ssidTracker = ssidTracker - 12;
          if(previous && ssidTracker < 7){
            previous = false;
            wifiPage();
          }
          else{
            wifiPage();
          }
          
        }
      }
        

    }
    else if(currentPage == "password page"){
      //back button
      if(p.y > 20 && p.y < 60){
        if(p.x > 10 && p.x < 60){
          ssidTracker = 0;
          ssidText = "";
          text = "";
          wifiPage();
        }
      }
      //done button
    else if(p.y > 80+10 && p.y < 100+10){
        if(p.x > 10 && p.x < 80){
          disconnectFromWifi();
          connectingPage();
          connectToWifi(ssidText, text);    

          ssidText = "";
          text = "";
          delay(5000);
          mainMenu();
        }
        //space button
        else if(p.x > 85 && p.x < 155){
          
           text = text + " ";
          tft.setCursor(65, 35);  
          tft.println(text);
          delay(500);
        }
        //delete button
        else if(p.x > 160 && p.x < 230){
         
          int index = text.length() - 1;
          String toDel = "";
          String newText = "";
          for(int i = 0; i < text.length()-1; i++){
            toDel = toDel + " ";
            newText = newText + text[i];
          }
          toDel = toDel + text[index];
          tft.setCursor(65, 35);  
          tft.setTextColor(BLACK);
          tft.println(toDel);
          tft.setTextColor(WHITE);
          text = newText;
          delay(500);
          
        }
      }
      else if(p.y > 110+10 && p.y < 130+10){
        if(p.x > 10 && p.x < 30){
          
          text = text + "A";
          tft.setCursor(65, 35);  
          tft.println(text);
          delay(500);
        }
        else if(p.x > 35 && p.x < 55){
        
           text = text + "B";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
       
           text = text + "C";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
     
           text = text + "D";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
         
           text = text + "E";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
      
           text = text + "F";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
          
           text = text + "G";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
          
           text = text + "H";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
        
           text = text + "I";
            tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
     else if(p.y > 140+10 && p.y < 160+10){
        if(p.x > 10 && p.x < 30){
       
          text = text + "J";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
      
          text = text + "K";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
         
          text = text + "L";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
         
          text = text + "M";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
      
          text = text + "N";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
        
          text = text + "O";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
         
          text = text + "P";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
          
          text = text + "Q";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
         
          text = text + "R";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
      else if(p.y > 170+10 && p.y < 190+10){
        if(p.x > 10 && p.x < 30){
  
          text = text + "S";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
         
          text = text + "T";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
          
          text = text + "U";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
         
          text = text + "V";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
     
          text = text + "W";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
    
          text = text + "X";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
       
          text = text + "Y";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
        
          text = text + "Z";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
       
          text = text + "a";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
      else if(p.y > 200+10 && p.y < 220+10){
        if(p.x > 10 && p.x < 30){
        
          text = text + "b";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
        
          text = text + "c";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
        
          text = text + "d";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
        
          text = text + "e";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
         
          text = text + "f";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
         
          text = text + "g";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
    
          text = text + "h";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
        
          text = text + "i";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
     
          text = text + "j";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
      else if(p.y > 230+10 && p.y < 250+10){
        if(p.x > 10 && p.x < 30){
        
          text = text + "k";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
       
          text = text + "l";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
      
          text = text + "m";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
         
          text = text + "n";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
        
          text = text + "o";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
         
          text = text + "p";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
        
          text = text + "q";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
    
          text = text + "r";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
       
          text = text + "s";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
      else if(p.y > 260+10 && p.y < 280+10){
        if(p.x > 10 && p.x < 30){
        
          text = text + "t";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
       
          text = text + "u";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
         
          text = text + "v";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
      ;
          text = text + "w";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
         
          text = text + "x";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
         
          text = text + "y";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
         
          text = text + "z";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
     
          text = text + ".";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
        
          text = text + ",";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
     else if(p.y > 290+10 && p.y < 310+10){
        if(p.x > 10 && p.x < 30){
          
          text = text + "!";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 35 && p.x < 55){
        
          text = text + "@";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 60 && p.x < 80){
         
          text = text + "#";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 85 && p.x < 105){
        
          text = text + "$";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 110 && p.x < 130){
         
          text = text + "%";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 135 && p.x < 155){
         
          text = text + "^";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 160 && p.x < 180){
     
          text = text + "&";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 185 && p.x < 205){
         
          text = text + "*";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
        else if(p.x > 210 && p.x < 230){
        
          text = text + ";";
           tft.setCursor(65, 35);  
          tft.println(text);
           delay(500);
        }
      }
    }
  }
  
}





