# How to use the Interrupt Mode 1 ISR MSX SDCC Library

---

## Index

- [1 Description](#1-Description)
- [2 Requirements](#2-Requirements)
- [3 Definitions](#3-Definitions)
- [4 Functions](#4-Functions)
- [5 How to use this library](#5-How-to-use-this-library)
   - [5.1 If you use an ISR](#51-If-you-use-an-ISR)
   - [5.2 If you use the hooks](#52-If-you-use-the-hooks)
- [6 References](#6-References)
   - [6.1 English](#61-English)
   - [6.2 Spanish](#62-Spanish)

<br/>

---

## 1 Description

Library with basic functions to control the Interrupt Service Routine (ISR) for Z80 Mode 1 interrupts on MSX system.
Allows you to save, replace, disable and recover the ISR under the MSX-DOS environment.

You can use it in combination with the [Interrupt M1 Hooks library](../Hooks), to work with the hooks defined in MSX system, but with an optimized ISR.

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

Name         | Description
------------ | -------------
`DisableI`   | Disable interrupts. <br/> Add `DI` code in Z80 assembler.
`EnableI`    | Enable interrupts. <br/> Add `EI` code in Z80 assembler.
`HALT`       | Suspends all actions until the next interrupt. <br/> Add `HALT` code in Z80 assembler.


<br/>

---


## 4 Functions

<table>
<tr><th colspan=2 align="left">Save_ISR</th></tr>
<tr><td colspan="2">Save Old ISR vector</td></tr>
<tr><th>Function</th><td>Save_ISR()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Save_ISR();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Install_ISR</th></tr>
<tr><td colspan="2">Set new ISR vector</td></tr>
<tr><th>Function</th><td>Install_ISR(isr)</td></tr>
<tr><th>Input</th><td>[isr] ISR Function</td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Install_ISR(my_ISR);</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Restore_ISR</th></tr>
<tr><td colspan="2">Restore old ISR vector/td></tr>
<tr><th>Function</th><td>Restore_ISR()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Restore_ISR();</code></td></tr>
</table>


<table>
<tr><th colspan=2 align="left">Disable_ISR</th></tr>
<tr><td colspan="2">Disable ISR (Install an ISR with the minimum code so as not to block the Z80.)</td></tr>
<tr><th>Function</th><td>Disable_ISR()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Disable_ISR();</code></td></tr>
</table>



<table>
<tr><th colspan=2 align="left">ISR_Basic</th></tr>
<tr><td colspan="2">Basic ISR for M1 interrupt of Z80<br/>
* Saves all Z80 registers on the stack.
* Calls the two hooks of the system: TIMI (VBLANK) and KEYI</td></tr>
<tr><th>Function</th><td>ISR_Basic()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th>
<td><code>Install_ISR(ISR_Basic);</code></td></tr>
</table>




<br/>

---

## 5 How to use this library

This library aims to simplify the use of interrupt in your programs for MSX computers, but depending on how you want your program to behave, it will require you to follow a series of steps that are described in the following use cases.

In the event that you do not know how interruptions work in MSX, I recommend that you read some articles that are referenced at the end of this document.

This library contains various functions to have full control of the Interrupt Service Routine (ISR), of the Z80's Mode 1 interrupt. 
It will allow you to save the original system links, replace them, disable them and recover them.

It is designed for the development of MSX-DOS applications, although it is possible to use it in the programming of 48K ROMs where the BIOS is not used or where interrupts are disabled before accessing the BIOS.

| Note: |
| :---  | 
| If your application does not need to return to DOS, you will not need to save and retrieve the ISR and system hooks. |



### 5.1 If you use an ISR

The MSX system uses an Interrupt Service Routine (ISR) that manages DOS and BASIC functionalities.
This produces two problems:
- It consumes some CPU cycles (Maybe you need all the power of MSX in your program).
- The DOS ISR also executes the BIOS ISR (KEYINT), from which hooks are called.
At that time, the BIOS will be visible to the CPU. If your function is on page 0, the Z80 will lose the thread of execution and will throw a fatal error.

You have two options:
- create your own ISR and replace the system one.
- use `ISR_Basic` included in library and install your function in one of the system hooks (requires the interruptM1_Hooks library).

If your application has to go back to DOS, before assigning a new ISR, you must save the link of the system ISR, executing the `Save_ISR ()` function and before exiting, you must restore the system ISR with the `Restore_ISR ( ) `.


#### Example:

This example is illustrative only. 
In the `examples\test01_isr\` folder of the project sources you will find a complete application where you can check the behavior of the library.


```c
/* -----------------------------------------------------------------------------
   Mode 1 ISR example
----------------------------------------------------------------------------- */
#include "../include/interruptM1_ISR.h"

void my_ISR(void);

usigned int conta;


void main(void)
{
    char n=255;
    
    conta=0;
    
    Save_ISR();
    
    Install_ISR(my_ISR);
    
    while(n>0)
    {
        HALT;
        n--;
    }
    
    Restore_ISR();
    
__asm  
  ld   C,#0x00	;Program terminate
  call 0x0005   ;MSX-DOS entry  
__endasm;      
}


void my_ISR(void) __naked
{  
__asm
  push   IY     ;<--- save all the Z80 records although it is only necessary to save those that are used in the ISR    
  push   IX     ;     in this case, it would be vast to save only the pairs of registers AF and IY
  push   HL         
  push   DE         
  push   BC         
  push   AF
           
  exx            ;now we go for the alternative registers  
  ex     AF,AF      
  push   HL         
  push   DE         
  push   BC         
  push   AF      
           
  in     A,(0x99)       ;read if VDP interrupt and Disable interrupt call to CPU  
  and    A          
  jp     P,exitISR      ;IF NOT (VDP INTERRUPT) THEN exit
  
  ld     (0xF3E7),A     ;save VDP reg#0 in STATFL system variable

  ld	IY,#_conta
  inc	0 (IY)
  jr	NZ,exitISR
  inc	1 (IY)
  
exitISR:
          
  pop    AF      ;<--- recovers all Z80 records       
  pop    BC        
  pop    DE        
  pop    HL        
  ex     AF,AF     
  exx
                
  pop    AF        
  pop    BC        
  pop    DE        
  pop    HL
  pop    IX        
  pop    IY
          
  ei            ;<--- IMPORTANT! Before exiting, enable interrupts.  
  ret  
__endasm;
}

```



### 5.2 If you use the hooks

If you want to use the hooks defined in the MSX system (TIMI and KEYI) you have to consider what was explained in the previous point about the change of page 0 (when executing the ISR KEYINT of the BIOS).

So as not to complicate, we recommend changing the system ISR to `ISR_Basic` included in this library. 
This interrupt routine minimizes what is necessary to execute the hooks.
Your function will not need anything more than to do a `ret` at the end, since the `ISR_Basic` will take care of protecting all the Z80 registers and what is necessary for everything to work well.

To work with hooks, you will need to include the interruptM1_Hooks library to your project.

| WARNING! |
| -------- | 
| Before switching to a new ISR, it is recommended to disable the hooks (use `Disable_ISR()` and `Disable_KEYI()`), because system routines make changes to memory pages and could crash the system. <br/> Disabling interrupts is not a good solution, because many functions turn interrupts on. |

| ATTENTION! |
| ---------- | 
| SDCC provides some extended keywords to program an Interrupt Service Routine (ISR), but it is useless in our case as we use the system ISR (BIOS). <br/> Therefore we should **NOT ADD** `__interrupt` in our functions since it would add redundant code that could affect the correct execution of our program. |



#### Example:

This example is illustrative only. 
In the `examples\test02_hooks\` folder of the project sources you will find a complete application where you can check the behavior of the library.

```c
/* -----------------------------------------------------------------------------
   Interrupts Mode 1 TIMI hook Example
----------------------------------------------------------------------------- */

#include "../include/interruptM1_ISR.h"
#include "../include/interruptM1_Hooks.h"


void TestM1(void);

void my_TIMI1(void);
void my_TIMI2(void);


char conta;

void main(void)
{
    conta=0;

    //Save the ISRs and hooks of the system so that when the program leave, can retrieve them.    
    Save_ISR();
    Save_TIMI();
    Save_KEYI();  
    
    //Disabled the hooks, so that when assigning the ISR_Basic, the system routines are not executed.
    Disable_TIMI();
    Disable_KEYI();
    
    Install_ISR(ISR_Basic);  //Install the ISR_Basic 

    Install_TIMI(my_TIMI1);  //Install the first function for the interrupt    
    TestM1();
    
    Disable_TIMI(); //disable hook vector    
    TestM1();
    
    Install_TIMI(my_TIMI2);   //Install the second function for the interrupt    
    TestM1();        

    //restore hooks and system ISR.
    Restore_TIMI(); 
    Restore_KEYI();
    Restore_ISR();

__asm  
  ld   C,#0x00	;Program terminate
  call 0x0005   ;MSX-DOS entry  
__endasm;
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
    conta++;    
}



// assembler
void my_TIMI2(void) __naked
{
__asm
  ld   HL,_conta
  inc  (HL)
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
- VBLANK - [Vertical Blank interrupt](https://en.wikipedia.org/wiki/Vertical_blank_interrupt) (Wikipedia)


### 6.2 Spanish

- Karoshi MSX Community - [Interrupciones del MSX](http://karoshi.auic.es/karoshi.auic.es/index.php/topic%2c947.0.html) (por SapphiRe)
- MSX Inside 005 - [Interrupciones](https://www.youtube.com/watch?v=dJymuMfgw9I) (por SapphiRe)
- MSX Inside 006 - [Interrupciones VBLANK](https://www.youtube.com/watch?v=aUkHk_mjtOU) (por SapphiRe)
- MSX Inside 007 - [Interrupciones de Línea](https://www.youtube.com/watch?v=E8nTwqaxEAg) (por SapphiRe)

<br/>

---

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png) 
<br/>This document is licensed under a [Creative Commons Attribution-NonCommercial 4.0 International License](http://creativecommons.org/licenses/by-nc/4.0/).