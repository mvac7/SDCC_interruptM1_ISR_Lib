# How to use the Interrupt M1 Hooks MSX SDCC Library

---

## Index

- [1 Description](#1-Description)
- [2 Requirements](#2-Requirements)
- [3 Definitions](#3-Definitions)
- [4 Functions](#4-Functions)
   - [4.1 TIMI Hook Functions](#41-TIMI-Hook-Functions)
   - [4.2 KEYI Hook Functions](#42-KEYI-Hook-Functions)
- [5 How to use](#5-How-to-use)
- [6 References](#6-References)
   - [6.1 English](#61-English)
   - [6.2 Spanish](#62-Spanish)

<br/>

---

## 1 Description

Library with basic functions to control the MSX system hooks called from the Z80 Mode 1 interrupt ISR (Interrupt Service Routine).
Allows working with both the interruptions produced by the VBLANK from VDP (TIMI), and those produced by other peripherals (KEYI).

This library can be used to program applications that run from MSX-BASIC and as ROMs. 
It can also be used in MSX-DOS but under certain conditions.

This project is an Open Source library. 
You can add part or all of this code in your application development or include it in other libraries/engines.

Use them for developing MSX applications using Small Device C Compiler [`SDCC`](http://sdcc.sourceforge.net/).

This library is part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

<br/>

---

## 2 Requirements

- [Small Device C Compiler (SDCC) v4.4](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)


<br/>

---


## 3 Definitions

Name       | Description
---------- | -------------
`DisableI` | Disable interrupts. <br/> Add `DI` code in Z80 assembler.
`EnableI`  | Enable interrupts. <br/> Add `EI` code in Z80 assembler.
`PUSH_AF`  | Saves the AF value on the stack. Required for starting TIMI (VBLANK) type functions. <br/> Add `PUSH AF` code in Z80 assembler.
`POP_AF`   | Retrieves the value of AF from the stack. Required for the end of TIMI (VBLANK) type functions. <br/> Add `POP AF` code in Z80 assembler.
`HALT`     | Suspends all actions until the next interrupt. <br/> Add `HALT` code in Z80 assembler.


<br/>

---


## 4 Functions

### 4.1 TIMI Hook Functions

<table>
<tr><th colspan=2 align="left">Save_TIMI</th></tr>
<tr><td colspan="2">Save TIMI hook vector</td></tr>
<tr><th>Function</th><td>Save_TIMI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Save_TIMI();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Install_TIMI</th></tr>
<tr><td colspan="2">Set new TIMI hook vector</td></tr>
<tr><th>Function</th><td>Install_TIMI(func)</td></tr>
<tr><th>Input</th><td>[func] Function</td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Install_TIMI(my_TIMI);</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Restore_TIMI</th></tr>
<tr><td colspan="2">Restore old TIMI hook vector</td></tr>
<tr><th>Function</th><td>Restore_TIMI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Restore_TIMI();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Disable_TIMI</th></tr>
<tr><td colspan="2">Disable the TIMI hook (Add a ret on the hook)</td></tr>
<tr><th>Function</th><td>Disable_TIMI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Disable_TIMI();</code></td></tr>
</table>



### 4.2 KEYI Hook Functions

<table>
<tr><th colspan=2 align="left">Save_KEYI</th></tr>
<tr><td colspan="2">Save KEYI hook vector</td></tr>
<tr><th>Function</th><td>Save_KEYI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Save_KEYI();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Install_KEYI</th></tr>
<tr><td colspan="2">Set new KEYI hook vector</td></tr>
<tr><th>Function</th><td>Install_KEYI(func)</td></tr>
<tr><th>Input</th><td>[func] Function</td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Install_KEYI(my_KEYI);</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Restore_KEYI</th></tr>
<tr><td colspan="2">Restore old KEYI hook vector</td></tr>
<tr><th>Function</th><td>Restore_KEYI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Restore_KEYI();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Disable_KEYI</th></tr>
<tr><td colspan="2">Disable the KEYI hook (Add a ret on the hook)</td></tr>
<tr><th>Function</th><td>Disable_KEYI()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Disable_KEYI();</code></td></tr>
</table>

 
<br/>

---

## 5 How to use

This library contains several functions to have full control of the hooks of the M1 interrupt. 
Allows you to save the system hook, replace it, disable it, and retrieve it. 
The way you work is up to you.

If you want to use the VBLANK interrupt you will have to use the TIMI hook. 
The KEYI hook will be executed whenever the M1 interrupt is triggered (like VBLANK), 
but it is only recommended when you have specific hardware that uses it (RS232C, MIDI, etc ...).

Remember that in the hook you do not have to connect an ISR type function. 
The hook is called by the system's ISR, so you will have interrupts disabled and all records saved (including alternates).

Nor do you have to worry about reading the state of register 0 of the VDP since the ISR does, 
but it is recommended that at the beginning and end of your function, you save and retrieve the pair of AF registers. 
For this I have included two definitions for when it is programmed in C: PUSH_AF and POP_AF.

When you leave your function you do not have to do anything either. 
The system ISR is responsible for retrieving the values from the registers and triggering the interrupts.

Although they can be used for **MSX-DOS** applications, you must be aware of an existing problem. 
Because the ISR when it executes the hook has the BIOS visible, you will have to control that your functions for the interruption are located above page 0. 
If your application is small, you should copy your function in the highest area of the RAM, 
although you can also substitute the ISR of the system for yours but it will require a different library than this one.

| ATTENTION! |
| ---------- | 
| SDCC provides some extended keywords to program an Interrupt Service Routine (ISR), but it is useless in our case as we use the system ISR (BIOS). <br/> Therefore we should **NOT ADD** `__interrupt` in our functions since it would add redundant code that could affect the correct execution of our program. |



### Example:

This example is illustrative only. 
In the `\examples` folder of the project sources you will find a complete application where you can check the behavior of the library.

```c
/* -----------------------------------------------------------------------------
   Interrupt Mode 1 TIMI hook Example
----------------------------------------------------------------------------- */
#include "../include/interruptM1.h"

void TestM1(void);

void my_TIMI1(void);
void my_TIMI2(void);

char conta;


void main(void)
{
    conta=0;
    
    Save_TIMI();   // save the old hook vector 

    Install_TIMI(my_TIMI1);  // Install the first function for the interrupt
    
    TestM1();
    
    Disable_TIMI(); // disable hook vector
    
    TestM1();
    
    Install_TIMI(my_TIMI2);   // Install the second function for the interrupt 
    
    TestM1();        

    Restore_TIMI(); // restore the old hook vector
}


void TestM1(void)
{
    char n=255;
    
    while(n>0)
    {
        HALT;
        n--;
    }
}



// C
void my_TIMI1(void)
{
  PUSH_AF;
    
  conta++;  
    
  POP_AF;
}



// assembler
void my_TIMI2(void) __naked
{
__asm
  push AF
      
  ld   HL,_conta
  inc  (HL)
    
  pop AF
  ret  
__endasm;
}

```


<br/>

---

## 6 References

### 6.1 English

- Z80 Family - [CPU User Manual](https://zany80.github.io/documentation/Z80/UserManual.html) (by ZiLOG) [(PDF)](http://map.grauw.nl/resources/cpu/z80.pdf)
- MSX2 Technical Handbook - [Chapter 2](https://github.com/Konamiman/MSX2-Technical-Handbook/blob/master/md/Chapter2.md#45-interrupt-usage) - 4.5 Interrupt usage
- The MSX Red Book - [4. ROM BIOS - KEYINT](https://github.com/gseidler/The-MSX-Red-Book/blob/master/the_msx_red_book.md#chapter_4)
- [Z80 Family Interrupt Structure](http://www.z80.info/1653.htm) (by Michael Moore)
- MSX Computer Magazine - Nº 51 Januari 1992 - [Interrupts](http://map.grauw.nl/articles/interrupts.php) - Page 6. (In English on MSX Assembly Page) 
- MSX Resource Center - Wiki - [System hooks](https://www.msx.org/wiki/System_hooks)



### 6.2 Spanish

- Karoshi MSX Community - [Interrupciones del MSX](http://karoshi.auic.es/karoshi.auic.es/index.php/topic%2c947.0.html) (por SapphiRe)
- MSX Inside 005 - [Interrupciones](https://www.youtube.com/watch?v=dJymuMfgw9I) (por SapphiRe)
- MSX Inside 006 - [Interrupciones VBLANK](https://www.youtube.com/watch?v=aUkHk_mjtOU) (por SapphiRe)
- MSX Inside 007 - [Interrupciones de Línea](https://www.youtube.com/watch?v=E8nTwqaxEAg) (por SapphiRe)

<br/>

---

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png) 
<br/>This document is licensed under a [Creative Commons Attribution-NonCommercial 4.0 International License](http://creativecommons.org/licenses/by-nc/4.0/).