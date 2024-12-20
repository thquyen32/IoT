#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <MFRC522.h>
#include <SPI.h>
#include<WebSocketsClient.h>
#include<ArduinoJson.h>

#define SS_PIN 5
 

const char* ssid = "TuGiang";          
const char* password = "0986712793"; 
const char* serverUrl_pass = "http://192.168.1.10:8483/pass";  
const char* serverUrl_changepass = "http://192.168.1.10:8483/changepass";
const char *server_card = "http://192.168.1.10:8483/card"; 
const char *server_addcard = "http://192.168.1.10:8483/add"; 

MFRC522 rfid(SS_PIN, -1); 
MFRC522::MIFARE_Key key;

LiquidCrystal_I2C lcd(0x27, 16, 2); 
WebSocketsClient wSocket;

const byte ROW_NUM    = 4; 
const byte COLUMN_NUM = 3;

byte rowPins[ROW_NUM] = {4,17,16,33}; 
byte colPins[COLUMN_NUM] = {25, 26, 27};  

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROW_NUM, COLUMN_NUM);

String enteredPassword = "";
// bool passwordCorrect = false;  
int i=5; 
bool doiPass = false;
String cardID = "";
bool addstate = false;

void setup() {
  pinMode(14,OUTPUT);
  pinMode(32,OUTPUT);
  Serial.begin(115200);
  SPI.begin(); 
  rfid.PCD_Init();
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  wSocket.begin("192.168.1.10",8483,"/");
  wSocket.onEvent(webSocketEvent);
  lcd.setCursor(0, 1); 
  lcd.print("DOOR LOCK"); 
  lcd.setCursor(0, 0); 
  lcd.print("PASS:"); 
}

void loop() {
  wSocket.loop();
  cardID = "";
  if(addstate == false)
  {
if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      Serial.println("Card detected!");
      for (byte i = 0; i < rfid.uid.size; i++) {
        cardID += String(rfid.uid.uidByte[i]);
      }
      Serial.print("Card UID: ");
      Serial.println(cardID);

      if (sendCardToServer(cardID)) {
        buzzer_access();
        clearLine(0);
        clearLine(1);
        lcd.setCursor(3, 0); 
        lcd.print("DOOR IS OPEN"); 
        relay();
        clearLine(0);
        clearLine(1);
        lcd.setCursor(0, 1); 
        lcd.print("DOOR LOCK"); 
        lcd.setCursor(0, 0); 
        lcd.print("PASS:");
        i=5;
      } else {
        buzzer_denied();
        clearLine(0);
        clearLine(1);
        lcd.setCursor(2, 0); 
        lcd.print("Wrong Password");
        delay(2000);
        clearLine(0);
        clearLine(1);
        lcd.setCursor(0, 1); 
        lcd.print("DOOR LOCK"); 
        lcd.setCursor(0, 0); 
        lcd.print("PASS:"); 
        i=5;
      }
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1(); 
    }
  }
  }
