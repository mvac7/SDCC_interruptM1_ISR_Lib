/* =============================================================================
  Interrupt Mode 1 MSX SDCC Library
  Version: 1.1 (1 September 2021)
  Author: mvac7
  Architecture: MSX
  Format: C Object (SDCC .rel)
  Programming language: C and Z80 assembler

  Description:
    Functions to have full control over the Z80's Mode 1 interrupts.
    Allows you to save, replace, recover and disable the ISR.
    Includes the hooks used in the MSX system (TIMI and KEYI)    
  
History of versions:
- v1.1 (1 September 2021) More functions to control ISR and Hooks (TIMI/KEYI).
- v1.0 (16 November 2004) by Avelino Herrera
                          http://msx.avelinoherrera.com/index_es.html#sdccmsxdos
============================================================================= */

#include "../include/interruptM1_ISR.h"


#define HINT     0x0038 //Z80 INT (RST $38)  · M1 Interrupt vector

#define HKEYI	 0xFD9A //Hook KEYI. Interrupt handler device other than the VDP. (RS-232C, MSX-Midi, etc) 
#define HTIMI	 0xFD9F //Hook TIMI. Interrupt handler VDP VBLANK


char OLD_ISR[3];


void ISR_empty();



/* =============================================================================
 Save_ISR

 Function : Save Old ISR vector
 Input    : -
 Output   : -
============================================================================= */
void Save_ISR() __naked
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
  push IX
  ld   IX,#0
  add  IX,SP
   
  ; Set new ISR vector
  di
  ld   A,#0xC3       ;add a JP
  ld   (#HINT),A
  ld   L,4(IX)
  ld   H,5(IX)
  ld   (#HINT+1),HL
  ei
  
  pop IX
  ret
__endasm;
}





/* =============================================================================
 Restore_ISR

 Function : Restore old ISR vector 
 Input    : -
 Output   : -
============================================================================= */
void Restore_ISR() __naked
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
void Disable_ISR()
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
void ISR_Basic() __naked
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
  jp     P,exitISR       ;IF NOT (VDP INTERRUPT) THEN exit
  
  push   AF
  call   HTIMI           ;Hook TIMI VDP Interrupt handler
  pop    AF
  ld     (0xF3E7),A      ;save VDP reg#0 in STATFL system variable

  
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




void ISR_empty() __naked
{  
__asm
  push   AF
  in     A,(0x99)        ;read if VDP interrupt and Disable interrupt call to CPU
  pop    AF
  
  ei                     ;Enable Interrupts
  ret 
__endasm;
}