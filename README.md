# C code to (parametric) timed Petri nets

```
   ____                 _        _          ____
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

You then have to create a `generated_files` directory at the root of the codeToPN repository if none exists.
```
mkdir generated_files
```

## Usage
```
python3 main.py [path to C file]
```

This function will automatically compile the C file, extract the memory and instruction data, generate and update the Roméo project.
It ends with the print of the function to compute the execution times of the model.

## Executing Roméo on the generated model
When Roméo is open, open `[name of the C file].xml`, and check the property that was printed in the terminal output (resembling `EF[p,p](INST...[0]>0)`)

NB: currently, the generated model will be in `generated_files/` and you will have to copy the chosen hardware models (one of the `hardware_models` subdirectories' content) into `generated_files/` *before* executing Roméo.

## Authors
Developed by Jean-Luc Béchennec, with contributions by Étienne André, and Didier Lime and Dylan Marinho.
