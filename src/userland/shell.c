#include "shell.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitklib.h"
#include "stdint.h"

static unsigned char buffer[2048] = {0};

static char *err_codes[] = {
    "",
    "Changed font to 'Solarize'.",
    "Changed font to 'Powerline'.",
    "No filesystem.",
    "\f",
    "Faulting...",
    "Command not found!",
};

void ask_cmd(struct out *otp) {
  otp->fg = 0xffff00;
  prints("\n<< ", otp);
  otp->fg = 0xffffff;
  gets(otp, (char *)buffer, 2048);
  otp->fg = 0x00ff00;
  prints("\n>> ", otp);
  uint64_t err = kexec((char *)buffer);
  prints(err_codes[err], otp);
  otp->fg = 0xffffff;
}

int drop_into_shell(struct out *output, struct out *err) {
  prints("Have not crashed going into userland!\n", output);

  while (1) {
    ask_cmd(output);
    memset(buffer, 0, 2048 * sizeof(char));
  }

  return 0;
}
