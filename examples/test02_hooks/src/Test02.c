/* =============================================================================
  test 2 ISR_Basic + Hooks 
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
#include "../include/interruptM1_Hooks.h"

#include "../include/VDP_TMS9918A.h"
#include "../include/VDP_PRINT.h"
#include "../include/keyboard.h"





// ---------------------------------------------------- Declaration of functions
void System(char code);

void SCREEN0();
void SCREEN1();
char PEEK(uint address);

char GetSysVar(uint address);
boolean GetVFrequency();

void TestHooks();

void my_KEYI();
void my_TIMI1();
//void my_TIMI_ASM(void);


// ------------------------------------------------------------------- constants

const char text01[] = "Test ISR_Basic+Hooks v1.0";
const char text02[] = "InterruptM1_ISR Lib";
const char text03[] = "Press any key";

const char VFreqSTRINGS[2][5]={"60Hz","50Hz"};
const char msxSTRINGS[4][7]={"MSX   ","MSX2  ","MSX2+ ","TurboR"};



// ------------------------------------------------------------ global variables
uint contaKEYI;
uint contaTIMI;

char MSXvers;
char VFreq;    
boolean is60Hz;  // 0=NTSC/60Hz, 1=PAL/50Hz
  


// Functions -------------------------------------------------------------------


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
  
  SCREEN1();
//  WIDTH(32);
  
  
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
  VPRINT("             ");  //clear 'press any key'
   
  

  TestHooks();
  
  

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
  
  ld   HL,#0x002D   ;MSXVER=002D MSX version number (0=MSX,1=MSX2,2=MSX2+,3=TurboR)
  ld   A,(#EXPTBL)  ;EXPTBL=FCC1 main BIOS-ROM slot address
  call RDSLT        ;Reads the value of an address in another slot
  or   A
  jr   NZ,readHZfromVDP  //IF A!=0
  
;in the MSX1, the information about the video frequency is in a system variable
  ld   HL,#0x002B  ;MSXROM1=002B
  ld   A,(#EXPTBL)
  call RDSLT    
  bit  7,A          ;Default interrupt frequency (0=NTSC/60Hz, 1=PAL/50Hz)
  or   A
  jr   Z,_isNTSC
  jr   _isPAL   

;If it is run on an MSX2 or higher, we can check the video frequency in the VDP registers.
readHZfromVDP:
;look at the system variable that contains the VDP registry value
  ld   A,(0xFFE8)    ;RG9SAV=FFE8 Mirror of VDP register 9
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



void TestHooks()
{
  char line[]="-------------------------------";
  char cursorLine = 11;
  uint contaTest = 0;
  
  contaKEYI = 0;
  contaTIMI = 0;
  
  //save the old hooks vectors
  Save_ISR();
  Save_TIMI();   
  Save_KEYI(); 
         

  VLOCATE(0,5);  
  VPRINT("Hook TIMI:    ");
  
  VLOCATE(0,7);  
  VPRINT("Hook KEYI:");
  
  VLOCATE(0,9);  
  VPRINT("Time (sec):");
  
  VLOCATE(0,cursorLine++);
  VPRINT("Log:");
  
  VLOCATE(0,cursorLine++);
  VPRINT(line);
  
  Install_TIMI(my_TIMI1);  
  VLOCATE(0,cursorLine++);
  VPRINT(">Install_TIMI(my_TIMI1)");
  
  Install_ISR(ISR_Basic); /* When you switch to a new ISR, the hooks should be 
                           off or include jumps to your routines. The default 
                           routines are designed to work with the ISR system 
                           and could potentially crash your program. */
  VLOCATE(0,cursorLine++);
  VPRINT(">Install_ISR(ISR_Basic)");
  
  while(1)
  {
    HALT;
    contaTest++;
    
    VLOCATE(12,5);
    VPrintNumber(contaTIMI,5);    
    
    VLOCATE(12,7);
    VPrintNumber(contaKEYI,5);
    
    VLOCATE(12,9);
    VPrintNumber(contaTest/VFreq,5);
    
    if (contaTest==10*VFreq)
    { 
        Disable_TIMI();
        VLOCATE(0,cursorLine++);
        VPRINT(">Disable_TIMI()");
    }
    
    if (contaTest==20*VFreq)
    {
        Install_KEYI(my_KEYI);
        VLOCATE(0,cursorLine++);
        VPRINT(">Install_KEYI(my_KEYI) ");
    }
    
    if (contaTest==30*VFreq)
    {
        Install_TIMI(my_TIMI1);  
        VLOCATE(0,cursorLine++);
        VPRINT(">Install_TIMI(my_TIMI1)");
    }
    
    if (contaTest==40*VFreq)
    {
        Disable_KEYI();
        VLOCATE(0,cursorLine++);
        VPRINT(">Disable_KEYI()        ");
    }
    
    if (contaTest==50*VFreq)
    {
        break;  //exit test
    }
        
    //keyPressed = GetKeyMatrix(7);      
    //if (!(keyPressed&Bit2)) break;   //ESC Key
  }
  
  Disable_TIMI();
  
  VLOCATE(0,cursorLine++);
  VPRINT(line);
  
  VLOCATE(0,cursorLine++);
  VPRINT("End TEST");
  //PRINT("TEST result: ");
  //if (contaKEYI==1002 && contaTIMI==1503) PRINT("OK");
  //else PRINT("ERROR");
  
  VLOCATE(0,23);  
  VPRINT(text03);
  INKEY();
  
  //It is not needed in ROM because it reboots the system at the end
  Restore_ISR();
  Restore_TIMI();  
  Restore_KEYI();

}



void my_KEYI() 
{ 
  contaKEYI++;
}



void my_TIMI1()
{
  contaTIMI++;
}












