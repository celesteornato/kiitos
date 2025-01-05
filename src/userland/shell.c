#include "shell.h"

unsigned char pressed_keys[255] = {0};

int drop_into_shell(struct out output) {
  char key;
  while (1) {
    key = get_kb_raw();
    pressed_keys[key] = ((key & 128) != 128);
    if (pressed_keys[0x38] && pressed_keys[0x1D] && pressed_keys[0xE0])
      *(char*)(0x1) = 0;
  }
  return 0;
}
