#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <MFRC522.h>

#define SS_PIN 4   
#define RST_PIN 5 

const char* ssid = "Bong Nhim.5G";          
const char* password = "Minh@151208"; 
const char* serverUrl_pass = "http://192.168.1.17:8483/pass";  
const char* serverUrl_changepass = "http://192.168.1.17:8483/changepass";
const char *server_card = "http://192.168.0.104:8483/card"; 
const char *server_addcard = "http://192.168.0.104:8483/add"; 

MFRC522 mfrc522(SS_PIN, RST_PIN); 
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const byte ROW_NUM    = 4; 
const byte COLUMN_NUM = 3;

byte rowPins[ROW_NUM] = {12, 13, 14, 15}; 
byte colPins[COLUMN_NUM] = {16, 17, 18};  

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROW_NUM, COLUMN_NUM);

String enteredPassword = "";
bool passwordCorrect = false;  
int i=5; 
bool doiPass = false;
String cardID = "";

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 1); 
  lcd.print("DOOR LOCK"); 
  lcd.setCursor(0, 0); 
  lcd.print("PASS:"); 
}

void loop() {
if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Card detected!");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardID += String(mfrc522.uid.uidByte[i]);
      }
      Serial.print("Card UID: ");
      Serial.println(cardID);

      if ( sendCardToServer(cardID)) {
        // Thêm hành động
        Serial.println("Card is already in database.");
      } else {
        Serial.println("Card not found in database. Waiting for PIN to add...");
        // thêm LCD
      }
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1(); 
    }
  }
numPad();
}
bool addCardToDatabase(String cardID) {
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
      if (response == "Accept") {
        // thêm hành động
        Serial.println("Door is open!");
        return true;
      }
    } else {
      Serial.print("Error in HTTP request, response code: ");
      Serial.println(httpResponseCode); 
      Serial.println("Unable to connect to server.");
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot connect to server.");
  }
  return false;
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
        // thêm hành động
        Serial.println("Door is open!");
        return true;
      }
    } else {
      Serial.print("Error in HTTP request, response code: ");
      Serial.println(httpResponseCode); 
      Serial.println("Unable to connect to server.");
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot connect to server.");
  }
  return false;
}
void numPad()
{
    if (passwordCorrect) {
    Serial.println("Password correct! No further input will be accepted.");
    delay(10000);
    return;  
  } 
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
      if(enteredPassword == "####"){
        Serial.print("Change Password");
        doiPass = true;
        enteredPassword = "";
        clearLine(0);
        lcd.setCursor(0, 0);
        lcd.print("NEW PASS:");
        i=9;
        return;
      }
      if(doiPass == false){
        Serial.print("Password entered: ");
        Serial.println(enteredPassword);

        if(WiFi.status() == WL_CONNECTED) {  
          HTTPClient http;
          http.begin(serverUrl_pass);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");  

          String Payload = "PASS+=" + enteredPassword;

          int httpCode = http.POST(Payload);

          if (httpCode > 0) {
            String response = http.getString();
            Serial.println("Server Response: ");
            Serial.println(response);
            if (response == "Door is open") {
              clearLine(0);
              lcd.setCursor(0, 0); 
              lcd.print("PASS CORRECT"); 
              clearLine(1);
              lcd.setCursor(0, 1); 
              lcd.print("DOOR OPEN"); 
              delay(1000);
              passwordCorrect = true; 
            } else {
              Serial.println("Incorrect password.");
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
      delay(10000);  
      } else {
          Serial.print("NewPassword entered: ");
          Serial.println(enteredPassword);

        if(WiFi.status() == WL_CONNECTED) { 
          HTTPClient http;
          http.begin(serverUrl_changepass);
          http.addHeader("Content-Type", "application/x-www-form-urlencoded");  

          String Payload = "PASS+=" enterPassword;
          int httpCode = http.POST(jsonPayload);

          if (httpCode > 0) {
            String response = http.getString(); 
            Serial.println("Server Response: ");
            Serial.println(response);  
            if (response == "Update Success") {
              clearLine(0);
              lcd.setCursor(0, 0); 
              lcd.print("PASS UPDATED");
              delay(1000);
            }
          } else {
              Serial.println("Error on HTTP request");
            } 
        http.end(); 
        }
        delay(10000);  
        clearLine(0);
        lcd.setCursor(0, 0);
        lcd.print("PASS:");
        i=5;
        doiPass = false;
        enteredPassword = "";
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












