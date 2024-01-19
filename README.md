# C Code to Petri-Nets

## Installation
- ARM GNU toolchain https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads (in the PATH) (most probably `x86_64 Linux hosted cross toolchains` then `AArch32 bare-metal target (arm-none-eabi)`)
- compile src/extract.cpp with `cd src ; g++ -std=c++17 extract.cpp -o extract`

## Usage
`python3 main.py [path to c file]`

This function will automatically compile the c file, extract the memory and instruction data, generate and update the Romeo project. It ends with the print of the function to compute the execution times of the model.

## To use Roméo
When Roméo is open, open [name of the c file].xml and check the printed property (`EF[p,p](INST...[0]>0)`)

## Author
Developed by Dylan Marinho
