/* =============================================================================
Z80 interrupt M1 Hooks MSX SDCC Library (fR3eL Project)
Version: 1.2 (31/12/2024)
Author: mvac7/303bcn
Architecture: MSX
Format: C Object (SDCC .rel)
Programming language: C and Z80 assembler

fR3eL Project
https://github.com/mvac7/SDCC_MSX_fR3eL

Description:
Library with basic functions to control the MSX system hooks called from the 
Z80 Mode 1 interrupt ISR (Interrupt Service Routine).    

History of versions:
- v1.2 (31/12/2024) Update to SDCC (4.1.12) Z80 calling conventions
- v1.1 ( 4/07/2021) More functions to control the two Hooks (added KEYI)
- v1.0 ( 4/07/2011) First version. Published in Avelino Herrera's WEB 
					 http://msx.avelinoherrera.com/index_es.html#sdccmsx
============================================================================= */

#include "../include/interruptM1_Hooks.h"


#define HINT     0x0038 //Z80 INT (RST $38)  · IM 1 interrupts entry poin

#define HKEYI	 0xFD9A //Hook KEYI. Interrupt handler device other than the VDP. (RS-232C, MSX-Midi, etc) 
#define HTIMI	 0xFD9F //Hook TIMI. Interrupt handler VDP VBLANK


char OLD_TIMI[5];
char OLD_HKEYI[5];




/* =============================================================================
 Save_TIMI

 Function : Save TIME hook vector
 Input    : -
 Output   : -
============================================================================= */
void Save_TIMI(void) __naked
{
__asm
	di
  
	ld	 HL,#HTIMI
	ld	 DE,#_OLD_TIMI
	ld	 BC,#5
	ldir
  
	ei
	ret
__endasm;
}



/* =============================================================================
 Install_TIMI

 Function : Set new TIMI hook vector  
 Input    : Function address
 Output   : -
============================================================================= */
void Install_TIMI(void (*func)(void)) __naked
{
func;	//HL
__asm
  
  ; Set new TIMI vector
  di
  ld   A,#0xC3       ;add a JP
  ld   (#HTIMI),A
  ld   (#HTIMI+1),HL
  ei
  
  ret
__endasm;
}



/* =============================================================================
 Restore_TIMI

 Function : Restore old TIMI hook vector 
 Input    : -
 Output   : -
============================================================================= */
void Restore_TIMI(void) __naked
{
__asm
	di
  
    ld	 HL,#_OLD_TIMI
	ld	 DE,#HTIMI
	ld	 BC,#5
	ldir
  
	ei
	ret
__endasm;
}




/* =============================================================================
 Disable_TIMI

 Function : Disable the TIMI hook (Add a ret on the hook).
 Input    : -
 Output   : -
============================================================================= */
void Disable_TIMI(void) __naked
{
__asm
    di
	ld  A,#0xC9     ; ret
    ld  (#HTIMI),A
    ei    
    ret
__endasm;
}



/* =============================================================================
 Save_KEYI

 Function : Save KEYI hook vector
 Input    : -
 Output   : -
============================================================================= */
void Save_KEYI(void) __naked
{
__asm
	di
  
	ld	 HL,#HKEYI
	ld	 DE,#_OLD_HKEYI
	ld	 BC,#5
	ldir
  
	ei
	ret
__endasm;
}



/* =============================================================================
 Install_KEYI

 Function : Set new KEYI hook vector  
 Input    : Function address
 Output   : -
============================================================================= */
void Install_KEYI(void (*func)(void)) __naked
{
func; //HL
__asm
  
	di
	ld   A,#0xC3       ;add a JP
	ld   (#HKEYI),A
	ld   (#HKEYI+1),HL
	ei
	
	ret
__endasm;
}



/* =============================================================================
 Restore_KEYI

 Function : Restore old KEYI hook vector 
 Input    : -
 Output   : -
============================================================================= */
void Restore_KEYI(void) __naked
{
__asm
	di
  
    ld	 HL,#_OLD_HKEYI
	ld	 DE,#HKEYI
	ld	 BC,#5
	ldir
  
	ei
	ret
__endasm;
}




/* =============================================================================
 Disable_KEYI

 Function : Disable the KEYI hook (Add a ret on the hook).
 Input    : -
 Output   : -
============================================================================= */
void Disable_KEYI(void) __naked
{
__asm
    di
	ld  A,#0xC9     ; ret
    ld  (#HKEYI),A
    ei    
    ret
__endasm;
}