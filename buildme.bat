
@echo off

del pcdrvtest.exe
del pcdrvtest.elf
del pcdrvtest.o
del pcdrvtest.sym
del pcdrvtest.map
del *.temp

call dockermake -f buildme.mk all


