#!/bin/sh
qemu-system-x86_64 -d int,cpu_reset -M smm=off -no-reboot ./kiitos.iso
