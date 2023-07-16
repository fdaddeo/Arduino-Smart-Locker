#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Keypad.h>

// #define USE_4X3_KEYPAD

#ifdef USE_4X3_KEYPAD
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 3
#else
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
#endif

#define RELE_PIN 2

#define RFID_SDA 53 // arduino mega default
#define RFID_DIO 49 // arduino mega default

#define LOCKER_WAIT_TIME 5000 // unlocked locker time interval

// Functions definitions
void byteArrayToHexCharArray(byte * byteArray, size_t lenght, char * hexArray);
void displayDefault();
char byteToHexChar(byte input);

#ifdef USE_4X3_KEYPAD
char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {{'1', '2', '3'},
                                             {'4', '5', '6'},
                                             {'7', '8', '9'},
                                             {'*', '0', '#'}};

char rowPins[KEYPAD_ROWS] = {8, 3, 4, 6};
char colPins[KEYPAD_COLS] = {7, 10, 5};
#else
char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {{'1', '2', '3', 'A'},
                                             {'4', '5', '6', 'B'},
                                             {'7', '8', '9', 'C'},
                                             {'*', '0', '#', 'D'}};

char rowPins[KEYPAD_ROWS] = {22, 23, 24, 25};
char colPins[KEYPAD_COLS] = {26, 27, 28, 29};
#endif

LiquidCrystal_I2C lcd(0x27, 16, 2);  
MFRC522 cardReader(RFID_SDA, RFID_DIO); 
Keypad keypad = Keypad(makeKeymap(keypadKeys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

size_t cur_idx = 0;

unsigned long startedOpen = 0;

boolean flagRele = false;

char keypadTry[6];

void setup()
{
  // Used for debug.
  Serial.begin(19200);
  // Used for comunication with ESP8266.
  Serial1.begin(115200);

  Wire.begin();
  SPI.begin();
  
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
    // Open the locker.
    digitalWrite(RELE_PIN, HIGH);

    // Lock the locker after LOCKER_WAIT_TIME milliseconds.
    if (millis() - startedOpen > LOCKER_WAIT_TIME)
    {
      flagRele = false;

      // Reset the inserted PIN.
      for (size_t i = 0; i < 6; ++i)
      {
        keypadTry[i] = 0; 
      }
    }
  } 
  else 
  {
    // Lock the locker.
    digitalWrite(RELE_PIN, LOW);
  }

  // Get the digited key on the keypad.
  char key = keypad.getKey();

  if (key)
  {
    // Confirm button.
    if (key == '*' || key == '#')
    {
      // Initialize the ESP8266 message.
      char msgArray[8];
      // Add first char.
      strcpy(msgArray, "$");
      // Copy the inserted pin.
      strncat(msgArray, keypadTry, sizeof(keypadTry) * sizeof(char));
      // Add terminator.
      strncat(msgArray, "\0", sizeof(char));

      Serial.print("Sending: ");
      Serial.println(msgArray);
      Serial1.write(msgArray);

      // wait for response
      delay(500);

      String response = Serial1.readString();

      // Check whether the inserted pin is corrected or not.
      if (response[0] == '1')
      {
        flagRele = true;
        startedOpen = millis();
        
        lcd.setCursor(0, 1);
        lcd.print("Opening         ");
        Serial.println("Access Granted");
      }
      else
      {
        lcd.setCursor(0, 1);
        lcd.print("Wrong pin       ");
        Serial.println("Wrong pin!");
      }

      cur_idx = 0;
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(cur_idx, 0);
    }
    else
    {
      // Display the digited key.
      lcd.setCursor(cur_idx, 0);
      lcd.print(key);
      Serial.print(key);
      
      // Update the current attempt.
      keypadTry[cur_idx] = key;
      cur_idx = (cur_idx + 1) % 6;
    }
  }

  // Check if it has been read a new RFID card.
  if (cardReader.PICC_IsNewCardPresent())
  {
    Serial.println("Card readed: ");

    // Get the readed card info.
    if (cardReader.PICC_ReadCardSerial())
    {
      Serial.print("Tag UID: ");

      size_t readedCardLength = cardReader.uid.size;
      char readedCard[readedCardLength * 2];

      // Convert the readed card to hexadecimal values.
      byteArrayToHexCharArray(cardReader.uid.uidByte, readedCardLength, readedCard);

      char msgArray[10];
      
      // Add first char.
      strcpy(msgArray, "<");
      // Copy the readed card.
      strncat(msgArray, readedCard, 8 * sizeof(char));
      // Add the terminator.
      strncat(msgArray, "\0", sizeof(char));

      Serial.println(msgArray);
      Serial1.write(msgArray);

      // wait for response
      delay(500);

      String response = Serial1.readString();

      // Check if the readed card is an authorized card.
      if (response[0] == '1')
      {
        flagRele = true;
        startedOpen = millis();
        
        lcd.setCursor(0, 1);
        lcd.print("Opening         ");
        Serial.println("Access Granted");
      }
      else if (response[0] == '2')
      {
        // The locker is booked.
        lcd.setCursor(0, 1);
        lcd.print("Booked          ");
      }
      else if (response[0] == '3')
      {
        // The locker is occupied.
        lcd.setCursor(0, 1);
        lcd.print("Occupied        ");
      }
      else if (response[0] == '4')
      {
        // It has been completed the locker occupation.
        flagRele = true;
        startedOpen = millis();

        lcd.setCursor(0, 1);
        lcd.print("Occupation Done ");
      }
      else
      {
        // The card is not authorized.
        lcd.setCursor(0, 1);
        lcd.print("Un-authorized   ");
      }

      Serial.println();
      cardReader.PICC_HaltA();
    }
  }
}

/**
 * @brief Converts a byte array into an hexadecimal char array.
 *
 * @param byteArray The input byte array to be converted.
 * @param length The length of the input byte array.
 * @param hexArray The destination char array.
 * 
**/
void byteArrayToHexCharArray(byte * byteArray, size_t length, char * hexArray)
{
  for (size_t i = 0; i < length; ++i)
  {
    byte firstChunk = (byteArray[i] >> 4) & 0x0F;
    byte secondChunk = byteArray[i] & 0x0F;

    hexArray[i * 2] = byteToHexChar(firstChunk);
    hexArray[i * 2 + 1] = byteToHexChar(secondChunk);
  }

  hexArray[length * 2] = '\0';
}

/**
  * @brief Converts a byte value into a char hexadecimal value.
  *
  * @param input The byte value to converted.
  *
  * @return The converted hexadecimal value.
  *
**/
char byteToHexChar(byte input)
{
  return (input <= 9) ? '0' + input : 'A' + (input - 10);
}
