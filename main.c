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
  unsigned char bit = 0;
  unsigned int previous_change;

  wiringPiSetup();
  piHiPri(99);
  pinMode (0, INPUT);

  while(digitalRead(0) == 0);

  for(;;) {
    unsigned char data;

    // while bus is idle
    while (digitalRead(0) == 1);

    // start bit present right now
    previous_change = micros();

    //printf("Got start byte\n");

    data = 0;
    for(unsigned char i = 0; i < 8; i++) {
      // wait for 1 bit time before proceeding
      while((micros() - previous_change) < ((i+1) * 8.68));

      data |= (digitalRead(0) << i);
    }

    // wait for stop bit
    while((micros() - previous_change) < ((8+1) * 8.68));

    //printf("Found byte!\n");
    printf("%c %d\n", data, binary(data));
    //fwrite(&data, 1, 1, stdout);
    fflush(stdout);
  }
  return 0 ;
}
