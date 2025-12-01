#!/bin/sh

set -e

rm -rf build iso
mkdir -p build
cd build

cmake ..
make

cd ..

mkdir -p iso/boot/grub
cp build/kernel.elf iso/boot/
cp boot/grub/grub.cfg iso/boot/grub/

grub-mkrescue -o PlayfulOS.iso iso
