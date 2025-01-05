#include "shell.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitklib.h"

unsigned char pressed_keys[255] = {0};

unsigned char buffer[2048] = {0};

static char *error_messages[] = {
    "",
    "lol not implemented there's no filesystem why would you have files",
    "Invalid command!",
    "\f",
};

int drop_into_shell(struct out *output, struct out *err) {
  output->fg = 0xffff00;
  prints("\n>>", output);
  output->fg = 0xffffff;

  char last_key = 0;
  unsigned int buff_stackptr = 0;
  char key = 0;
  while (1) {
    key = get_kb_key();
    if (key == 0 || last_key == key)
      continue;
    last_key = key;

    if (key == '\n') {
      // prints((char*)buffer, err);
      printc('\n', output);
      prints(error_messages[kexec((char*)buffer)], output);
      memset(buffer, 0, 2048);
      buff_stackptr = 0;

      output->fg = 0xffff00;
      output->fg = 0xffffff;
      prints("\n>>", output);
      continue;
    }
    else if (key == '\b')
    {
      buffer[buff_stackptr--] = 0;
    }

    printc(key, output);
    buffer[buff_stackptr++] = key;
  }
  return 0;
}
