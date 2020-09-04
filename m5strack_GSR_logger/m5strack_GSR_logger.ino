/*
  弘前実験M5STRACK用GSR記録プログラム v1.0
  三菱实验更新——》追加更新NTP时间自动获取功能
*/
#include <M5Stack.h>
#include <Wire.h>
#include <TimeLib.h>
#include <SPI.h>
#define GSR_pin 36 //GSR in 36
int GSR_data = 0;
int start_time = 0;
int lcd_light = 100;
String pre_dataString = "";
String dataString = "";
String file_name = "/";
// the setup routine runs once when M5Stack starts up
void dis_IU() {//画丑爆了的UI
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.println("Wear GSR sensor");
  M5.Lcd.println("File name:" + file_name); //显示文件名
  //M5.Lcd.println("Please press left button to start log.");
  M5.Lcd.setCursor(30, 210 ); //lcd size :320*240
  M5.Lcd.println("START  -    +");//显示菜单
  /*
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.println(" Charging...But I don't know if it is full.");//假装我知道在充电
  */

}

//WIFI相关代码：
//Ambient和wifi的设置
//研究室wifi aterm-ac2876-g
//pass       3295a7101338b
//杉本さんMY ASUS   2d2bc72444a8
WiFiClient client;
const char* ssid = "MY ASUS";
const char* password = "2d2bc72444a8";

void setup() {

  // Initialize the M5Stack object
  M5.begin();
  //M5.Speaker.setVolume(1);
  dacWrite(25, 0); // Speaker OFF
  pinMode(GSR_pin, INPUT);
  // LCD display
  M5.Lcd.setBrightness(100);
  M5.Lcd.fillScreen(BLACK);

  //尝试解决噪音问题
  ledcDetachPin(SPEAKER_PIN); 
  pinMode(SPEAKER_PIN,INPUT);
  dacWrite(25,0);


  //启动完毕
  File file = SD.open("/FILE_ID.txt");
  int file_id = file.read();
  file.close();
  //M5.Lcd.println(file_id);
  if (file_id == -1) {
    file_id = 1;
  }
  int next_id = file_id + 1;
  file_name += file_id;
  file_name += ".csv";

  dis_IU();//画丑爆了的UI

  while ( !M5.BtnA.isPressed() ) {
    M5.BtnA.read();//等待循环（假装有充电UI
  }//wait button 开始记录！！！
  file = SD.open("/FILE_ID.txt", FILE_WRITE); //记录下一个文件的ID
  file.write(next_id);
  file.close();
  M5.Lcd.setCursor(30, 210 ); //光标回到原处吧START反色
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.print("START");
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(5);//
  start_time = millis();
}

// the loop routine runs over and over again forever
void loop() {
  //M5.Lcd.fillScreen(BLACK);
  //M5.Lcd.setTextColor(WHITE);
  GSR_data = analogRead(GSR_pin);
  //从这里开始
  pre_dataString = dataString;
  dataString = "";
  dataString += millis() - start_time;
  dataString += ",";
  File dataFile = SD.open(file_name, FILE_APPEND);
  if (dataFile) {
    dataString += GSR_data;

    if (dataString != pre_dataString && lcd_light>1) {
      M5.Lcd.setCursor(50, 100);
      M5.Lcd.print("GSR:");
      M5.Lcd.print(GSR_data);
      M5.Lcd.println("  ");

    }
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    // M5.Lcd.println(GSR_data);
  }
  else {
    M5.Lcd.println("error opening datalog.txt");
  }
  //调节屏幕亮度
  M5.BtnB.read();
  if ( M5.BtnB.wasPressed() ) {
    if (lcd_light == 100) {
      lcd_light = 1;
      M5.Lcd.setBrightness(lcd_light);
    } else if (lcd_light == 1) {
      lcd_light = 0;
      M5.Lcd.setBrightness(lcd_light);
      M5.Lcd.sleep();
      M5.Speaker.end();
      //      M5.Wire.end();

    }

  }
  M5.BtnC.read();
  if ( M5.BtnC.isPressed() ) {
    if (lcd_light == 0) {
      lcd_light = 1;
      M5.Lcd.begin();
      M5.Lcd.setBrightness(lcd_light);
      dis_IU();//画丑爆了的UI      
      M5.Lcd.setTextSize(5);//
    } else if (lcd_light == 1) {
      lcd_light = 100;
      M5.Lcd.setBrightness(lcd_light);
    }

  }
delay(200);
}
