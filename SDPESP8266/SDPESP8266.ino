#include <ESP8266WiFi.h>


WiFiClient client;
char* ssid = "AGAG";
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
      Serial.println("SCANNED all networks");
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

    Serial.println("Pass your comma separated string");
     while(Serial.available() < 1){
        //Serial.println("inside the loop");
     
      }

     
     String request = Serial.readStringUntil(',');
     String gameID = Serial.readStringUntil(',');
     String numOfPlayers = Serial.readStringUntil(',');
     String currentPlayerTurn = Serial.readStringUntil(',');
     String sourceCol = Serial.readStringUntil(',');
     String sourceRow = Serial.readStringUntil(',');
     String destCol = Serial.readStringUntil(',');
     String destRow = Serial.readStringUntil('\n');

     Serial.println(request);
     Serial.println(gameID);
     Serial.println(numOfPlayers);
     Serial.println(currentPlayerTurn);
     Serial.println(sourceCol);
     Serial.println(sourceRow);
     Serial.println(destCol);
     Serial.println(destRow);
      
    delay(1000);
    String parameters = "Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow; // for multiple parameters, do var1=val1&var2=val2...
    client.println("POST /test/GameLobby.php HTTP/1.1");
    client.println("Host: www.abugharbieh.com");
    client.print("Content-length:");
    client.println(parameters.length());
    //client.println("Connection: Close");
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
        Serial.println(str);
      }
    }

      if(!client.connected()){
                  Serial.println("post disconnected the client");
                 isConnected = false;
  }
    
}

void testGET(){
    Serial.println("Pass your comma separated string");
     while(Serial.available() < 1){
        //Serial.println("inside the loop");
     
      }

     
     String request = Serial.readStringUntil(',');
     String gameID = Serial.readStringUntil(',');
     String numOfPlayers = Serial.readStringUntil(',');
     String currentPlayerTurn = Serial.readStringUntil(',');
     String sourceCol = Serial.readStringUntil(',');
     String sourceRow = Serial.readStringUntil(',');
     String destCol = Serial.readStringUntil(',');
     String destRow = Serial.readStringUntil('\n');

     if (request == "getListOfGames"){
  client.println("GET /test/GameLobby.php?Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow+" HTTP/1.1");
     }
     else{
      client.println("GET /test/GameStatus.php?Request="+request+"&gameID="+gameID+"&numOfPlayers="+numOfPlayers+"&currentPlayerTurn="+currentPlayerTurn+"&sourceCol="+sourceCol+"&sourceRow="+sourceRow+"&destCol="+destCol+"&destRow="+destRow+" HTTP/1.1");
     
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
        Serial.println(str);
      }
    }

  if(!client.connected()){
                   Serial.println("get disconnected the client");
                 isConnected = false;
  }
}

void scanNets(){
 Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
  Serial.println("");

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
   scanNets();
  
   Serial.println("please select your network:");
  
       while(Serial.available() < 2){
        //Serial.println("inside the loop");
     
      }
     
       String t = Serial.readStringUntil('\n');

    
   for (int i = 0; i < t.length(); i++){
    ssidSelection[i] = t[i];
 }
 
   ssid = ssidSelection;
   
   Serial.println("please select your password :");
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
  



