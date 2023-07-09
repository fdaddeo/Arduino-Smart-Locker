#ifndef CREDENTIALS
#define CREDENTIALS

// Insert your network credentials.
const char * ssid = "";
const char * password = "";

const char queryKey[] = "";

const char serverName[] = "http://smartlock.altervista.org/Queries/";

char * generateStudentCardQuery(char rfidCard[])
{
  char scriptName[] = "get_student_card.php?rfid=";
  int arrayDimension = (sizeof(serverName) + sizeof(scriptName) + sizeof(rfidCard));
  
  char * requestUrl = (char *) malloc(arrayDimension * sizeof(char));

  strcpy(requestUrl, serverName);
  strncat(requestUrl, scriptName, sizeof(scriptName) * sizeof(char));
  strncat(requestUrl, rfidCard, 8 * sizeof(char));

  return requestUrl;
}

char * generatePinQuery(char pinNumber[])
{
  char scriptName[] = "get_student_pin.php?pin=";
  int arrayDimension = (sizeof(serverName) + sizeof(scriptName) + sizeof(pinNumber));
  
  char * requestUrl = (char *) malloc(arrayDimension * sizeof(char));

  strcpy(requestUrl, serverName);
  strncat(requestUrl, scriptName, sizeof(scriptName) * sizeof(char));
  strncat(requestUrl, pinNumber, 6 * sizeof(char));

  return requestUrl;
}

#endif