// This sketch is based on a sketch from this site http://samopal.pro
// Modified by Northstrix https://github.com/Northstrix
#include <arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <ArduinoJson.h>
#include "TM1637.h"
#define TM1637_CLK  4 
#define TM1637_DIO  5
TM1637 tm1637(TM1637_CLK,TM1637_DIO);
const unsigned char logo2 [] PROGMEM = {
0xff, 0x7f, 0xff, 0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf0, 0x07, 0xc0, 0x01, 0xaa, 0xaa, 0x80, 0x00,
0xe0, 0x03, 0xc0, 0x01, 0xee, 0x3b, 0xee, 0x3b, 0xf6, 0x37, 0xf6, 0x37, 0xfa, 0x2f, 0xfa, 0x2f };
const unsigned char wifi_icon [] PROGMEM = {0x07, 0xfb, 0xfd, 0x1e, 0xee, 0xf6, 0x36, 0xb6 };
const unsigned char digit0 [] PROGMEM = {  0x83,0x01,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x01,0x83 };
const unsigned char digit1 [] PROGMEM = {  0xF7,0xE7,0xC7,0x87,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0xE7,0x81 };
const unsigned char digit2 [] PROGMEM = {  0x83,0x01,0x39,0x39,0xF9,0xF9,0xF3,0xE3,0xC7,0x8F,0x1F,0x3F,0x01,0x01 };
const unsigned char digit3 [] PROGMEM = {  0x83,0x01,0x39,0xF9,0xF9,0xF9,0xE3,0xE3,0xF9,0xF9,0xF9,0x39,0x01,0x83 };
const unsigned char digit4 [] PROGMEM = {  0xF3,0xE3,0xC3,0xC3,0x93,0x93,0x33,0x31,0x01,0x01,0xF3,0xF3,0xF3,0xF3 };
const unsigned char digit5 [] PROGMEM = {  0x01,0x01,0x3F,0x3F,0x3F,0x03,0x01,0xF9,0xF9,0xF9,0xF9,0x39,0x01,0x83 };
const unsigned char digit6 [] PROGMEM = {  0x83,0x01,0x39,0x3F,0x3F,0x03,0x01,0x39,0x39,0x39,0x39,0x39,0x01,0x83};
const unsigned char digit7 [] PROGMEM = {  0x01,0x01,0xF9,0xF9,0xF9,0xF1,0xF3,0xE3,0xE7,0xCF,0xCF,0x9F,0x9F,0x9F };
const unsigned char digit8 [] PROGMEM = {  0x83,0x01,0x39,0x39,0x39,0x83,0x83,0x39,0x39,0x39,0x39,0x39,0x01,0x83 };
const unsigned char digit9 [] PROGMEM = {  0x83,0x01,0x39,0x39,0x39,0x39,0x39,0x01,0x81,0xF9,0xF9,0x39,0x01,0x83 };
const char W_SSID[] = "Your SSID(for example ASUS RT-N10)";
const char W_PASS[] = "Your password(for example 123456)";
String     W_URL    = "http://api.openweathermap.org/data/2.5/weather";
String     W_API    = "Your weather key, you can get it in https://openweathermap.org/";
String     W_ID     = "2643743"; //Weather id for the London, you can change it for your city
String     W_NAME   = "Weather in London: "; //You can change this string
WiFiUDP udp;
const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
const char NTP_SERVER[]   = "0.ru.pool.ntp.org";          
int TZ                    = 0;//timesone for the London according UTC
uint32_t NTP_TIMEOUT      = 600000;
int pinCS = 16;
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays   = 2;
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);
String tape = "";
int wait = 20; // In milliseconds
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels
int8_t DispMSG[] = {1, 2, 3, 4};
uint32_t ms, ms0=0, ms1=0, ms2=0, ms3=0, ms_mode=0;
uint32_t tm         = 0;
uint32_t t_cur      = 0;    
long  t_correct     = 0;
bool  pp = false;
int   mode = 0;

