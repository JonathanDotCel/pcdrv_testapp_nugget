#!/bin/zsh

#example
alias nops="mono ~/NOTPSXSerial/nops.exe"

nops /debug

sleep 1

#your /dev/tty.* device may vary
nops /fast /exe pcdrvtest.exe /m /dev/tty.SLAB_USBtoUART

