/*  =============================================================================
Test interruptM1_Hooks Library
Version: 1.3 (13/12/2024) 
Author: mvac7/303bcn
Architecture: MSX
Format: MSX 8K ROM
Programming language: C and Z80 assembler
Compiler: SDCC 4.4 or newer

History of versions:
- v1.3 (13/12/2024) update to SDCC (4.1.12) Z80 calling conventions
- v1.2 ( 4/10/2021) 
- v1.1 ( 4/07/2021)
- v1.0 (10/08/2019) 
============================================================================= */

#include "../include/newTypes.h"
#include "../include/msxBIOS.h"
#include "../include/msxSystemVariables.h"

#include "../include/interruptM1_Hooks.h"

#include "../include/textmode_MSX.h"
#include "../include/keyboard_MSX.h"




#ifndef _VFREQ
#define _VFREQ
  typedef enum {NTSC = 0, PAL = 1} VFreq;
#endif




// ---------------------------------------------------- Declaration of functions
char PEEK(uint address);
void WAIT(uint cicles);

VFreq GetVFrequency(void);

void TestHooks(void);

void my_KEYI(void);

void my_TIMI1(void);
void my_TIMI2(void);


// ------------------------------------------------------------------- constants
const char AppAbout[] = "Test interruptM1_Hooks Lib";
const char txtPressKey[] = "Press any key";

const char VFreqSTRINGS[2][12]={"60Hz (NTSC)","50Hz (PAL) "};
const char msxSTRINGS[4][7]={"MSX   ","MSX2  ","MSX2+ ","TurboR"};


// ------------------------------------------------------------ global variables
uint contaKEYI;
uint contaTIMI;  

char MSXvers;
char VfreqHz;    



// ------------------------------------------------------------------- Functions


//
void main(void)
{
	VFreq videoType;  // 0=NTSC/60Hz, 1=PAL/50Hz
	
	MSXvers = PEEK(MSXVER);

	videoType = GetVFrequency();

	if(videoType==PAL) VfreqHz=50;
	else VfreqHz=60;  

	SCREEN1();
	WIDTH(32);

	LOCATE(0,0);
	PRINT(AppAbout);

	LOCATE(0,2);
	PRINT("MSX version: ");
	PRINT(msxSTRINGS[MSXvers]);

	LOCATE(0,3);
	PRINT("Video Freq.: ");
	PRINT(VFreqSTRINGS[videoType]);


	LOCATE(0,5);  
	PRINT(txtPressKey);
	INKEY();

	LOCATE(0,5);  
	PRINT("             ");  //clear 'press any key'


	TestHooks();
  

//Return to MSXDOS -------------------------------------------------------------
	LOCATE(0,23);  
	PRINT(txtPressKey);
	INKEY();
}



char PEEK(uint address) __naked
{
address;
__asm
	ld   A,(HL)  
	ret
__endasm;
}



// Generates a pause in the execution of n interruptions.
// PAL: 50=1second. ; NTSC: 60=1second. 
void WAIT(uint cicles)
{
	uint i;
	for(i=0;i<cicles;i++) HALT;
}




 
/* -----------------------------------------------------------------------------
   GetVFrequency
  
   Get Video Frequency 
   for ROM (except 48K) or BASIC environments (page 0 = BIOS)
   Input: ---
   Output: NTSC=0 (60Hz), PAL=1 (50Hz)
----------------------------------------------------------------------------- */  
VFreq GetVFrequency(void) __naked
{
__asm
; -----------------------------------
  ld   A,(#0x002D)  ;MSXVER=002D MSX version number (0=MSX,1=MSX2,2=MSX2+,3=TurboR)
  or   A
  jr   NZ,readHZfromVDP  //IF A!=0
  
;in the MSX1, the information about the video frequency is in a system variable
  ld   A,(#0x002B)  ;MSXROM1
  bit  7,A          ;Default interrupt frequency (0=60Hz, 1=50Hz)
  jr   Z,VFreq_isNTSC
  jr   VFreq_isPAL   

;If it is run on an MSX2 or higher, we can check the video frequency in the VDP registers
readHZfromVDP:
;look at the system variable that contains the VDP registry value
  ld   A,(0xFFE8)    ;(RG9SAV) Mirror of VDP register 9
  bit  1,A           ;(0=60Hz, 1=50Hz)  
  jr   Z,VFreq_isNTSC
  
VFreq_isPAL:
  ld   A,#1
  ret
  
VFreq_isNTSC:  
  xor  A
  ret  
__endasm;
}



void TestHooks(void)
{
	char line[]="-------------------------------";
	char cursorLine = 11;
	uint contaTest = 0;

	contaKEYI = 0;
	contaTIMI = 0;       

	LOCATE(0,5);  
	PRINT("Hook TIMI:    ");

	LOCATE(0,7);  
	PRINT("Hook KEYI:");

	LOCATE(0,9);  
	PRINT("Time (sec):");

	//LOCATE(0,12);  
	//PRINT("Press ESC for Exit");  

	//save the old hooks vectors
	//Save_ISR();
	Save_TIMI();   
	Save_KEYI(); 

	LOCATE(0,cursorLine++);
	PRINT("Log:");

	LOCATE(0,cursorLine++);
	PRINT(line);

	Install_TIMI(my_TIMI1);  
	LOCATE(0,cursorLine++);
	PRINT(">Install_TIMI(my_TIMI1) C");

	while(1)
	{
		HALT;
		contaTest++;
		
		LOCATE(12,5);
		PrintFNumber(contaTIMI,32,5);    

		LOCATE(12,7);
		PrintFNumber(contaKEYI,32,5);

		LOCATE(12,9);
		PrintFNumber(contaTest/VfreqHz,32,5);

		if (contaTest==10*VfreqHz)
		{ 
			Disable_TIMI();
			LOCATE(0,cursorLine++);
			PRINT(">Disable_TIMI()");
		}else if (contaTest==20*VfreqHz)
		{
			Install_KEYI(my_KEYI);
			LOCATE(0,cursorLine++);
			PRINT(">Install_KEYI(my_KEYI) ");
		}else if (contaTest==30*VfreqHz)
		{
			Install_TIMI(my_TIMI2);  
			LOCATE(0,cursorLine++);
			PRINT(">Install_TIMI(my_TIMI2) Asm");
		}else if (contaTest==40*VfreqHz)
		{
			Disable_KEYI();
			LOCATE(0,cursorLine++);
			PRINT(">Disable_KEYI()        ");
		}else if (contaTest==50*VfreqHz)
		{ 
			Disable_TIMI();
			LOCATE(0,cursorLine++);
			PRINT(">Disable_TIMI()");
		}else if (contaTest==60*VfreqHz)
		{
			break;  //exit test
		}
			
		//keyPressed = GetKeyMatrix(7);      
		//if (!(keyPressed&Bit2)) break;   //ESC Key
	
	}

	LOCATE(0,cursorLine++);
	PRINT(line);

	LOCATE(0,cursorLine++);
	PRINT("End TEST");

	//It is not needed in ROM because it reboots the system at the end
	//Restore_ISR();
	Restore_TIMI();  
	Restore_KEYI();

}



void my_KEYI(void) 
{ 
	contaKEYI++;
}



void my_TIMI1(void)  // __interrupt 
{
	PUSH_AF;

	contaTIMI++;

	POP_AF;	
}



void my_TIMI2(void) __naked
{
__asm

	push AF

	ld	iy,#_contaTIMI
	inc	0 (iy)
	jr	NZ,TIMI_exit
	inc	1 (iy)

TIMI_exit:
	pop  AF

	ret
__endasm;		
}















