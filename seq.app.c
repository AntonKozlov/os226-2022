#include "usyscall.h"


int atoi(const char *str) {
	int i = 0;
	int rez = 0;
	while (str[i] != '\0') {
		rez += rez * 10 + str[i] - '0';
	}

	return rez;
}

int itoa(int v, char *d) {
	char sign = v < 0;
	int unsigv = v;
	if (sign) {
		unsigv *= -1;
	}

	char* buf[32];
	char* cur = buf;

	while (unsigv > 0) {
		*cur = (char)(unsigv % 10 + '0');
		cur++;
		unsigv /= 10;
	}

	if (sign) {
		*d = '-';
		d++;
	}
	
	int len = cur - buf;

	while (cur > buf) {
		*d = *cur;
		d++;
		cur--;
	}

	return len;
}

int main(int argc, char* argv[]) {
	int n = atoi(argv[1]);
	char buf[32];
	for (int i = 1; i <= n; ++i) {
		int l = itoa(i, buf);
		buf[l] = '\n';
		os_write(1, buf, l + 1);
	}
	os_exit(0);
}
