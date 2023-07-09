#ifndef DATA_STRUCTS
#define DATA_STRUCTS

#define CARD 0
#define PIN 1

/**
  * @brief The struct message to be sent to ESP8266 in order to check the readed card and the inserted PIN.
  *
**/
struct CheckMessage
{
  /**
    * The message type:
    *  - 0 for card,
    *  - 1 for PIN.
  **/
  size_t type;

  /**
    * The readed card number or the inserted PIN.
    *
  **/
  char number[10];

  /**
    * The response coded:
    *  - 0 then response not received,
    *  - 1 then positive response received,
    *  - 2 negative response received.
   **/
  size_t responseCode;
};

#endif