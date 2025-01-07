# MSX Interrupt Mode 1 Hooks SDCC Library(fR3eL Project)

<table>
<tr><td>Architecture</td><td>MSX</td></tr>
<tr><td>Format</td><td>C Object (SDCC .rel)</td></tr>
<tr><td>Programming language</td><td>C and Z80 assembler</td></tr>
<tr><td>Compiler</td><td>[`SDCC`](http://sdcc.sourceforge.net/) v4.4 or newer</td></tr>
</table>

---

## Description

Library with basic functions to control the MSX system hooks called from the Z80 Mode 1 interrupt ISR (Interrupt Service Routine).
Allows working with both the interruptions produced by the VBLANK from VDP (TIMI), and those produced by other peripherals (KEYI).

This library can be used to program applications that run from MSX-BASIC and as ROMs. 
It can also be used in MSX-DOS but under certain conditions.

You can access the documentation here with [`How to use the library`](docs/HOWTO.md).

In the source code [`examples/`](examples/), you can find applications for testing and learning purposes.

This project is an Open Source library. 
You can add part or all of this code in your application development or include it in other libraries/engines.

Use them for developing MSX applications using Small Device C Compiler [`SDCC`](http://sdcc.sourceforge.net/).

This library is part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

Enjoy it!

<br/>


## License

<br/>

---


## History of versions

- v1.2 (31/12/2024) Update to SDCC (4.1.12) Z80 calling conventions.
- v1.1 ( 4/07/2021) More functions to control the two Hooks (added KEYI).
- v1.0 ( 4/07/2011) First version. Published in [Avelino Herrera's WEB](http://msx.avelinoherrera.com/index_es.html#sdccmsx)

<br/>

---

## Requirements

- [Small Device C Compiler (SDCC) v4.4](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)