numPad();
} 

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
switch(type)
{
  case WStype_DISCONNECTED:
  Serial.println("Websocket disconnect");
  break;
  case WStype_CONNECTED:
  Serial.println("Websocket conneted successfully");
  ID_WSOCKET();
  break;
  case WStype_TEXT:
  Serial.printf("Response WS: %s ", payload);
  if(String((char*)payload) == "DOOR IS OPEN")
  {
    clearLine(0);
    clearLine(1);
    lcd.setCursor(3,0);
    lcd.print("DOOR OPEN");
    relay();
    clearLine(0);
    clearLine(1);
    lcd.setCursor(0, 1); 
    lcd.print("DOOR LOCK"); 
    lcd.setCursor(0, 0); 
    lcd.print("PASS:"); 
  }
  break;
  default:
  break;
}
}
void addCardToDatabase() {
  while (addstate != false)
  {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      Serial.println("Dang o day nay ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        cardID += String(rfid.uid.uidByte[i]);
      }

 if (WiFi.status() == WL_CONNECTED) {  
    HTTPClient http;
    http.begin(server_addcard);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
    String payload = "ID=" +cardID;
    Serial.println("Sending payload: " + payload); 
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString(); 
      Serial.println("Response from server: " + response); 
      if (response == "Success") {
        clearLine(0);
          clearLine(1);
          lcd.setCursor(3,0);
          lcd.print("Success");
          delay(2000);
          clearLine(0);
          clearLine(1);
          lcd.setCursor(0, 1); 
          lcd.print("DOOR LOCK"); 
          lcd.setCursor(0, 0); 
          lcd.print("PASS:");
          i=5;
      }
    } else {
       lcd.print("Failed");
        delay(2000);
        clearLine(0);
        clearLine(1);
        lcd.setCursor(0, 1); 
        lcd.print("DOOR LOCK"); 
        lcd.setCursor(0, 0); 
        lcd.print("PASS:");
        i=5;
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot connect to server.");
  }   
      addstate = false;
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1(); 
  }
  }
  }
}
bool sendCardToServer(String cardID) {
  if (WiFi.status() == WL_CONNECTED) {  
    HTTPClient http;
    http.begin(server_card);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  

    String payload = "ID=" +cardID;
    Serial.println("Sending payload: " + payload); 

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response from server: " + response); 
      if (response == "Accept") {
        return true;
      }
    } else {
   return false;
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot connect to server.");
  }
  return false;
}
void numPad()
{

  char key = keypad.getKey();  
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    lcd.setCursor(i, 0); 
    if(key == '#') lcd.print("#"); 
    else lcd.print("*");
    delay(500);
    i+=2;

    if (enteredPassword.length() < 4) {
      enteredPassword += key;
    }
    
    if (enteredPassword.length() == 4) {
      if(enteredPassword == "****")
      {
        clearLine(0);
        clearLine(1);
        lcd.setCursor(5,0);
        lcd.print("Add Card");
        addstate = true;
        addCardToDatabase();
        enteredPassword = "";
      }
      else if(enteredPassword == "####"){
        Serial.print("Change Password");
        doiPass = true;
        enteredPassword = "";
        clearLine(0);
        lcd.setCursor(0, 0);
        lcd.print("NEW PASS:");
        i=9;
        return;
      }
      else if(doiPass == false){
        Serial.print("Password entered: ");
        Serial.println(enteredPassword);

        if(WiFi.status() == WL_CONNECTED) {  
          HTTPClient http;
          http.begin(serverUrl_pass);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");  
          String Payload = "PASS=" + enteredPassword;
          int httpCode = http.POST(Payload);

          if (httpCode > 0) {
            String response = http.getString();
            Serial.println("Server Response: ");
            Serial.println(response);
            if (response == "Door is open") {
              buzzer_access();
              relay();
              clearLine(0);
              lcd.setCursor(0, 0); 
              lcd.print("PASS CORRECT"); 
              clearLine(1);
              lcd.setCursor(0, 1); 
              lcd.print("DOOR OPEN");
              delay(5000);
              i=5;
              enteredPassword = "";
             clearLine(0);
              clearLine(1);
              lcd.setCursor(0, 1); 
              lcd.print("DOOR LOCK"); 
              lcd.setCursor(0, 0); 
              lcd.print("PASS:"); 
            } else {
              buzzer_denied();
              lcd.setCursor(0, 0); 
              lcd.print("PASS INCORRECT"); 
              delay(1000);
              clearLine(0);
              lcd.setCursor(0, 0);
              lcd.print("PASS:"); 
              i=5;
              enteredPassword = ""; 
            }
          } else {
              Serial.println("Error on HTTP request");
            } 
        http.end();  
        }
      } else {
          Serial.print("NewPassword entered: ");
          Serial.println(enteredPassword);

        if(WiFi.status() == WL_CONNECTED) { 
          HTTPClient http;
          http.begin(serverUrl_changepass);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");  

          String Payload = "PASS=" + enteredPassword;
          int httpCode = http.POST(Payload);

          if (httpCode > 0) {
            String response = http.getString(); 
            Serial.println("Server Response: ");
            Serial.println(response);  
            if (response == "Update Success") {
              clearLine(0);
              lcd.setCursor(0, 0); 
              lcd.print("PASS UPDATED");
              delay(2000);
              clearLine(0);
              lcd.setCursor(0, 0);
              lcd.print("PASS:");
              i=5;
              doiPass = false;
              enteredPassword = "";
            }
          } else {
              Serial.println("Error on HTTP request");
            } 
        http.end(); 
        }
        
      }
    }
  }
}

void clearLine(int line) {
  lcd.setCursor(0, line);    
  for (int i = 0; i < 16; i++) {
    lcd.print(" "); 
  }
}

void relay()
{
  digitalWrite(32,HIGH);
  delay(3000);
  digitalWrite(32,LOW);
  delay(3000);
}
void buzzer_access()
{
  digitalWrite(14,HIGH);
  delay(500);
  digitalWrite(14,LOW);
}
void buzzer_denied()
{
  digitalWrite(14,HIGH);
  delay(500);
  digitalWrite(14,LOW);
  delay(500);
  digitalWrite(14,HIGH);
  delay(500);
  digitalWrite(14,LOW);
  delay(500);
}
void ID_WSOCKET()
{
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["type"] = "Register";
  jsonDoc["ID"] = "1";
  String jsonString;
  serializeJson(jsonDoc,jsonString);
  wSocket.sendTXT(jsonString);
}