void setup() {
Serial.begin(115200);
tm1637.init();
tm1637.set(5);
matrix.setIntensity(7); // Use a value between 0 and 15 for brightness
matrix.setPosition(0, 3, 1); // The first display is at <0, 0>
matrix.setPosition(1, 2, 1); // The first display is at <0, 0>
matrix.setPosition(2, 1, 1); // The first display is at <0, 0>
matrix.setPosition(3, 0, 1); // The first display is at <0, 0>
matrix.setPosition(7, 3, 0); // The first display is at <0, 0>
matrix.setPosition(6, 2, 0); // The first display is at <0, 0>
matrix.setPosition(5, 1, 0); // The first display is at <0, 0>
matrix.setPosition(4, 0, 0); // The first display is at <0, 0>
matrix.setRotation(0, 3);    // The first display is position upside down
matrix.setRotation(1, 3);    // The first display is position upside down
matrix.setRotation(2, 3);    // The first display is position upside down
matrix.setRotation(3, 3);    // The first display is position upside down
matrix.setRotation(4, 1);    // The first display is position upside down
matrix.setRotation(5, 1);    // The first display is position upside down
matrix.setRotation(6, 1);    // The first display is position upside down
matrix.setRotation(7, 1);    // The first display is position upside down
matrix.fillScreen(LOW);
matrix.drawBitmap(0, 0,  logo2, 16, 16, 0, 1); 
matrix.write();
delay(5000); 
Serial.print("\nConnecting to ");
Serial.println(W_SSID);
WiFi.begin(W_SSID, W_PASS);
for (int i=0;WiFi.status() != WL_CONNECTED&&i<150; i++) {
Serial.print(".");
matrix.drawBitmap(20, 4,  wifi_icon, 8, 8, 0, 1); 
matrix.write();
delay(500);
matrix.fillRect(20, 4, 8, 8, LOW);  
matrix.write();
delay(500);
  }

matrix.drawBitmap(20, 4,  wifi_icon, 8, 8, 0, 1); 
matrix.write();
Serial.println("\nWiFi connected\nIP address: ");
Serial.println(WiFi.localIP());
Serial.println();
ms_mode = millis();
udp.begin(2390);
}

int i_ms0 = 0;
void loop() {
 ms = millis();

   if( ms0 == 0 || ms < ms0 || (ms - ms0)>wait ){
       ms0 = ms;
       switch(mode){
           case 0:
           case 1:
           case 4:              
           case 5:              
              break;
           default:
              PrintTicker();
              
       }
       
   }
   if( ms1 == 0 || ms < ms1 || (ms - ms1)>60000 ){
       ms1 = ms;
       GetWeather();
   }

  if( ms2 == 0 || ms < ms2 || (ms - ms2)>500 ){
       ms2 = ms;
       t_cur  = ms/1000;
       tm     = t_cur + t_correct;
       switch(mode){
           case 0:
           case 1:
           case 4:              
           case 5:              
              PrintBigTime();
              break;
           default:
              PrintTime();
       }
  
       
  }
   

   if(  ms3 == 0 || ms < ms3 || (ms - ms3)>NTP_TIMEOUT ){
       uint32_t t = GetNTP();
       if( t!=0 ){
          ms3 = ms;
          t_correct = t - t_cur;
       }
       Serial.println(t);
   }

   
}


void PrintTime(){
tm1637.point(true);
char s[20];
matrix.fillRect(0, 0, 32, 8, LOW);  
if( pp )sprintf(s, "%02d:%02d", (int)( tm/3600 )%24, (int)( tm/60 )%60);
else sprintf(s, "%02d %02d", (int)( tm/3600 )%24, (int)( tm/60 )%60);
pp = !pp;
matrix.setCursor(1, 0);
matrix.print(s);
ms_mode = ms;
int h = (int)( tm/3600 )%24;
int m = (int)( tm/60 )%60;
Serial.print("Time: ");
Serial.print(h);
Serial.print(":");
Serial.println(m);
tm1637.display(0,h/10);
tm1637.display(1,h%10);
tm1637.display(2,m/10);
tm1637.display(3,m%10);
tm1637.point(false);
   
      
}


