#include <Servo.h>

const int BOARD_WIDTH = 4;
const int BOARD_HEIGHT = 8;

//Default to 1.  If we join a game, it will change to 2, if we create a game it will stay as 1.
int gamePlayer = 1;

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

struct tempMovedPiece{
  
  int startXPos;
  int destXPos;

  //yPos won't be changing for temporarily moved pieces, so we can just store 1 yPos as a general one. 
  int yPos;

};

//Game parameters to be sent to database when move is made
int sourceCol = 0;
int sourceRow = 0;
int destCol = 0;
int destRow = 0;
int gameID = 0;


//Game board tiles to calculate which move was made.
//Initialize 1 for tiles with player 1, 2 for player 2, 
//-1 for empty tiles 
int currentGameTileStatus[4][8] = {{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1}};
int previousGameTileStatus[4][8] = {{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1},{-1,-1,-1,-1,-1,-1,-1,-1}};
int gameTiles[4][8] = {{-1,1,-1,-1,-1,-1,-1,2},{1,-1,-1,-1,-1,-1,2,-1},{-1,1,-1,-1,-1,-1,-1,2},{1,-1,-1,-1,-1,-1,2,-1}};


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


const int NUM_OF_JSON_FIELDS = 8;
const int MAX_CONTENT_SIZE = 512;



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
  int numOfWifiNetworks = responseString.substring(0,endIndex).toInt();
  responseString.remove(0,endIndex+1);
  
  String wifiNetworks[200];
  Serial.print("Num of networks:");
  Serial.println(numOfWifiNetworks);
  for(int i = 0; i < numOfWifiNetworks; ++i){
    if(i != numOfWifiNetworks - 1){
      endIndex = responseString.indexOf(',');
    }
    else{
      endIndex = responseString.indexOf('*');
    }
    wifiNetworks[i] = responseString.substring(0,endIndex);
    responseString.remove(0,endIndex+1);
  }

    Serial.println("Parsed networks");
    for(int i = 0; i < numOfWifiNetworks; ++i){
      Serial.println(wifiNetworks[i]);
   }
}

//List of games response is {numOfGames,gameId1, gameId2,gameIdN}
void parseListOfGamesResponse(String responseString){
  Serial.println("Before parsing:");
  Serial.println(responseString);
  int endIndex = responseString.indexOf(',');
  int gameListSize = responseString.substring(1,endIndex ).toInt();
  responseString.remove(0, endIndex + 1 );

  int gameID = -1;
  int games[100];
  memset(games,0,sizeof(games));
  Serial.println("Parsing games");
  for(int i = 0; i < gameListSize; ++i)
  {
    endIndex = responseString.indexOf(',');
    Serial.print("Converting to int:");
    Serial.println(responseString.substring(0,endIndex));
    gameID = responseString.substring(0,endIndex).toInt();
    Serial.println(gameID);
    responseString.remove(0,endIndex + 1);
    //Serial.println(responseString);
    games[i] = gameID;
  }
  Serial.println("Games IDs:");
  for(int i = 0; i < gameListSize; ++i){
    Serial.println(games[i]);
  }
}

void parseGameStatusResponse(String responseString){
  int endIndex = responseString.indexOf(',');
  int status = responseString.substring(1,endIndex ).toInt();
   Serial.println(responseString);
  responseString.remove(0, endIndex + 1 );
  Serial.println(responseString);
  //Parse gameID
  endIndex  = responseString.indexOf(',');
  int gameID = responseString.substring(0,endIndex).toInt();;
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse numOfPlayers
  endIndex  = responseString.indexOf(',');
  int numOfPlayers = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse currentPlayerTurn
  endIndex  = responseString.indexOf(',');
  int currentPlayerTurn = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse sourceCol
  endIndex  = responseString.indexOf(',');
  int sourceCol = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse sourceRow
  endIndex  = responseString.indexOf(',');
  int sourceRow = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse destCol
  endIndex  = responseString.indexOf(',');
  int destCol = responseString.substring(0,endIndex).toInt();
  responseString.remove(0, endIndex+1);
 Serial.println(responseString);
  //Parse destRow
  endIndex  = responseString.indexOf('}');
  int destRow = responseString.substring(0,endIndex).toInt();

  Serial.println(status);
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


void setup() {
  setupMotors();
  setupReedSwitchPins();
  Serial.begin(115200);

  //ESP8266 serial
  Serial1.begin(115200);

gameID = 4;
}

void loop() {
    if(Serial.available()>0)
    {
        char letter = Serial.read();
        if(letter == 'p'){
          gameID = 17;
          postToGameServer("joinGame");
        }
        else if(letter == 'n'){
          connectToWifi("John","holaamigo");
        }
        else if(letter == 'd'){
          disconnectFromWifi();
        }else if (letter == 'g'){
          gameID = 11;
          getGameStatus("0");
        }
        else if (letter == 's'){
          getWifiNetworks();
        }
      }
  
}





