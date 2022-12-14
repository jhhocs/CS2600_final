#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include "secrets.h"

#include <Keypad.h>

//For LED Matix
int latchPin = 2;
int clockPin = 4;
int dataPin = 15;

int matrixLights[8] = {0, 0, 0, 0, 0, 0, 0, 0};
char board[3][3];

//Define the symbols on the buttons of the keypad
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[4] = {14, 27, 26, 25}; // connect to the row pinouts of the keypad
byte colPins[4] = {13, 21, 22, 23}; // connect to the column pinouts of the keypad

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);

//Network name and password are stored in secrets.h file
char ssid[] = SSID;
char pass[] = PASS;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
const int port = 1883;
const char startTopic[] = "ticTacToe/gameStart";
const char gameStateTopic[] = "ticTacToe/gameState";
const char player1MoveTopic[] = "ticTacToe/p1Move";
int gameStarted = 0;
int player = 0;

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;
void setup() {

  //Set pins to output for LED matrix
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //MQTT
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();


  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  Serial.print("Subscribing to topic: ");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe(startTopic);
  mqttClient.subscribe(gameStateTopic);

  // topics can be unsubscribed using:
  // mqttClient.unsubscribe(topic);
  Serial.println(startTopic);
  Serial.println(gameStateTopic);

  Serial.println();
}
void loop() {
  // Get the character input
  char keyPressed = myKeypad.getKey();

  mqttClient.poll();

  if(gameStarted == 0) {
    if(keyPressed == '0') {
      mqttClient.beginMessage(startTopic);
      mqttClient.print(1);
      mqttClient.endMessage();
    }
  }
   // Define a one-byte variable (8 bits) which is used to represent the selected state of 8 column.
  int cols = 128;  
  for(int i = 0; i < 8; i++) {
    matrixRowsVal(matrixLights[i]);
    matrixColsVal(~cols);
    delay(1);
    cols >>= 1;
  }

  // If there is a character input, sent it to the serial port
  if (keyPressed && player == 1) {
    Serial.println(keyPressed);
    mqttClient.beginMessage(player1MoveTopic);
    mqttClient.print(keyPressed);
    mqttClient.endMessage();
  }
}

void updateMatrixLights() {
  //Reset lights
  for(int i = 0; i < 8; i++) {
    matrixLights[i] = 0;    
  }

  //Read board and update lights
  for(int i = 0; i < 3; i++) {
    int l;
    if(i == 0) {
      l = 0;
    }
    else if(i == 1) {
      l = 3;
    }
    else {
      l = 6;
    }
    for(int j = 0; j < 3; j++) {
      if(j == 0) {
        if(board[i][j] == 'O') {
          matrixLights[l] += 192;
          matrixLights[l+1] += 192;
        } 
        else if(board[i][j] == 'X') {
          matrixLights[l] += 128;
          matrixLights[l+1] += 64;
        }  
      }
      else if(j == 1) {
        if(board[i][j] == 'O') {
          matrixLights[l] += 24;
          matrixLights[l+1] += 24;
        } 
        else if(board[i][j] == 'X') {
          matrixLights[l] += 16;
          matrixLights[l+1] += 8;
        }  
      }
      else if(j == 2) {
        if(board[i][j] == 'O') {
          matrixLights[l] += 3;
          matrixLights[l+1] += 3;
        } 
        else if(board[i][j] == 'X') {
          matrixLights[l] += 2;
          matrixLights[l+1] += 1;
        }  
      }
    }
  }
}

void matrixRowsVal(int value) {
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, LSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output
  digitalWrite(latchPin, HIGH);
}
void matrixColsVal(int value) {
  // make latchPin output low level
  digitalWrite(latchPin, LOW);
  // Send serial data to 74HC595
  shiftOut(dataPin, clockPin, MSBFIRST, value);
  // make latchPin output high level, then 74HC595 will update the data to parallel output 
  digitalWrite(latchPin, HIGH);
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  if(mqttClient.messageTopic() == gameStateTopic) {
    if(mqttClient.read() == 49) {
      player = 1;
    }
    else {
      player = 2;
    }

    int i = 0;
    int j = 0;
    while(mqttClient.available()) {
      if(j == 3) {
        j = 0;
        i += 1;
      }
      if(i == 3) {
        Serial.println("UH OH SOMETHING PROBABLY BROKE???");
      }
      board[i][j] = ((char)mqttClient.read());
      j += 1; 
    }
    updateMatrixLights();
  }
}