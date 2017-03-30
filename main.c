#include <wiringPi.h>
#include <stdio.h>

#define NUM_PORTS 1

unsigned int binary(unsigned char n)
{
    unsigned int b = 0;
    unsigned int ten_to_the_i = 1;

    while(n) {
        if (n & 0x1)
            b += ten_to_the_i;
        ten_to_the_i *= 10;
        n >>= 1;
    }

    return b;
}

struct SerialPort {
  unsigned char pin;
  /*
    0: waiting for start bit
    1-8: waiting for nth data bit
    9: waiting for stop bit
  */
  unsigned char bit;
  unsigned int pBitChange;
  char buffer[80];
  unsigned char buffer_length;
};

struct SerialPort ports[NUM_PORTS];

int main (void) {
  wiringPiSetup();
  piHiPri(99);
  ports[0].pin = 0;

  for(unsigned char i = 0; i < NUM_PORTS; i++) {
    pinMode(ports[i].pin, INPUT);
    ports[i].bit = 0;
    ports[i].buffer_length = 0;
  }

  for(;;) {
    // start bit found
    for(unsigned char i = 0; i < NUM_PORTS; i++) {
      if (ports[i].bit == 0 && digitalRead(ports[i].pin) == 0) {
        ports[i].buffer[ports[i].buffer_length] = 0;

        ports[i].bit++;
        ports[i].pBitChange = micros();
      }

      if (ports[i].bit > 0 && (micros() - ports[i].pBitChange) > (ports[i].bit * 8.68)) {
        if (ports[i].bit >= 1 && ports[i].bit <= 8) {
          ports[i].buffer[ports[i].buffer_length] |= (digitalRead(ports[i].pin) << (ports[i].bit - 1));
        }
        else if (ports[i].bit == 9) {
          if (digitalRead(ports[i].pin) == 1) {
            // valid data here
            ports[i].buffer_length++;
            ports[i].bit = 0;

            printf("%c", ports[i].buffer[ports[i].buffer_length - 1]);
            fflush(stdout);
            continue;
          }
        }

        ports[i].bit++;
      }
    }
  }
  return 0 ;
}
