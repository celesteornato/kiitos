#!/bin/sh
qemu-system-x86_64 -D log.log -d int,cpu_reset -M smm=off -no-reboot ./kiitos.iso
