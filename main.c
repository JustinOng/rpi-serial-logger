#include <wiringPi.h>
#include <stdio.h>

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

int main (void) {
  /*
    0: waiting for start bit
    1-8: waiting for nth data bit
    9: waiting for stop bit
  */
  unsigned char bit = 0;
  unsigned int previous_change;
  unsigned char data;

  wiringPiSetup();
  piHiPri(99);
  pinMode (0, INPUT);

  while(digitalRead(0) == 0);

  for(;;) {
    // start bit found
    if (bit == 0 && digitalRead(0) == 0) {
      data = 0;

      bit++;
      previous_change = micros();
    }

    if (bit > 0 && (micros() - previous_change) > (bit * 8.68)) {
      if (bit >= 1 && bit <= 8) {
        data |= (digitalRead(0) << (bit - 1));
      }

      bit++;

      if (bit == 10) {
        bit = 0;
        printf("%c %d\n", data, binary(data));
      }
    }

    //printf("Found byte!\n");
  }
  return 0 ;
}
