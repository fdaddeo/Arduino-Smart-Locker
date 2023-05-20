#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Keypad.h>

#define ROWS 4
#define COLS 3

#define SDA_DIO 7  // TODO
#define RESET_DIO 9  // TODO

char keypadKeys[ROWS][COLS] = {{'1', '2', '3'},
                               {'4', '5', '6'},
                               {'7', '8', '9'},
                               {'*', '0', '#'}};

char rowPins[ROWS] = {8, 3, 4, 6};
char colPins[COLS] = {7, 10, 5};

LiquidCrystal_I2C lcd(0x27, 16, 2);  
MFRC522 cardReader(SDA_DIO, RESET_DIO); 
Keypad keypad = Keypad(makeKeymap(keypadKeys), rowPins, colPins, ROWS, COLS);

size_t cur_idx = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  SPI.begin();

  lcd.init(); 
  lcd.backlight();

  cardReader.PCD_Init();
}

void loop()
{
  lcd.setCursor(cur_idx, 0);

  char key = keypad.getKey();
  if (key)
  {
    lcd.print(key);
    Serial.print(key);
    ++cur_idx;
  }
  

  if (cardReader.PICC_IsNewCardPresent())
  {
    // TODO
  }
}
