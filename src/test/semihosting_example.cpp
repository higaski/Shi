#include <cstdio>

/// A simple example of what's possible with semihosting
void semihosting_example() {
  printf("Hello World!\n");

  int c;
  do {
    c = getchar();
    putchar(c);
  } while (c != EOF);

  FILE* fp;
  fp = fopen("log.txt", "w");
  if (fp != NULL) {
    fprintf(fp, "Hello log\n");
    fclose(fp);
  }
}
