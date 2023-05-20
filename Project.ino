#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <U8g2lib.h>

// #define USE_4X3_KEYPAD

#ifdef USE_4x3_KEYPAD
#define ROWS 4
#define COLS 3
#else
#define ROWS 4
#define COLS 4
#endif

#define RELE 2

#define SDA_DIO 53
#define RESET_DIO 49

#ifdef USE_4x3_KEYPAD
char keypadKeys[ROWS][COLS] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'},
                               {'*', '0', '#'}};

char rowPins[ROWS] = {8, 3, 4, 6};
char colPins[COLS] = {7, 10, 5};
#else
char keypadKeys[ROWS][COLS] = {{'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};
#endif

char keyPwd[4] = {'1', '2', '3', '4'};
byte pwd[4] = {199, 81, 171, 121};

char keyTry[4];

// ======================================== Setup DISPLAY.
//Adafruit_SSD1306 display(128, 64, &Wire, 4);
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0); // I2C / TWI
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
// ========================================

LiquidCrystal_I2C lcd(0x27, 16, 2);  
MFRC522 cardReader(SDA_DIO, RESET_DIO); 
Keypad keypad = Keypad(makeKeymap(keypadKeys), rowPins, colPins, ROWS, COLS);

size_t cur_idx = 0;

boolean flagRele = false;

unsigned long startedOpen = 0;
int waitTime = 5000;

void setup()
{
  Serial.begin(19200);
  Wire.begin();
  SPI.begin();
  u8g2.begin();
  
  pinMode(RELE, OUTPUT);
  
  lcd.init(); 
  lcd.backlight();

  cardReader.PCD_Init();
  Serial.println("partito");

  flagRele=false;
}

void loop()
{

  if(flagRele == true)
  {
    digitalWrite(RELE,HIGH);

    if (millis() - startedOpen > waitTime)
    {
      flagRele = false;

      for (size_t i = 0; i < 4; ++i)
      {
        keyTry[i] = 0; 
      }
    }
  } 
  else 
  {
    digitalWrite(RELE,LOW);
  }


  
  lcd.setCursor(cur_idx, 0);

  char key = keypad.getKey();
  if (key)
  {
    if (key == '*' || key == '#')
    {
      if (memcmp(keyPwd, keyTry, sizeof(keyPwd)) == 0)
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
      keyTry[cur_idx] = key;
  
      cur_idx = (cur_idx + 1) % 4;
    }
  }
  

  if (cardReader.PICC_IsNewCardPresent())
  {
    Serial.println("Carta riconosciuta: ");
    if (cardReader.PICC_ReadCardSerial())
    {
      byte readedCard[4];
      
      Serial.print("Tag UID: ");

      for (size_t i = 0; i < cardReader.uid.size; ++i)
      {
        Serial.print(cardReader.uid.uidByte[i]);
        Serial.print(" ");

        readedCard[i] = cardReader.uid.uidByte[i];
        
        // memcpy(&readedCard[i][0], "\x0A", cardReader.uid.uidByte[i]);
      }

      Serial.print("Carta letta: ");
      for (size_t i = 0; i < 4; ++i)
      {
        Serial.print(readedCard[i]);
      }
      Serial.println();

      Serial.print("Pwd: ");
      for (size_t i = 0; i < 4; ++i)
      {
        Serial.print(pwd[i]);
      }
      Serial.println();

      if (memcmp(pwd, readedCard, sizeof(pwd)) == 0)
      {
        Serial.println("Uguale");
        flagRele = true;
        startedOpen = millis();
      }
      else
      {
        Serial.println("Nope");
      }

      Serial.println();
      cardReader.PICC_HaltA();
    }
  }

  displayDefault();
}


//=======================================================Display Data/Ora
void displayDefault() {
  u8g2.firstPage();
  do {
    u8g2.drawBox(0, 0, 128, 16);
    u8g2.setColorIndex(0);  //inverti colore
    u8g2.setFont(u8g2_font_unifont_tf);
    u8g2.drawStr(0, 14, "Sabato");

    u8g2.setColorIndex(1);
    //u8g2.setFont(u8g_font_7x13r);
    u8g2.setFont(u8g2_font_7x13_tf);
    u8g2.setCursor(0, 27);
    u8g2.print("20-05-2023");

    u8g2.drawBox(0, 32, 128, 25);
    u8g2.setColorIndex(0);  //inverti colore
    u8g2.setFont(u8g2_font_fub17_tf);
    u8g2.setCursor(10, 55);

    char alfa[2];

    
    u8g2.print("12");
    u8g2.print(":");

    
    u8g2.print("25");

    u8g2.setFont(u8g_font_8x13r);
    u8g2.setCursor(72, 47);

    
    u8g2.print("33");

    u8g2.setColorIndex(1);
  } while (u8g2.nextPage());
}
//=======================================================
