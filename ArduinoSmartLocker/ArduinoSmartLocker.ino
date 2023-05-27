#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <U8g2lib.h>

// #define USE_4X3_KEYPAD

#ifdef USE_4x3_KEYPAD
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3
#else
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#endif

#define RELE_PIN 2

#define RFID_SDA 53 // arduino mega default
#define RFID_DIO 49 // arduino mega default

#ifdef USE_4x3_KEYPAD
char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'},
                               {'*', '0', '#'}};

// char rowPins[KEYPAD_ROWS] = {8, 3, 4, 6};
// char colPins[KEYPAD_COLS] = {7, 10, 5};
char rowPins[KEYPAD_ROWS] = {27, 22, 23, 25};
char colPins[KEYPAD_COLS] = {26, 28, 24};
#else
char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {{'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}};

byte rowPins[KEYPAD_ROWS] = {22, 23, 24, 25};
byte colPins[KEYPAD_COLS] = {26, 27, 28, 29};
#endif

LiquidCrystal_I2C lcd(0x27, 16, 2);  
MFRC522 cardReader(RFID_SDA, RFID_DIO); 
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
Keypad keypad = Keypad(makeKeymap(keypadKeys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

size_t cur_idx = 0;

boolean flagRele = false;

char keypadPsw[4] = {'1', '2', '3', '4'};
char keypadTry[4];

byte rfidPsw[4] = {199, 81, 171, 121};
byte rfidReadedCard[4];

unsigned long startedOpen = 0;
int waitTime = 5000;

void setup()
{
  Serial.begin(19200);
  Wire.begin();
  SPI.begin();
  u8g2.begin();
  
  pinMode(RELE_PIN, OUTPUT);
  
  lcd.init(); 
  lcd.backlight();

  cardReader.PCD_Init();
  Serial.println("Started");
}

void loop()
{
  if(flagRele == true)
  {
    digitalWrite(RELE_PIN, HIGH);

    if (millis() - startedOpen > waitTime)
    {
      flagRele = false;

      for (size_t i = 0; i < 4; ++i)
      {
        keypadTry[i] = 0; 
      }
    }
  } 
  else 
  {
    digitalWrite(RELE_PIN, LOW);
  }

  lcd.setCursor(cur_idx, 0);

  char key = keypad.getKey();

  if (key)
  {
    if (key == '*' || key == '#')
    {
      if (memcmp(keypadPsw, keypadTry, sizeof(keypadPsw)) == 0)
      {
        Serial.println("Uguale");
        flagRele = true;
        startedOpen = millis();
      }
      else
      {
        Serial.println("Nope");
      }
    }
    else
    {
      lcd.print(key);
      Serial.print(key);
      keypadTry[cur_idx] = key;
  
      cur_idx = (cur_idx + 1) % 4;
    }
  }

  if (cardReader.PICC_IsNewCardPresent())
  {
    Serial.println("Card readed: ");
    if (cardReader.PICC_ReadCardSerial())
    {
      Serial.print("Tag UID: ");

      for (size_t i = 0; i < cardReader.uid.size; ++i)
      {
        Serial.print(cardReader.uid.uidByte[i]);
        Serial.print(" ");

        rfidReadedCard[i] = cardReader.uid.uidByte[i];
        // memcpy(&rfidReadedCard[i][0], "\x0A", cardReader.uid.uidByte[i]);
      }

      // Check if the readed card is an authorized card
      if (memcmp(rfidPsw, rfidReadedCard, sizeof(rfidPsw)) == 0)
      {
        Serial.println("Uguale");
        flagRele = true;
        startedOpen = millis();
      }
      else
      {
        Serial.println("Unauthorized card!");
      }

      Serial.println();
      cardReader.PICC_HaltA();
    }
  }

  // displayDefault();
}


void displayDefault() 
{
  u8g2.firstPage();
  
  do 
  {
    u8g2.drawBox(0, 0, 128, 16);
    u8g2.setColorIndex(0);
    u8g2.setFont(u8g2_font_unifont_tf);
    u8g2.drawStr(0, 14, "Sabato");

    u8g2.setColorIndex(1);
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.setCursor(0, 27);
    u8g2.print("20-05-2023");

    u8g2.drawBox(0, 32, 128, 25);
    u8g2.setColorIndex(0);
    u8g2.setFont(u8g2_font_fub17_tf);
    u8g2.setCursor(10, 55);
    
    u8g2.print("12");
    u8g2.print(":");
    u8g2.print("25");

    u8g2.setFont(u8g_font_8x13r);
    u8g2.setCursor(72, 47);
    u8g2.print("33");
    u8g2.setColorIndex(1);
  } 
  while (u8g2.nextPage());
}