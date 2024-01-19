# C code to (parametric) timed Petri nets

```____                 _        _          ____
  / ___|   ___ ___   __| | ___  | |_ ___   |  _ \ ___  _ __ ___   ___  ___
 | |      / __/ _ \ / _` |/ _ \ | __/ _ \  | |_) / _ \| '_ ` _ \ / _ \/ _ \
 | |___  | (_| (_) | (_| |  __/ | || (_) | |  _ < (_) | | | | | |  __/ (_) |
  \____|  \___\___/ \__,_|\___|  \__\___/  |_| \_\___/|_| |_| |_|\___|\___/
```

This tool converts automatically C programs into timed Petri nets readable by the [Roméo model checker](https://romeo.ls2n.fr/).

## Installation

### Install ARM GNU toolchain
Download the [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

(most probably `x86_64 Linux hosted cross toolchains` then `AArch32 bare-metal target (arm-none-eabi)`)

Do not forget to add the `bin` repository to your `$PATH`.

### Compile
Compile `src/extract.cpp` with
```
cd src ; g++ -std=c++17 extract.cpp -o extract ; cd ..
```

You then have to create a `tmp` directory at the root of the codeToPN repository.
```
mkdir tmp
```

## Usage
`python3 main.py [path to c file]`

This function will automatically compile the C file, extract the memory and instruction data, generate and update the Roméo project.
It ends with the print of the function to compute the execution times of the model.

## To use Roméo
When Roméo is open, open `[name of the C file].xml` and check the printed property (`EF[p,p](INST...[0]>0)`)

## Author
Developed by Dylan Marinho
