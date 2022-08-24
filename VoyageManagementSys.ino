#include <MFRC522.h>
#include <SPI.h>

#include "Ubidots.h"
#define SS_PIN D4
#define RST_PIN D3
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 
double total = 100000;
int code[3][4] = {{07,232,208,44},{66,119,137,171},{00,117,111,50}}; //This is the stored UID
int charge[3]={25,30,18};
String Carte[3]={"0057673","0085321","0056737"};
int codeRead = 0;
String uidString;
const char* UBIDOTS_TOKEN = "BBFF-BqeAviEuNxHgPvoXCUCNq5krzqg8Yy";  // Put here your Ubidots TOKEN
const char* WIFI_SSID = "SSP";      // Put here your Wi-Fi SSID
const char* WIFI_PASS = "amine003";      // Put here your Wi-Fi password
Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);

void setup() {
  lcd.init();
  lcd.backlight();
  LCDi();
  pinMode(D0,OUTPUT);
  pinMode(10,OUTPUT);
  digitalWrite(10,LOW);
  pinMode(D8,OUTPUT);
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);

}

void loop() {
  if(  rfid.PICC_IsNewCardPresent())
  {
      
      readRFID();
  }
  delay(100);


}

void readRFID()
{

  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }
   
    Serial.println("Scanned PICC's UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);

    uidString = String(rfid.uid.uidByte[0])+" "+String(rfid.uid.uidByte[1])+" "+String(rfid.uid.uidByte[2])+ " "+String(rfid.uid.uidByte[3]);

    int h=0;
    int i = 0;
    boolean match = true;
    for(int j=0;j<3;j++)
    {
      while(i<rfid.uid.size)
      {
        if(!(rfid.uid.uidByte[i] == code[j][i]))
        {
             match = false;
             break;
        }
        i++;
      }
      if(match==true)
      {
        h=j;
        break;
      }
      if(j==2 && match==false)
        break;
      match=true;
      i=0;
    }
    
    if(match)
    {  
      buzzerVrai();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Voyageur "+String(h+1));
      lcd.setCursor(7,0);
      lcd.print("charge:"+String(charge[h])+"dt");
      Serial.println("\nI know this card!");Serial.println("\nlivre"+String(h+1));Serial.println("charge: "+String(charge[h]));
      lcd.setCursor(1,0);
      lcd.print("Carte:"+String(Carte[h]));
      Serial.println("Carte: "+String(Carte[h]));
      delay(3000);      
    }else
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Ce voyageur est");
      lcd.setCursor(4,1);
      lcd.print("inconnu");
      
      Serial.println("\nUnknown Card");
      buzzerFaux();
      delay(3000);
      lcd.clear();
      LCDi();
    }
    

    // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  
  
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
void buzzerVrai()
{
  digitalWrite(D0,HIGH);
  tone(D8,2000);
  delay(200);
  digitalWrite(D0,LOW);
  noTone(D8);
}
void buzzerFaux()
{
  tone(D8,2000);
  digitalWrite(D0,HIGH);
  delay(100);
  digitalWrite(D0,LOW);
  noTone(D8);
  delay(100);
  digitalWrite(D0,HIGH);
  tone(D8,2000);
  delay(100);
  digitalWrite(D0,LOW);
  noTone(D8);
}
void LCDi()
{
  lcd.setCursor(2,0);
  lcd.print("Mettez votre");
  lcd.setCursor(5,1);
  lcd.print("carte");
}
