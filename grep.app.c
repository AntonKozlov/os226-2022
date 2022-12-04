#include "usyscall.h"

char *strstr(const char *where, const char *what) {
  char *black_ptr = where;
  while (*black_ptr != '\0') {
    char *red_ptr = black_ptr;
    char *sub_ptr = what;
    while (*red_ptr != '\0' && *sub_ptr != '\0' && *red_ptr == *sub_ptr) {
      ++red_ptr;
      ++sub_ptr;
    }
    if (*sub_ptr == '\0') {
      return black_ptr;
    }
    ++black_ptr;
  }
}

void *memchr(const void *str, int c, long unsigned n) {
  char *end = (char *)(str + n);
  for (char *p = (char *)str; p < end; p++) {
    if (*p == c) {
      return p;
    }
  }

  return 0;
}

void *memmove(void *dst, const void *src, long unsigned n) {

  const char *psrc = (const char *)src;
  char *pdest = (char *)dst;
  if (pdest <= psrc || pdest >= psrc + n) {
    while (n--) {
      *pdest++ = *psrc++;
    }
  } else {
    pdest = pdest + n - 1;
    psrc = psrc + n - 1;
    while (n--) {
      *pdest-- = *psrc--;
    }
  }
  return dst;
}

int main(int argc, char *argv[]) {
  char buf[5];
  int len;
  int o = 0;
  while (0 < (len = os_read(0, buf + o, sizeof(buf) - o))) {
    len += o;
    char *p = buf;
    char *p2 = memchr(p, '\n', len);
    while (p2) {
      *p2 = '\0';
      if (strstr(p, argv[1])) {
        os_write(1, p, p2 - p);
        os_write(1, "\n", 1);
      }
      p = p2 + 1;
      p2 = memchr(p, '\n', len - (p - buf));
    }
    o = len - (p - buf);
    memmove(buf, p, o);
  }

  os_exit(0);
}
