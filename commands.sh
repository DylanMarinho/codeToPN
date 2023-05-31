if [ $# != 2 ]; then
  echo "Syntax: commands.sh [name of script in examples, without .c] [address for extract] ";
else
  arm-none-eabi-gcc examples/$1.c -o tmp/$1 -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -mfpu=fpv4-sp-d16 -nostartfiles -fno-builtin --specs=nosys.specs -nostdlib

  arm-none-eabi-objdump -d tmp/$1 | awk -f src/extract.awk > tmp/$1.bin

  arm-none-eabi-objdump -s -j .rodata tmp/$1 > tmp/$1.rowdata
  src/extract_variables tmp/$1.rowdata

  cat tmp/$1.bin | src/extract $2  > tmp/instructions_$1.c
  mv program.xml tmp/program_$1.xml
fi
