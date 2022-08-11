#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define FloatTimes10000 int

FloatTimes10000 mul(FloatTimes10000 a, FloatTimes10000 b) {
  return (a * b + 5000) / 10000;
}

int k;

void update2(FloatTimes10000 *cos, FloatTimes10000 *sin, FloatTimes10000 cos_delta, FloatTimes10000 sin_delta) {
  FloatTimes10000 new_cos = mul(*cos, cos_delta) - mul(*sin, sin_delta);
  FloatTimes10000 new_sin = mul(*sin, cos_delta) + mul(*cos, sin_delta);
  *cos = new_cos;
  *sin = new_sin;
}

int main() {
  FloatTimes10000 zTimes10000[1760];
  char b[1760];
  printf("\e[2J");
  FloatTimes10000 cosATimes10000 = 10000; FloatTimes10000 sinATimes10000 = 0;
  FloatTimes10000 cosBTimes10000 = 10000; FloatTimes10000 sinBTimes10000 = 0;
  for (;;update2(&cosATimes10000, &sinATimes10000, 9992, 400), update2(&cosBTimes10000, &sinBTimes10000, 9998, 200), usleep(50000)) {
    memset(b, 32, 1760 * sizeof(char));
    memset(zTimes10000, 0, 1760 * sizeof(FloatTimes10000));
    FloatTimes10000 sinthetaTimes10000 = 0; FloatTimes10000 costhetaTimes10000 = 10000;
    for (int theta_times_14 = 0; theta_times_14 < 88; theta_times_14++, update2(&costhetaTimes10000, &sinthetaTimes10000, 9974 + theta_times_14 % 2, 714)){
      FloatTimes10000 sinphiTimes10000 = 0; FloatTimes10000 cosphiTimes10000 = 10000;
      for (int phi_times_50 = 0; phi_times_50 < 314; phi_times_50++, update2(&cosphiTimes10000, &sinphiTimes10000, 9998, 200)) {
        FloatTimes10000 hTimes10000 = costhetaTimes10000 + 20000;
        FloatTimes10000 DTimes10000 = 100000000 / (
          mul(mul(sinphiTimes10000, hTimes10000), sinATimes10000) 
          + mul(sinthetaTimes10000, cosATimes10000)
          + 50000);
        FloatTimes10000 tTimes10000 = mul(mul(sinphiTimes10000, cosATimes10000), hTimes10000) - mul(sinthetaTimes10000, sinATimes10000);
        int x = 40 + 30 * mul(DTimes10000, mul(mul(cosphiTimes10000, cosBTimes10000), hTimes10000) - mul(tTimes10000 , sinBTimes10000)) / 10000,
            y = 12 + 15 * mul(DTimes10000, mul(mul(cosphiTimes10000, sinBTimes10000), hTimes10000) + mul(tTimes10000 , cosBTimes10000)) / 10000, 
            o = x + 80 * y,
            N = 8 * (
                mul(mul(sinthetaTimes10000, sinATimes10000) - mul(mul(sinphiTimes10000, cosATimes10000), costhetaTimes10000), cosBTimes10000)
                - mul(mul(sinphiTimes10000, sinATimes10000), costhetaTimes10000)
                - mul(sinthetaTimes10000, cosATimes10000)
                - mul(mul(cosphiTimes10000, sinBTimes10000), costhetaTimes10000)
              ) / 10000;
        if (22 > y && y > 0 && x > 0 && 80 > x && DTimes10000 > zTimes10000[o]) {
          zTimes10000[o] = DTimes10000;
          b[o] = ".,-~:;=!*#$@"[N >= 1 ? N : 0];
        }
      }
    }
    printf("\e[H");
    for (k = 0; 1761 > k; k++)
      putchar(k % 80 ? b[k] : 10);
  }
}