#include <ESP8266WiFi.h>


WiFiClient client;
char* ssid = "John";
char* password = "holaamigo";

const char* host = "www.abugharbieh.com";
const int httpsPort = 80;

bool isConnected = false;

bool wifiConnection = false;


char ssidSelection [50];
char passSelection [50];

void setup() {
  //pinMode(13,OUTPUT);

  
  Serial.begin(115200);
  
  while(!Serial){}

   // initializeWifiClient();
  //testPOST();
  //testGET();

  
}

void loop() {
  if(Serial.available()>0 && isConnected == false){
    
    char command = (char)Serial.read();
    if(command == 'p'){
       Serial.println("\nInitializing Client");
       initializeWifiClient();
       isConnected = true;
       testPOST();
       Serial.println("POST sent");
    }
    else if(command == 'g'){
       Serial.println("\nInitializing Client");
       initializeWifiClient();
       isConnected = true;
       testGET();
       Serial.println("GET sent");
    }
     else if(command == 's'){
      scanNets();
    }
    else if(command == 'c'){
      connect();
    }
    else if(command == 'd'){
      disconnect();
    }
    else if(command == 'n'){
      networkAndPassSelection();
    }
    else if(command == 't'){
      Serial.println("before while");
      while(Serial.available() < 1){
        Serial.println("inside the loop");
     
      }
      Serial.println("after while");
       String t = Serial.readString();

      Serial.print("the string is: ");
      Serial.println(t);
    }
    
  }
  else if (Serial.available()>0 && isConnected == true) {
    Serial.println("isConnected is true");
    isConnected = false;
  }

}


void initializeWifiClient()
{
  if(wifiConnection == false){
  connect();
  }
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }

    Serial.println("connected");

}

void testPOST()
{ 

    Serial.println("*");
     while(Serial.available() < 1){
        //Serial.println("inside the loop");
     
      }

     
     String request = Serial.readStringUntil(',');
     request.trim();
     String gameID = Serial.readStringUntil(',');
     String numOfPlayers = Serial.readStringUntil(',');
     String currentPlayerTurn = Serial.readStringUntil(',');
     String sourceCol = Serial.readStringUntil(',');
     String sourceRow = Serial.readStringUntil(',');
     String destCol = Serial.readStringUntil(',');
     String destRow = Serial.readStringUntil('\n');


    delay(1000);
    String parameters = "source=ARDUINO&Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow; // for multiple parameters, do var1=val1&var2=val2...
    if(request == "deleteGame" || request == "createGame" || request == "joinGame"){
       client.println("POST /test/GameLobby.php HTTP/1.1");
    }
    else{
       client.println("POST /test/GameStatus.php HTTP/1.1");
    }
    client.println("Host: www.abugharbieh.com");
    client.print("Content-length:");
    client.println(parameters.length());
    client.println("Connection: Close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.println();
    client.println(parameters);
    Serial.println("POST Request Sent.");
    //delay(3000);
    Serial.println("Response String:");
    while(client.connected())
    {
      if(client.available()){
        char str=client.read();
        Serial.print(str);
      }
    }

      if(!client.connected()){
        Serial.println("Params received from ESP:");
        Serial.print(request); //Request
  Serial.print(',');
  Serial.print(gameID);
  Serial.print(",2,"); //numOfPlayers
  Serial.print(currentPlayerTurn);
  Serial.print(',');
  Serial.print(sourceCol);
  Serial.print(',');
  Serial.print(sourceRow);
  Serial.print(',');
  Serial.print(destCol);
  Serial.print(',');
  Serial.print(destRow);
                  Serial.println("post disconnected the client");
                 isConnected = false;
  }

  Serial.println("COMPLETE");
    
}

void testGET(){
    Serial.print("*");
    
     while(Serial.available() < 1){
        //Wait for get parameters
      }
      
     
     String request = Serial.readStringUntil(',');
     String gameID = Serial.readStringUntil(',');
     String numOfPlayers = Serial.readStringUntil(',');
     String currentPlayerTurn = Serial.readStringUntil(',');
     String sourceCol = Serial.readStringUntil(',');
     String sourceRow = Serial.readStringUntil(',');
     String destCol = Serial.readStringUntil(',');
     String destRow = Serial.readStringUntil('\n');

    Serial.print("request:");
    Serial.println(request);
    Serial.print("gameID:");
    Serial.println(gameID);
    Serial.println("numOfPlayers:");
    Serial.print(numOfPlayers);
     if (request == "getListOfGames"){
  client.println("GET /test/GameLobby.php?source=ARDUINO&Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow+" HTTP/1.1");
     }
     else{
      client.println("GET /test/GameStatus.php?source=ARDUINO&Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow+" HTTP/1.1");
     
     }
  client.println("Host: www.abugharbieh.com");
   client.println("Connection: Close");
   client.println("Content-Type: application/x-www-form-urlencoded;");
   client.println();
   Serial.println("GET Request Sent.");
   delay(3000);
    Serial.println("Response String:");
    while(client.connected())
    {
      if(client.available()){
        char str=client.read();
        Serial.print(str);
      }
    }

  if(!client.connected()){
                   Serial.println("get disconnected the client");
                 isConnected = false;
  }

  Serial.println("COMPLETE");
}

void scanNets(){
 //Serial.println("scan start");
  int n = WiFi.scanNetworks();
   //Send a * to signal the arduino that we're about to start sending the wifi networks
    Serial.println("*");
    //Send the number of wifi networks that were found
    Serial.print(n);
  if (n > 0){
 
    Serial.print(',');
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(WiFi.SSID(i));
      if(i != n-1){
        Serial.print(',');
      }
    }
  }
  //Signal arduino that list of networks is done
  Serial.println("*");

  //Signal arduino that scan is complete
  Serial.println("COMPLETE");

}

void disconnect(){
  Serial.println("disconnecting wifi");
  if(WiFi.disconnect()){
  wifiConnection = false;
  Serial.println("wifi disconnected");
  }
  else{
    Serial.println("wifi disconnection failed");
  }
}

void connect(){
  Serial.println("connecting wifi");
  Serial.println(ssid);
  Serial.println(password);
  if(WiFi.begin(ssid, password)){
  wifiConnection = true;
  Serial.println("wifi connected");
  }
  else{
  Serial.println("wifi connection failed");
  }
}

void networkAndPassSelection(){
   memset(ssidSelection, 0, 50);
    memset(passSelection, 0, 50);
  //Send signal to wait for network id
   Serial.print("*");
  
       while(Serial.available() < 2){
        //Serial.println("inside the loop");
     
      }
     
       String t = Serial.readStringUntil('\n');

    
   for (int i = 0; i < t.length(); i++){
    ssidSelection[i] = t[i];
 }
 
   ssid = ssidSelection;

   //Send signal to wait for network password
   Serial.print("*");
     while(Serial.available() < 1){
        //Serial.println("inside the loop");
     
      }
  
 
    String p = Serial.readStringUntil('\n');
      
      for (int i = 0; i < p.length(); i++){
    passSelection[i] = p[i];
 }
   
   password = passSelection;
  
   connect();

  
}
  



