#!/bin/bash -e

build() {
  cd $FILENAME

  avr-gcc \
    -mmcu=$DEVICE \
    -O2 $FILENAME.c \
    -o ../build/$FILENAME.elf
  
  avr-objcopy \
    -Wl,-u,-vfprintf,-lprintf_flt,-lm \
    -j .text \
    -j .data \
    -O ihex ../build/$FILENAME.elf \
    ../build/$FILENAME.hex
  
  cd ..
}

export DEVICE=atmega2560
export AVRDUDE_DEVICE=m2560
export FILENAME=receiver
build

export DEVICE=atmega328p
export AVRDUDE_DEVICE=m328p
export FILENAME=transmitter
build


