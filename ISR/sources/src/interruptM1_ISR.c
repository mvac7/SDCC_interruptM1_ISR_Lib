/* =============================================================================
Z80 interrupt Mode 1 MSX SDCC Library (fR3eL Project)
Version: 1.2 (31/12/2024)
Author: mvac7/303bcn
Architecture: MSX
Format: C Object (SDCC .rel)
Programming language: C and Z80 assembler
Compiler: SDCC 4.4 or newer

fR3eL Project
https://github.com/mvac7/SDCC_MSX_fR3eL

Description:
Library with basic functions to control the Interrupt Service Routine (ISR) for 
Z80 Mode 1 interrupts on MSX system.  
  
History of versions:
- v1.2 (31/12/2024) Update to SDCC (4.1.12) Z80 calling conventions
- v1.1 ( 1/09/2021) More functions to control ISR and Hooks (TIMI/KEYI).
- v1.0 (16/11/2004) First version developed by Avelino Herrera.
                          http://msx.avelinoherrera.com/index_es.html#sdccmsxdos
============================================================================= */

#include "../include/interruptM1_ISR.h"


#define HINT     0x0038 //Z80 INT (RST $38)  · M1 Interrupt vector

#define HKEYI	 0xFD9A //Hook KEYI. Interrupt handler device other than the VDP. (RS-232C, MSX-Midi, etc) 
#define HTIMI	 0xFD9F //Hook TIMI. Interrupt handler VDP VBLANK


char OLD_ISR[3];


void ISR_empty(void);




/* =============================================================================
 Save_ISR

 Function : Save Old ISR vector
 Input    : -
 Output   : -
============================================================================= */
void Save_ISR(void) __naked
{
__asm
  di
  
  ; Save old ISR vector
  ld   A,(#HINT)
  ld   (#_OLD_ISR),A
  ld   HL,(#HINT+1)
  ld   (#_OLD_ISR+1),HL
  
  ei
  ret
__endasm;
}



/* =============================================================================
 Install_ISR

 Function : Set ISR vector  
 Input    : Function address
 Output   : -
============================================================================= */
void Install_ISR(void (*isr)(void)) __naked
{
isr;
__asm
  ; Set new ISR vector
  di
  ld   A,#0xC3       ;add a JP
  ld   (#HINT),A
  ld   (#HINT+1),HL
  ei
  
  ret
__endasm;
}



/* =============================================================================
 Restore_ISR

 Function : Restore old ISR vector 
 Input    : -
 Output   : -
============================================================================= */
void Restore_ISR(void) __naked
{
__asm
  di
    
  ld   A,(#_OLD_ISR)
  ld   (#HINT),A
  
  ld   HL,(#_OLD_ISR+1)
  ld   (#HINT+1),HL
  
  ei
  ret  
__endasm;
}



/* =============================================================================
 Disable_ISR

 Function : Disable ISR
 Input    : -
 Output   : -
============================================================================= */
void Disable_ISR(void)
{
   Install_ISR(ISR_empty);
}



/* =============================================================================
## Basic ISR for M1 interrupt of Z80

* Saves all Z80 registers on the stack.
* Calls the two hooks of the system: TIMI (VBLANK) and KEYI

Note: 
  You can optimize it, commenting on those records that you know you don't use 
  (such as alternate records) or removing the KEYI hook. 
============================================================================= */
void ISR_Basic(void) __naked
{  
__asm
  push   IY         
  push   IX
  push   HL         
  push   DE         
  push   BC         
  push   AF
           
  exx               
  ex     AF,AF      
  push   HL         
  push   DE         
  push   BC         
  push   AF
  
  
  call   HKEYI           ;Hook KEYI Not VDP Interrupt handler (RS232, MIDI, etc)
           
  in     A,(0x99)        ;read if VDP interrupt and Disable interrupt call to CPU  
  and    A          
  jp     P,exitISR       ;IF Not VDP Interrupt THEN exit

;is a VDP Interrupt
  ld     (0xF3E7),A      ;save VDP reg#0 in STATFL system variable
  
  call   HTIMI           ;Hook TIMI VDP Interrupt handler

;restore all Z80 registers and exit 
exitISR:
          
  pop    AF        
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
          
  ei               
  ret  
__endasm;
}



/*
	Minimum code to be executed by an ISR of an M1 interrupt, necessary for it to work correctly.
*/
void ISR_empty(void) __naked
{  
__asm
  push   AF
  in     A,(0x99)        ;read if VDP interrupt and Disable interrupt call to CPU
  ld     (0xF3E7),A      ;save VDP reg#0 in STATFL system variable
  pop    AF
  
  ei                     ;Enable Interrupts
  ret 
__endasm;
}