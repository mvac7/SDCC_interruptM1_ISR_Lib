/* =============================================================================
  test 1 ISR 
  Short: Test interrupt M1 ISR MSX SDCC Library
  Version: 1.0 (16/09/2021) 
  Author: mvac7
  Architecture: MSX
  Format: COM (MSX-DOS v1)
  Programming language: C and Assembler
    
History of versions:
 - v1.0 (16/09/2021)
============================================================================= */

#include "../include/newTypes.h"
#include "../include/msxBIOS.h"
#include "../include/msxDOS.h"
#include "../include/msxSystemVariables.h"

#include "../include/interruptM1_ISR.h"

#include "../include/VDP_TMS9918A.h"
#include "../include/VDP_PRINT.h"
#include "../include/keyboard.h"



// -----------------------------------------------------------------------------
#define RG8SAV 0xFFE7   //Mirror of VDP register 8
#define RG9SAV 0xFFE8   //Mirror of VDP register 9
#define CGTABL 0x0004	//(2B) Base address of the MSX character set in ROM


// ---------------------------------------------------- Declaration of functions
void System(char code);
void SCREEN0();
void SCREEN1();
char PEEK(uint address);

boolean GetVFrequency();

char GetSysVar(uint address);

void my_ISR();

void TestISR_M1();


// ------------------------------------------------------------------- constants
const char text01[] = "Test ISR (Interrupt M1 Lib)";
const char text02[] = "v1.0 (16/Sep/2021)";
const char text03[] = "Press a key to start.";

const char VFreqSTRINGS[2][5]={"60Hz","50Hz"};
const char msxSTRINGS[4][7]={"MSX   ","MSX2  ","MSX2+ ","TurboR"};



// ------------------------------------------------------------ global variables
uint frames;  

char MSXvers;
char VFreq;    
boolean is60Hz;  // 0=NTSC/60Hz, 1=PAL/50Hz


// ------------------------------------------------------------------- Functions


//
void main(void)
{
  char colorInk;
  char colorBG;
  char colorBDR;
  char scrcolumns;
  
  colorInk=PEEK(FORCLR);
  colorBG=PEEK(BAKCLR);
  colorBDR=PEEK(BDRCLR);
  scrcolumns=PEEK(LINLEN);
  
  MSXvers = GetSysVar(MSXVER);
  
  is60Hz = GetVFrequency();
  
  if(is60Hz) VFreq=50;
  else VFreq=60; 


  
  TestISR_M1();

  
  

//return to MSX-DOS ------------------------------------------------------------  
  //put the screen as it was.
  COLOR(colorInk,colorBG,colorBDR);
//  WIDTH(scrcolumns);

  if(scrcolumns<33) SCREEN1();
  else SCREEN0();
  //
  
  //PRINT("End of the test...");
    
  System(_TERM0); 
//--------------------------------------------------------------------- end EXIT
}




void TestISR_M1()
{
  char keyPressed;
  
  SCREEN1();
  
  frames = 0;
  
  //CopyToVRAM((uint) TILESET,BASE7,128*8);
  
  VLOCATE(0,0);
  VPRINT(text01);
  VLOCATE(0,1);
  VPRINT(text02);
  VLOCATE(0,2); 
  VPRINT("MSX version: ");
  VPRINT(msxSTRINGS[MSXvers]);
  VLOCATE(0,3);
  VPRINT("Video Freq.: ");
  VPRINT(VFreqSTRINGS[is60Hz]);
  
  VLOCATE(0,5);  
  VPRINT(text03);
  INKEY();
  
  VLOCATE(0,5);  
  VPRINT("                     ");  //clear 'press any key'

  VLOCATE(0,6);  
  VPRINT("VBLANK: ");
  
  VLOCATE(0,8);  
  VPRINT("Seconds:");  
  
  VLOCATE(0,12);  
  VPRINT("Press ESC for Exit");  

  Save_ISR();
  Install_ISR(my_ISR);
  
  
  while(1)
  {
    HALT;
    VLOCATE(8,6);
    VPrintNumber(frames,5);
    
    VLOCATE(8,8);
    VPrintNumber(frames/VFreq,5);
        
    keyPressed = GetKeyMatrix(7);      
    if (!(keyPressed&Bit2)) break;   //ESC Key
  }

  Restore_ISR();
}




void System(char code) __naked
{
code;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   C,4(IX)
  call 0x0005  ;MSX-DOS entry
  
  pop IX
  ret
__endasm; 
}


void SCREEN0() __naked
{
__asm
  push IX
  
  ld   A,(#LINLEN)
  ld   (#LINL40),A   ;copy columns seting with WIDTH to LINL40 system var
   
  ld   IX,#INITXT
  ld   IY,(EXPTBL-1)
  call CALSLT
  ei
    
  pop  IX
  ret
__endasm;
}



void SCREEN1() __naked
{
__asm     
  push IX
  
  ld   A,(#LINLEN)   ;get a last value set with WIDTH function 
  ld   (#LINL32),A   ;set system variable
   
  ld   IX,#INIT32
  ld   IY,(EXPTBL-1)
  call CALSLT
  ei
    
  pop  IX
  ret
__endasm;
}



char PEEK(uint address) __naked
{
address;
__asm
  push IX
  ld   IX,#0
  add  IX,SP

  ld   L,4(IX)
  ld   H,5(IX)
  ld   L,(HL)
  
  pop  IX
  ret
__endasm;
}


/* -----------------------------------------------------------------------------
   GetVFrequency
  
   Get Video Frequency (for MSX-DOS)
   Input: ---
   Output: 0=NTSC/60Hz, 1=PAL/50Hz 
----------------------------------------------------------------------------- */  
boolean GetVFrequency() __naked
{
__asm
  push IX
  
  ld   HL,#MSXVER   ;MSXVER=002D MSX version number (0=MSX,1=MSX2,2=MSX2+,3=TurboR)
  ld   A,(#EXPTBL)  ;EXPTBL=FCC1 main BIOS-ROM slot address
  call RDSLT        ;Reads the value of an address in another slot
  or   A
  jr   NZ,readHZfromVDP  //IF A!=0
  
;in the MSX1, the information about the video frequency is in a system variable
  ld   HL,#MSXROM1  ;MSXROM1=002B
  ld   A,(#EXPTBL)
  call RDSLT    
  bit  7,A          ;Default interrupt frequency (0=NTSC/60Hz, 1=PAL/50Hz)
  or   A
  jr   Z,_isNTSC
  jr   _isPAL   

;If it is run on an MSX2 or higher, we can check the video frequency in the VDP registers.
readHZfromVDP:
;look at the system variable that contains the VDP registry value
  ld   A,(RG9SAV)    ;RG9SAV=FFE8 Mirror of VDP register 9
  bit  1,A           ;(0=NTSC/60Hz, 1=PAL/50Hz)  
  or   A  
  jr   Z,_isNTSC
  
_isPAL:
  ld   L,#1
  pop  IX
  ret
  
_isNTSC:  
  ld   L,#0
  pop  IX  
  ret  
__endasm;
}



char GetSysVar(uint address) __naked
{
address;
__asm
  push IX
  ld   IX,#0
  add  IX,SP

  ld   L,4(IX)
  ld   H,5(IX)
  
  ld   A,(#EXPTBL)
  call RDSLT
  
  ld   L,A  
  
  pop  IX
  ret
__endasm;
}



void my_ISR() __naked
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

  ld	IY,#_frames
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













