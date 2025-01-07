/* =============================================================================
Test interruptM1_ISR Library
Version: 1.1 (26/12/2024)
Author: mvac7/303bcn
Architecture: MSX
Format: COM (MSX-DOS v1)
Programming language: C and Assembler
Compiler: SDCC 4.4 or newer
    
History of versions:
- v1.1 (26/12/2024) update to SDCC (4.1.12) Z80 calling conventions
- v1.0 (16/09/2021)
============================================================================= */

#include "../include/newTypes.h"
#include "../include/msxBIOS.h"
#include "../include/msxDOS.h"
#include "../include/msxSystemVariables.h"

#include "../include/interruptM1_ISR.h"

#include "../include/keyboard_MSX.h"




// ----------------------------------------------------------------- definitions
//VDP Ports
#define VDPVRAM   0x98  //VRAM Data (Read/Write)
#define VDPSTATUS 0x99  //VDP Status Registers

#define G1_MAP	0x1800 // Name Table
#define G1_PAT	0x0000 // Pattern Table
#define G1_COL	0x2000 // Color Table


#ifndef _VFREQ
#define _VFREQ
	typedef enum {NTSC = 0, PAL = 1} VFreq;
#endif



// ---------------------------------------------------- Declaration of functions
char PEEK(uint address);

void WIDTH(char columns);
void SCREEN0(void);
void SCREEN1(void);

void SetVRAMtoWRITE(unsigned int vaddr);
void FastVPOKE(char value);

void LOCATE(char column, char line);
void PRINT(char* text);
void PrintNumber(unsigned int value);

VFreq GetVFrequency(void);

char GetSysVar(uint address);

void my_ISR(void);

void TestISR_M1(void);




// ------------------------------------------------------------------- constants
const char text00_AppAbout[] = "Test InterruptM1_ISR Lib";
const char text03_Presskey[] = "Press any key";

const char VFreqSTRINGS[2][12]={"60Hz (NTSC)","50Hz (PAL) "};
const char msxSTRINGS[4][7]={"MSX   ","MSX2  ","MSX2+ ","TurboR"};



// ------------------------------------------------------------ global variables
uint frames;  

char MSXvers;
char VfreqHz;  

char PrintNumber_Digits;



// ------------------------------------------------------------------- Functions


//
void main(void)
{
	VFreq videoType;  // 0=NTSC/60Hz, 1=PAL/50Hz
	
	char scrcolumns;

	scrcolumns=PEEK(LINLEN);

	MSXvers = GetSysVar(MSXVER);

	videoType = GetVFrequency();

	if(videoType==PAL) VfreqHz=50;
	else VfreqHz=60;

	WIDTH(32);
	SCREEN1();
	
	LOCATE(0,0);
	PRINT(text00_AppAbout);

	LOCATE(0,2);
	PRINT("MSX version: ");
	PRINT(msxSTRINGS[MSXvers]);
	
	LOCATE(0,3);
	PRINT("Video Freq.: ");
	PRINT(VFreqSTRINGS[videoType]);
	

	LOCATE(0,5);
	PRINT(text03_Presskey);
	INKEY();
	
	LOCATE(0,5);
	PRINT("                     ");  //clear 'press any key'

	TestISR_M1();

  
  

//return to MSX-DOS ------------------------------------------------------------  
	//put the screen as it was.
    WIDTH(scrcolumns);

	if(scrcolumns<33) SCREEN1();
	else SCREEN0();
	//

	//PRINT("End of the test...");
//--------------------------------------------------------------------- end EXIT
}




char PEEK(uint address) __naked
{
address;
__asm
	ld   A,(HL)
	ret
__endasm;
}



