target remote localhost:3333
monitor reset init
monitor flash write_image erase firmware.elf
monitor reset init
monitor resume