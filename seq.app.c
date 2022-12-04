#include "usyscall.h"

int atoi(const char *str) {

  if (!str)
    return -1;
  int bMinus = 0;
  int result = 0;

  if (('0' > *str || *str > '9') && (*str == '+' || *str == '-')) {
    if (*str == '-')
      bMinus = 1;
    *str++;
  }
  while (*str != '\0') {
    if ('0' > *str || *str > '9')
      break;
    else
      result = result * 10 + (*str++ - '0');
  }

  if (*str != '\0')
    return -2;

  return bMinus == 1 ? -result : result;
}

int itoa(int v, char *d) {
  int power = 0, j = 0;
  int len = 0;
  j = v;
  if (v < 0) {
    j *= -1;
  }
  for (power = 1; j > 10; j /= 10)
    power *= 10;

  for (; power > 0; power /= 10) {
    len++;
    *d++ = '0' + v / power;
    v %= power;
  }

  if (v < 0) {
    *d++ = '-';
    len++;
  }

  *d = '\0';
  return len;
}

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  char buf[32];
  for (int i = 1; i <= n; ++i) {
    int l = itoa(i, buf);
    buf[l] = '\n';
    os_write(1, buf, l + 1);
  }
  os_exit(0);
}