/* =============================================================================
 WIDTH
 
 Description: 
			Specifies the number of characters per line in text mode.
 Input:		1 to 40 in T1 40 columns mode
			41 to 80 in T2 80 columns mode (only MSX with V9938 and a BIOS that 
                                           supports this mode)
			1 to 32 in G1 mode
 Output:	-  
============================================================================= */
void WIDTH(char columns) __naked
{
columns;
__asm

  ld   (#LINLEN),A

  ret
__endasm;  
}



/* =============================================================================
 SCREEN0
 
 Description: 
           Initialice TEXT 1 (40 columns) or TEXT 2 (80 columns) screen mode.
		   
           Note: 
             To set the T2 mode, you must first set 80 columns with the WIDTH
			 function (only MSX computers with V9938 and BIOS that supports
			 this mode).
 Input:    -
 Output:   -
============================================================================= */
void SCREEN0(void) __naked
{
__asm
  push IX
  
  ld   A,(#LINLEN)
  ld   (#LINL40),A   ;copy columns seting with WIDTH to LINL40 system var
   
  ld   IX,#BIOS_INITXT
CallBIOS:
  ld   IY,(#EXPTBL-1)
  call BIOS_CALSLT
  
  ei    
  pop  IX
  ret  
__endasm;
}



/* =============================================================================
 SCREEN1
 
 Description: 
           Initialice GRAPHIC 1 screen mode (32 columns x 24 lines).

 Input:    -
 Output:   -
============================================================================= */
void SCREEN1(void) __naked
{
__asm
  push IX
  
  ld   A,(#LINLEN)   ;get a last value set with WIDTH function 
  ld   (#LINL32),A   ;set system variable
   
  ld   IX,#BIOS_INIT32
  jp   CallBIOS  
__endasm;
}



/* =============================================================================
 SetVRAMtoWRITE
 Description: Enable VDP to write (Similar to BIOS SETWRT)
 Input      : [char] VRAM address
 Output     : -             
============================================================================= */
void SetVRAMtoWRITE(unsigned int vaddr) __naked
{
vaddr;	//HL
__asm
  push IX

  ld   IX,#BIOS_SETWRT
  jp   CallBIOS
__endasm;
}   



/*===============================================================================
 FastVPOKE                                
 Function :	Writes a value to the next video RAM position. 
				Requires the VDP to be in write mode, either by previously 
				using VPOKE or SetVRAMtoWRITE functions.
 Input    : A - value
===============================================================================*/
void FastVPOKE(char value) __naked
{
value;	//A
__asm
  out  (VDPVRAM),A  
  ret
__endasm;
}




void LOCATE(char column, char line)
{
	uint vaddr = G1_MAP + (line*32)+column; //<------ BASE0 for screen 0; use BASE5 for screen 1 
  
	SetVRAMtoWRITE(vaddr);
}


/* =============================================================================
 Display a string to screen
============================================================================= */
void PRINT(char* text)
{ 
  while(*(text)) FastVPOKE(*(text++)); 
}




/* =============================================================================
 PrintNumber

 Description: 
           Displays an unsigned integer at the current cursor position.
			
 Input:    (unsigned int) or (char) numeric value          
 Output:   -
============================================================================= */
void PrintNumber(uint value) __naked
{
value;	//HL 

//PrintFNumber(value,0,5);
__asm  
	ld   D,#0
	ld   E,#5
  
; ------------------------------------------------ 
; 16-bit Integer to ASCII (decimal)
; Based on num2Dec16 by baze 
; https://baze.sk/3sc/misc/z80bits.html#5.1
;  HL = value
;  D  = zero/empty Char (0,32,48)
;  E  = length
PRNUM$:

	ld   A,#5		;number Digit
	ex   AF,AF
  	
	ld   BC,#-10000
	call $Num1
	ld   BC,#-1000
	call $Num1
	ld   BC,#-100
	call $Num1
	ld   C,#-10
	call $Num1

;Last figure
	ld   C,B
	ld   D,#48	;set "0"

;   call $Num1


;   ret   ;<--- END
    
$Num1:	
	ld   A,#47     ;"0" ASCII code - 1
   
$Num2:
	inc  A
	add  HL,BC
	jr   C,$Num2

	sbc  HL,BC

	cp   #48		;"0" ASCII code
	jr   NZ,$Num3	;if A!=48 then goto $Num3

	ld   A,D		;get ZeroDigit
	jr   $Num4


$Num3:
	;change space/empty for 0 zero ASCII code
	ld   D,#48
	
$Num4:
	ex   AF,AF
	dec  A  ;number Digit
	cp   E  ;length
	jr   NC,$next5  ;IF num.Digit>length THEN dont print

	ex   AF,AF
	or   A
	ret  Z  ;only print A>0

	out  (VDPVRAM),A  ;Print digit
	ret


$next5:
	ex   AF,AF
	ret
  
__endasm;
}



/* -----------------------------------------------------------------------------
   GetVFrequency
  
   Get video frequency type in MSX-DOS environments
   Input: ---
   Output: NTSC=0 (60Hz), PAL=1 (50Hz)
----------------------------------------------------------------------------- */  
VFreq GetVFrequency(void) __naked
{
__asm
  push IX

; -----------------------------------
  ld   HL,#0x002D	;(MSXVER) MSX version number (0=MSX,1=MSX2,2=MSX2+,3=TurboR)
  ld   A,(#0xFCC1)	;(EXPTBL) main BIOS-ROM slot address
  call 0x000C		;(RDSLT) Reads the value of an address in another slot
  or   A
  jr   NZ,readHZfromVDP  //IF A!=0

  
;in the MSX1, the information about the video frequency is in a system variable
  ld   HL,#0x002B	;(MSXROM1)
  ld   A,(#0xFCC1)	;(EXPTBL) main BIOS-ROM slot address
  call 0x000C		;(RDSLT) Reads the value of an address in another slot
  bit  7,A          ;Default interrupt frequency (0=60Hz, 1=50Hz)
  jr   Z,VFreq_isNTSC
  jr   VFreq_isPAL   

;If it is run on an MSX2 or higher, we can check the video frequency in the VDP registers.
readHZfromVDP:
;look at the system variable that contains the VDP registry value
  ld   A,(0xFFE8)    ;(RG9SAV) Mirror of VDP register 9
  bit  1,A           ;(0=60Hz, 1=50Hz)  
  jr   Z,VFreq_isNTSC
  
VFreq_isPAL:
  ld   A,#1
  pop  IX
  ret
  
VFreq_isNTSC:  
  xor  A
  pop  IX  
  ret  
__endasm;
}



char GetSysVar(uint address) __naked
{
address;	//HL
__asm
	push IX

	ld   A,(#EXPTBL)
	call BIOS_RDSLT

	pop  IX
	ret		;A
__endasm;
}




/* -----------------------------------------------------------------------------
   Test source-code
----------------------------------------------------------------------------- */ 
void TestISR_M1(void)
{
	char keyPressed;

	frames = 0;

	LOCATE(0,6);  
	PRINT("VBLANK: ");

	LOCATE(0,8);  
	PRINT("Seconds:");  

	LOCATE(0,12);  
	PRINT("Press ESC for Exit");  

	Save_ISR();
	Install_ISR(my_ISR);


	while(1)
	{
		HALT;
		
		LOCATE(8,6);
		PrintNumber(frames);

		LOCATE(8,8);
		PrintNumber(frames/VfreqHz);
			
		keyPressed = GetKeyMatrix(7);      
		if (!(keyPressed&Bit2)) break;   //ESC Key
	}

	Restore_ISR();
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













