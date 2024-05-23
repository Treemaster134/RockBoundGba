cd c:\VSCODE\gba
del *.gba
del *.elf
cd c:\devkitadv\arm-agb-elf\bin\
gcc -o c:\VSCODE\gba\M4demoDoubleBuff.gba c:\VSCODE\gba\main.c
cd c:\VSCODE\gba
