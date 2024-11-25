#include <ESP8266WiFi.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>

const char *ssid = "Minh9303-1258";  // Tên mạng Wi-Fi của bạn
const char *pass = "09032003";       // Mật khẩu Wi-Fi của bạn

#define SS_PIN 4   // Chân SDA (D2 trên ESP8266)
#define RST_PIN 5  // Chân RST (D1 trên ESP8266)

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Khởi tạo đối tượng MFRC522

// Địa chỉ IP của server và route API kiểm tra ID thẻ
const char *server = "http://192.168.0.104:8483/card"; 
const char *addCardUrl = "http://192.168.0.104:8483/add"; 

HTTPClient http;
WiFiClient client;  // Khởi tạo client cho HTTP
String url = String(server); 
String enteredPin = "";
const String correctPin = "####";

void setup() {
  Serial.begin(115200);   // Mở Serial Monitor
  WiFi.begin(ssid, pass);  // Kết nối WiFi

  // Chờ đến khi kết nối Wi-Fi thành công
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("YES SIRRRRRRR :))");
  Serial.println(WiFi.localIP());
  SPI.begin();  // Khởi tạo giao thức SPI
  mfrc522.PCD_Init();  // Khởi tạo module RFID
  
  Serial.println("Scan a card to read UID...");
}

void loop() {
  // Kiểm tra nếu có thẻ mới được quét
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.println("Card detected!");

      String cardID = "";
      
      // Lấy ID thẻ và chuyển đổi nó thành chuỗi
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        cardID += String(mfrc522.uid.uidByte[i], HEX);
      }
      cardID.toUpperCase();  // Chuyển ID thành chữ hoa

      Serial.print("Card UID: ");
      Serial.println(cardID);  // In ID của thẻ

      // Gửi ID thẻ đến server qua HTTP POST
      if ( sendCardToServer(cardID)) {
        Serial.println("Card is already in database.");
      } else {
        Serial.println("Card not found in database. Waiting for PIN to add...");
        waitForUserInputAndAddCard(cardID);  // Chờ PIN và thêm thẻ
      }

      mfrc522.PICC_HaltA();  // Dừng quét thẻ
      mfrc522.PCD_StopCrypto1();  // Dừng giao thức đọc
    }
  }
}

// Hàm gửi yêu cầu POST tới server với ID thẻ

bool sendCardToServer(String cardID) {

  if (WiFi.status() == WL_CONNECTED) {  // Kiểm tra kết nối WiFi
    http.begin(client, url);  // Thiết lập kết nối HTTP
 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Thêm header Content-Type, urlencode kpjson

    // Tạo payload JSON chứa cardID
    String payload = "ID=" +cardID;
    Serial.println("Sending payload: " + payload);  // In ra payload gửi đến server

    // Gửi yêu cầu POST
    int httpResponseCode = http.POST(payload);

    // Kiểm tra mã phản hồi HTTP
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response from server: " + response);  // In phản hồi từ server

      // Nếu phản hồi là "Accept", mở khóa (giả sử mở khóa thành công)
      if (response == "Accept") {
        Serial.println("Door is open!");
        return true;
        // Thực hiện mở khóa ở đây, ví dụ:
        // Bật tín hiệu để mở khóa, hoặc điều khiển mạch điện khác.
      }
      // đây là m đang quét thẻ mới hay cũ v sao nó vẫn là đuôi 21 kia
      
    } else {
      // Nếu không có phản hồi hoặc có lỗi trong HTTP request
      Serial.print("Error in HTTP request, response code: ");
      Serial.println(httpResponseCode);  // In mã lỗi HTTP
      Serial.println("Unable to connect to server.");
    }

    http.end();  // Đóng kết nối HTTP
  } else {
    // Nếu WiFi không kết nối
    Serial.println("WiFi not connected. Cannot connect to server.");
  }
  return false;
}

void waitForUserInputAndAddCard(String cardID) {
  Serial.println("Enter '1' to add the card:");

  // Đợi người dùng nhập "1"
  while (true) {
    if (Serial.available() > 0) {
      char userInput = Serial.read();  // Đọc dữ liệu từ Serial
      if (userInput == '1') {  // Nếu người dùng nhập "1"
        Serial.println("Input received: '1'. Adding card to database...");
        addCardToDatabase(cardID);  // Thêm thẻ vào cơ sở dữ liệu
        break;  // Thoát khỏi vòng lặp
      }
    }
  }
}

// Hàm gửi yêu cầu POST tới server để thêm thẻ
void addCardToDatabase(String cardID) {
  if (WiFi.status() == WL_CONNECTED) {  // Kiểm tra kết nối WiFi
    http.begin(client, addCardUrl);  // URL để thêm thẻ vào cơ sở dữ liệu
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Tạo payload chứa cardID để thêm thẻ vào cơ sở dữ liệu
    String payload = "ID=" + cardID;
    Serial.println("Sending payload to add card: " + payload);

    // Gửi yêu cầu POST
    int httpResponseCode = http.POST(payload);

    // Kiểm tra mã phản hồi HTTP
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response from server: " + response);

      // Nếu phản hồi là "Added", thẻ đã được thêm thành công
      if (response == "Added") {
        Serial.println("Card added successfully!");
      }
    } else {
      Serial.print("Error in HTTP request, response code: ");
      Serial.println(httpResponseCode);  // In mã lỗi HTTP
    }
    http.end();  // Đóng kết nối HTTP
  } else {
    Serial.println("WiFi not connected. Cannot connect to server.");
  }
}

