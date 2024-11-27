#include <WiFi.h>;
#include <HTTPClient.h>;
#include <Wire.h>;
#include <LiquidCrystal_I2C.h>;
#include <Keypad.h>;

const char* ssid = "Bong Nhim.5G";          // Thay thế với tên WiFi của bạn
const char* password = "Minh@151208";  // Thay thế với mật khẩu WiFi của bạn

const char* serverUrl = "http://192.168.1.17:8483/pass";  // Địa chỉ server của bạn
const char* serverUrl1 = "http://192.168.1.17:8483/changepass";  // Địa chỉ server của bạn

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 16x2

// Xác định số lượng hàng và cột của bàn phím ma trận 4x3
const byte ROW_NUM    = 4; // Số hàng
const byte COLUMN_NUM = 3; // Số cột

// Chân kết nối của các hàng và cột
byte rowPins[ROW_NUM] = {12, 13, 14, 15}; // Chân hàng
byte colPins[COLUMN_NUM] = {16, 17, 18};  // Chân cột

// Định nghĩa các phím trên bàn phím ma trận
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Tạo đối tượng bàn phím
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROW_NUM, COLUMN_NUM);

String enteredPassword = "";
bool passwordCorrect = false;  // Cờ để kiểm tra mật khẩu đúng hay sai
int i=5;
bool doiPass = false;

void setup() {
  // Bắt đầu giao tiếp Serial để debug
  Serial.begin(115200);

  lcd.init();
  lcd.backlight(); // Bật đèn nền

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Kết nối với server
  //HTTPClient http;
  //http.begin(serverUrl);  // Bắt đầu kết nối với URL của server

  // Gửi yêu cầu HTTP GET đến server
  //int httpCode = http.GET();

  // Kiểm tra mã trạng thái của yêu cầu
  //if (httpCode > 0) {
  //  Serial.printf("HTTP GET request sent. Response code: %d\n", httpCode);
  //  String payload = http.getString();  // Lấy dữ liệu trả về từ server
  //  Serial.println("Server Response:");
  //  Serial.println(payload);
  //} else {
  //  Serial.printf("HTTP GET request failed. Error code: %d\n", httpCode);
  //}

  // Đóng kết nối
  //http.end();

  lcd.setCursor(0, 1); // Di chuyển con trỏ đến cột 0, hàng 0
  lcd.print("DOOR LOCK"); // Hiển thị chuỗi
  lcd.setCursor(0, 0); // Di chuyển con trỏ đến cột 0, hàng 0
  lcd.print("PASS:"); // Hiển thị chuỗi
}

