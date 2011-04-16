/* Be careful, this is an old dirty code with single-letter variables */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>
#include <math.h>

#define BASEPORT 0x201 /* joystick port */
#define MAX 5000 /* the max value for the looper */

unsigned int dec2bin(unsigned char h) {
    double n;
    unsigned int b = 0;

    for (n = 0; n < 8; n++) {
        b += (pow(10, n) * (h % 2));
        h /= 2;
    }

    return b;
}

unsigned char mask(unsigned char i) {
    unsigned char r;
    if (i < 4)
        r = pow(2, i);
    else
        r = 0;

    return r;
}

int main(int argc, char **argv) {
    unsigned char i, raw, temp, e[4], sum;
    unsigned long c, o[4];
    /* obtained by calibration */
    double a = 1322., b = -312., g = 10, h = 800., f;

    if (ioperm(BASEPORT, 3, 1)) {
        perror("ioperm");
        return EXIT_FAILURE;
    }

    do {

        outb(0, BASEPORT);

        temp = 0;
        for (i = 0; i < 4; i++) {
            e[i] = 0;
            o[i] = 0;
        }
        c = 0;

        while (sum != 4 && c < MAX) {
            //outb(255, BASEPORT); // reset the port
            raw = inb(BASEPORT); // read the port
            temp = ~raw;

            sum = 0;

            for (i = 0; i < 4; i++) {
                if((temp & mask(i)) == mask(i)) {
                    if(!e[i]) o[i] = c;
                    e[i] = 1;
                }
                sum += e[i];
            }

            //printf("raw=%d, temp=%d, temp & 1=%d, temp & 2=%d, c=%ld\n", dec2bin(raw), temp, (temp & 1), (temp & 2), c);

            c++;
        }

        //if(c >= MAX) printf("Timeout!\n");

        for (i = 0; i < 4; i++) {
            if(i == 3) /* linear sensor */
                f = g + h / o[i];
            else /* logarithmic sensor */
                f = b + a / log((double)o[i]);

            printf("%.1f ", f);
        }

        printf("\n");

    } while(0);

    if (ioperm(BASEPORT, 3, 0)) {
        perror("ioperm");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

