// Copyright (c) Sergey Lyubka, 2013.
// All rights reserved.
// Released under the MIT license.

// This program is used to embed arbitrary data into a C binary. It takes
// a list of files as an input, and produces a .c data file that contains
// contents of all these files as collection of char arrays.
// Usage:
//   1. Compile this file:
//      cc -o embed embed.c
//
//   2. Convert list of files into single .c:
//      ./embed file1.data file2.data > embedded_data.c
//
//   3. In your application code, you can access files using this function:
//
//      const char *find_embedded_file(const char *file_name, size_t *size);
//      size_t size;
//      const char *data = find_embedded_file("file1.data", &size);
//
//   4. Build your app with embedded_data.c:
//      cc -o my_app my_app.c embedded_data.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *code =
  "const char *find_embedded_file(const char *name, size_t *size) {\n"
  "  const struct embedded_file *p;\n"
  "  for (p = embedded_files; p->name != NULL; p++) {\n"
  "    if (!strcmp(p->name, name)) {\n"
  "      if (size != NULL) { *size = p->size; }\n"
  "      return (const char *) p->data;\n"
  "    }\n"
  "  }\n"
  "  return NULL;\n"
  "}\n";

int main(int argc, char *argv[]) {
  FILE *fp;
  int i, j, ch;
  int is_text;

  printf("#include <stdlib.h>\n");
  printf("#include <string.h>\n\n");
  printf("const char *find_embedded_file(const char *name, size_t *size);\n\n");
  
  for (i = 1; i < argc; i++) {
    if ((fp = fopen(argv[i], "rb")) == NULL) {
      exit(EXIT_FAILURE);
    } else {
      while ((ch = fgetc(fp)) != EOF && ch != 0) ;
      is_text = ch == EOF;
      rewind(fp);

      if (is_text) {
        int is_comment = 0;
        int c = 0;
        int s = 0;
        printf("static const unsigned char v%d[] = ", i);
        for (j = 0; (ch = fgetc(fp)) != EOF; j++) {
          if (is_comment >= 2) {
            if (ch == '\n') {
              is_comment = 0; c = 0; s = 0;
            }
            putchar(ch);
          } else {
            switch (ch) {
              case '\n':
                if (c) {
                  printf("\\n\"\n");
                  c = 0; s = 0;
                }
                is_comment = 0;
                break;
              case '\t': s += 2; break;
              case '\r': break;
              case '/': is_comment++;
                if (is_comment == 2) {
                  if (c) printf("\\n\" ");
                  printf("//");
                }
                break;
              case '"': if (c == 0) putchar('"'); printf("\\\""); break;
              default: 
                if (ch == ' ') s++;
                else {
                  if (c == 0) putchar('"');
                  for(;s;--s) putchar(' ');
                  putchar(ch);
                  c++;
                }
            }
          }
        }
        if (c) putchar('"');
        printf(";\n");
      } else {
        printf("static const unsigned char v%d[] = {", i);
        for (j = 0; (ch = fgetc(fp)) != EOF; j++) {
          if ((j % 12) == 0) {
            printf("%s", "\n ");
          }
          printf(" %#04x,", ch);
        }
        // Append zero byte at the end, to make text files appear in memory
        // as nul-terminated strings.
        printf("%s", " 0x00\n};\n");
      }
      fclose(fp);
    }
  }

  printf("%s", "\nconst struct embedded_file {\n");
  printf("%s", "  const char *name;\n");
  printf("%s", "  const unsigned char *data;\n");
  printf("%s", "  size_t size;\n");
  printf("%s", "} embedded_files[] = {\n");

  for (i = 1; i < argc; i++) {
    char* name = strrchr(argv[i], '/');
    if (name) ++name; else name = argv[i];
    printf("  {\"%s\", v%d, sizeof(v%d) - 1},\n", name, i, i);
  }
  printf("%s", "  {NULL, NULL, 0}\n");
  printf("%s", "};\n\n");
  printf("%s", code);

  return EXIT_SUCCESS;
}