void loop() {
  // Nếu mật khẩu đã đúng, không cần nhận thêm mật khẩu nữa
  if (passwordCorrect) {
    Serial.println("Password correct! No further input will be accepted.");
    delay(10000);
    return;  // Dừng vòng lặp (hoặc có thể làm gì đó sau khi xác nhận mật khẩu)
  }
  char key = keypad.getKey();  // Đọc giá trị phím nhấn
  if (key) {
    Serial.print("Key Pressed: ");
    Serial.println(key);
    lcd.setCursor(i, 0); // Di chuyển con trỏ đến cột i, hàng 0
    if(key == '#') lcd.print("#"); // Hiển thị chuỗi
    else lcd.print("*");
    delay(500);
    i+=2;

    // Nếu người dùng nhập mật khẩu, thêm vào chuỗi enteredPassword
    if (enteredPassword.length() < 4) {
      enteredPassword += key;  // Thêm số vào mật khẩu
    }
    
    // Nếu đã nhập đủ 4 chữ số, kiểm tra mật khẩu
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

        // Gửi mật khẩu đến server qua HTTP POST
        if(WiFi.status() == WL_CONNECTED) {  // Kiểm tra kết nối Wi-Fi
          HTTPClient http;
          http.begin(serverUrl);  // Kết nối đến server
          http.addHeader("Content-Type", "application/json");  // Định dạng JSON

          // Tạo JSON với mật khẩu
          String jsonPayload = "{\"PASS\":\"" + enteredPassword + "\"}";

        // Gửi HTTP POST yêu cầu
          int httpCode = http.POST(jsonPayload);

          // Kiểm tra mã trạng thái HTTP
          if (httpCode > 0) {
            String payload = http.getString();  // Lấy dữ liệu trả về từ server

            Serial.println("Server Response: ");
            Serial.println(payload);  // In ra phản hồi từ server

            // So sánh mật khẩu nhập vào với mật khẩu chính xác
            if (payload == "Door is open") {
              clearLine(0);
              lcd.setCursor(0, 0); // Di chuyển con trỏ đến cột 0, hàng 0
              lcd.print("PASS CORRECT"); // Hiển thị chuỗi
              clearLine(1);
              lcd.setCursor(0, 1); // Di chuyển con trỏ đến cột 0, hàng 0
              lcd.print("DOOR OPEN"); // Hiển thị chuỗi
              delay(1000);
              passwordCorrect = true;  // Đặt cờ để ngừng nhận mật khẩu
              // Có thể mở cửa hoặc thực hiện hành động nào đó
            } else {
              Serial.println("Incorrect password.");
              // Sau khi kiểm tra, reset chuỗi mật khẩu để nhập lại
              lcd.setCursor(0, 0); // Di chuyển con trỏ đến cột 0, hàng 0
              lcd.print("PASS INCORRECT"); // Hiển thị chuỗi
              delay(1000);
              clearLine(0);
              lcd.setCursor(0, 0); // Di chuyển con trỏ đến cột 0, hàng 0
              lcd.print("PASS:"); // Hiển thị chuỗi
              i=5;
              enteredPassword = "";  // Reset mật khẩu sau khi kiểm tra
            }
          } else {
              Serial.println("Error on HTTP request");
            } 
        http.end();  // Đóng kết nối
        }
      delay(10000);  // Chờ 10 giây trước khi thử lại (hoặc bạn có thể điều chỉnh để nhận input từ người dùng)
      } else {
          Serial.print("NewPassword entered: ");
          Serial.println(enteredPassword);

        // Gửi mật khẩu đến server qua HTTP POST
        if(WiFi.status() == WL_CONNECTED) {  // Kiểm tra kết nối Wi-Fi
          HTTPClient http;
          http.begin(serverUrl1);  // Kết nối đến server
          http.addHeader("Content-Type", "application/json");  // Định dạng JSON

          // Tạo JSON với mật khẩu
          String jsonPayload = "{\"PASS\":\"" + enteredPassword + "\"}";

        // Gửi HTTP POST yêu cầu
          int httpCode = http.POST(jsonPayload);

          // Kiểm tra mã trạng thái HTTP
          if (httpCode > 0) {
            String payload = http.getString();  // Lấy dữ liệu trả về từ server

            Serial.println("Server Response: ");
            Serial.println(payload);  // In ra phản hồi từ server

            // So sánh mật khẩu nhập vào với mật khẩu chính xác
            if (payload == "Update Success") {
              clearLine(0);
              lcd.setCursor(0, 0); // Di chuyển con trỏ đến cột 0, hàng 0
              lcd.print("PASS UPDATED"); // Hiển thị chuỗi
              delay(1000);
            }
          } else {
              Serial.println("Error on HTTP request");
            } 
        http.end();  // Đóng kết nối
        }
        delay(10000);  // Chờ 10 giây trước khi thử lại (hoặc bạn có thể điều chỉnh để nhận input từ người dùng)
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

// Hàm xóa một dòng cụ thể trên LCD
void clearLine(int line) {
  lcd.setCursor(0, line);    // Đặt con trỏ về vị trí đầu dòng (cột 0)
  for (int i = 0; i < 16; i++) { // Lặp qua tất cả các cột của dòng
    lcd.print(" ");  // In dấu cách để xóa nội dung trên LCD
  }
}












