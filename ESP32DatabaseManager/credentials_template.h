#ifndef CREDENTIALS
#define CREDENTIALS

// Function definitions
char * generateStudentCardQuery(char rfidCard[]);
char * generatePinQuery(char pinNumber[]);

const char ssid[] = "";
const char password[] = "";

const char serverName[] = "http://smartlock.altervista.org/Queries/";

/**
 * @brief Generates the query starting from the readed card.
 *
 * @param rfidCard The readed card.
 *
**/
char * generateStudentCardQuery(char rfidCard[])
{
  char scriptName[] = "get_student_card.php?id=101&rfid=";
  int arrayDimension = sizeof(serverName) + sizeof(scriptName) + sizeof(rfidCard);
  
  char * requestUrl = (char *) malloc(arrayDimension * sizeof(char));

  strcpy(requestUrl, serverName);
  strncat(requestUrl, scriptName, sizeof(scriptName) * sizeof(char));
  strncat(requestUrl, rfidCard, 8 * sizeof(char));

  return requestUrl;
}

/**
 * Generates the query starting from the inserted pin.
 *
 * @param pinNumber The inserted pin.
 *
**/
char * generatePinQuery(char pinNumber[])
{
  char scriptName[] = "get_student_pin.php?id=101&pin=";
  int arrayDimension = (sizeof(serverName) + sizeof(scriptName) + sizeof(pinNumber));
  
  char * requestUrl = (char *) malloc(arrayDimension * sizeof(char));

  strcpy(requestUrl, serverName);
  strncat(requestUrl, scriptName, sizeof(scriptName) * sizeof(char));
  strncat(requestUrl, pinNumber, 6 * sizeof(char));

  return requestUrl;
}

#endif