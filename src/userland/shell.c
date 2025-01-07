#include "shell.h"
#include "../kernel/kiitkio.h"
#include "../libc/kiitklib.h"

#define SHELL_BUFF_SIZE 2048

static unsigned char buffer[SHELL_BUFF_SIZE] = {0};

static char *err_codes[] = {
    "",
    "Changed font to 'Solarize'.",
    "Changed font to 'Powerline'.",
    "No filesystem.",
    "\f",
    "Faulting...",
    "Command not found!",
};

static void ask_cmd(struct out *otp) {
  otp->fg = 0xffff00;
  prints("\n<< ", otp);
  otp->fg = 0xffffff;
  gets(otp, (char *)buffer, SHELL_BUFF_SIZE);
  otp->fg = 0x00ff00;
  prints("\n>> ", otp);
  uint64_t err = kexec((char *)buffer);
  prints(err_codes[err], otp);
  otp->fg = 0xffffff;
}

extern void* isr_stub_table[];
int drop_into_shell(struct out *output, struct out *err) {
  prints("Welcome to userland!\n", output);
  while (1) {
    ask_cmd(output);
    memset(buffer, 0, SHELL_BUFF_SIZE * sizeof(char));
  }

  while(1)
  {
    printd((uint64_t)isr_stub_table[2],err);
    printc('\n',err);
  }

  return 0;
}
