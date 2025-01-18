# MSX Interrupt Mode 1 ISR SDCC Library (fR3eL Project)

<table>
<tr><td>Architecture</td><td>MSX</td></tr>
<tr><td>Format</td><td>C Object (SDCC .rel)</td></tr>
<tr><td>Programming language</td><td>C and Z80 assembler</td></tr>
<tr><td>Compiler</td><td>SDCC v4.4 or newer</td></tr>
</table>

---

## Description

Library with basic functions to control the Interrupt Service Routine (ISR) for Z80 Mode 1 interrupts on MSX system.
Allows you to save, replace, disable and recover the ISR under the MSX-DOS environment.

You can use it in combination with the [Interrupt M1 Hooks library](../Hooks), to work with the hooks defined in MSX system, but with an optimized ISR.

You can access the documentation here with [`How to use the library`](docs/HOWTO.md).

In the source code [`examples/`](examples/), you can find applications for testing and learning purposes.

This project is an Open Source library. 
You can add part or all of this code in your application development or include it in other libraries/engines.

Use them for developing MSX applications using Small Device C Compiler [`SDCC`](http://sdcc.sourceforge.net/).

This library is part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

Enjoy it!

<br/>

---

## History of versions
- v1.2 (31/12/2024) Update to SDCC (4.1.12) Z80 calling conventions.
- v1.1 ( 1/09/2021) More functions to control ISR and two Hooks (TIMI/KEYI).
- v1.0 (16/11/2004) First version developed by [Avelino Herrera](http://msx.avelinoherrera.com/index_es.html#sdccmsxdos)

<br/>

---

## Requirements

- [Small Device C Compiler (SDCC) v4.4](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)