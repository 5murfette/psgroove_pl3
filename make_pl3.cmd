del *.o
"%1\ppu-lv2-gcc.exe" -c -x assembler -Xassembler -mlocal-fixup PL3/shellcode_panic.S
"%1\ppu-lv2-gcc.exe" -c -Xassembler -mlocal-fixup PL3/default_payload.S
"%1\ppu-lv2-gcc.exe" -c -Xassembler -mlocal-fixup PL3/dump_lv2.S
"%1\ppu-lv2-gcc.exe" -c -Xassembler -mlocal-fixup PL3/shellcode_egghunt.S

"%1\ppu-lv2-objcopy.exe" -O binary dump_lv2.o
"%1\ppu-lv2-objcopy.exe" -O binary shellcode_egghunt.o
"%1\ppu-lv2-objcopy.exe" -O binary shellcode_panic.o
"%1\ppu-lv2-objcopy.exe" -O binary default_payload.o

".\tools2\bin2header\bin2headerD.exe" dump_lv2.o pl3/dump_lv2.h PAYLOAD
".\tools2\bin2header\bin2headerD.exe" shellcode_egghunt.o pl3/shellcode_egghunt.h SHELLCODE
".\tools2\bin2header\bin2headerD.exe" shellcode_panic.o pl3/shellcode_panic.h SHELLCODE
".\tools2\bin2header\bin2headerD.exe" default_payload.o pl3/default_payload.h PAYLOAD

del *.o

pause

@echo off
cls

echo Done. Rename the PL3/default_payload.h to PL3/default_payload_3_XX.h (for example default_payload_3_15.h)

pause