void PrintBigTime(){
char s[20];
matrix.fillScreen(LOW);  
int h = (int)( tm/3600 )%24;
int m = (int)( tm/60 )%60;
PrintBigDigit(0,1,h/10);
PrintBigDigit(8,1,h%10);
PrintBigDigit(17,1,m/10);
PrintBigDigit(25,1,m%10);
Serial.print("Time: ");
Serial.print(h);
Serial.print(":");
Serial.println(m);
tm1637.display(0,h/10);
tm1637.display(1,h%10);
tm1637.display(2,m/10);
tm1637.display(3,m%10);
    if( pp ){
      matrix.fillRect(15, 4,  2, 2, HIGH);
      matrix.fillRect(15, 9, 2, 2, HIGH);
    }
    
    pp = !pp;
    matrix.write();
    if( ms -ms_mode > 5000){
         mode++;
         if( mode >= 8 )mode = 0;
         ms_mode = ms;
    }
    
}


 void PrintBigDigit(int x, int y, int dig){
    switch(dig){
      case 0  : matrix.drawBitmap(x, y,  digit0, 7, 14, 0, 1); break;
      case 1  : matrix.drawBitmap(x, y,  digit1, 7, 14, 0, 1); break;
      case 2  : matrix.drawBitmap(x, y,  digit2, 7, 14, 0, 1); break;
      case 3  : matrix.drawBitmap(x, y,  digit3, 7, 14, 0, 1); break;
      case 4  : matrix.drawBitmap(x, y,  digit4, 7, 14, 0, 1); break;
      case 5  : matrix.drawBitmap(x, y,  digit5, 7, 14, 0, 1); break;
      case 6  : matrix.drawBitmap(x, y,  digit6, 7, 14, 0, 1); break;
      case 7  : matrix.drawBitmap(x, y,  digit7, 7, 14, 0, 1); break;
      case 8  : matrix.drawBitmap(x, y,  digit8, 7, 14, 0, 1); break;
      case 9  : matrix.drawBitmap(x, y,  digit9, 7, 14, 0, 1); break;
      
    }
 }


void PrintTicker(){

     if( i_ms0 >= width * tape.length() + matrix.width() - 1 - spacer ){
         i_ms0 = 0;
         mode++;
         ms_mode = ms;
         if( mode >=8 )mode=0;
     }


         matrix.fillRect(0, 8, 32, 8, LOW);

         int letter = i_ms0 / width;
         int x = (matrix.width() - 1) - i_ms0 % width;
         int y = 8; // center the text vertically

         while ( x + width - spacer >= 0 && letter >= 0 ) {
            if ( letter < tape.length() ) {
               matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
           }

         letter--;
         x -= width;
      }

      matrix.write(); // Send bitmap to display

     i_ms0++;

  
}


String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
//          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
//          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}


void GetWeather(){
   HTTPClient client;
   String url = W_URL;
   url += "?id=";
   url += W_ID;
   url += "&APPID=";
   url += W_API;
   url += "&units=metric&lang=ru";
   Serial.println(url);
   client.begin(url);
   int httpCode = client.GET();
   if( httpCode == HTTP_CODE_OK ){
       String httpString = client.getString(); 
       ParseWeather(httpString);
   }
   client.end();

}


void ParseWeather(String s){
   DynamicJsonBuffer jsonBuffer;
   JsonObject& root = jsonBuffer.parseObject(s);

   if (!root.success()) {
      Serial.println("Json parsing failed!");
      return;
   }
 
   tape = "Weather in London:";// You can change this string
   tape += ". Temperature C ";
   int t = root["main"]["temp"].as<int>(); 
   tape += String(t);      
   tape += ". Temperature F ";
   int temp, Fahrenheit;
   Fahrenheit = t*9/5+32;
   tape += String(Fahrenheit);   
   tape += "С. Humidity ";
   tape += root["main"]["humidity"].as<String>(); 
   tape += "%. Pressure ";
   double p = root["main"]["pressure"].as<double>()/1.33322;
   tape += String((int)p);
   tape += "mm. wind ";
   double deg = root["wind"]["deg"];
   if( deg >22.5 && deg <=67.5 )tape += "north-eastern ";
   else if( deg >67.5 && deg <=112.5 )tape += "eastern ";
   else if( deg >112.5 && deg <=157.5 )tape += "south-eastern ";
   else if( deg >157.5 && deg <=202.5 )tape += "southern ";
   else if( deg >202.5 && deg <=247.5 )tape += "south-western ";
   else if( deg >247.5 && deg <=292.5 )tape += "western ";
   else if( deg >292.5 && deg <=337.5 )tape += "north-western ";
   else tape += "northern ";
   tape += root["wind"]["speed"].as<String>();
   tape += " m/s";
   tape += "    ";   
   tape = utf8rus(tape);
   s = ""; 
}

time_t GetNTP(void) {
  IPAddress ntpIP;
  time_t tm = 0;
  WiFi.hostByName(NTP_SERVER, ntpIP); 
  sendNTPpacket(ntpIP); 
  delay(1000);
 
  int cb = udp.parsePacket();
  if (!cb) {
    return tm;
  }
  else {
    udp.read(packetBuffer, NTP_PACKET_SIZE); 

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    tm = epoch + TZ*3600;    
  }
  return tm;
}


unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;
  packetBuffer[1] = 0;
  packetBuffer[2] = 6;
  packetBuffer[3] = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123); 
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();

    
}

