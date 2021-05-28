@echo off

REM yours may vary
set PATH=%PATH%;d:\notpsxserial\

REM yours may vary
tools\nops /fast /exe pcdrvtest.exe COM14 /m
