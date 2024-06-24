;//-------------------------------------------------------------------------------
;//  _______  .___ _______       ____.  _____                             
;//  \      \ |   |\      \     |    | /  _  \                            
;//  /   |   \|   |/   |   \    |    |/  /_\  \                           
;// /    |    |   /    |    /\__|    /    |    \                          
;// \____|__  |___\____|__  \________\____|__  /                          
;// ________\/   _____ ___\/_____ _______    \/____    ___________________
;// \_   ___ \  /  _  \\______   \\      \   /  _  \  /  _____/\_   _____/
;// /    \  \/ /  /_\  \|       _//   |   \ /  /_\  \/   \  ___ |    __)_ 
;// \     \___/    |    |    |   /    |    /    |    \    \_\  \|        \
;//  \______  \____|__  |____|_  \____|__  \____|__  /\______  /_______  /
;//         \/        \/       \/        \/        \/        \/        \/ 
;// By Christophe Kohler 
;// 2024
;//-----------------------------LICENSE NOTICE------------------------------------
;// License Creative Commons CC-BY-NC-SA
;// Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
;// This is a human-readable summary of (and not a substitute for) the license. Disclaimer.
;// You are free to:
;// Share — copy and redistribute the material in any medium or format
;// Adapt — remix, transform, and build upon the material
;// The licensor cannot revoke these freedoms as long as you follow the license terms.
;// Under the following terms:
;// Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
;// NonCommercial — You may not use the material for commercial purposes.
;// ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
;// No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
;// Notices:
;// You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
;// No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
;//-------------------------------------------------------------------------------
; Game formats:
; A level contain all langages. So there is 4 data section, then 1+32+32 strings (max) for each langage (7)
; HAM6 picture. 3 levels share the same 16 color palette.
; Amiga_ham_convert183 tool is used to convert pictures to HAM6.
;  A palette can be forced and IFF RLE compression can be turned off.
;  Iff is user friendly. $74 header then planes.
;  Save file as Pic91.iff and if alternates Pic91_alt1.iff ... Pic91_alt5.bin
;  The tool will read all data and save number of variations in a long. Save palette (16 words), then Then each plane (6) side by side. Pictures are all 96x128.
; For each level there is a 16 color palette. (so ham colors are optimized for the 3 pictures)
; The brackground tiles should also match that palette.
; So for each palette, there are tiles. There are store in fast memory.
; Palette is inside each HAM IFF. On CMAP, at RR GG BB format.
; Font is 8 colors. All characters are stored sequencialy in 8*3 bytes. There is 3 colors (black, good, bad)

NUMBERLANGUAGE=7
STARTLEVELNAME1='0' ; Start at '01'
STARTLEVELNAME2='1'

; Debug flag. Set to 1 for debug. Else should be ok for shipping
DISPLAYDEBUGMEMORY=0 ; 1 display memory debugger
NOINTRO=0
NOMUSIC=0
ALLOWSKIPLEVEL=0
LANGAGEDEFAULT=0 ; (if not intro) CurrentLangage: dc.b 1 ; 0=french 1=english ; 0 to 6

;----------------------------------------------------------------

	jmp	startup
	
;----------------------------------------------------------------
; 
	include "../../LDOS/src/kernel.inc"

	code_f
	
startup:

    move.w #$0200,$dff100 ; no bitplanes

    ;clr.l $100 ; Debug
    ;clr.l $104

	;move.b #$11,$107 ; Debug, steps

	LEA	$DFF000,A6	
	move.w	#$7fff,$96(a6)		; Turn off all DMA
	move.l	#$2c8130c1,$8e(a6)	; Screen definition and size
	move.l	#$003800d0,$92(a6)	; 40 de large
	move.w	#%1000011111111111,$96(a6) ; Turn on DMA Bit 15=Set. 10=NastyBlitter 9=AllDma 8-4=DMAs 3-0=Audio

    ; -- Init Music IRQ (will replace the one in LDOS)
    ; Management of music and SFX is done in the game, not in LDOS
    lea $dff000,a6 ; a6 = CUSTOM
    move.l #0,a0 ; a0 = VectorBase
    move.b #1,d0 ; d0 = PALflag.b (0 is NTSC)
    jsr _mt_install_cia ; Install a CIA-B interrupt for calling _mt_music. Will also call trackload update


;    bsr LoadMusic1  ; Test music
;    move.w #$008,$dff180  ; Test music
;    bsr MusicStartIntro  ; Test music

    if NOINTRO==0 
    ; -- Intro
    ; Init copper dualplayfield.
    ; Background is 2 lignes of tiles (and copper point to it).  3840 bytes. Use memory in BannerTextsPlanes (7K)
    ; Second dual playfield. 320*200 8 colors = 24 K . Use memory of HAMPLANS (34K)    
    bsr InitIntro1
    
;testmus:  ; Test music
;	btst	#6,$bfe001		; Left mouse button  ; Test music
;	bne.w	testmus   ; Test music
 ;   bra testmus      ; Test music
;    move.l ModuleLoadedSamples ,a0
;    add.l #$30ab0,a0
;    move.l (a0),$100
;    move.l 4(a0),$104
  
	lea COPPIntro1,a0
    move.l	a0,$dff080 
    
	lea 	Intro_irq,a0
	move.l (LDOS_BASE).w,a6
	jsr		LDOS_INSTALL_VCALLBACK(a6)
    
    ; Create Background
    bsr IntroCreateBackground
   
    ; Load flag data
    move.l (LDOS_BASE).w,a6
	move.w #37,d0 ; Flags 184x120
	move.w #0,d1 ; Level type
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,IntroData 

    ; Init palette
    bsr InitIntro1b

    ; Display flags and cursor.
    ; Delete flag data
    move.l (LDOS_BASE).w,a6
	move.w #0,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6) ; See freeMemoryArea
  
IntroWait:
    ; wait for a valid clic
	Btst	#6,$bfe001		; Left mouse button
	Bne.w	IntroWait 
    Bsr IntroTestFlagClic
    cmp.w #0,d0 ; D0=failed , 1=Sucess
    beq IntroWait
    
    ; Hide sprite
 	Lea	IntroSpr,a0
	Move.l	#Sprite_nul,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)     

    ; Clear 2nd screen
    bsr ClearSecondScreen

    ; Display wait message
    bsr IntroDisplayLoadMessage
    
    ; Load first module
    bsr LoadMusic1 

    ; Load intro data
    move.l (LDOS_BASE).w,a6
	move.w #38,d0 ; datas/INTRO.BIN
	move.w #0,d1 ; Level type
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,IntroData 

    ; Display Logo
    ; Display "Presents".
    bsr IntroDisplayLogo
    
    ; Sound Resistance
    lea SfxIntroResistance_Desc,a0
    move.l IntroData,a1
    add.l #42282+5402,a1
    move.l a1,(a0)
    lea $dff000,a6
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)
    
    bsr waitabit ; Wait 1 second
    bsr waitabit ; Wait 1 second
 
    ; Change copper list to picture
    bsr InitIntroTitle ; And build copper list 2
    
	move.l COPPIntro2_ptr,a0
    move.l	a0,$dff080
 
    ; Play audio "Ninja Carnage"
    lea SfxIntroNinjaCarnage_Desc,a0
    move.l IntroData,a1
    add.l #42282+5402+11172,a1
    move.l a1,(a0)
    lea $dff000,a6
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)
    
    bsr IntroAnimTitle
    
    ; Start scrolling  (done in IRQ)
    move.b #0,gTextScrollAnimationCounter ; start effect
        
    ; Start intro music
    if NOMUSIC==0
    bsr MusicStartIntro
    endc
    
IntroWait2:
	Btst	#6,$bfe001		; Left mouse button
	Bne.w	IntroWait2  

    ; Stop scroll
    move.b #50,gTextScrollAnimationCounter

    ; Delete intro data
    move.l (LDOS_BASE).w,a6
	move.w #0,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6) ; See freeMemoryArea    
 
    ; Blood
    bsr IntroBloodEffect
    
    ; Play audio "Ninja Carnage"
    lea SfxIntroNinjaCarnage_Desc,a0
    move.l IntroData,a1
    add.l #42282+5402+11172,a1
    move.l a1,(a0)
    lea $dff000,a6
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)    
    
    bsr IntroBloodEffectBlack

    endc 
    ; -- End Intro -------------------------------------
    
    
    ; -- Game init
    
	lea COPPEmpty,a0  ; Use a temporary empty static copper list, because the function Init is going to erase the place where the current intro copperlist is, will create glitch
    move.l	a0,$dff080     

	bsr		Init ; Init pointers
    
	lea COPP1,a0
    move.l	a0,$dff080       

	Lea 	main_irq,a0
	move.l (LDOS_BASE).w,a6
	jsr		LDOS_INSTALL_VCALLBACK(a6)

    bsr InitGame ; General Init. Load TimeUpBin
    
    if NOINTRO==1
    if NOMUSIC==0
    bsr LoadMusic1 ; Debug no intro
    endc
    endc

    bsr InitLevel ; load and init level
    
    bsr LoopGame
    
    ; For debug ....
BigLoop:
    bsr wait1Frame
    bra BigLoop

; ----------------------------
; wait 1 seconds    
waitabit:    
;	; Wait a bit
	move.w #0,wait
.wait:
	cmp.w #50,wait
	bne .wait    
    rts

; ----------------------------
; wait  
wait1Frame:    
    ; Wait a bit
	move.w #0,wait
.wait:
	cmp.w #1,wait
	bne .wait    
    rts
; ----------------------------

LoadMusic1:
    ; Load paterns in fast ram
    ; Load Picture data
    move.l (LDOS_BASE).w,a6
    move.w #1,d0 ; Second file of the list
    move.w #1,d1 ; Mem Mod fast
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,ModuleLoadedTrack    

    ; Load samples in chip ram
    move.l (LDOS_BASE).w,a6
    move.w #2,d0 ; Second file of the list
    move.w #2,d1 ; Mem Mod Chip
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,ModuleLoadedSamples 

    rts
   
MusicStartIntro:
    lea $dff000,a6
    move.l ModuleLoadedTrack,a0
    move.l ModuleLoadedSamples,a1    
    move.b #83,d0  ; Tuto . Level 0
    jsr _mt_init ; _mt_init(a6=CUSTOM, a0=TrackerModule, a1=Samples|NULL, d0=InitialSongPos.b) 
    move.b #1,_mt_Enable ; Start music
    move.b #4,_mt_MusicChannels  ; 4 channels for music      
    rts
    
LoadMusic2:
    ; Load second module
    ; Be sure music is stopped because we are going to destroy existing module.
    ; Be also sure no data block is allocated, module should be allocated first.
    lea $dff000,a6
    jsr _mt_end ; stop module. Will clear mt_enable and lower volume.
    bsr waitabit
    
    move.l (LDOS_BASE).w,a6
	move.w #0,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6) 

    move.l (LDOS_BASE).w,a6
	move.w #1,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6) 

    move.l (LDOS_BASE).w,a6
	move.w #2,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6)     

    ; Load paterns in fast ram
    ; Load Picture data
    move.l (LDOS_BASE).w,a6
    move.w #3,d0 ; Second file of the list
    move.w #1,d1 ; Mem Mod fast
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,ModuleLoadedTrack    

    ; Load samples in chip ram
    move.l (LDOS_BASE).w,a6
    move.w #4,d0 ; Second file of the list
    move.w #2,d1 ; Mem Mod Chip
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,ModuleLoadedSamples 

    rts    
    
;------------------------------- 
; Irq running during all intro   
Intro_irq:

	add.w	#1,wait

    bsr IntroTestMouseAndMoveSprite
    bsr IntroDisplaySpriteCursor    
    
    bsr UpdateIntroScroll
    
    rts
;-------------------------------
 
;-------------------------------
; Irq running during all game
main_irq:	

	bsr VideoIrq
    
    if DISPLAYDEBUGMEMORY==1
    bsr fillDebugMem
    endc
    
	rts  								;end of irq	

;----------------------------------------------------------------
InitIntro1:

	; Clear planes
	lea		start_planes,a0
	moveq.l	#$00,d0
.clr2:
	move.l	d0,(a0)+
	cmp.l	#end_planes,a0
	bmi.b	.clr2
    
    ; Pointers: Background is 2 lignes of tiles (and copper point to it).  3840 bytes. Use memory in BannerTextsPlanes (7K)
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr,a0
    bsr     PutPointersIntro1
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr2,a0
    bsr     PutPointersIntro1
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr3,a0
    bsr     PutPointersIntro1
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr4,a0
    bsr     PutPointersIntro1
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr5,a0
    bsr     PutPointersIntro1
    move.l  #BannerTextsPlanes,d0
    lea     IntroPlansPtr6,a0
    bsr     PutPointersIntro1
    
    ; Pointers : Second dual playfield. 320*200 8 colors = 24 K . Use memory of HAMPLANS (34K)  
    move.l  #HAMPLANS,d0
    lea     IntroPlansPtr,a0
    add.l   #8*3,a0
    bsr     Put_pointeurs
    add.l   #40*200,d0
    add.l   #8,a0
    bsr     Put_pointeurs
    add.l   #40*200,d0
    add.l   #8,a0
    bsr     Put_pointeurs
    
    ; Palette. IntroBack ami file 32x16 8 colors
    lea IntroBack,a0
    add.l #2+2+2+4+2,a0
    lea IntroPalettePtr,a1
    add.l #2,a1
    move.w #$0000,(a1) ; black
    add.l #4,a1
    move.w #7-1,d0
.loopcolor:
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcolor
    
    ; Sprite
 	Lea	IntroSpr,a0
	Move.l	#Sprite_p,d0
	move.w	d0,6(a0)
	swap	d0
	move.w	d0,2(a0)     

    rts
;---------------------------------------------------------------- 
; With delay, so create animation  
IntroCreateBackground:

    move.w #20-1,d0 ; number of tiles
    move.w #0,d1 ; flip flop

    lea BannerTextsPlanes+38,a2 ; Start of first line
    lea BannerTextsPlanes+(16*40),a3 ; Start of second line

.loop:

    cmp.b #1,d1
    beq .case2

.case1:
    move.l a2,a1
    bsr CopyHalfIntroBack1
    move.l a3,a1
    bsr CopyHalfIntroBack1
    bra .next
.case2:
    move.l a2,a1
    bsr CopyHalfIntroBack2
    move.l a3,a1
    bsr CopyHalfIntroBack2   
.next    

    add.b #1,d1 ; flip d1
    and.b #1,d1
    
    bsr wait1Frame
    
    sub.l #2,a2
    add.l #2,a3
    
    dbra d0,.loop
    rts

;----------------------------------------------------------------  

ClearSecondScreen:
    move.w #((40*200*3)/4)-1,d0
    lea HAMPLANS,a0
.ClearLoop:
    clr.l (a0)+
    dbra d0,.ClearLoop   
    rts

;----------------------------------------------------------------  
IntroTestFlagClic: ; D0=failed , 1=Sucess
    ; Line 1
    ; Test Greek
    move.w #$c7,d0 ; xmin
    move.w #$ff,d1 ; xmax
    move.w #$58,d2 ; ymin
    move.w #$75,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .nogreek
    move.b #4,CurrentLangage
    bra IntroTestFlagClic_ok
.nogreek:
    ; Test it
    move.w #$103,d0 ; xmin
    move.w #$13a,d1 ; xmax
    move.w #$58,d2 ; ymin
    move.w #$75,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .noitalian
    move.b #3,CurrentLangage
    bra IntroTestFlagClic_ok
.noitalian:     
    ; Test french
    move.w #$13f,d0 ; xmin
    move.w #$176,d1 ; xmax
    move.w #$58,d2 ; ymin
    move.w #$75,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .nofrench
    move.b #0,CurrentLangage
    bra IntroTestFlagClic_ok
.nofrench: 

    ; Line2
    ; Test German
    move.w #$c7,d0 ; xmin
    move.w #$ff,d1 ; xmax
    move.w #$7f,d2 ; ymin
    move.w #$a0,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .nogerman
    move.b #5,CurrentLangage
    bra IntroTestFlagClic_ok
.nogerman:
    ; Test Spanish
    move.w #$103,d0 ; xmin
    move.w #$13a,d1 ; xmax
    move.w #$7f,d2 ; ymin
    move.w #$a0,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .nospanish
    move.b #2,CurrentLangage
    bra IntroTestFlagClic_ok
.nospanish:     
    ; Test English
    move.w #$13f,d0 ; xmin
    move.w #$176,d1 ; xmax
    move.w #$7f,d2 ; ymin
    move.w #$a0,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .noenglish
    move.b #1,CurrentLangage
    bra IntroTestFlagClic_ok
.noenglish:     
    ; Line 3
    ; Test Polish
    move.w #$103,d0 ; xmin
    move.w #$13a,d1 ; xmax
    move.w #$a6,d2 ; ymin
    move.w #$c8,d3 ; ymax
    bsr IntroTestflag
    cmp.w #0,d0
    beq .nopolish
    move.b #6,CurrentLangage
    bra IntroTestFlagClic_ok
.nopolish:      

    move.w #0,d0 ; failed
    rts
IntroTestFlagClic_ok:    
    move.w #1,d0 ; Sucess   
    rts
    
IntroTestflag:
    cmp.w IntroSpr_x,d0
    bpl IntroTestflag_failed
    cmp.w IntroSpr_x,d1
    bmi IntroTestflag_failed
    cmp.w IntroSpr_y,d2
    bpl IntroTestflag_failed
    cmp.w IntroSpr_y,d3
    bmi IntroTestflag_failed    
    move.w #1,d0
    rts   

IntroTestflag_failed:
    move.w #0,d0
    rts
    
;----------------------------------------------------------------
InitIntro1b:
    ; Flags colors copy
    move.l IntroData,a0
    add.l #2+2+2+4+2,a0
    lea IntroPalettePtr,a1
    add.l #8*4,a1
    add.l #2,a1
    move.w #$0000,(a1) ; black
    add.l #4,a1
    move.w #7-1,d0
.loopcolor:
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcolor
    ; a1 is plane here
    
    ; Copy arts to screen.
    ; Second dual playfield. 320*200 8 colors = 24 K . Use memory of HAMPLANS (34K) 
    ; Flags is 184x120 = 23 bytes. 5 long + 1 word + 1 bytes
    ; Real flag data is 176x113.
    lea HAMPLANS,a1
    add.l #((200-113)/2)*40,a1 ; Centering Y
    add.l #9,a1 ; Centering X
    ; Copy plan1
    move.w #120-1,d0
.copy
    move.w #23-1,d1
.copyline
    move.b (a0)+,(a1)+
    dbra d1,.copyline
    add.l #40-23,a1 ; next line
    dbra d0,.copy
    add.l #(200-120)*40,a1 ; next bitplan
    ; Copy plan2
    move.w #120-1,d0
.copy2
    move.w #23-1,d1
.copyline2
    move.b (a0)+,(a1)+
    dbra d1,.copyline2
    add.l #40-23,a1 ; next line
    dbra d0,.copy2
    add.l #(200-120)*40,a1 ; next bitplan  
     ; Copy plan3
    move.w #120-1,d0
.copy3
    move.w #23-1,d1
.copyline3
    move.b (a0)+,(a1)+
    dbra d1,.copyline3
    add.l #40-23,a1 ; next line
    dbra d0,.copy3
  

    rts
    
    
;----------------------------------------------------------------    
    
PutPointersIntro1:
    bsr     Put_pointeurs
    add.l   #40*32,d0
    add.l   #8,a0
    bsr     Put_pointeurs
    add.l   #40*32,d0
    add.l   #8,a0
    bsr     Put_pointeurs  
    rts
    
;CopyFullIntroBack:
;    movem.l d0/a1,-(sp)
;    lea IntroBack,a0
;    add.l #2+2+2+4+(2*8),a0   
;    move.w #16-1,d0
;.loopplan1:
;    move.w (a0)+,(a1)
;    move.w (a0)+,2(a1)
;    add.l #40,a1
;    dbra d0,.loopplan1
;    add.l #16*40,a1
;    move.w #16-1,d0
;.loopplan2:
;    move.w (a0)+,(a1)
;    move.w (a0)+,2(a1)
;    add.l #40,a1
;    dbra d0,.loopplan2
;    add.l #16*40,a1   
;    move.w #16-1,d0
;.loopplan3:
;    move.w (a0)+,(a1)
;    move.w (a0)+,2(a1)
;    add.l #40,a1
;    dbra d0,.loopplan3
;    movem.l (sp)+,d0/a1
;    rts

; Dest A1    
CopyHalfIntroBack1:
    movem.l d0/a0-a1,-(sp)
    lea IntroBack,a0
    add.l #2+2+2+4+(2*8),a0   
    move.w #16-1,d0
.loopplan1:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan1
    add.l #16*40,a1
    move.w #16-1,d0
.loopplan2:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan2
    add.l #16*40,a1   
    move.w #16-1,d0
.loopplan3:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan3
    movem.l (sp)+,d0/a0-a1
    rts   

CopyHalfIntroBack2:
    movem.l d0/a0-a1,-(sp)
    lea IntroBack,a0
    add.l #2+2+2+4+(2*8)+2,a0   
    move.w #16-1,d0
.loopplan1:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan1
    add.l #16*40,a1
    move.w #16-1,d0
.loopplan2:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan2
    add.l #16*40,a1   
    move.w #16-1,d0
.loopplan3:
    move.w (a0),(a1)
    add.l #4,a0
    add.l #40,a1
    dbra d0,.loopplan3
    movem.l (sp)+,d0/a0-a1
    rts 


;---------------------------------------------------------------
;	Mouse
;--------------------------------------------------------------- 
IntroTestMouseAndMoveSprite:
        move.w  $dff00a,d1
        move.w  d1,d0
        sub.b   oldhorizcnt,d0
        ext.w   d0
        add.w   d0,IntroSpr_x
        move.b  d1,oldhorizcnt
        lsr.w   #8,d1
        move.b  oldvertcnt,d0
        move.b  d1,oldvertcnt
        sub.b   d0,d1
        ext.w   d1
        add.w   d1,IntroSpr_y 
    
        ; -- limit spr_Y and spr_X
MOUSELIMIT_YMIN=$2c  
MOUSELIMIT_YMAX=$f0+(7*8) 
MOUSELIMIT_XMIN=$80  
MOUSELIMIT_XMAX=$1c0-8
        
        cmp.w	#MOUSELIMIT_YMIN,IntroSpr_y	; top
        bpl .noup
        move.w #MOUSELIMIT_YMIN,IntroSpr_y
.noup        
        cmp.w	#MOUSELIMIT_YMAX,IntroSpr_y	; bottom
        bmi .nobottom
        move.w #MOUSELIMIT_YMAX,IntroSpr_y
.nobottom        
        cmp.w	#MOUSELIMIT_XMIN,IntroSpr_x	; left
        bpl .noleft
        move.w #MOUSELIMIT_XMIN,IntroSpr_x
.noleft        
        cmp.w	#MOUSELIMIT_XMAX,IntroSpr_x	; right
        bmi .noright
        move.w #MOUSELIMIT_XMAX,IntroSpr_x
.noright   
        ; Display values
        ;move.w IntroSpr_x,$100
        ;move.w IntroSpr_y,$102
        rts 

;---------------------------------------------------------------
IntroDisplaySpriteCursor:
	Clr.l	d0
	Clr.l	d1
	Lea	Sprite_p,a0
	Move.w	IntroSpr_x,d0	; X
	Bclr	#0,3(a0)
	Btst	#0,d0
	Beq.b	.no_first
	Bset	#0,3(a0)
.no_first:
    lsr	#1,d0
	Move.b	d0,1(a0)
	Clr.l	d0
	Move.w	IntroSpr_y,d0
	Move.l	d0,d1
	Add.l	#16,d1 ; height
	Move.b	d0,(a0)
	Move.b	d1,2(a0)	
	And.w	#$ff00,d0
	And.w	#$ff00,d1
	Lsr	#8,d0
	Lsr	#8,d1
	Bclr	#2,3(a0)
	Btst	#0,d0
	Beq.b	.no_first_1
	Bset	#2,3(a0)
.no_first_1:
	Bclr	#1,3(a0)
	Btst	#0,d1
	Beq.b	.no_first_2
	Bset	#1,3(a0)
.no_first_2:

    rts

;---------------------------------------------------------------

    
;----------------------------------------------------------------
IntroDisplayLoadMessage:
    ; Init font palette
    lea TextPaletteIntro,a0
    lea IntroPalettePtr,a1 ; dest
    add.l #8*4,a1
    add.l #2,a1
    move.w #8-1,d0
.loopcopycolorsBanner2
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcopycolorsBanner2   
    
    ; Display text
    lea TextsIntro,a0
    clr.l d0
    move.b CurrentLangage,d0
    mulu #9*41,d0
    add.l d0,a0 ; Get correct langage
    
    move.l #40,d0
    move.l #0,d1
    move.l #100-4,d2
    bsr IntrodrawStringwithSize
   
    rts
    
; ----------------------------------------------------------
; u8 drawStringwithSize(u8* ptext, u8 x, u8 y, u8 totalsize)
; a0 text
; d0 nbchar
; d1 x (bytes)
; d2 y (lines)
; return d7 number of chars processed
IntrodrawStringwithSize:
	movem.l	d0-d6/a0-a6,-(sp)			;save all registers to stack
    move.l a0,a3 ; save start
    ;move.l #3268,a4 ; Will be offset for bad or good alternate font
    Lea HAMPLANS,a1
    add.l d1,a1 ; x
    mulu #40,d2
    add.l d2,a1    
    sub.w #1,d0 ; to loop exactly
.drawStringwithSize_loop:    
    clr.l d3
    lea Font8Intro,a2
    ;move.b 24(a2),$100
    ;move.b 26(a2),$101
    move.b (a0)+,d3 ; d3 is character code
    sub.w #32,d3 ; 0 equal to space
    mulu #8*3,d3 ; Each letter is 8*3. All letter are 152*8*3 = 3 648    
    ;add.l a4,a2 ; 0 3268 or 3268*2 ... Offset for Black/Good/bad font
    add.l d3,a2
    bsr IntroDisplayLetter8
    bsr wait1Frame ; do animation
    add.l #1,a1 ; Next char on screen
    dbra d0,.drawStringwithSize_loop ; Display all chars
    ;sub.l a3,a0 ; compute how much chars have been processed
    ;move.l a0,d7 ; send back in d3
	movem.l	(sp)+,d0-d6/a0-a6
    rts
   
; ---------------------------------------------------
; DisplayLetter8
; a1 plans
; a2 font source. 3 planes side by side. Each letter.
; 152 letter (8 * 19 cells f 8x8) black. Then same for green, then same for red.
IntroDisplayLetter8:
    movem.l	d0-d7/a0-a6,-(sp)			;save all registers to stack
    move.l a2,a0 ; font source to a0
    ; Preset 3 planes. a1 a2 a3
    move.l a1,a2
    add.l #40*200,a2
    move.l a2,a3
    add.l #40*200,a3
    move.w #8-1,d0
.drawletter
    move.b (a0)+,(a1)
    move.b (a0)+,(a2)
    move.b (a0)+,(a3)
    add.l #40,a1
    add.l #40,a2
    add.l #40,a3
    dbra d0,.drawletter
    movem.l	(sp)+,d0-d7/a0-a6
    rts  

    
;----------------------------------------------------------------   
IntroDisplayLogo:

    ; Clear planes
    bsr ClearSecondScreen

    ; IntroData
    ; IntroTitle.ami 42282 (320*200*16) 42282
    ; IntroLogo.ami 5402 (224*64*8)
    ; Font_8ColorsIntro_pal.bin 16 bytes
    ; Font_8ColorsIntro.bin 1536 bytes
    
    ; logo colors copy
    move.l IntroData,a0
    add.l #42282,a0
    add.l #2+2+2+4+2,a0
    lea IntroPalettePtr,a1
    add.l #8*4,a1
    add.l #2,a1
    move.w #$0000,(a1) ; black
    add.l #4,a1
    move.w #7-1,d0
.loopcolor:
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcolor
    ; a0 is plane here

    ; Copy to screen
    ; Copy arts to screen.
    ; Second dual playfield. 320*200 8 colors = 24 K . Use memory of HAMPLANS (34K) 
    ; Logo is (224*64*8) = 28 bytes
    lea HAMPLANS,a1
    add.l #6,a1 ; Center X
    add.l #(66-8)*40,a1 ; Center Y
    
    ; Animation of logo here
    move.w #0,d1
    move.w #63,d2
    move.w #32-1,d5
.coplines
    ; A1 is start adress on screen
    ; A0 is start of logo data
    ; d0 is line number
    move.w d1,d0 ; lines going from top to down
    bsr IntroLogoCopyLine
    move.w d2,d0 ; lines going from down to top
    bsr IntroLogoCopyLine    
    bsr wait1Frame
    add.w #2,d1
    sub.w #2,d2 
    dbra d5,.coplines
    
    ; Add text
    lea TextsIntro,a0
    clr.l d0
    move.b CurrentLangage,d0
    mulu #9*41,d0
    add.l d0,a0 ; Get correct langage
    add.l #41,a0 ; go to second message "present"
    
    move.l #40,d0 ; nb chars
    move.l #0,d1 ; pos X
    move.l #(66-8)+64+4,d2 ; pos Y
    bsr IntrodrawStringwithSize   

    rts
    
;----------------------------------------------------------------   
    ; A1 is start adress on screen
    ; A0 is start of logo data
    ; d0 is line number
IntroLogoCopyLine:
    movem.l	d0-d7/a0-a6,-(sp)			;save all registers to stack
    clr.l d2
    move.w d0,d2
    mulu #28,d2
    add.l d2,a0 ; data start
    clr.l d3
    move.w d0,d3
    mulu #40,d3
    add.l d3,a1 ; screen position of line
    
    ; A1 is plane 1
    move.l a1,a2 
    add.l #200*40,a2 ; plane2
    move.l a2,a3 ; plane3
    add.l #200*40,a3 ; plane2
LOGO1BITPLAN=28*64   
    move.w #28-1,d1
.copyline:
    move.b (a0),(a1)+
    move.b LOGO1BITPLAN(a0),(a2)+
    move.b (LOGO1BITPLAN*2)(a0),(a3)+
    add.l #1,a0 ; next byte of line
    dbra d1,.copyline
  
    movem.l	(sp)+,d0-d7/a0-a6   
    rts
    
;----------------------------------------------------------------

COPPIntro2_ptr: dc.l    0
IntroScrollLines_ptr: dc.l 0

; Title screen (and scrolling)
InitIntroTitle:
    ; Build copper list.
    move.l  #HAMPLANS+2560,a5 ; We need about 5K for this copper list.
    move.l  a5,COPPIntro2_ptr
    move.l  #$01800000,(a5)+ ; dc.w	$0180,$000
    move.l  #$01000200,(a5)+ ; DC.L	$01000200,$01020000
    move.l  #$01020000,(a5)+ ; 
    move.l  #$010a0000,(a5)+ ; dc.l    $010a0000,$01080000 ; Modulo 0	
    move.l  #$01080000,(a5)+ ; 
    move.l  #$01040064,(a5)+ ; dc.l    $01040064 ; BPLCON2. All sprite in front.  
    move.l  #$01fc0000,(a5)+ ; dc.l	$01fc0000,$010c0011 ; Aga fix
    move.l  #$010c0011,(a5)+ ;
    ; Sprite
    lea Sprite_nul,a0
    move.l a0,d0 ; low
    move.l a0,d1
    swap d1 ; high
    ;IntroSpr2:	
    move.w #$0120,(a5)+ ; dc.l	$01200000,$01220000 ; spr 0. Cursor. 4 colors
    ; dc.l	$01240000,$01260000 ; spr 1  Unused
    ; dc.l    $01280000,$012a0000 ; spr 2  Unused
    ; dc.l	$012c0000,$012e0000 ; spr 3  Unused
    ; dc.l    $01300000,$01320000 ; spr 4  Unused
    ; dc.l	$01340000,$01360000 ; spr 5  Unused
    ; dc.l    $01380000,$013a0000 ; spr 6  Unused
    ; dc.l	$013c0000,$013e0000 ; spr 7  Unused
    move.w d1,(a5)+
    move.w #$0122,(a5)+
    move.w d0,(a5)+
    move.w #$0124,(a5)+ 
    move.w d1,(a5)+
    move.w #$0126,(a5)+
    move.w d0,(a5)+
    move.w #$0128,(a5)+ 
    move.w d1,(a5)+
    move.w #$012a,(a5)+
    move.w d0,(a5)+
    move.w #$012c,(a5)+ 
    move.w d1,(a5)+
    move.w #$012e,(a5)+
    move.w d0,(a5)+
    move.w #$0130,(a5)+ 
    move.w d1,(a5)+
    move.w #$0132,(a5)+
    move.w d0,(a5)+
    move.w #$0134,(a5)+ 
    move.w d1,(a5)+
    move.w #$0136,(a5)+
    move.w d0,(a5)+
    move.w #$0138,(a5)+ 
    move.w d1,(a5)+
    move.w #$013a,(a5)+
    move.w d0,(a5)+
    move.w #$013c,(a5)+ 
    move.w d1,(a5)+
    move.w #$013e,(a5)+
    move.w d0,(a5)+        
    ;  Sprite colors
    move.l  #$01a00000,(a5)+;        dc.l    $01a00000 ; color 16 ; Transparent
    move.l  #$01a20888,(a5)+;        dc.l    $01a20888 ; color 17 ; Cursor Color 1
    move.l  #$01a40000,(a5)+;        dc.l    $01a40000 ; color 18 ; Cursor Color 2
    move.l  #$01a60fff,(a5)+;        dc.l    $01a60fff ; color 19 ; Cursor Color 3
    ;  Color remaining (12)
    move.l  #$01a80000,(a5)+;        dc.l    $01a80000,$01aa0000,$01ac0000,$01ae0000
    move.l  #$01aa0000,(a5)+
    move.l  #$01ac0000,(a5)+
    move.l  #$01ae0000,(a5)+
    move.l  #$01b00000,(a5)+;        dc.l    $01b00000,$01b20000,$01b40000,$01b60000
    move.l  #$01b20000,(a5)+
    move.l  #$01b40000,(a5)+
    move.l  #$01b60000,(a5)+
    move.l  #$01b80000,(a5)+; dc.l    $01b80000,$01ba0000,$01bc0000,$01be0000
    move.l  #$01ba0000,(a5)+
    move.l  #$01bc0000,(a5)+
    move.l  #$01be0000,(a5)+
    move.l a5,a1 ; store IntroPalettePtr2
    ; IntroPalettePtr2:
    move.l  #$01800000,(a5)+ ; dc.w    $0180,$0F00,$0182,$0FFF ; 16 colors
    move.l  #$01820000,(a5)+
    move.l  #$01840000,(a5)+ ; dc.w    $0184,$0888,$0186,$0F00
    move.l  #$01860000,(a5)+
    move.l  #$01880000,(a5)+ ; dc.w    $0188,$00F0,$018A,$000F
    move.l  #$018A0000,(a5)+
    move.l  #$018C0000,(a5)+ ; dc.w    $018C,$0F0F,$018E,$0FF0
    move.l  #$018E0000,(a5)+
    move.l  #$01900000,(a5)+ ; dc.w    $0190,$00FF,$0192,$0808
    move.l  #$01920000,(a5)+
    move.l  #$01940000,(a5)+ ; dc.w    $0194,$0008,$0196,$0FF8
    move.l  #$01960000,(a5)+
    move.l  #$01980000,(a5)+ ; dc.w    $0198,$088F,$019A,$0F88
    move.l  #$019A0000,(a5)+
    move.l  #$019C0000,(a5)+ ; dc.w    $019C,$08F8,$019E,$0F84
    move.l  #$019E0000,(a5)+
    ; Title colors copy
    move.l IntroData,a0
    add.l #2+2+2+4,a0
    ;lea IntroPalettePtr2,a1 ; Already in there
    add.l #2,a1
    move.w #16-1,d0
.loopcolor:
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcolor
    ; a0 is plane here
    move.l  #$2b07fffe,(a5)+ ; dc.b    $2b,$07,$ff,$fe ; First line 
    move.l a5,a1
    ;IntroPlansPtrIntro2:
    move.l  #$00e00000,(a5)+ ; Dc.l	$00e00000,$00e20000 ; 16 colors
    move.l  #$00e20000,(a5)+
    move.l  #$00e40000,(a5)+ ; Dc.l	$00e40000,$00e60000 
    move.l  #$00e60000,(a5)+        
    move.l  #$00e80000,(a5)+ ; Dc.l	$00e80000,$00ea0000
    move.l  #$00ea0000,(a5)+
    move.l  #$00ec0000,(a5)+ ; Dc.l	$00ec0000,$00ee0000
    move.l  #$00ee0000,(a5)+
    ; Set pointers
    move.l a0,d0
    ;lea IntroPlansPtrIntro2,a0
    move.l a1,a0
    bsr Put_pointeurs
    add.l #40*264,d0
    add.l #8,a0
    bsr Put_pointeurs
    add.l #40*264,d0
    add.l #8,a0
    bsr Put_pointeurs
    add.l #40*264,d0
    add.l #8,a0
    bsr Put_pointeurs
    ;  dc.b    $2c,$df,$ff,$fe ; First line 
    move.l  #$2cdffffe,(a5)+
    move.l  #$01004200,(a5)+ ;  Dc.l    $01004200 ; Planes
    ; IntroScrollLines: ; 5 longs = 20 bytes. WAIT, $100 (nb planes) , $10A , $108 (modulos), $102 (decay)       
    ; dc.b    $2c+$00,$df,$ff,$fe,$01,$00,$42,$00,$01,$0a,$00,$00,$01,$08,$00,$00,$01,$02,$00,$00
    move.l a5,IntroScrollLines_ptr
    ; blk.b   200*20,0
    ; lea IntroScrollLines,a0
    move.b #$2c,d0
    move.w #200-1,d5
.loopbuild
    move.b d0,(a5)+
    move.b #$df,(a5)+
    move.b #$ff,(a5)+
    move.b #$fe,(a5)+
    move.b #$01,(a5)+
    move.b #$00,(a5)+
    move.b #$02,(a5)+ ; 02 Start all black (else 4200 for 4 planes)
    move.b #$00,(a5)+    
    move.b #$01,(a5)+
    move.b #$0a,(a5)+
    move.b #$00,(a5)+ ; 10
    move.b #$00,(a5)+  
    move.b #$01,(a5)+
    move.b #$08,(a5)+
    move.b #$00,(a5)+ ; 14
    move.b #$00,(a5)+  
    move.b #$01,(a5)+
    move.b #$02,(a5)+
    move.b #$00,(a5)+
    move.b #$00,(a5)+      
    add.b #1,d0
    dbra d5,.loopbuild
    ; dc.b    $2c+200,$df,$ff,$fe ; 200 lines = end 
    move.b  #$2c+200,(a5)+ 
    move.b  #$df,(a5)+ 
    move.w  #$fffe,(a5)+    
    move.l  #$01000200,(a5)+ ; DC.L	$01000200
    ;IntroTitleScrollTextPal:
    move.l a5,a1
    move.l  #$01800000,(a5)+ ; dc.w    $0180,$0F00,$0182,$0FFF ; 16 colors
    move.l  #$01820000,(a5)+
    move.l  #$01840000,(a5)+ ; dc.w    $0184,$0888,$0186,$0F00
    move.l  #$01860000,(a5)+
    move.l  #$01880000,(a5)+ ; dc.w    $0188,$00F0,$018A,$000F
    move.l  #$018A0000,(a5)+
    move.l  #$018C0000,(a5)+ ; dc.w    $018C,$0F0F,$018E,$0FF0
    move.l  #$018E0000,(a5)+
    ; Init font palette
    lea TextPaletteIntro+2,a0 ; Palette 8 colors
    ;lea IntroTitleScrollTextPal,a1 ; dest
    add.l #2,a1
    move.w #$0000,(a1)+
    add.l #2,a1
    move.w #7-1,d0
.loopcopycolorsBanner2
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcopycolorsBanner2        
    ;IntroTitleScrollTextPtr:
    move.l a5,a1
    move.l  #$00e00000,(a5)+ ; Dc.l	$00e00000,$00e20000 ; 8 colors
    move.l  #$00e20000,(a5)+
    move.l  #$00e40000,(a5)+ ; Dc.l	$00e40000,$00e60000   
    move.l  #$00e60000,(a5)+
    move.l  #$00e80000,(a5)+ ; Dc.l	$00e80000,$00ea0000
    move.l  #$00ea0000,(a5)+     
    ; Scroll text here. 8 lignes
    ; Plans
    lea HAMPLANS+1280,a0
    move.l a0,d0
    ;lea IntroTitleScrollTextPtr,a0
    move.l a1,a0
    bsr Put_pointeurs
    add.l #8*40,d0
    add.l #8,a0
    bsr Put_pointeurs
    add.l #8*40,d0
    add.l #8,a0
    bsr Put_pointeurs       
    ; dc.b    $2c+201,$df,$ff,$fe ; 200 lines = end  
    move.b  #$2c+201,(a5)+ 
    move.b  #$df,(a5)+ 
    move.w  #$fffe,(a5)+     
    ; DC.L	$01003200,$01080000,$010a0000
    move.l  #$01003200,(a5)+
    move.l  #$01080000,(a5)+
    move.l  #$010a0000,(a5)+
    ; dc.b    $2c+201+8,$df,$ff,$fe ; 200 lines = end 
    move.b  #$2c+201+8,(a5)+ 
    move.b  #$df,(a5)+ 
    move.w  #$fffe,(a5)+     
    ; dc.l    $01000200
    move.l  #$01000200,(a5)+
    move.l  #$fffffffe,(a5)+; Dc.l	$fffffffe ; end
    
    ; -- Scrolling
    ; Use space at  HAMPLANS + 1280   
    ; Clear space of text
    move.w #((40*8*3)/4)-1,d0
    lea HAMPLANS+1280,a0
.ClearLoop:
    clr.l (a0)+
    dbra d0,.ClearLoop   

    ; Init scroll text
    
    ; Display text
    lea TextsIntro,a0
    clr.l d0
    move.b CurrentLangage,d0
    mulu #9*41,d0
    add.l d0,a0 ; Get correct langage
    add.l #41*2,a0 ; skip 2 first texts
    move.l a0,gTextScrollStringStart
    move.l a0,gTextScrollString ; Current
    add.l #(7*41)-1,a0
    move.b #255,(a0) ; end marker on last byte
    move.b #0,gTextScrollWaitTime
  
    rts
    
; ---------------------------------------------------    
IntroAnimTitle:
    movem.l	d0-d7/a0-a6,-(sp)			;save all registers to stack

    ; Loop display
    move.w #200-1,d5 ; 200-1
    clr.l d0
    move.l IntroScrollLines_ptr,a0
    move.l a0,a5
    add.l #(200*20),a5 ; end

.loopDisplay
    ; a0 is starting line of effect
    move.l a0,a1
    
    ; compute start offset
    clr.l d2
    move.b d0,d2 ; d0 0 to 200
    mulu #40,d2 ; start offset (help to compute when a line will be out of screen) ; 200*40 is end picture 
    move.w d2,d3 ; real modulo (d2 is wanted modulo)
    
    move.b #0,d4 ; Flag to tell(we reached bottom)
    
    ; --Process table (the roll effect)
    lea TableIntroTitle,a2
.looptable  
    move.b #$42,6(a1)  ; nb plan
    ; Visible or not visible ? (is pointing out of picture ?)
    ; If yes, then display normal line.
    add.w (a2),d2 ; Current offset

    cmp.l #200*40,d2
    bhi .outofpicture
    ; ----------------------------Inside picture
    cmp.b #1,d4 ; jump modulo have been skipped ?
    beq .moduloskipped
    
    add.w (a2),d3 ; real modulo
    move.w (a2),10(a1) ; modulo   
    move.w (a2),14(a1) ; modulo     
    bra .continue
.moduloskipped:    
; need to inject a new "jump" modulo (to go to end of picture)
    move.b #0,d4 ; reset flag
    move.w d2,d6
    sub.w d3,d6
    move.w d6,10(a1) ; modulo   "jump" to end of picture
    move.w d6,14(a1) ; modulo
    move.w d2,d3 ; save current wanted modulo = real module
    bra .continue
.outofpicture:  ; ------------------------ out of picture
    ; Compute modulo to stay in picture
    move.w #0,10(a1) ; modulo   
    move.w #0,14(a1) ; modulo  
    move.b #1,d4 ; Say "from this line we were out of picture" so "jump" modulo have been skipped, need to inject a new one
.continue:
    ; go to next table line, and next copper line
    add.l #2,a2
    add.l #5*4,a1
    cmp.l a1,a5
    beq .next ; end of copper ?
    cmp.w #$ffff,(a2) ; end of table ?
    bne .looptable
    
.next    
    bsr wait1Frame ; wait a frame, and add new line
    add.l #5*4,a0
    add.l #1,d0 ; next starting line
    dbra d5,.loopDisplay 

    movem.l	(sp)+,d0-d7/a0-a6			;save all registers to stack

    rts    

TableIntroTitle:
    dc.w    0*40
    dc.w    49*2*40
    dc.w    -4*40
    dc.w    -4*40
    dc.w    -4*40
    dc.w    -4*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -1*40
    dc.w    -2*40
    dc.w    -1*40
    ; mid
    dc.w    -1*40
    dc.w    -2*40    
    dc.w    -1*40
    dc.w    -2*40    
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -2*40   
    dc.w    -2*40
    dc.w    -2*40 
    dc.w    -2*40
    dc.w    -2*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -3*40
    dc.w    -4*40
    dc.w    -4*40   
    dc.w    -4*40
    dc.w    -4*40 
    
    dc.w    $ffff
    
    
    

; ---------------------------------------------------
; IntroDisplayLetter8TitleScroll
; d0 is X
; d1 is step. Value shoudl be 0 (nothing shown). 1 to 8, then 9 upper (nothing shown)
; d3 is character to display A to Z or space
IntroDisplayLetter8TitleScroll:
    movem.l	d0-d7/a0-a6,-(sp)			;save all registers to stack
    
    ; Test is Y value is ok.
    cmp.w #0,d1
    beq .exit ; nothing to display 
    ; more than 9 nothing to display
    cmp.w #9,d1 
    bge .exit
    
    ; Test X is ok
    cmp.l #0,d0
    blt .exit
    cmp.l #39,d0
    bgt .exit
 
    ; Start plane
    lea HAMPLANS+1280,a1
    clr.l d2
    move.w #8,d2
    sub.w d1,d2
    mulu #40,d2
    add.l d2,a1 ; Start line
    add.l d0,a1 ; Start X 
    lea Font8Intro,a2
    sub.w #32,d3 ; 0 equal to space
    mulu #8*3,d3 ; Each letter is 8*3. All letter are 152*8*3 = 3 648    
    add.l d3,a2       
    move.l a2,a0 ; font source to a0
    ; Preset 3 planes. a1 a2 a3
    move.l a1,a2
    add.l #8*40,a2
    move.l a2,a3
    add.l #8*40,a3
    sub.w #1,d1
.drawletter
    move.b (a0)+,(a1)
    move.b (a0)+,(a2)
    move.b (a0)+,(a3)
    add.l #40,a1
    add.l #40,a2
    add.l #40,a3
    dbra d1,.drawletter
    
.exit: 
    movem.l	(sp)+,d0-d7/a0-a6   
    rts      


;// -------------------------------------------------------
; Step = progressive from bottom 0=nothing, 1=one line at bottom, 8=all line displayed
UpdateIntroScroll: ;void UpdateScrollString()
    
    cmp.b #0,gTextScrollWaitTime
    beq .nowait
    sub.b #1,gTextScrollWaitTime
    bra .exit   

.nowait
    cmp.b #49,gTextScrollAnimationCounter     ;	if (gTextScrollAnimationCounter>49)
    ble .nocounterned ;		return;
    ; Effect is inactive
    bra .exit
    
.nocounterned:
    clr.l d4
    clr.l d5
    move.b gTextScrollAnimationCounter,d4     ;		signed char start = gTextScrollAnimationCounter;
    move.b d4,d5     ;		signed char end = start - 8;
    sub.l #8,d5
    move.w #8,d1 ; u8 step = 1;
    move.l gTextScrollString,a0
    add.l d5,a0
    move.l d5,d0 ; Current X (can be below 0)
    
    move.w #8-1,d6
.loop:    
    move.b (a0),d3
    bsr IntroDisplayLetter8TitleScroll
    
    add.l #1,a0 ; next character
    add.l #1,d0 ; Next X
    sub.w #1,d1 ; step--;
    dbra d6,.loop
    
    ;	gTextScrollAnimationCounter++;    
    add.b #1,gTextScrollAnimationCounter
    
    cmp.b #49,gTextScrollAnimationCounter
    bne .exit
    ; End of line reached
    move.b #100,gTextScrollWaitTime ; Wait 2 seconds.
    move.b #0,gTextScrollAnimationCounter ; Restart
    ; last line ?
    move.l gTextScrollString,a0
    cmp.b #255,40(a0)
    bne .noend
    move.l gTextScrollStringStart,a0
    move.l a0,gTextScrollString
    bra .exit
.noend
    ; go to next text
    add.l #41,gTextScrollString 
   
    
.exit    
    rts

;----------------------------------------------------------------    
IntroBloodEffect:
    ; We can use HAMPLANS as this zone is not used anymore.
    ; Color 5 on title is red
    ; We use 320 pixels.
    ; Speed is 8 bits. FF is full speed, 80 is half speed.
    
    ; Clear array (and scroll text zone)
    lea HAMPLANS,a0
    move.w #(320+320+(20*8*3))-1,d0 ; in words
.clear:
    clr.w (a0)+
    dbra d0,.clear
    
    ; Set speed
    lea HAMPLANS+(320*2),a0
    move.w #320-1,d1
.rand:
    bsr GetRandom
    and.w #$007f,d0
    move.w #$0080,(a0) ; 8 bit integer part, 8 bits decimal part. So $80 is 0.5
    add.w d0,(a0)+ 
    dbra d1,.rand   
    
    move.l IntroData,a4
    add.l #2+2+2+4+(16*2),a4

    clr.l d0
    clr.l d2
    ; Loop 400 times (max iterations)
    move.w #401-1,d6
.mainLoop:
    ; Iterate all points
    lea HAMPLANS,a0 ; positions
    lea HAMPLANS+(320*2),a1 ; Speed
    move.w #320-1,d5
.allPoints:
    move.w (a0)+,d3
    move.w (a1)+,d4
    cmp.w #$c700,d3 ; line 200
    bhi .outofscreen
    add.w d4,d3
    move.w d3,d0 ; y of point, d5 will be x
    move.w d3,-2(a0) ; store new position
    bsr IntroBloodDrawPoint
.outofscreen:    
    dbra d5,.allPoints
    ; iterate all points
    bsr wait1Frame
    dbra d6,.mainLoop
    rts
 ;----------------------------------------------------------------    
IntroBloodEffectBlack:
    ; We can use HAMPLANS as this zone is not used anymore.
    ; Color 5 on title is red
    ; We use 320 pixels.
    ; Speed is 8 bits. FF is full speed, 80 is half speed.
    
    ; Clear array
    lea HAMPLANS,a0
    move.w #320-1,d0
.clear:
    clr.w (a0)+
    dbra d0,.clear
    ; Set speed
    lea HAMPLANS+(320*2),a0
    move.w #320-1,d1
.rand:
    bsr GetRandom
    and.w #$007f,d0
    move.w #$0080,(a0) ; 8 bit integer part, 8 bits decimal part. So $80 is 0.5
    add.w d0,(a0)+ 
    dbra d1,.rand   
    
    move.l IntroData,a4
    add.l #2+2+2+4+(16*2),a4

    clr.l d0
    clr.l d2
    ; Loop 400 times (max iterations)
    move.w #401-1,d6
.mainLoop:
    ; Iterate all points
    lea HAMPLANS,a0 ; positions
    lea HAMPLANS+(320*2),a1 ; Speed
    move.w #320-1,d5
.allPoints:
    move.w (a0)+,d3
    move.w (a1)+,d4
    cmp.w #$c700,d3 ; line 200
    bhi .outofscreen
    add.w d4,d3
    move.w d3,d0 ; y of point, d5 will be x
    move.w d3,-2(a0) ; store new position
    bsr IntroBloodClrPoint
.outofscreen:    
    dbra d5,.allPoints
    ; iterate all points
    bsr wait1Frame
    dbra d6,.mainLoop
    rts   

    
;----------------------------------------------------------------    
; d0 y of point (0 to 200+) , d5 will be x (0-320) 
; a0 and a1 should not be changed, neither d5 & d6
; a4 base art
IntroBloodDrawPoint:
    lsr.w #4,d0 ; Get integer part
    lsr.w #4,d0
    mulu.w #40,d0
    move.l a4,a3
    add.l d0,a3 ; line
    move.w d5,d2 ; X
    lsr.w #3,d2; /8 get byte
    add.l d2,a3 ; Word on screen
    move.w d5,d2
    and.w #$7,d2 ; bit number.
    ; OLD: we want color 6, so planes will be 0, 1, 1, 0. (high)
    ; we want color 12, so planes will be     0, 0, 1, 1.
    ; bit on plane 1
    bclr.b d2,(a3)
    add.l #264*40,a3
    ; bit on plane 2
    bclr.b d2,(a3)
    add.l #264*40,a3   
    ; bit on plane 3
    bset.b d2,(a3)
    add.l #264*40,a3     
    ; bit on plane 4
    bset.b d2,(a3)
.outofscreen:    
    rts
    
;----------------------------------------------------------------    
; d0 y of point (0 to 200+) , d5 will be x (0-320) 
; a0 and a1 should not be changed, neither d5 & d6
; a4 base art
IntroBloodClrPoint:
    lsr.w #4,d0 ; Get integer part
    lsr.w #4,d0
    mulu.w #40,d0
    move.l a4,a3
    add.l d0,a3 ; line
    move.w d5,d2 ; X
    lsr.w #3,d2; /8 get byte
    add.l d2,a3 ; Word on screen
    move.w d5,d2
    and.w #$7,d2 ; bit number.
    ; we want color 5, so planes will be 1, 0, 1, 0.
    ; Set bit on plane 1
    bclr.b d2,(a3)
    add.l #264*40,a3
    ; Clea bit on plane 2
    bclr.b d2,(a3)
    add.l #264*40,a3   
    ; Clea bit on plane 3
    bclr.b d2,(a3)
    add.l #264*40,a3     
    ; set bit on plane 4
    bclr.b d2,(a3)
.outofscreen:    
    rts
    

;----------------------------------------------------------------
; Init the FX	
Init:
	; Clear planes
	lea		start_planes,a0
	moveq.l	#$00,d0
.clr2:
	move.l	d0,(a0)+
	cmp.l	#end_planes,a0
	bmi.b	.clr2
    
	; Set 1 color plane
	;Move.l	#plans1,d0
	;Lea		P1Ptr,a0
	;Bsr		Put_pointeurs
	
    if DISPLAYDEBUGMEMORY==1
	; Set debug planes
	Move.l	#plansDebugMem,d0
	Lea		P1DBG,a0
    Bsr     Put_pointeurs
	Move.l	#plansDebugMem+(12*40),d0
	Lea		P2DBG,a0
    Bsr     Put_pointeurs
	Move.l	#plansDebugMem+(12*40*2),d0
	Lea		P3DBG,a0
    Bsr     Put_pointeurs
	Move.l	#plansDebugMem+(12*40*3),d0
	Lea		P4DBG,a0
    Bsr     Put_pointeurs	
    endc
    
	; Init du sprite
	Lea	Spr,a0
	Move.l	#Cursor_sprite,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)
    
    ; Set pointers. 144 lines, 6 planes
 	move.l	#HAMPLANS,d0
	Lea		HAMPlansPtr,a0
    Bsr     Put_pointeurs 
    add.l   #40,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    
    ; Normally, playfield 1 appears in front of playfield 2.
    ; Banner Back Palette
    ; BannerDualPlayfieldPalette1
    ; BannerBack
    ; Init HAM
    ; 16 colors ham
    lea BannerBack,a0
    add.l #2+2+2+4,a0
    lea BannerDualPlayfieldPalette2,a1 ; dest
    add.l #2,a1
    move.w #8-1,d0
.loopcopycolorsBanner
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcopycolorsBanner 

    ; a0 is plane banner
    move.l a0,d0
    lea BannerDualPlayfieldPtr2,a0
    Bsr     Put_pointeurs 
    add.l   #40*61,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40*61,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    
    ; set text palette
    lea TextPalette,a0
    lea BannerDualPlayfieldPalette1,a1 ; dest
    add.l #2,a1
    move.w #8-1,d0
.loopcopycolorsBanner2
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcopycolorsBanner2   
   
    ; Set text plans
    move.l #BannerTextsPlanes,d0
    lea BannerDualPlayfieldPtr1,a0
    Bsr     Put_pointeurs 
    add.l   #40*61,d0
    add.l   #8,a0
    Bsr     Put_pointeurs
    add.l   #40*61,d0
    add.l   #8,a0
    Bsr     Put_pointeurs    
    
    ; Set Sprite colors (12 last colors)
    lea SpriteColors2,a1
    lea IconSpritesPalettes16,a0 ; 16 colors.
    add.l #4*2,a0 ; We do not care about 4 first one
    move.w #12-1,d0
.loopcolors:
    move.w (a0)+,2(a1)
    add.l #4,a1
    dbra d0,.loopcolors

    ; -- Game Init
    move.w #1,d0 ; Image 1 2 3
    bsr SetOffsetImage   
    
    bsr SpriteSetNoIcon 

    bsr InitRandom
    
    bsr InitTimeUpTexts
    
    bsr DisableSpriteCursor ; Start with no cursor
    
	rts
    
; -----------------------------------    
CopyBackgroundTiles:
    lea BackgroundDatas,a0
    move.l CurrentLevelInfoPtr,a2
    clr.l d0
    move.b 2(a2),d0
    mulu #1232,d0 ; Size of 1 background data is 1232
    add.l d0,a0
    lea Back1,a1
    move.w #1232-1,d5
.loop:
    move.b (a0)+,(a1)+
    dbra d5,.loop
    rts
; ----------------------------------- 
; will compute HAMPalette. HAM IFF is Back1.
; Cmap start at offset $3c and then RR GG BB 16 times (size 48 bytes)   
StoreRGBPaletteFromHAMIFF:     
    lea Back1,a0
    lea HAMPalette,a1
    add.l #$3c,a0
    move.w #16-1,d5
.loop:
    clr.l d0
    clr.l d1
    move.b (a0)+,d1 ; R
    lsr.w #4,d1
    lsl #8,d1
    move.b (a0)+,d0; G
    lsr.w #4,d0
    lsl.w #4,d0
    or.w  d0,d1
    move.b (a0)+,d0 ; B
    lsr.w #4,d0
    or.w  d0,d1 
    move.w d1,(a1)+
    dbra d5,.loop
    rts
    
; ----------------------------------- 
    cnop 0,4
DeathIconDataPtr:
    dc.w    0,0 ; X and Y position from center
    dc.l    IconDeath_part1a
    dc.l    IconDeath_part1b
    dc.l    IconDeath_part2a
    dc.l    IconDeath_part2b 
QTE1IconDataPtr:
    dc.w    32,0 ; X and Y position from center
    dc.l    IconQTE1_part1a
    dc.l    IconQTE1_part1b
    dc.l    IconQTE1_part2a
    dc.l    IconQTE1_part2b
QTE2IconDataPtr:
    dc.w    0,-32 ; X and Y position from center
    dc.l    IconQTE2_part1a
    dc.l    IconQTE2_part1b
    dc.l    IconQTE2_part2a
    dc.l    IconQTE2_part2b
QTE3IconDataPtr:
    dc.w    -32,0 ; X and Y position from center
    dc.l    IconQTE3_part1a
    dc.l    IconQTE3_part1b
    dc.l    IconQTE3_part2a
    dc.l    IconQTE3_part2b
QTE4IconDataPtr:
    dc.w    0,32 ; X and Y position from center 
    dc.l    IconQTE4_part1a
    dc.l    IconQTE4_part1b
    dc.l    IconQTE4_part2a
    dc.l    IconQTE4_part2b
QTEActionIconDataPtr:
    dc.w    0,0 ; X and Y position from center
    dc.l    IconQTEAction_part1a
    dc.l    IconQTEAction_part1b
    dc.l    IconQTEAction_part2a
    dc.l    IconQTEAction_part2b
IconSucessIconDataPtr:
    dc.w    0,0 ; X and Y position from center 
    dc.l    IconSucess_part1a
    dc.l    IconSucess_part1b
    dc.l    IconSucess_part2a
    dc.l    IconSucess_part2b
 
    cnop 0,4 
;---------------------------------------------------------------    
; Screen start at 128 in X ... MOUSE_CURSOR_TOP MOUSE_CURSOR_LEFT
SpriteSetDeadIcon:
    lea DeathIconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetQTE1Icon:
    lea QTE1IconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetQTE2Icon:
    lea QTE2IconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetQTE3Icon:
    lea QTE3IconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetQTE4Icon:
    lea QTE4IconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetQTEActionIcon:
    lea QTEActionIconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetSucessIcon:
    lea IconSucessIconDataPtr,a1
    bsr SpriteSetIcon
    rts
SpriteSetNoIcon:
    ; Set pointers
    lea Spr16,a0
    lea Sprite_nul,a1
 	Move.l	a1,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)   
    add.l #8,a0
  	Move.l	a1,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)    
    add.l #8,a0
  	Move.l	a1,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)  
    add.l #8,a0
  	Move.l	a1,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)  
    rts
 
;---------------------------------------------------------------  
; SpriteSetIcon
; From structure
; A1 = structure
; OffsetX from center, OffsetY from center
; Then ptr to 4 sprites Negative 
SpriteSetIcon: 
; Central position.
ICONCENTERX=(MOUSE_CURSOR_LEFT + (96/2) - 16 )
ICONCENTERY=(MOUSE_CURSOR_TOP  + (128/2)- 16 )

    clr.l  d0
    move.w gOffsetPicX,d2 ; Offset in byte
    lsl #3,d2 ; *2 to get in pixels.

    ; Set position of icon
    move.l 4(a1),a0
    move.l #ICONCENTERX,d0 ; X
    add.l d2,d0 ; Add offset
    ; Add offset from center
    clr.l d2
    move.w (a1),d2
    ext.l d2
    add.l d2,d0
    ; Y
    move.l #ICONCENTERY,d1 ; Y
    clr.l d2
    move.w 2(a1),d2
    ext.l d2
    add.l d2,d1
    bsr updateSpriteData ; it will save d0 & d1
    
    move.l 8(a1),a0
    bsr updateSpriteData 

    move.l 12(a1),a0
    add.l #16,d0
    bsr updateSpriteData
    
    move.l 16(a1),a0
    bsr updateSpriteData 

    ; Set pointers

    lea Spr16,a0
 	Move.l	4(a1),d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)   

    add.l #8,a0
  	Move.l	8(a1),d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)    

    add.l #8,a0
  	Move.l	12(a1),d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)  
    
    add.l #8,a0
  	Move.l	16(a1),d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)  
   
    rts
    
;---------------------------------------------------------------
; A0 is sprite structure. Word 0 and word 1 are control words.
; d0 is X
; d1 is Y
; We consider these 2 words as 4 bytes. 0 1 2 3
; one bit is on last byte (not convenient).
; E7 E6 E5 E4 E3 E2 E1 E0 = VSTART
; H8 H7 H6 H5 H4 H3 H2 H1 = HSTART
; L7 L6 L5 L4 L3 L2 L1 L0 = VSTOP
; AT 0  0  0  0  E8 L8 H0
updateSpriteData:
    movem.l	d0-d1,-(sp)
SPRITEHEIGHT=32
    ; -- HSTART. Bit 0 byte 3, is horizontal first bit.
	Bclr	#0,3(a0)
	Btst	#0,d0
	Beq.b	.no_first
	Bset	#0,3(a0)
.no_first:
    ; -- Set horizontal pos (the remaining 8 bits)
    lsr	#1,d0 ; divide X by 2 (we already set the last bit)
 	Move.b	d0,1(a0) ; Byte 1 is H8 to H1. (HSTART)
    ; -- Take care of Y now.
	Move.b	d1,(a0)  ; VSTART (8 least bits, not higher bit)
    move.l  d1,d2
	Add.l	#SPRITEHEIGHT,d2    
	Move.b	d2,2(a0) ; VSTOP  (8 least bits, not higher bit)
	; -- Write higher bits of VSTART/VSTOP
	And.w	#$ff00,d1
	And.w	#$ff00,d2
	Lsr	    #8,d1
	Lsr	    #8,d2
    ; Clear and set E8
	Bclr	#2,3(a0)
    ; test and write bit
	Btst	#0,d1
	Beq.b	.no_first_1
	Bset	#2,3(a0)
.no_first_1:
    ; Clear and set L8
	Bclr	#1,3(a0)
	Btst	#0,d2
	Beq.b	.no_first_2
	Bset	#1,3(a0)
.no_first_2:
    ; Set attached bit
    Bset	#7,3(a0)
    movem.l	(sp)+,d0-d1
    rts
 
;---------------------------------------------------------------    
EnableSpriteCursor:
	Lea	Spr,a0 ; Init du sprite
	Move.l	#Cursor_sprite,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)
    rts
    
DisableSpriteCursor:
	Lea	Spr,a0 ; Init du sprite
	Move.l	#Sprite_nul,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)
    rts 

EnableSpriteGauge:
    ;move.b #$aa,$105
	Lea	SprGauge,a0 ; Init du sprite
	Move.l	#Gauge_sprite,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)
    ; -- set position
    Lea	Gauge_sprite,a0
    ; Compute X (in d0)
    move.w gOffsetPicX,d1 
    lsl #3,d1
	Move.w	#MOUSE_CURSOR_LEFT+96,d0	; X 
    add.w d1,d0 ; Add offset
	Bclr	#0,3(a0)
	Btst	#0,d0
	Beq.b	.no_first
	Bset	#0,3(a0)
.no_first:
    lsr	#1,d0
	Move.b	d0,1(a0)
	Clr.l	d0
	Move.w	#MOUSE_CURSOR_TOP-1,d0 ; Y 
	Move.l	d0,d1
	Add.l	#130,d1 ; Sprite height
	Move.b	d0,(a0)
	Move.b	d1,2(a0)	
	And.w	#$ff00,d0
	And.w	#$ff00,d1
	Lsr	#8,d0
	Lsr	#8,d1
	Bclr	#2,3(a0)
	Btst	#0,d0
	Beq.b	.no_first_1
	Bset	#2,3(a0)
.no_first_1:
	Bclr	#1,3(a0)
	Btst	#0,d1
	Beq.b	.no_first_2
	Bset	#1,3(a0)
.no_first_2:    
    rts
    
DisableSpriteJauge:
    ;move.b #$ff,$105
	Lea	SprGauge,a0 ; Init du sprite
	Move.l	#Sprite_nul,d0
	Move.w	d0,6(a0)
	Swap	d0
	Move.w	d0,2(a0)
    rts
    
;---------------------------------------------------------------
Display_Cursor_sprite:
    ;move.w Posx,$100 ; ok ... 0 to $16
    ;move.w Posy,$102 ; ok ... 0 to $1e
    Lea	Cursor_sprite,a0

	Clr.l	d0
	Clr.l	d1
    
    ; Compute X (in d0)
    move.w gOffsetPicX,d1
    lsl #3,d1
	Move.w	Spr_x,d0	; X
    add.w d1,d0 ; Add offset
    
	Bclr	#0,3(a0)
	Btst	#0,d0
	Beq.b	no_first
	Bset	#0,3(a0)
no_first:
    lsr	#1,d0
	Move.b	d0,1(a0)
	Clr.l	d0
	Move.w	Spr_y,d0
	Move.l	d0,d1
	Add.l	#8,d1 ; Sprite height
	Move.b	d0,(a0)
	Move.b	d1,2(a0)	
	And.w	#$ff00,d0
	And.w	#$ff00,d1
	Lsr	#8,d0
	Lsr	#8,d1
	Bclr	#2,3(a0)
	Btst	#0,d0
	Beq.b	no_first_1
	Bset	#2,3(a0)
no_first_1:
	Bclr	#1,3(a0)
	Btst	#0,d1
	Beq.b	no_first_2
	Bset	#1,3(a0)
no_first_2:
    rts

;---------------------------------------------------------------
; deplacement curseur
TestMouseAndMoveSprite:	

    move.b #0,mouse_want_right
    move.b #0,mouse_want_left
    move.b #0,mouse_want_up
    move.b #0,mouse_want_down
	
    ; Save old coordinates. m1->m1s
	Lea	m1,a1
	Lea	m1s,a2
	Move.l	(a1),(a2)
    ; Save old coordinates RELATIVES. m1->m1s
	Lea	m1r,a1
	Lea	m1rs,a2
	Move.l	(a1),(a2)
    ; coordonnees de la souris
	Clr.l	d1
	Move.w	$dff00a,d1
	And.w 	#$00ff,d1
	Move.w	d1,m1		; M1 = X current
	Move.w 	$dff00a,d1
	Lsr.w	#8,d1
	And.w 	#$00ff,d1
	Move.w	d1,m2		; M2 = Y current
	; convert to relative
	Lea	m1,a1
	Move.w	(a1),d1
	Lea	m1s,a1
	Move.w	(a1),d2
	Sub.w	d2,d1
	Move.w	d1,m1r		; Resultat en relatif X = M1r
	lea	m2,a1
	move.w	(a1),d1
	lea	m2s,a1
	move.w	(a1),d2
	sub.w	d2,d1
	move.w	d1,m2r		; Resultat en relatif Y = M2r
Vm=125 ; regle le depassement
	cmp.w	#Vm,m1r
	Bmi.b	lp
	Move.w	m1rs,m1r	
lp:	cmp.w	#-Vm,m1r
	Bpl.b	lp1
	Move.w	m1rs,m1r	
lp1:	
    cmp.w	#Vm,m2r
	Bmi.b	lp3
	Move.w	m2rs,m2r	
lp3:	Cmp.w	#-Vm,m2r
	Bpl.b	lp4
	Move.w	m2rs,m2r	
lp4:
    ; Test directions
    ;move.w m1r,$100
    ;move.w m2r,$102
	cmp.w	#0,m1r
	bpl.b	b_d
	move.w	m1r,d0
	neg	d0
	Add.w	d0,Mouse_g
b_d	cmp.w	#0,m1r
	bmi.b	b_g
	move.w	m1r,d0
	Add.w	d0,Mouse_d
b_g	cmp.w	#0,m2r
	bpl.b	b_h
	move.w	m2r,d0
	neg	d0
	Add.w	d0,Mouse_h
b_h	cmp.w	#0,m2r
	bmi.b	b_b
	move.w	m2r,d0
	Add.w	d0,Mouse_b
;MOUSESENSIVITY=20
MOUSESENSIVITY=10
b_b
	cmp.w	#MOUSESENSIVITY,Mouse_g
	bmi.b	no_dg
	;clr.w	Mouse_g
	;beq.w	Gauche
    bra Gauche
no_dg:	
    cmp.w	#MOUSESENSIVITY,Mouse_d
	bmi.b	no_dd
	;clr.w	Mouse_d
	;beq.w	Droite
    bra Droite
no_dd:	
    cmp.w	#MOUSESENSIVITY,Mouse_h
	bmi.b	no_dh
	;clr.w	Mouse_h
	;beq.b	Haut
    bra Haut
no_dh:	
   cmp.w	#MOUSESENSIVITY,Mouse_b
	bmi.b	no_db
	;clr.w	Mouse_b
	;beq.b	Bas
    bra Bas
no_db:	
    rts
;---------------------------------------------------------------
Haut:	
    move.b #1,mouse_want_up
    ;bsr MouseApplyUp
    sub.w #MOUSESENSIVITY+1,Mouse_h ; N Set if the result is negative. Cleared otherwise.
    ; until this is positive then go left
    bmi .end ; Test flag N is true.
    ;bra .end
    
    bra Haut
.end  
    clr.w	Mouse_h  
    rts

Bas:
    move.b #1,mouse_want_down
    ;bsr MouseApplyDown
    sub.w #MOUSESENSIVITY+1,Mouse_b ; N Set if the result is negative. Cleared otherwise.
    ; until this is positive then go left
    bmi .end ; Test flag N is true.
    ;bra .end
    
    bra Bas
.end  
    clr.w	Mouse_b      
    Rts

Gauche:	
    move.b #1,mouse_want_left
    ;bsr MouseApplyLeft
    sub.w #MOUSESENSIVITY+1,Mouse_g ; N Set if the result is negative. Cleared otherwise.
    ; until this is positive then go left
    bmi .end ; Test flag N is true.
    ;bra .end
    
    bra Gauche
.end      
    clr.w	Mouse_g  
    bra no_dd ; continue testing h/d

Droite:	
    move.b #1,mouse_want_right
    ;bsr MouseApplyRight
    
    sub.w #MOUSESENSIVITY+1,Mouse_d ; N Set if the result is negative. Cleared otherwise.
    ; until this is positive then go left
    bmi .end ; Test flag N is true.
    ;bra .end
     
    bra Droite
.end        
    clr.w	Mouse_d 
    bra no_dd ; continue testing h/d

;-------------------------------------------------------------

MOUSE_CURSOR_TOP=$2c+1+8
MOUSE_CURSOR_LEFT=$80

MouseApplyUp:	
    Cmp.w	#MOUSE_CURSOR_TOP,Spr_y	; Ligne du haut ?
	Beq.b	.no_h1
	Sub.w	#4,Spr_y	
	Sub.w	#1,Posy
.no_h1	
    rts

MouseApplyDown:
	Cmp.w	#MOUSE_CURSOR_TOP+(15*8),Spr_y	; comparé avec le bas
	beq	.no_h2
	Add.w	#4,Spr_y
	Add.w	#1,Posy
.no_h2	
    Rts

MouseApplyLeft:	
    Cmp.w	#MOUSE_CURSOR_LEFT,Spr_x
	Beq.b	.no_h2
	Sub.w	#4,Spr_x
	Sub.w	#1,Posx
.no_h2
   Rts

MouseApplyRight:	
    Cmp.w	#MOUSE_CURSOR_LEFT+(11*8),Spr_x
	Beq.b	.no_h2
	Add.w	#4,Spr_x
	Add.w	#1,Posx
.no_h2	
    rts

;---------------------------------------------------------------
; Work done during interruption (scrolling)
VideoIrq:

	add.w	#1,wait
    
    bsr TestMouseAndMoveSprite ; Mouse is always moving, so we can not control position of cursor out of play mode
    bsr Display_Cursor_sprite

	rts

;---------------------------------------------------------------
; Convert mem block label to color (to be able to track them)  
    if DISPLAYDEBUGMEMORY==1  
fillDebugMem:

sizedebugmemplan=12*40
    ; Convert colors, to pixels. 0 to 9 (one for each type of mem block). 1 Block = 4Kb.
    ; Display LABEL Chip ram.
    
	move.l (LDOS_BASE).w,a6
	jsr		LDOS_GETMEMBLOCKTABLE(a6) ; d0 = chip, d1 = fast   
    
    ;lea DebugMemData,a0 ; source data
    move.l d0,a0 ; Get table of chip mem
    lea plansDebugMem+40,a1 ; dest
    move.l #(128/8)-1,d6 ; 128 blocks
fillDebugMem_mainloop1:
    move.w #7,d7 ; loop on 8 pixels
    clr.b d0 ; d0 to d3 for each plan
    clr.b d1
    clr.b d2
    clr.b d3
fillDebugMem_8pixelsloop1:
    move.b (a0)+,d5 ; get color to convert
; color 2 = grey (for free) then 3 to 10 for each label
;MEMLABEL_SYSTEM		=	$7f ; 10 0008 DARK BLUE
;MEMLABEL_TRACKLOAD		=	$7e ; 9  0808 DARK PURPLE
;MEMLABEL_PRECACHED_FX	=	$7d ; 8  00FF LIGHT BLUE
;MEMLABEL_MUSIC			=	$7c ; 7  0FF0 YELLOW
;MEMLABEL_DEBUG_SCREEN	=	$7b ; 6  0F0F PURPLE
;MEMLABEL_BOOTREAD		=	$7a ; 5  000F BLUE
;MEMLABEL_USER_FX		=	$79 ; 4  00F0 GREEN
;MEMLABEL_PERSISTENT_CHIP=	$78 ; 3  0F00 RED
        
    cmp.b #0,d5
    beq .isfree
    sub.b #$76-3,d5 ; remap $76 to $7f to 3 to 12
    bra .endcolorremap
.isfree
    move.b #2,d5; grey
.endcolorremap
    ; test plan 1
    btst #0,d5
    beq .nobitplan1
    bset d7,d0
.nobitplan1
    ; test plan 2
    btst #1,d5
    beq .nobitplan2
    bset d7,d1
.nobitplan2
    ; test plan 3
    btst #2,d5
    beq .nobitplan3
    bset d7,d2
.nobitplan3
    ; test plan 4
    btst #3,d5
    beq .nobitplan4
    bset d7,d3
.nobitplan4
    dbra d7,fillDebugMem_8pixelsloop1
    ; one each byt is computed, write to dest
    move.b d0,(a1)
    move.b d1,sizedebugmemplan(a1)
    move.b d2,(sizedebugmemplan*2)(a1)
    move.b d3,(sizedebugmemplan*3)(a1)
    add.l #1,a1
    dbra d6,fillDebugMem_mainloop1
    
    ; Fast mem
	move.l (LDOS_BASE).w,a6
	jsr		LDOS_GETMEMBLOCKTABLE(a6) ; d0 = chip, d1 = fast   
    
    ;lea DebugMemData,a0 ; source data
    move.l d1,a0 ; Get table of chip mem
    lea plansDebugMem+40*3,a1 ; dest
    move.l #(128/8)-1,d6 ; 128 blocks
fillDebugMem_mainloop2:
    move.w #7,d7 ; loop on 8 pixels
    clr.b d0 ; d0 to d3 for each plan
    clr.b d1
    clr.b d2
    clr.b d3
fillDebugMem_8pixelsloop2:
    move.b (a0)+,d5 ; get color to convert
; color 2 = grey (for free) then 3 to 10 for each label
;MEMLABEL_SYSTEM		=	$7f ; 10
;MEMLABEL_TRACKLOAD		=	$7e ; 9
;MEMLABEL_PRECACHED_FX	=	$7d ; 8
;MEMLABEL_MUSIC			=	$7c ; 7
;MEMLABEL_DEBUG_SCREEN	=	$7b ; 6
;MEMLABEL_BOOTREAD		=	$7a ; 5
;MEMLABEL_USER_FX		=	$79 ; 4
;MEMLABEL_PERSISTENT_CHIP=	$78 ; 3 
    cmp.b #0,d5
    beq .isfree
    sub.b #$77-3,d5 ; remap $78 tp $7f to 3 to 10
    bra .endcolorremap
.isfree
    move.b #2,d5; grey
.endcolorremap
    ; test plan 1
    btst #0,d5
    beq .nobitplan1
    bset d7,d0
.nobitplan1
    ; test plan 2
    btst #1,d5
    beq .nobitplan2
    bset d7,d1
.nobitplan2
    ; test plan 3
    btst #2,d5
    beq .nobitplan3
    bset d7,d2
.nobitplan3
    ; test plan 4
    btst #3,d5
    beq .nobitplan4
    bset d7,d3
.nobitplan4
    dbra d7,fillDebugMem_8pixelsloop2
    ; one each byt is computed, write to dest
    move.b d0,(a1)
    move.b d1,sizedebugmemplan(a1)
    move.b d2,(sizedebugmemplan*2)(a1)
    move.b d3,(sizedebugmemplan*3)(a1)
    add.l #1,a1
    dbra d6,fillDebugMem_mainloop2    

    rts
    endc
	
;----------------------------------------------------------------
; Side by side pointers
Put_pointeurs:
	move.w	d0,6(a0)
	swap	d0
	move.w	d0,2(a0)
	swap	d0
	rts

;---------------------------------------------------------------	
waitblitter:	
	;move.w #$f00,$dff180
	btst	#14,$dff002 ; Wait blitter to be ready
	;move.w #$000,$dff180
	Bne	waitblitter
	Rts
	

;---------------------------------------------------------------	
;---------------------------------------------------------------	
;---------------------------------------------------------------	
	data_f
;---------------------------------------------------------------	
;---------------------------------------------------------------	
;---------------------------------------------------------------	    

; Variables
    cnop 0,4
    
ModuleLoadedTrack:    dc.l 0
ModuleLoadedSamples:    dc.l 0
gTextScrollStringStart: dc.l 0 ;// u8 gTextScrollString[41]; // One line    
gTextScrollString: dc.l 0 ;// u8 gTextScrollString[41]; // One line  
IntroData:     dc.l 0   

IntroSpr_x:		dc.w	$90 ; min X
IntroSpr_y:		dc.w	$60 ; min Y
Spr_x:		Dc.w	MOUSE_CURSOR_LEFT
Spr_y:		Dc.w	MOUSE_CURSOR_TOP      ; Top screen
Posx:		Dc.w	0		; position in grid 4x4 
Posy:		Dc.w	0
Mouse_h:	dc.w	0
Mouse_b:	dc.w	0
Mouse_g:	dc.w	0
Mouse_d:	dc.w	0 
m1:	dc.w	0 ; mouse
m2:	dc.w	0
m1s:	dc.w	0
m2s:	dc.w	0
m1r:	dc.w	0
m2r:	dc.w	0
m1rs:	dc.w	0
m2rs:	dc.w	0
wait:	dc.w	0 ; Wait frame

mouse_want_right:    dc.b    0
mouse_want_left:    dc.b    0    
mouse_want_up:     dc.b    0    
mouse_want_down:     dc.b    0   
oldhorizcnt:        ds.b    1
oldvertcnt:        ds.b    1  
gTextScrollAnimationCounter: dc.b 50 ;// u8 gTextScrollAnimationCounter=255; // 0 to 49 = active, >49 = inactive
gTextScrollWaitTime: dc.b 0 ; >0 means wait
gCreditLooping: dc.b 0
pointedzone: dc.b 0 ;	u8 pointedzone=0; // 0 means none
shouldloopingame: dc.b 0;	u8 shouldloopingame=1; // Main loop. False to allow level change.
; All input states (computed only once)
firepressed: dc.b 0;	u8 firepressed;
firepressedlast: dc.b 0;	u8 firepressedlast=0; // To be able to check triggering "just pressed"
iskeyup: dc.b 0;
iskeydown: dc.b 0;
iskeyleft: dc.b 0;
iskeyright: dc.b 0;
iskeyuplast: dc.b 0;
iskeydownlast: dc.b 0;
iskeyleftlast: dc.b 0;
iskeyrightlast: dc.b 0;
keyboarddata: dc.b 0 ; value of current key pressed. 0 if no key pressed
; Locals
win: dc.b 0; QTE 
lost: dc.b 0; QTE
keypressed: dc.b 0 ; QTE
wantedaction: dc.b 0 ; QTE
actiontodo: dc.b 0
message: dc.b 0 ;QTE
    even
    
;// -- Globals ---------------------------------------------
;
;// -- Structure for a level binary
;struct sleveldata
;{
;	u8 position; // 1 2 3 (image 1 (left), middle 2 or right 3)
;	u8 nbzones; // 1 to 9
;} sleveldata;
;
;// -- Data zone
;// The begining of the zone are the pointer to the data. All is absolute encoded
;// The binary data is one block and read at once from 0x9C00. The pointer are automatically mapped and point to the data.
    cnop 0,4
    
; Longs
gLevelData:          dc.l 0 ;struct sleveldata* gLevelData;
gNextLevelName:      dc.l 0 ;u8* gNextLevelName;
gZonesData:          dc.l 0 ;u8* gZonesData;
gScenario:           dc.l 0 ;u8* gScenario;
gMessageIntro:       dc.l 0 ;u8* gMessageIntro;
gMessageSucces:      ds.l 32 ;u8* gMessageSucces[32];
gMessageDeath:       ds.l 32 ;u8* gMessageDeath[32];
gTilesData:          dc.l 0 ;u8* gTilesData;
gTimeUpDeathsPtr:    ds.l 7 ;u8* gTimeUpDeathsPtr[7]; // 14 bytes
LoadedLevel:         dc.l 0 ; Pointer to dynamic zone where level is loaded
CurrentLevelInfoPtr: dc.l 0 ; From Name we find info about level.
gCurrentScenarioLine: dc.l 0 ;u8* gCurrentScenarioLine;  // Pointer to current scenario line
gCurrentScenarioLine_Save: dc.l 0 ;u8* gCurrentScenarioLine_Save;  // Pointer to current scenario line
gSfxTextDeathSave: dc.l 0 ;u8* gSfxTextDeathSave; // Save pointer to text to display after SFX

; Words
gOffsetPicX:  ds.w    1 ; Reserve 1 word
gGauge: dc.w 0 ;u16 gGauge; // Current value of gauge. max is 128*256. 0 is not activated
gGaugeSpeed: dc.w 0 ;u16 gGaugeSpeed; // Decrease speed (each frame)
gNbDeath: dc.w 0 ;u16 gNbDeath; // Stats on number of death
TEXTCOLORGOOD=3648
TEXTCOLORBAD=(3648*2)
textboldcolor: dc.w TEXTCOLORGOOD ; u8  textboldcolor = TEXTCOLORGOOD; // Offset 

; Bytes
myjoy: dc.b 0 ;u8  myjoy; // Bug always send back $FF
gNbZones: dc.b 0 ;u8  gNbZones;
gSfxDeathCount: dc.b 0;//u8  gSfxDeathCount; // Each xx death play sound
gSfxLastRand: dc.b 0 ; to avoid having same sound again
gamestate: dc.b 0 ;u8  gamestate; // 0=playing 1=dead & wait space
QTEstate: dc.b 0 ;u8  QTEstate; // For managing the QTE sequence.
QTEcurrentstep: dc.b 0 ;u8  QTEcurrentstep; // Step of the rythmic sequence.
gGaugeEnded: dc.b 0 ;u8  gGaugeEnded; // Switch to 1 when time is over.
gGaugeInUse: dc.b 0 ;u8  gGaugeInUse; // 0=not used, 1=used
gGaugeReadyToStart: dc.b 0 ; u8 Fix, pause before gauge start (wait for mouse move)
gPaletteMode: dc.b 0 ;u8  gPaletteMode; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall
gPaletteNextSwap: dc.b 0 ;u8  gPaletteNextSwap; // >0 in frame
gNextPalette: dc.b 0 ;u8  gNextPalette; // 0=Palette 1=PaletteAlt
gCursorMode: dc.b 0 ;u8  gCursorMode; // Special mode for cursor. 0=normal. 1=Slow and random (hit), 2=reversed (all) 3=reversed Up, 4=reversed down 5=always falling
gCursorAnimCount: dc.b 0 ; local for simply slowing down cursor auto anim
gQTEForceNoWait: dc.b 0;u8  gQTEForceNoWait; // False. For level 92 force no wait
gCheatEnable: dc.b 0;u8  gCheatEnable;
gCheatEnableTimer: dc.b 0
gSkippingLevel: dc.b 0;u8  gSkippingLevel; // For level skip
gCheatSequenceCorrectLetters: dc.b 0 ;u8  gCheatSequenceCorrectLetters; 
gTime: dc.b 0 ;u8  gTime; // Seconds
gTimeFrag: dc.b 0 ;u8  gTimeFrag; // 10th of seconds
gTimePrecise: dc.b 0 ;u8  gTimePrecise; // Frame precise (0 to 255)
gQte_auto_mode: dc.b 0 ;u8  gQte_auto_mode; // 0 = off 1=on
gQte_next: dc.b 0 ;u8  gQte_next; // Time before next QTE
gScenario_qte_live: ds.b 2*10 ;u8  gScenario_qte_live[2][10]; // Random QTE for last level
NBMESFAILTIME=6 ;#define NBMESFAILTIME 6
gMessageFailTimeCurrent: dc.b 0 ;u8  gMessageFailTimeCurrent;
gPicAltCurrentIndex: dc.b 0 ;u8  gPicAltCurrentIndex; // For animation, store current alt picture 0 or more
gSoundGoodHaveBeenPlayedThisFrame: dc.b 0 ;u8  gSoundGoodHaveBeenPlayedThisFrame; // To avoid playing twice
NBMAXZONES=9 ;#define NBMAXZONES 9
zones: ds.b NBMAXZONES*5 ;u8 zones[NBMAXZONES*5]; // start with no values
CurrentLangage: dc.b    LANGAGEDEFAULT ; 0=french 1=english ; 0 to 6
; Current START LEVEL
CurrentLevelName: dc.b    STARTLEVELNAME1,STARTLEVELNAME2; This is name of level LEVEL01.BIN will only be "01".
 
 
    even

; -------------------------------------------------------
; DisplayBack1
; d0 x column
; d1 y line
DisplayBack1:
	movem.l	d0-d3,-(sp)
    lea Back1,a0    ; Src
    add.l #$74,a0; skip header
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    move.w #(16*6)-1,d3
.copy
    ; Copy data
    move.w (a0)+,(a1)+
    add.l #38,a1
    dbra d3,.copy
    movem.l	(sp)+,d0-d3 
    rts
; -------------------------------------------------------
; DisplayBack2
; d0 x column
; d1 y line
DisplayBack2:
	movem.l	d0-d3,-(sp)
    lea Back2,a0    ; Src
    add.l #$74,a0; skip header
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    move.w #(16*6)-1,d3
.copy
    ; Copy data
    move.w (a0)+,(a1)+
    add.l #38,a1
    dbra d3,.copy
    movem.l	(sp)+,d0-d3     
    rts
; -------------------------------------------------------
; DisplayBack1_half
; d0 x column
; d1 y line
DisplayBack1_half:
	movem.l	d0-d3,-(sp)
    lea Back1_half,a0    ; Src
    add.l #$74,a0; skip header
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    move.w #(16*6)-1,d3
.copy
    ; Copy data
    move.b (a0)+,(a1)+
    add.l #1,a0  ; minimum picture is padded to 16 pixels   
    add.l #39,a1
    dbra d3,.copy
    movem.l	(sp)+,d0-d3 
    rts
; -------------------------------------------------------
; DisplayBack2_half
; d0 x column
; d1 y line
DisplayBack2_half:
	movem.l	d0-d3,-(sp)
    lea Back2_half,a0    ; Src
    add.l #$74,a0; skip header
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    move.w #(16*6)-1,d3
.copy
    ; Copy data
    move.b (a0)+,(a1)+
    add.l #1,a0 ; minimum picture is padded to 16 pixels
    add.l #39,a1
    dbra d3,.copy
    movem.l	(sp)+,d0-d3 
    rts      
;// -------------------------------------------------------
;// Display Background with pattern
;void DisplayBackground()
; Display 4 lines of tiles
; Then 4 lines of tiles
; Then last line (9 lines in total) 9*16 = 144 lines
DisplayBackground:
    move.w #4-1,d3
    move.l #0,d1 ; y
.alllines 
    ; Line even
    move.w #10-1,d2
    move.l #0,d0 ; x
.oneline
    bsr DisplayBack1
    add.l #2,d0
    bsr DisplayBack2    
    add.l #2,d0
    dbra d2,.oneline 
    add.l #16,d1
    ; Line odd
    move.w #10-1,d2
    move.l #0,d0 ; x
.oneline2
    bsr DisplayBack2
    add.l #2,d0
    bsr DisplayBack1    
    add.l #2,d0
    dbra d2,.oneline2
    add.l #16,d1
    dbra d3,.alllines
    ; last line
    ; Line even
    move.w #10-1,d2
    move.l #0,d0 ; x
.oneline3
    bsr DisplayBack1
    add.l #2,d0
    bsr DisplayBack2    
    add.l #2,d0
    dbra d2,.oneline3
    add.l #16,d1    
    
    rts

; As HAM6 need to have fixed colors on start pixel
; We need to display half background tiles after pic, as picture
; is cutting the 16x16 tile in half. We got two half pictures of the back tiles.
;        move.l gLevelData,a0
;        move.b (a0),d0;  ==> pic id, can be 1 2 3  
; gOffsetPicX (w) start of picture in x (byte). The border is 12 bytes farer
DisplayBackgoundAfterPic:
    ; For picture 1, we start from back1 (and display 9 half tiles)
    ; For picture 2 and 3, we sdtart from back2.
    ; d0 is x (fixed here)
    ; d1 is y.
    move.l gLevelData,a0
    move.b (a0),d2;  ==> pic id, can be 1 2 3 
    move.w gOffsetPicX,d0
    add.w #12,d0
    clr.l d1    
    cmp.b #1,d2
    bne .startForid2and3 
    bsr DisplayBack1_half
    add.l #16,d1
.startForid2and3   
    bsr DisplayBack2_half
    add.l #16,d1   
    bsr DisplayBack1_half
    add.l #16,d1
    bsr DisplayBack2_half
    add.l #16,d1  
    bsr DisplayBack1_half
    add.l #16,d1
    bsr DisplayBack2_half
    add.l #16,d1  
    bsr DisplayBack1_half
    add.l #16,d1
    bsr DisplayBack2_half
    add.l #16,d1  
    bsr DisplayBack1_half ; line 9 for id 1
    cmp.b #1,d2
    beq .endforid1    
    add.l #16,d1  
    bsr DisplayBack2_half ; line 9 for id 2 and 3 
.endforid1   
    rts

; --------------------------------------------
; a0 picture
; d0 x column
; d1 y line
DisplayOnePicture: ; 96*128 = 6 words
	movem.l	d0-d3,-(sp)
    ;add.l #$74,a0; skip header ; no header in this version
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    
    ;move.l a0,$100
    ;move.l a1,$104
    
    ; Blitter version.
    move.l a1,d0
    btst #0,d0 ; test if 0
    bne .odddest
    
    ; This required aligned of data of Source (data loaded) and destination.
    ; For image 1 and 3 destination is on ODD adress, so we can not blit in one time.
    ; We blit center, and then borders are copied with CPU at the same time.
    
    ; Even version
    ; Source and dest adress are even.
    move.w #$0000,d2
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#0,$DFF064	; MOD A Source
	MOVE.W	#(40-12),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace

	move.w #((128*6)<<6)+6,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    bra .end

    ; -- Odd adress version. blitter for main part
    ; The rest in CPU (for 1 and last column)
.odddest:  
    add.l   #1,a0 ; source aligned on even adress.
    add.l   #1,a1 ; dest aligned on even adress.
    move.w #$0000,d2 ; Decay A of 8 to the right
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#2,$DFF064	; MOD A Source
	MOVE.W	#(40-10),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace
	move.w #((128*6)<<6)+5,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    
    ; -- CPU part (1 and last byte)
    sub.l #1,a0
    sub.l #1,a1
    move.w #(128)-1,d3
.copy
    ;  Copy data. 12 bytes width, we only copy first and last one
    ; Plane 1
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 2
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 3
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 4
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 5
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 6
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1    
    dbra d3,.copy    

    bra .end
    
.end:   
    ; -- CPU Version
;    move.w #(128*6)-1,d3
;.copy
;    ; Copy data. 12 bytes
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+
;    move.b (a0)+,(a1)+    
;    add.l #40-12,a1
;    dbra d3,.copy
    
    
    movem.l	(sp)+,d0-d3 
    rts 

;// -------------------------------------------------------
;// Displaypic (one game picture 48*128 piwels low res)
;// -------------------------------------------------------
DisplayPic: ;void DisplayPic()
    clr.l d0
    move.w gOffsetPicX,d0 ; gOffsetPicX (w) 1 14 27
    move.l gTilesData,a0 ; TilesDatas=Level Picture (not background tile). Start of graphic data. One long, then palette (16)
    add.l #4+(16*2),a0 ; first picture (no header)
    move.l #8,d1 ; Y
    bsr DisplayOnePicture 
    move.b #0,gPicAltCurrentIndex ;	gPicAltCurrentIndex=0;
    ;move.b #0,gIconLastDisplayedId ;	gIconLastDisplayedId=0; // No icons displayed at start
    rts

;// -------------------------------------------------------
;// DisplayPicAlt
;// altindex = 0 1 2 3 4 5 6
;// Will only change the colors, not the bitmap
;// -------------------------------------------------------
DisplayPicAlt: ;void DisplayPicAlt(int altindex d0.w)
    ; Check if pic already displayed
    cmp.b gPicAltCurrentIndex,d0
    beq .end
    move.b d0,gPicAltCurrentIndex ; save for next time

    clr.l d2
    move.w d0,d2
    mulu #9216,d2 ; 12*6*128
    clr.l d0
    move.w gOffsetPicX,d0 ; gOffsetPicX (w) 1 14 27
    move.l gTilesData,a0 ; TilesDatas=Level Picture (not background tile). Start of graphic data. One long, then palette (16)
    add.l #4+(16*2),a0 ; first picture (no header)
    add.l d2,a0 ; go to alt picture
    move.l #8,d1 ; Y
    bsr DisplayOnePicture 
    ;move.b #0,gPicAltCurrentIndex ;	gPicAltCurrentIndex=0;
.end:    
    rts
    
;// -------------------------------------------------------
;// DisplayPicAltFall
;// altindex = 0 1 2 3 
;// Will only change the colors, not the bitmap
;// -------------------------------------------------------
DisplayPicAltFall:
    ; Check if pic already displayed
    cmp.b gPicAltCurrentIndex,d0
    beq .end
    move.b d0,gPicAltCurrentIndex ; save for next time
    move.b d0,d2 ; alt step
    clr.l d0 ; will be used to add to adress of dest later
    move.w gOffsetPicX,d0 ; gOffsetPicX (w) 1 14 27
    move.l gTilesData,a0 ; TilesDatas=Level Picture (not background tile). Start of graphic data. One long, then palette (16)
    add.l #4+(16*2),a0 ; first picture (no header)
    move.l #8,d1 ; Y
    bsr DisplayOnePictureFall 
.end:    
    rts 


;// -------------------------------------------------------
;// DisplayPicAltHeight4
; Call only for Alt 1 (not 0 !!) ;// altindex = (0) 1
; Only for level 11
; Picture start at line 12 (and height is 4 lines)
;// -------------------------------------------------------
DisplayPicAltHeight4: ;void DisplayPicAlt(int altindex d0.w)
    ; Check if pic already displayed
    cmp.b gPicAltCurrentIndex,d0
    beq .end
    move.b d0,gPicAltCurrentIndex ; save for next time
    clr.l d2
    move.w d0,d2
    mulu #9216,d2 ; 12*6*128, skip default picture
    clr.l d0
    move.w gOffsetPicX,d0 ; gOffsetPicX (w) 1 14 27
    move.l gTilesData,a0 ; TilesDatas=Level Picture (not background tile). Start of graphic data. One long, then palette (16)
    add.l #4+(16*2),a0 ; first picture (no header)
    add.l d2,a0 ; go to alt picture
    move.l #8,d1 ; Y
    bsr DisplayOnePictureHeight4 
.end:    
    rts
    
;// -------------------------------------------------------
;// DisplayPicAltHeight18
; Call only for Alt 1 and 2 (not 0 !!) ;// altindex = (0) 1 2
; Only for level 51
; Start line 47 (Height 19 lines)
;// -------------------------------------------------------
DisplayPicAltHeight18: ;void DisplayPicAlt(int altindex d0.w)
    ; Check if pic already displayed
    cmp.b gPicAltCurrentIndex,d0
    beq .end
    move.b d0,gPicAltCurrentIndex ; save for next time
    clr.l d2
    move.w d0,d2
    sub.w #1,d2 ; reduce 1 2 to 0 1
    mulu #19*12*6,d2
    clr.l d0
    move.w gOffsetPicX,d0 ; gOffsetPicX (w) 1 14 27
    move.l gTilesData,a0 ; TilesDatas=Level Picture (not background tile). Start of graphic data. One long, then palette (16)
    add.l #4+(16*2),a0 ; first picture (no header)
    add.l #9216,a0 ; Skip default picture
    add.l d2,a0 ; go to alt picture
    move.l #8,d1 ; Y
    bsr DisplayOnePictureHeight18 
.end:    
    rts    
    

; --------------------------------------------
; a0 picture
; d0 x column
; d1 y line
; d2 = step (0 1 2 3)
DisplayOnePictureFall:
	movem.l	a0-a3/d0-d3,-(sp)
    ;add.l #$74,a0; skip header ; no header in this version
    lea HAMPLANS,a1 ; Dst
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    
    ;move.w #$f00,$dff180
    ;move.b d2,$100
    ; Blitter version.
    
    ; This required aligned of data of Source (data loaded) and destination.
    ; For image 1 and 3 destination is on ODD adress, so we can not blit in one time.
    ; We blit center, and then borders are copied with CPU at the same time.
    
    ; Even version
    ; Source and dest adress are even. (Fall is center picture, so always in good condition)
    
    ; Animation fall.
    ; 0 1 2 3  = Full picture, decay of 4 8 12.
    move.l a0,a2
    move.l a1,a3
FALL2=12
FALL3=24
FALL4=36    
    cmp.b #0,d2
    bne .no1
    ; 1
    add.l #12*6*FALL4,a0
    move.w #(((128-FALL4)*6)<<6)+6,d6      
    ; CPU Copy. Copy FALL4 lines, A0 is source same (A2). Dest is end-FALL4
    move.w #(FALL4)-1,d3
    add.l #40*6*(128-FALL4),a3
    bra .next
.no1:
    cmp.b #1,d2
    bne .no2
    ; 2
    add.l #12*6*FALL3,a0
    move.w #(((128-FALL3)*6)<<6)+6,d6  
    ; CPU Copy. Copy FALL3 lines, A0 is source same (A2). Dest is end-FALL3
    move.w #(FALL3)-1,d3
    add.l #40*6*(128-FALL3),a3    
    bra .next
.no2:
    cmp.b #2,d2
    bne .no3
    ; 3
    add.l #12*6*FALL2,a0
    move.w #(((128-FALL2)*6)<<6)+6,d6 
    ; CPU Copy. Copy FALL2 lines, A0 is source same (A2). Dest is end-FALL2
    move.w #(FALL2)-1,d3
    add.l #40*6*(128-FALL2),a3    
    bra .next
.no3:
    cmp.b #3,d2
    bne .no4
    ; 4
    move.w #((128*6)<<6)+6,d6  
    bra .next
.no4:
 
.next
    
    move.w #$0000,d4
    OR.W	#%0000100111110000,D4 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D4,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#0,$DFF064	; MOD A Source
	MOVE.W	#(40-12),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace
	move.w d6,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
 
.testCK:

    ;move.l a3,$104

    cmp.b #3,d2
    beq .nocpu 
    ; -- CPU Version part
    ; d3 is defined above
    ;move.w d3,$102
.copy
    ; Copy data. 12 bytes
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    move.l (a2)+,(a3)+
    add.l #40-12,a3    
    dbra d3,.copy
.nocpu:  

    ;move.w #$000,$dff180  
    
    movem.l	(sp)+,a0-a3/d0-d3 
    rts 
    
; --------------------------------------------
; Only for level 11
; a0 picture
; d0 x column
; d1 y line
; Picture start at line 12 (and height is 4 lines)
DisplayOnePictureHeight4: ; 96*4 = 6 words
	movem.l	d0-d3,-(sp)
    ;add.l #$74,a0; skip header ; no header in this version
    lea HAMPLANS,a1 ; Dst
    add.w #12,d1 ; Start at line 12 of picture
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    
    ; Blitter version.
    move.l a1,d0
    btst #0,d0 ; test if 0
    bne .odddest
    
    ; This required aligned of data of Source (data loaded) and destination.
    ; For image 1 and 3 destination is on ODD adress, so we can not blit in one time.
    ; We blit center, and then borders are copied with CPU at the same time.
    
    ; Even version
    ; Source and dest adress are even.
    move.w #$0000,d2
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#0,$DFF064	; MOD A Source
	MOVE.W	#(40-12),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace

	move.w #((4*6)<<6)+6,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    bra .end

    ; -- Odd adress version. blitter for main part
    ; The rest in CPU (for 1 and last column)
.odddest:  
    add.l   #1,a0 ; source aligned on even adress.
    add.l   #1,a1 ; dest aligned on even adress.
    move.w #$0000,d2 ; Decay A of 8 to the right
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#2,$DFF064	; MOD A Source
	MOVE.W	#(40-10),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace
	move.w #((4*6)<<6)+5,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    
    ; -- CPU part (1 and last byte)
    sub.l #1,a0
    sub.l #1,a1
    move.w #(4)-1,d3
.copy
    ;  Copy data. 12 bytes width, we only copy first and last one
    ; Plane 1
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 2
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 3
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 4
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 5
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 6
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1    
    dbra d3,.copy    
    bra .end
.end:   
    movem.l	(sp)+,d0-d3 
    rts 

; --------------------------------------------
; Only for level 51
; a0 picture
; d0 x column
; d1 y line
; Start line 47 (Height 19 lines)
DisplayOnePictureHeight18: ; 96*18 = 6 words
	movem.l	d0-d3,-(sp)
    ;add.l #$74,a0; skip header ; no header in this version
    lea HAMPLANS,a1 ; Dst
    add.w #47,d1 ; Start line 54
    mulu #40*6,d1
    add.l d0,a1
    add.l d1,a1
    
    ; Blitter version.
    move.l a1,d0
    btst #0,d0 ; test if 0
    bne .odddest
    
    ; This required aligned of data of Source (data loaded) and destination.
    ; For image 1 and 3 destination is on ODD adress, so we can not blit in one time.
    ; We blit center, and then borders are copied with CPU at the same time.
    
    ; Even version
    ; Source and dest adress are even.
    move.w #$0000,d2
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#0,$DFF064	; MOD A Source
	MOVE.W	#(40-12),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace

	move.w #((19*6)<<6)+6,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    bra .end

    ; -- Odd adress version. blitter for main part
    ; The rest in CPU (for 1 and last column)
.odddest:  
    add.l   #1,a0 ; source aligned on even adress.
    add.l   #1,a1 ; dest aligned on even adress.
    move.w #$0000,d2 ; Decay A of 8 to the right
    OR.W	#%0000100111110000,D2 ; 09f0
    ;         ssss1234mmmmmmmm  
    bsr waitblitter
	MOVE.W	D2,$DFF040 ; 4 Shift Source A + 4 Dma Activation + 8 mintern    
	MOVE.W	#2,$DFF064	; MOD A Source
	MOVE.W	#(40-10),$DFF066	; MOD D Dest
    MOVE.L	#$FFFFFFFF,$DFF044 ; First word mask and last word
	MOVE.L	a0,$DFF050  ; SOURCE A
    move.l  a1,$DFF054	; dest D (screen)
	Move.w	#0,$dff042			; Decay source B + flag line trace
	move.w #((19*6)<<6)+5,$dff058 ; BltSize, height*64 , width (WORDS!!) launch transfert
    
    ; -- CPU part (1 and last byte)
    sub.l #1,a0
    sub.l #1,a1
    move.w #(19)-1,d3
.copy
    ;  Copy data. 12 bytes width, we only copy first and last one
    ; Plane 1
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 2
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 3
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 4
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 5
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1
    ; Plane 6
    move.b (a0),(a1)
    add.l #11,a0
    add.l #11,a1
    move.b (a0)+,(a1)    
    add.l #40-11,a1    
    dbra d3,.copy    
    bra .end
.end:   
    movem.l	(sp)+,d0-d3 
    rts 

    
; ----------------------------------------------------------
; u8 drawStringwithSize(u8* ptext, u8 x, u8 y, u8 totalsize)
; a0 text
; d0 nbchar
; d1 x (bytes)
; d2 y (lines)
; return d7 number of chars processed
drawStringwithSize:
	movem.l	d0-d6/a0-a6,-(sp)			;save all registers to stack

    move.l a0,a3 ; save start
    
    move.l #0,a4 ; Will be offset for bad or good alternate font
    
    Lea BannerTextsPlanes,a1
    add.l d1,a1 ; x
    mulu #40,d2
    add.l d2,a1    
    sub.w #1,d0 ; to loop exactly
drawStringwithSize_loop:    
    clr.l d3
    lea Font8,a2
    move.b (a0)+,d3 ; d3 is character code
 
    cmp.b #1,d3 ; Switch to normal color. Not counted in loop
    beq SwitchNormal
    cmp.b #2,d3 ; Switch to bold color. GOOD or BAD. Not counted in loop
    beq SwitchBold

    sub.w #32,d3 ; 0 equal to space
    mulu #8*3,d3 ; Each letter is 8*3. All letter are 152*8*3 = 3 648    
  
    add.l a4,a2 ; 0 3268 or 3268*2 ... Offset for Black/Good/bad font
    add.l d3,a2
    
    bsr DisplayLetter8
    
    add.l #1,a1 ; Next char on screen
    
    dbra d0,drawStringwithSize_loop ; Display all chars
    
    sub.l a3,a0 ; compute how much chars have been processed
    move.l a0,d7 ; send back in d3
    
	movem.l	(sp)+,d0-d6/a0-a6

    rts
    
SwitchNormal:
    move.l #0,a4 ; offset to 0
    bra drawStringwithSize_loop
    
SwitchBold:
    move.l #0,a4
    move.w textboldcolor,a4 ; offset to 3268 or 3268*2
    bra drawStringwithSize_loop
   
; ---------------------------------------------------
; DisplayLetter8
; a1 plans
; a2 font source. 3 planes side by side. Each letter.
; 152 letter (8 * 19 cells f 8x8) black. Then same for green, then same for red.
DisplayLetter8:
    movem.l	d0-d7/a0-a6,-(sp)			;save all registers to stack
    move.l a2,a0 ; font source to a0
    ; Preset 3 planes. a1 a2 a3
    move.l a1,a2
    add.l #40*61,a2
    move.l a2,a3
    add.l #40*61,a3
    move.w #8-1,d0
.drawletter
    move.b (a0)+,(a1)
    move.b (a0)+,(a2)
    move.b (a0)+,(a3)
    add.l #40,a1
    add.l #40,a2
    add.l #40,a3
    dbra d0,.drawletter
    movem.l	(sp)+,d0-d7/a0-a6
    rts
;// ------------------------------------------------------- 
; icon is 48*35
; IconFloppy ami format  
; Banner is 320x61
DisplayFloppyOnBanner: 
    movem.l	d0-d7/a0-a6,-(sp)
    lea IconFloppy,a0
    add.l #2+2+2+4+(8*2),a0 ; point to plans. 48*35 ... 6 bytes
    Lea BannerTextsPlanes,a1 
    add.l #17,a1 ; Center X
    add.l #13*40,a1 ; Center Y
    move.w #3-1,d1 ; 3 planes
.loopplanes 
    move.w #35-1,d0 ; 35 lines
.loop
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+
    move.b (a0)+,(a1)+    
    add.l #40-6,a1
    dbra d0,.loop
    add.l #(61-35)*40,a1
    dbra d1,.loopplanes
    movem.l	(sp)+,d0-d7/a0-a6
    rts
    

;// -------------------------------------------------------
;u8 GetPointedZone(u8 x, u8 y)
; d0 pointed zone. 0 if none
; x and y are from 0 to 11 and 0 to 15
; Posx (w) Posy (w) this is *2 because move 4 by 4
;// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
; id is zero if zone is not valid
;// start at 0. X to 11. Y to 15
GetPointedZone:
    lea zones,a0
    clr.l d0
    clr.l d1
    clr.l d2
    move.b gNbZones,d1
    ;mulu #5,d1 ; total (d0) = 5*gNbZones; 
    move.w Posx,d2 ; 0 to 22
    lsr.w #1,d2 ; reduce to 8x8 grid. 0 to 11
    move.w Posy,d3 ; 0 to 30
    lsr.w #1,d3 ; reduce to 8x8 grid. 0 to 15
    
    ;move.w d2,$100 ; ok. Only last row is only one 4x4 (should be 2)
    ;move.w d3,$102
    
    sub.w #1,d1 ; nb zone to check
.allzones: ;	for (i=0; i<total; i+=5)
    ; if ( zones[i+4] != 0
    cmp.b #0,4(a0)
    beq .notthisone
    ; && x >= zones[i+0]  
    cmp.b (a0),d2
    blt .notthisone
    ; && y >= zones[i+1] 
    cmp.b 1(a0),d3
    blt .notthisone
    ; && x <= zones[i+2] 
    cmp.b 2(a0),d2
    bgt .notthisone    
    ; && y <= zones[i+3] )
    cmp.b 3(a0),d3
    bgt .notthisone 
    ; return zones[i+4];
    move.b 4(a0),d0
    ;move.b d0,$102
    rts
.notthisone:
    add.l #5,a0 ; next zone
    dbra d1,.allzones
    move.w #0,d0 ;	return 0;

    rts

;// -------------------------------------------------------
;// Desactivate one zone
; zoneid = d0.b
; void MarkZoneAsUsed(u8 zoneid)
; need save d0 d1 d5 a0
MarkZoneAsUsed:
    movem.l d0-d5/a0,-(sp)
    lea zones,a0
    clr.l d2
    move.b gNbZones,d2
    sub.w #1,d2 ; nb zone to check
.allzones: ;	for (i=0; i<total; i+=5)
    ; if ( zones[i+4] == zoneid )
    cmp.b 4(a0),d0
    bne .notthisone
    move.b #0,4(a0) ; zones[i+4]=0;
    ;move.b d2,$101
.notthisone:
    add.l #5,a0 ; next zone
    dbra d2,.allzones
    movem.l (sp)+,d0-d5/a0
    rts
;// -------------------------------------------------------
;// Reset all zones
; void resetzones()
; One zone is 5 bytes
resetzones:
    move.l gLevelData,a0 ; gNbZones=gLevelData->nbzones;
    move.b 1(a0),gNbZones
    ; zones: ds.b 9*5 ;u8 zones[NBMAXZONES*5]; // start with no values
    clr.l d0
    move.b gNbZones,d0
    mulu #5,d0
    sub.w #1,d0
    move.l gZonesData,a0
    lea zones,a1
.loop ;   // Copie zones to dynamic array ;   for (i=0; i<5*gNbZones;i++)
    move.b (a0)+,(a1)+ ;	   zones[i]=gZonesData[i];
    dbra d0,.loop
    rts

;// -------------------------------------------------------
DisplayBanner: ;void DisplayBanner()
    rts

;// -------------------------------------------------------
;// Display Text
;void displayline2(u8* ptext)
; a0 text
displayline2:
    move.l #32,d0 ; nb char
    move.l #4,d1 ; x
    move.l #8,d2 ; X
    bsr drawStringwithSize  ; send back d7 number of read chars
    add.l d7,a0
    move.l #3,d1 ; x
    move.l #34,d0 ; nb char
    add.l #9,d2
    bsr drawStringwithSize  ; send back d7 number of read chars
    add.l d7,a0
    move.l #2,d1 ; x
    move.l #36,d0 ; nb char
    add.l #9,d2
    bsr drawStringwithSize  ; send back d7 number of read chars
    add.l d7,a0
    move.l #3,d1 ; x
    move.l #34,d0 ; nb char
    add.l #9,d2
    bsr drawStringwithSize  ; send back d7 number of read chars
    add.l d7,a0
    move.l #4,d1 ; x
    move.l #32,d0 ; nb char
    add.l #9,d2
    bsr drawStringwithSize  ; send back d7 number of read chars
    rts

;// -------------------------------------------------------
DisplayDeadStats: ;void DisplayDeadStats()
    ;// Display variable gNbDeath
    ;// Can go up to 9999
    ;int loop;
    ;u8* string;
    ;// Search string 0000 in death 
    ;int offsetfound=0; // Should be different than 0
    clr.l d0
    move.b #NBMESFAILTIME,d0
    lsl #2,d0
    lea gTimeUpDeathsPtr,a0
    add.l d0,a0
    move.l (a0),a0 ; A0 = string = gTimeUpDeathsPtr[NBMESFAILTIME]; 
    ; Search for '0' and '0'
    move.l a0,a1
    clr.l d0 ; loop
.search
    cmp.b #'0',(a1)
    bne .next
    cmp.b #'0',1(a1)
    bne .next    
    ; found
    bra .found
.next  
    add.l #1,a1
    add.w #1,d0
    cmp.w #178,d0
    beq .notfound
    bra .search
.found:

    ; A1 containe adress of first 0
    ; 4 numbers here. 0000
    ; Need to convert gNbDeath (w) to 4 digits.
    clr.l d1
    move.w gNbDeath,d1
    ; 1000
    divu #1000,d1
    move.b d1,(a1)
    add.b #'0',(a1)+
    mulu #1000,d1
    move.w gNbDeath,d2
    sub.w d1,d2
    move.w d2,d1
    ; 100
    divu #100,d1
    move.b d1,(a1)
    add.b #'0',(a1)+
    mulu #100,d1
    sub.w d1,d2
    move.w d2,d1 
    ; 10
    divu #10,d1
    move.b d1,(a1)
    add.b #'0',(a1)+
    mulu #10,d1
    sub.w d1,d2
    move.w d2,d1 
    ; 1   
    move.b d1,(a1)
    add.b #'0',(a1)+
    
    ; Debug
    ;sub.l #4,a1
    ;move.b (a1),$100
    ;move.b 1(a1),$101
    ;move.b 2(a1),$102
    ;move.b 3(a1),$103

    ; move.l string,a0 ; A0 is already ok
    bsr displayline2 ; displayline2(string); // Display stat message
.notfound:
    rts

;// -------------------------------------------------------
;// Erase an icon of 32x32 pixels (wich is 4 bytes width, on 4 bytes height)
;// Depend on value in gIconLastDisplayedId
EraseLastIcon: ;void EraseLastIcon() // 1=right, 2=up, 3=left 4=down, all others are in center
    bsr SpriteSetNoIcon 
    ;move.b #0,gIconLastDisplayedId ; gIconLastDisplayedId=0; // Nothing on screen
    rts
;// -------------------------------------------------------
DisplayCentralIconDeath: ;void DisplayCentralIconDeath()
    bsr SpriteSetDeadIcon
    ;move.b #5,gIconLastDisplayedId ;	gIconLastDisplayedId=5;
    rts
;// -------------------------------------------------------
DisplayCentralIconSucess: ;void DisplayCentralIconSucess()
    bsr SpriteSetSucessIcon
    ;move.b #6,gIconLastDisplayedId;	gIconLastDisplayedId=6;
    rts
;// -------------------------------------------------------
DisplayCentralIconQTEStart: ;void DisplayCentralIconQTEStart()
    bsr SpriteSetQTEActionIcon
    ;move.b #7,gIconLastDisplayedId;	gIconLastDisplayedId=7;
    rts
;// -------------------------------------------------------
DisplayCentralIconQTELeft: ;void DisplayCentralIconQTELeft()
    bsr SpriteSetQTE3Icon
    ;move.b #3,gIconLastDisplayedId ;	gIconLastDisplayedId=3;	
    rts
;// -------------------------------------------------------
DisplayCentralIconQTERight: ;void DisplayCentralIconQTERight()
    bsr SpriteSetQTE1Icon
    ;move.b #1,gIconLastDisplayedId ;	gIconLastDisplayedId=1;	
    rts
;// -------------------------------------------------------
DisplayCentralIconQTEUp: ;void DisplayCentralIconQTEUp()
    bsr SpriteSetQTE2Icon
    ;move.b #2,gIconLastDisplayedId ;	gIconLastDisplayedId=2;
    rts
;// -------------------------------------------------------
DisplayCentralIconQTEDown: ; IconQTE4 ;void DisplayCentralIconQTEDown()
    bsr SpriteSetQTE4Icon
    ;move.b #4,gIconLastDisplayedId ; gIconLastDisplayedId=4;
    rts
;// -------------------------------------------------------
;// 1=up, 2=down, 3=right, 4=left. in d0
DisplayQTEIcon: ;void DisplayQTEIcon(u8 QTEid)
    cmp.w #1,d0; if (QTEid==1)
    bne .no1
        bsr DisplayCentralIconQTEUp; DisplayCentralIconQTEUp();
        bra .no4
.no1:
    cmp.w #2,d0 ; else if (QTEid==2)
    bne .no2 
        bsr DisplayCentralIconQTEDown ; DisplayCentralIconQTEDown();
        bra .no4
.no2:
    cmp.w #3,d0 ; else if (QTEid==3)
    bne .no3 
        bsr DisplayCentralIconQTERight ; DisplayCentralIconQTERight();	
        bra .no4
.no3:
    cmp.w #4,d0 ; else if (QTEid==4)
    bne .no4 
        bsr DisplayCentralIconQTELeft ; DisplayCentralIconQTELeft();			
.no4:
    rts
    
;// -------------------------------------------------------
; d0 b imageid 1 2 3
SetOffsetImage: ;void SetOffsetImage(u8 imageid)
    move.w #1,gOffsetPicX
    cmp.b #2,d0
    bne .no2
    move.w #14,gOffsetPicX
    bra .no3
.no2
    cmp.b #3,d0
    bne .no3
    move.w #27,gOffsetPicX
.no3
    rts

;// -------------------------------------------------------
;// Update time
; Managed time in frames. gTime is seconds, gTimeFrag is 1/50 of second
UpdateTime: ;void UpdateTime()
    ; // Count time. 50th of seconds
    add.b #1,gTimeFrag ; gTimeFrag++;
    add.b #1,gTimePrecise ; gTimePrecise++;
    cmp.b #50,gTimeFrag; if (gTimeFrag==50)
    bne .no50
        add.b #1,gTime ; gTime++;
        move.b #0,gTimeFrag ; gTimeFrag=0;
.no50:        
    rts
;// -------------------------------------------------------
;// Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE 4=END LEVEL (go to next level)
;// 249 Change QTE mode 0 1
;// 250 Display text
;// 251 QTE
;// 252 Change cursor mode. 0 to 5
;// 253 Change palette mode. 0 1 2 3 4 
;// 254 Display Background
;// 255 End
;// 199 usually used for untimed game 
InitNextAction: ;void InitNextAction()
    ; d0 = nextaction 
    ; a0 = gCurrentScenarioLine
    move.l gCurrentScenarioLine,a0
    clr.w d0
    move.b (a0),d0 ; u8 nextaction = gCurrentScenarioLine[0]; // Time or next action 
    ;move.b d0,$101
    ; // -- Test special function for changing flags. They graphically do not do nothing.
    ; // We skip these lines as they do not require main loop to run.
.while: ; while ( nextaction == 249 || ( nextaction >= 252 && nextaction <= 254 ) )
        cmp.b #252,d0 ;		if ( nextaction == 252 ) // Change cursor mode 0 to 5
        bne .no252
            move.b 1(a0),gCursorMode ; gCursorMode=gCurrentScenarioLine[1];
            add.l #10,gCurrentScenarioLine; gCurrentScenarioLine+=10;
            move.l gCurrentScenarioLine,a0
            move.b (a0),d0 ; nextaction = gCurrentScenarioLine[0]; // Time or next action 
            bra .textwhile
.no252:
		cmp.b #253,d0 ; else if ( nextaction == 253 ) // Change Palette mode 0 1 2 3 4 5 6 7
        bne .no253
        move.b 1(a0),gPaletteMode ; gPaletteMode=gCurrentScenarioLine[1]; // 0=normal 1=blink 2=smoke 3=smoke&blood 
            ; //4=anim palette fall 5=river 6=diablo 7=fire 8=bomb 9=Camera blink
            move.b #10,gPaletteNextSwap ; gPaletteNextSwap=10; // >0 in frame
            move.b #1,gNextPalette ; gNextPalette=1; // 0=Palette 1=PaletteAlt		
            add.l #10,gCurrentScenarioLine; gCurrentScenarioLine+=10;
            move.l gCurrentScenarioLine,a0
            move.b (a0),d0 ; nextaction = gCurrentScenarioLine[0]; // Time or next action 
            ; // Specific modes, only one palette
            cmp.b #0,gSkippingLevel; if (!gSkippingLevel)
            bne .skip1
            cmp.b #0,gPaletteMode;  if (gPaletteMode==0)
            bne .no0
                move.w #0,d0 ; DisplayPicAlt(0); // Level 42 normal picture
                bsr DisplayPicAlt
.no0
            cmp.b #2,gPaletteMode ;  else if (gPaletteMode==2)
            bne .no1
                move.w #1,d0 ;   DisplayPicAlt(1); // Level 42 smoke			
                bsr DisplayPicAlt
.no1            
            cmp.b #3,gPaletteMode ;  else if (gPaletteMode==3)
            bne .no2
                move.w #2,d0 ;  DisplayPicAlt(2); // Level 42 blood		
                bsr DisplayPicAlt
.no2             
.skip1            
            bra .textwhile
.no253:
        cmp.b #249,d0 ; else if ( nextaction == 249 ) // Change QTE mode 
        bne .no249
            move.b 1(a0),gQte_auto_mode ; gQte_auto_mode=gCurrentScenarioLine[1];
            move.b #142,gQte_next ; gQte_next = 142 + (rand() % 64); // 142 + 64... difficulty medium
            bsr GetRandom
            and.w #64-1,d0
            add.b d0,gQte_next
            add.l #10,gCurrentScenarioLine; gCurrentScenarioLine+=10;
            move.l gCurrentScenarioLine,a0
            move.b (a0),d0 ; u8 nextaction = gCurrentScenarioLine[0]; // Time or next action 
            bra .textwhile
.no249:
        cmp.b #254,d0 ; else if ( nextaction == 254 ) // end, display background
        bne .no254
			; Erase background. Do nothing on amiga, do to next line (will be end). Used in 3 levels 61 62 81
            add.l #10,gCurrentScenarioLine; gCurrentScenarioLine+=10;
            move.l gCurrentScenarioLine,a0
            move.b (a0),d0 ; u8 nextaction = gCurrentScenarioLine[0]; // Time or next action 
            bra .textwhile
.no254:
	
.textwhile:
        move.l gCurrentScenarioLine,a0
        move.b (a0),d0 ; nextaction = gCurrentScenarioLine[0]; // Time or next action 
        ; while ( nextaction == 249 || ( nextaction >= 252 && nextaction <= 254 ) )
        cmp.b #249,d0
        beq .while
        cmp.b #252,d0
        beq .while
        cmp.b #253,d0
        beq .while  
        cmp.b #254,d0
        beq .while        
	
    ; // -- Do we have a game command ? ------------------
    cmp.w #240,d0 ;	if ( nextaction < 240 ) // Command
    bge .no240
        move.b #0,gamestate ; gamestate=0; // Default is using cursor
        ;move.b #$ff,$102
        cmp.b #0,gSkippingLevel; if (!gSkippingLevel)
        bne .skip
            bsr InitGauge ; InitGauge(nextaction); // Playing. Nextaction is time (d0.w)
.skip
        rts
.no240:

;	// -- Do I need to erase cursor ?	
;	//if (gNoNeedToEraseCursor==0)
;	//	erasecursor(xold,yold); // Erase cursor, we do not need it	

;	// -- Process commands
    cmp.b #250,d0 ;	if (  nextaction == 250 ) // Text $fa
    bne .no250
        move.b #2,gamestate ; gamestate = 2; // Text
        ;move.b gamestate,$103 ; gamestate = 2; // Text
        ;move.b #$fe,$102
        bra .endcommands
.no250:

    cmp.b #251,d0 ;	else if ( nextaction == 251 ) // QTE
    bne .no251
        cmp.b #1,gQTEForceNoWait; if (gQTEForceNoWait)
        bne .nogQTEForceNoWait
			move.b #3,gamestate ; gamestate = 3; // Start QTE gamestate. Level 92 only
            bra .nogQTEForceNoWait2
.nogQTEForceNoWait: ; else
			move.b #7,gamestate ; gamestate = 7 ; // Wait Key before starting QTE
.nogQTEForceNoWait2:
		move.b #1, QTEstate ; QTEstate=1; // Start the QTE sequence (here from beginning, with two Warning icon)
        move.l gCurrentScenarioLine,a0 ; if ( gCurrentScenarioLine[1]<50) // If QTE time is quick, then only display one warning icon.
        cmp.b #50,1(a0)
        bpl .noQteInf5
            move.b #3, QTEstate ; QTEstate=3;
.noQteInf5:
        move.b #0,gTimePrecise ; gTimePrecise=0; // Reset precise time
        cmp.b #0,gSkippingLevel; if (!gSkippingLevel)
        bne .skip3
            bsr DisplayCentralIconQTEStart ; DisplayCentralIconQTEStart(); // Icone for saying it is starting (to be done after)		
            bsr PlaySoundWarning ; PlaySoundWarning();
.skip3
        bra .endcommands
.no251:

    cmp.b #248,d0 ;	else if ( nextaction == 248 ) // Display stats
    bne .no248
		move.b #5,gamestate ; gamestate=5; // Display stats.
        bra .endcommands
.no248:

    cmp.b #255,d0 ;	else if ( nextaction == 255 ) // END
    bne .no255
		move.b #4,gamestate ; gamestate = 4;	
        bsr EraseLastIcon ; 
        bsr DisplayCentralIconSucess ; DisplaySucessIcon; ;		DisplayCentralIconSucess();
        bsr PlaySoundGood ; PlaySoundGood();
.no255:

.endcommands:
    rts

;// -------------------------------------------------------
;// Init Gauge
; void InitGauge(u8 totaltime)
; d0.w time (byte but word is clean)
InitGauge:
    movem.l d0/a0,-(sp) ; Save d0 & a0
    ;move.b d0,$100
    cmp.b #0,d0
    beq .gaugenotinuse    
    btst.b #7,d0 ; if (totaltime==0 || totaltime > 128) ; test bit 7 to 0 (True if 0) (so if > 128)
    beq .gaugeinuse
.gaugenotinuse    
        ;move.b #$ff,$101
        move.b #0,gGaugeInUse ; gGaugeInUse=0;
        bra .exit; return;
.gaugeinuse:
    ; // Init Gauge.
    ; // Start value is u8, in 10th of second. Means 10=1 seconds. 200 = 20 secondes.
    ; // Gauge is 128 is 128 pixels. Stored in u16 with 8 bits for seconds and 8 bits for fragments of seconds.
    ; // Gauge is updated 50 time a frame. Means one update is 0.02s.
    ; // Total time in s is time/10. Divided by 0.02 we get number of step (so same as x50).
    ; // 128*256/nbsteps = speed. 
    ; // Speed = 6553 / time. (approximated to have integer)
    move.w #6553,d1 ; gGaugeSpeed ; gGaugeSpeed = 6553 / totaltime;
    divu.w d0,d1
    move.w d1,gGaugeSpeed
    move.w #128*256-1,gGauge; gGauge = 128 * 256; // Gauge start full. $8000-1 (to avoid sign problems)
    move.b #0,gGaugeEnded ; gGaugeEnded=0;
    ; TODO : Fill gauge
    ;move.w #$3c,d0 ;  EraseAndStopGauge(0x3C); // graphic init (colors) and fill full gauge
    ;bsr EraseAndStopGauge ;
    bsr EnableSpriteGauge 
    bsr UpdateAndDisplayGauge ; Update once
    ;move.b #1,gGaugeInUse ; gGaugeInUse=1;	
    move.b #1,gGaugeReadyToStart ; Ask Gauge to start, as soon as mouse move
    ;move.b #$fe,$101
.exit	
    movem.l (sp)+,d0/a0
    rts
;// -------------------------------------------------------
; fillvalue d0.w
EraseAndStopGauge: ;void EraseAndStopGauge(u8 fillvalue) // Display black gauge or full gauge
    bsr DisableSpriteJauge ; erase gauge
    move.b #0,gGaugeInUse ; gGaugeInUse=0;
    rts
;// -------------------------------------------------------
;// Gauge
UpdateAndDisplayGauge: ;void DisplayGauge()
    ;move.w gGauge,$100
    ;move.w gGaugeSpeed,$102
    ; -- Manage Gauge
    cmp.b #1,gGaugeEnded ;	if (gGaugeEnded==1)
    beq .UpdateAndDisplayGauge_end ; return;
    move.w gGaugeSpeed,d0 ; example $0036
    cmp.w gGauge,d0 ; if (gGauge>=gGaugeSpeed) ... gGauge start at $8000
    bgt .gaugeend
    sub.w d0,gGauge ; gGauge -= gGaugeSpeed;
    bra .next
.gaugeend:
    move.b #1,gGaugeEnded ; // End if timer. ; gGaugeEnded=1
.next:
	
    ; -- Display gauge
    ; Display black during 128-switchpoint
    ; And display gauge during switchpoint-0
    move.w gGauge,d0 
    lsr #8,d0 ; switchpoint=(gGauge>>8); // Switch point is 128 to 0 
    move.w #128,d1
    sub.w   d0,d1 ; 128 - switchpoint .... Switch is 0 to 128. (0 top, 128 is bottom.
    
    ; Update sprite color ... d1 = 0 , yellow (ff0) .... d1 > 64 = Orange (FA0), D1 > 96 Red (f00).
    ; JaugeColor+2
    lea JaugeColor+2,a0
    move.w #$ff0,(a0) ; Yellow
    cmp.w #96,d1
    bmi .notred
    move.w #$f00,(a0) ; Red
    bra .endcolormanagment  
.notred
    cmp.w #64,d1
    bmi .endcolormanagment  
    move.w #$fA0,(a0) ; Orange
.endcolormanagment    
    
    lea Gauge_sprite,a0
    add.l #4+4+2,a0 ; jump header and first line and point to second word
    ; Display black
    cmp.w #0,d1
    beq .noblack
    sub.w #1,d1
    cmp.w #0,d1
    beq .noblack    
.loop1:
    move.b #0,(a0)
    add.l #4,a0
    dbra d1,.loop1
.noblack
    ; Display Gauge
    cmp.w #0,d0
    beq .nogauge
    sub.w #1,d0
    cmp.w #0,d0
    beq .nogauge    
.loop2:
    move.b #$78,(a0)
    add.l #4,a0
    dbra d0,.loop2
.nogauge   
.UpdateAndDisplayGauge_end:
    rts
;// -------------------------------------------------------
; HAMPLANS: ds.b    144*40*6 ; 34Kb
EraseTopScreen: ;void EraseTopScreen()
    move.w #(144*40*6)/16,d0
    lea HAMPLANS,a0
    sub.w #1,d0
.erase
    clr.l (a0)+
    clr.l (a0)+
    clr.l (a0)+
    clr.l (a0)+
    dbra d0,.erase
    rts

;// -------------------------------------------------------
; BannerTextsPlanes: ds.b    40*61*3; 320x61 8 colors
EraseBanner: ; void EraseBanner()
    move.w #40*61*3-1,d0
    lea BannerTextsPlanes,a0
.erase
    clr.b (a0)+
    dbra d0,.erase
    rts

;// -------------------------------------------------------	
PlaySoundGood: ;void PlaySoundGood()
    cmp.b #1,gSkippingLevel ; if (gSkippingLevel)
    beq .end
    cmp.b #1,gSoundGoodHaveBeenPlayedThisFrame ; if (gSoundGoodHaveBeenPlayedThisFrame)
    beq .end ; return
    
    ; Play sample
    lea $dff000,a6
    lea SfxGood_Desc,a0
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)
    
    move.b #1,gSoundGoodHaveBeenPlayedThisFrame ;	gSoundGoodHaveBeenPlayedThisFrame=1;
.end:
    rts
;// -------------------------------------------------------	
PlaySoundBad: ;void PlaySoundBad()
    lea $dff000,a6
    lea SfxBad_Desc,a0
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)
    rts
;// -------------------------------------------------------	
PlaySoundClick: 
    lea $dff000,a6
    lea SfxClick_Desc,a0
    jsr _mt_playfx
    rts
;// -------------------------------------------------------	
PlaySoundWarning: ;void PlaySoundWarning()
    cmp.b #0,gSkippingLevel;	if (gSkippingLevel)
    bne .nosoundwarning
    lea $dff000,a6
    lea SfxWarning_Desc,a0
    jsr _mt_playfx ; channelStatus(d0) = _mt_playfx(a6=CUSTOM, a0=SfxStructurePointer)
.nosoundwarning
   rts
   
;// -------------------------------------------------------
;// Digit voice (Ninja Shout) 		
PlayRandomSound: ;void PlayRandomSound()
    clr.l d0
.rand    
    bsr GetRandom ; d0.w
    ;move.w d0,$104
    ; 5 sounds
    and.w #$0007,d0 ; Keep 7 values
    cmp.w #5,d0
    bmi .nosub
    sub.w #3,d0 ; Reduce 0-7 to 0-4
.nosub    
    cmp.b gSfxLastRand,d0
    beq .rand ; loop until value is different
    move.b d0,gSfxLastRand

    lea $dff000,a6
    ;move.b d0,$101
    cmp.w #0,d0
    beq PlayRandomSound_Playshout1
    cmp.w #1,d0
    beq PlayRandomSound_Playshout2    
    cmp.w #2,d0
    beq PlayRandomSound_Playshout3
    cmp.w #3,d0
    beq PlayRandomSound_Playshout4
    cmp.w #4,d0
    beq PlayRandomSound_Playshout5
    rts
;// -------------------------------------------------------    
PlayRandomSound_Playshout1:    
    lea SfxShout1_Desc,a0
    bsr GetRandom ; d0.w
    and.w #$003f,d0 ; Keep 0-63 values
    add.w #275,d0
    move.w d0,6(a0) ; a0 + 6 = word with pitch. 275 + random 64
    jsr _mt_playfx
    rts
PlayRandomSound_Playshout2:    
    lea SfxShout2_Desc,a0
    bsr GetRandom ; d0.w
    and.w #$003f,d0 ; Keep 0-63 values
    add.w #275,d0
    move.w d0,6(a0) ; a0 + 6 = word with pitch. 275 + random 64    
    jsr _mt_playfx
    rts
PlayRandomSound_Playshout3:    
    lea SfxShout3_Desc,a0
    bsr GetRandom ; d0.w
    and.w #$003f,d0 ; Keep 0-63 values
    add.w #275,d0
    move.w d0,6(a0) ; a0 + 6 = word with pitch. 275 + random 64    
    jsr _mt_playfx
    rts
PlayRandomSound_Playshout4:    
    lea SfxShout4_Desc,a0
    bsr GetRandom ; d0.w
    and.w #$003f,d0 ; Keep 0-63 values
    add.w #275,d0
    move.w d0,6(a0) ; a0 + 6 = word with pitch. 275 + random 64    
    jsr _mt_playfx
    rts
PlayRandomSound_Playshout5:    
    lea SfxShout5_Desc,a0
    bsr GetRandom ; d0.w
    and.w #$003f,d0 ; Keep 0-63 values
    add.w #275,d0
    move.w d0,6(a0) ; a0 + 6 = word with pitch. 275 + random 64    
    jsr _mt_playfx
    rts 
    
;PlaySoundTest:
;    bsr PlaySoundGood
;    rts
    
SfxGood_Desc:
    dc.l sound_good_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_good_raw_end-sound_good_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 150    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)

SfxBad_Desc:
    dc.l sound_bad_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_bad_raw_end-sound_bad_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 600    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)   

SfxClick_Desc:
    dc.l sound_click_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_click_raw_end-sound_click_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 150    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)

SfxWarning_Desc:
    dc.l sound_warning_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_warning_raw_end-sound_warning_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 600    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127) 
  
SfxIntroResistance_Desc:
    dc.l 0    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w 11172/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 350    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)

SfxIntroNinjaCarnage_Desc:
    dc.l 0    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w 29331/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 350    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)
    
    ; -- sounds Shout
    
SfxShout1_Desc:
    dc.l sound_shout1_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_shout1_raw_end-sound_shout1_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 250    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)     

SfxShout2_Desc:
    dc.l sound_shout2_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_shout2_raw_end-sound_shout2_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 250    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)  

SfxShout3_Desc:
    dc.l sound_shout3_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_shout3_raw_end-sound_shout3_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 250    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)      

SfxShout4_Desc:
    dc.l sound_shout4_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_shout4_raw_end-sound_shout4_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 250    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)  

SfxShout5_Desc:
    dc.l sound_shout5_raw    ;     void *sfx_ptr  (pointer to sample start in Chip RAM, even address)
    dc.w (sound_shout5_raw_end-sound_shout5_raw)/2    ;     WORD  sfx_len  (sample length in words)
    dc.w 250    ;     WORD  sfx_per  (hardware replay period for sample)(lower value is higher pitch)
    dc.w 64    ;     WORD  sfx_vol  (volume 0..64, is unaffected by the song's master volume)
    dc.b 3    ;     BYTE  sfx_cha  (0..3 selected replay channel, -1 selects best channel)
    dc.b 127    ;     BYTE  sfx_pri  (priority, must be in the range 1..127)  

;// -------------------------------------------------------
;// UpdatePaletteMode
;// Called each frame, at start of interrupt.
;// This will replace the color page (so can have problem with icons)
UpdatePaletteMode: ;void UpdatePaletteMode()

    ; -- Palette mode 1
    cmp.b #1,gPaletteMode ;	if (gPaletteMode==1) // Blink (Level 52)
    bne .no1
        sub.b #1,gPaletteNextSwap ; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap; if (gPaletteNextSwap==0)
        bne .nopalswap
        move.b #5,gPaletteNextSwap; gPaletteNextSwap=5 + (rand()%16) ; // (cpct_getRandom_lcg_u8(0) >> 4); // 0 to 16
        bsr GetRandom
        and.w #$16-1,d0
        add.b d0,gPaletteNextSwap
        add.b #1,gNextPalette ; gNextPalette = !gNextPalette; 0 1
        and.b #$1,gNextPalette
.nopalswap
        cmp.b #1,gNextPalette ; if (gNextPalette==1)
        bne .alt0
            move.w #1,d0
            bsr DisplayPicAlt ; DisplayPicAlt(1); // cpct_setPalette(palettedark, 16); 
            bra .no1
.alt0 ;		else
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); // cpct_setPalette(palette, 16); 
.no1

    ; -- Palette mode 4
    cmp.b #4,gPaletteMode ;	else if (gPaletteMode==4) // Fall
    bne .no4
        sub.b #1,gPaletteNextSwap ; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap; if (gPaletteNextSwap==0)
        bne .nopalswap1
            move.b #7,gPaletteNextSwap ; gPaletteNextSwap=7;
            sub.b #1,gNextPalette ; gNextPalette--;
            and.b #4-1,gNextPalette ; gNextPalette = gNextPalette % 4;
.nopalswap1:	

        cmp.b #0,gNextPalette ; if (gNextPalette==0)
        bne .no1a
            move.w #0,d0
            bsr DisplayPicAltFall; DisplayPicAlt(0); // cpct_setPalette(paletteFall1, 16); 
        bra .no4
.no1a
        cmp.b #1,gNextPalette ; else if (gNextPalette==1)
        bne .no2a
            move.w #1,d0
            bsr DisplayPicAltFall ; DisplayPicAlt(1); // cpct_setPalette(paletteFall2, 16); 
        bra .no4
.no2a
        cmp.b #2,gNextPalette ; else if (gNextPalette==2)
        bne .no3a
            move.w #2,d0
            bsr DisplayPicAltFall ; DisplayPicAlt(2); // cpct_setPalette(paletteFall3, 16);
        bra .no4
.no3a
        cmp.b #3,gNextPalette; else if (gNextPalette==3)
        bne .no4
            move.w #3,d0
            bsr DisplayPicAltFall ; DisplayPicAlt(3); // cpct_setPalette(paletteFall4, 16); 
.no4	

    ; -- Palette mode 5 
    cmp.b #5,gPaletteMode ;	else if (gPaletteMode==5) // river
    bne .no5    
        sub.b #1,gPaletteNextSwap; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap ; if (gPaletteNextSwap==0)
        bne .nopalswap2
            move.b #7,gPaletteNextSwap ; gPaletteNextSwap=7;
            add.b #1,gNextPalette ; gNextPalette++;
            cmp.b #3,gNextPalette; gNextPalette = gNextPalette % 3;
            bne .nopalswap2
            move.b #0,gNextPalette
.nopalswap2:	
        cmp.b #0,gNextPalette ; if (gNextPalette==0)
        bne .no1b
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); // cpct_setPalette(paletteRiver3, 16); 
            bra .no5
.no1b
        cmp.b #1,gNextPalette ; else if (gNextPalette==1)
        bne .no2b
            move.w #1,d0
            bsr DisplayPicAlt ; DisplayPicAlt(1); // cpct_setPalette(paletteRiver1, 16); 
            bra .no5
.no2b
        cmp.b #2,gNextPalette ; else if (gNextPalette==2)
        bne .no5
            move.w #2,d0
            bsr DisplayPicAlt ; DisplayPicAlt(2); // cpct_setPalette(paletteRiver2, 16);
.no5 

    ; -- Palette mode 6
    cmp.b #6,gPaletteMode ;	else if (gPaletteMode==6) // Diablo
    bne .no6
        sub.b #1,gPaletteNextSwap ; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap  ; if (gPaletteNextSwap==0)
        bne .nopalswap3
        move.b #20,gPaletteNextSwap ; gPaletteNextSwap= 20 + (rand()%32);
        bsr GetRandom
        and.b #32-1,d0
        add.b d0,gPaletteNextSwap
        add.b #1,gNextPalette ; gNextPalette = !gNextPalette;
        and.b #$1,gNextPalette
.nopalswap3:
        cmp.b #1,gNextPalette ; if (gNextPalette==1)
        bne .no1d
            move.w #1,d0
            bsr DisplayPicAlt ; DisplayPicAlt(1); // cpct_setPalette(paletteDiablo, 16); 
            bra .no6
.no1d
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); // cpct_setPalette(palette, 16); 
.no6

    ; -- Palette mode 7
    cmp.b #7,gPaletteMode ;	else if (gPaletteMode==7) // Fire
    bne .no7
        sub.b #1,gPaletteNextSwap ; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap  ; if (gPaletteNextSwap==0)
        bne .nopalswap4
            move.b #7,gPaletteNextSwap ; gPaletteNextSwap=7;
            add.b #1,gNextPalette; gNextPalette++;
            cmp.b #3,gNextPalette; gNextPalette = gNextPalette % 3;
            bne .nopalswap4
            move.b #0,gNextPalette
.nopalswap4:	
        cmp.b #0,gNextPalette ; if (gNextPalette==0)
        bne .no1e
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); //cpct_setPalette(palette, 16); 
            bra .no7
.no1e
        cmp.b #1,gNextPalette ; else if (gNextPalette==1)
        bne .no2e
            move.w #1,d0
            bsr DisplayPicAlt ; DisplayPicAlt(1); //cpct_setPalette(paletteFire2, 16); 
            bra .no7
.no2e
        cmp.b #2,gNextPalette ; else if (gNextPalette==2)
        bne .no7
            move.w #2,d0
            bsr DisplayPicAlt ; DisplayPicAlt(2); //cpct_setPalette(paletteFire3, 16);
.no7:


    ; -- Palette mode 8  (Bomb Figures cycling). , bomb numbers
    ; Full picture, Variation 1 (19 lines) Variation 2 (19 lines)
    cmp.b #8,gPaletteMode ;	else if (gPaletteMode==7) // Fire
    bne .no8    
        sub.b #1,gPaletteNextSwap; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap ; if (gPaletteNextSwap==0)
        bne .nopalswap8
            move.b #7,gPaletteNextSwap ; gPaletteNextSwap=7;
            add.b #1,gNextPalette ; gNextPalette++;
            cmp.b #3,gNextPalette; gNextPalette = gNextPalette % 3;
            bne .nopalswap8
            move.b #0,gNextPalette
.nopalswap8:	
        cmp.b #0,gNextPalette ; if (gNextPalette==0)
        bne .no1f
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); 
            bra .no8
.no1f
        cmp.b #1,gNextPalette ; else if (gNextPalette==1)
        bne .no2f
            move.w #1,d0
            bsr DisplayPicAltHeight18 ; DisplayPicAlt(1);
            bra .no8
.no2f
        cmp.b #2,gNextPalette ; else if (gNextPalette==2)
        bne .no8
            move.w #2,d0
            bsr DisplayPicAltHeight18 ; DisplayPicAlt(2);
.no8: 


    ; Level 11 point cycling.
    ; Palette mode 9
    ; Special case as starting from intro and stop at first good action.
    cmp.b #9,gPaletteMode ;	else if (gPaletteMode==9) // Blinking point
    bne .no9
        sub.b #1,gPaletteNextSwap ; gPaletteNextSwap--;
        cmp.b #0,gPaletteNextSwap  ; if (gPaletteNextSwap==0)
        bne .nopalswap9
        move.b #20,gPaletteNextSwap ; gPaletteNextSwap= 20 + (rand()%32);
        bsr GetRandom
        and.b #32-1,d0
        add.b d0,gPaletteNextSwap
        add.b #1,gNextPalette ; gNextPalette = !gNextPalette;
        and.b #$1,gNextPalette
.nopalswap9:
        cmp.b #1,gNextPalette ; if (gNextPalette==1)
        bne .no1g
            move.w #1,d0
            bsr DisplayPicAltHeight4 ; DisplayPicAlt(1); // Camera point, 4 lines
            bra .no9
.no1g
            move.w #0,d0
            bsr DisplayPicAlt ; DisplayPicAlt(0); // cpct_setPalette(palette, 16); 
.no9

    rts
    
;// -------------------------------------------------------
;//
;// MAIN LOOP
;//
;// -------------------------------------------------------

; d0 new value, d1 old value, a0 byte dest, d2 value to set
; return d0 to say if value have been set or not
CheckUpTransitionAndSetValue:
    ; new value should be 1, last value should be 0
    cmp.b #1,d0
    bne .no
    cmp.b #0,d1
    bne .no
    move.b d2,(a0) ; Copy value
    move.b #1,d0 ; set
    rts
.no:
    move.b #0,d0 ; not set
    rts    
    
; ---------------------------------------------------  
InitGame:
    ; // Init vars
    clr.b gSfxDeathCount ;	gSfxDeathCount=0;
    clr.w gNbDeath;	gNbDeath=0; // Reset total number of death
    clr.b gMessageFailTimeCurrent ;	gMessageFailTimeCurrent=0;
    move.w #128,d0
    lsl #8,d0
    move.w d0,gGauge ;	gGauge=(128<<8);
    move.w #1,d0
    lsl #8,d0
    move.w d0,gGaugeSpeed ; gGaugeSpeed=(1<<8);
    move.b #0,gTime ; gTime=0; // Seconds
    move.b #0,gTimeFrag ; gTimeFrag=0; // 50th of seconds
    move.b #0,gTimePrecise ; gTimePrecise=0; // Frame precise (0 to 255) 
    move.b #0,gQTEForceNoWait ; gQTEForceNoWait=0; // For level 92
    move.b #0,gSkippingLevel ; gSkippingLevel=0;
    move.b #0,gCheatEnable; gCheatEnable=1; // Force cheat code to be enable at start
    if ALLOWSKIPLEVEL==1
    move.b #1,gCheatEnable ; // Allow user to skip level with key N
    endc

    move.b #0,firepressed
    move.b #0,iskeyup
    move.b #0,iskeydown
    move.b #0,iskeyleft
    move.b #0,iskeyright
    
    move.b #0,gCreditLooping ; To avoid relaunching music at end.
    
    move.b #0,gCheatSequenceCorrectLetters

    rts

; ---------------------------------------------------  
; use CurrentLevelName to find CurrentLevelInfoPtr
UpdateLevelInfoPointerFromName:
        lea CurrentLevelName,a0
        move.b (a0),d0
        move.b 1(a0),d1
        lea LevelsInfos,a0
        move.l a0,CurrentLevelInfoPtr
.loop:
        cmp.b #$ff,(a0) ; end of list, should never happend
        beq .gotit  
        cmp.b (a0),d0
        bne .nextloop
        cmp.b 1(a0),d1
        bne .nextloop 
.gotit ; We got it
        move.l a0,CurrentLevelInfoPtr
        bra .endlevelinfo
.nextloop:
        add.l #5,a0 ; next level
        bra .loop
.endlevelinfo:  
        rts

; ---------------------------------------------------
; gTimeUpDeathsPtr 7 long, contain adress to 7 texts.
; TimeUpOffset
InitTimeUpTexts:
    ; // Init ptr
    ; 7 (words) for 7 langages
    lea TimeUpOffset,a2
    clr.l d0
    move.b CurrentLangage,d0
    mulu #7*2,d0 ; We got offset of the table (70 offsets)
    add.l d0,a2 ; 7 words offsets here (from beginning of data file, means LoadedLevel)
    
    lea gTimeUpDeathsPtr,a1
    
    move.w #7-1,d1
.loop
    lea TimeUpOffset,a0
    clr.l d0
    move.w (a2)+,d0
    add.l d0,a0
    move.l a0,(a1)+ ; Save ptr to text TimeUp
    dbra d1,.loop
    rts

; ---------------------------------------------------   
InitLevel:    

    bsr UpdateLevelInfoPointerFromName
    
    ; -- If name finish by '1' then we need to init tiles and palettes (different for each 3 files)
    lea CurrentLevelName,a0
    cmp.b #'1',1(a0) ; if (gLevelData->position==1) // If first image, then redisplay background
    bne .nofirstpic
    
    bsr EraseTopScreen
    bsr EraseLastIcon ; Disable the last "sucess icon"
    
    bsr CopyBackgroundTiles ; Copy background tile to commun tile space.
    
    bsr StoreRGBPaletteFromHAMIFF ; will compute HAMPalette
    
    ; Init HAM
    ; 16 colors ham
    lea HAMPalette,a0
    lea HAMPalettePtr,a1 ; dest
    add.l #2,a1
    move.w #16-1,d0
.loopcopycolors
    move.w (a0)+,(a1)
    add.l #4,a1
    dbra d0,.loopcopycolors

    bsr DisplayBackground ; DisplayBackground();  
.nofirstpic: 
    ; end init palettes and tiles.
 
    bsr DisplayFloppyOnBanner ; Display loading icon
    
    if NOMUSIC==0 
    cmp.b #1,gCreditLooping ; when looping on credits
    beq .nochangemusic 
    cmp.l #ChangeModule,CurrentLevelInfoPtr
    bne .no_changemodule
    ; load second module
    bsr LoadMusic2
.no_changemodule:                    
    ; // Init next music to play
    move.l CurrentLevelInfoPtr,a0
    cmp.b #-1,3(a0) ; Pattern of module
    beq .nochangemusic
    ; Change music inside same module.
    lea $dff000,a6
    jsr _mt_end
    move.l ModuleLoadedTrack,a0
    move.l ModuleLoadedSamples,a1
    move.l CurrentLevelInfoPtr,a4
    move.b 3(a4),d0  ; Tuto . Level 0
    jsr _mt_init ; _mt_init(a6=CUSTOM, a0=TrackerModule, a1=Samples|NULL, d0=InitialSongPos.b) 
    move.b #1,_mt_Enable ; Start music
    move.b #3,_mt_MusicChannels  ; 3 channels for music 
.nochangemusic          
    endc

    ; -- Free any previous level data.
    cmp.l #0,LoadedLevel
    beq .nothingtofree
    move.l (LDOS_BASE).w,a6
	move.w #0,d1 ; Type LEVEL
    jsr LDOS_FREE_MEM_DATA(a6) ; TODO See freeMemoryArea
.nothingtofree

    ; -- Load level
    ; Load Picture data
    move.l (LDOS_BASE).w,a6
    move.l CurrentLevelInfoPtr,a0
    clr.w d0
	move.b 4(a0),d0 ; track where file is
	move.w #0,d1 ; Type LEVEL
    jsr LDOS_DATA_LOAD(a6) ; Blocking function. d0.l = id of file. Return a0 adress of file (we know its size)
    move.l a0,LoadedLevel
    ; The data loaded are tagged with MEMLABEL_DATALEVEL
    
    ; Level start with 70 offsets (words) for 7 langages
    clr.l d0
    move.b CurrentLangage,d0
    mulu #140,d0 ; We got offset of the table (70 offsets)
    move.l a0,a1
    add.l d0,a1 ; 70 words offsets here (from beginning of data file, means LoadedLevel)

    ; // Init pointers (offset are inside buffer)
    clr.l d0
    ; gLevelData
    move.l a0,gLevelData
    move.w (a1)+,d0
    add.l d0,gLevelData ; ok

    ; gNextLevelName = gLevelBuffer + ptrvalue; //u8* gNextLevelName;
    move.l a0,gNextLevelName
    move.w (a1)+,d0
    add.l d0,gNextLevelName
    
    ; gZonesData = gLevelBuffer + ptrvalue; //u8* gZonesData;
    move.l a0,gZonesData
    move.w (a1)+,d0
    add.l d0,gZonesData
    
    ; gScenario = gLevelBuffer + ptrvalue; //u8* gScenario;
    move.l a0,gScenario
    move.w (a1)+,d0
    add.l d0,gScenario
    
    ; gMessageIntro = gLevelBuffer + ptrvalue;
    move.l a0,gMessageIntro
    move.w (a1)+,d0
    add.l d0,gMessageIntro

    ; gMessageSucces[i]=0 for (i=0;i<32;i++)
    move.l #gMessageSucces,a2
    move.w #32-1,d1
.LoopMessageSucces:
    move.l a0,(a2)
    move.w (a1)+,d0
    add.l d0,(a2)+
    dbra d1,.LoopMessageSucces

    move.l #gMessageDeath,a2 ; gMessageDeath[i]=0  for (i=0;i<32;i++)
    move.w #32-1,d1
.LoopMessageDeath:
    move.l a0,(a2)
    move.w (a1)+,d0
    add.l d0,(a2)+
    dbra d1,.LoopMessageDeath

    move.l a0,gTilesData ; gTilesData = gLevelBuffer + ptrvalue;
    move.w (a1)+,d0
    add.l d0,gTilesData ; OK

    ; Here all data pointers are initialised.
		
;#ifdef WITHMUSIC	
;		// -- Load new music 
;		if (gLevelData->position==1) // If first image, then redisplay background
;		{
;			DEBUGCOLOR(2); // Debug. Change Background color
;			
;			if (cbm_load (musicload, getcurrentdevice(), 0x600) == 0) 
;			{
;				POKE(53280,5); // Debug. Change Background color. Purple=4. Use for ERROR
;				return 0;
;			}			
;			
;			DEBUGCOLOR(0); // Debug. Change Background color
;		}
;#endif	
	
		bsr EraseBanner ; EraseBanner(); // Erase loading icon

        move.b #0,gCursorMode ; gCursorMode=0; // 0 = Default mode
        move.l gScenario,a0 ; gCurrentScenarioLine = gScenario; 
        move.l a0,gCurrentScenarioLine
        move.b #0,firepressedlast ; firepressedlast=0;
        move.l gLevelData,a0
        move.b (a0),d0;
        bsr SetOffsetImage ; SetOffsetImage(gLevelData->position); // Init offset of level (1=Left 2=center 3=right)
		
        bsr DisplayPic ; DisplayPic();
        move.l gLevelData,a0
        cmp.b #2,(a0)
        beq .noneedcopyborders
        ; For odd (1 and 3)
        bsr DisplayBackgoundAfterPic ; Need to display ham border
.noneedcopyborders        
        bsr resetzones ; resetzones();
        ; // Set Default palette effect = No effect
        move.b #0,gPaletteMode ; gPaletteMode=0; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall(82) 5=river(82) 6=Diablo(81) 7=fire 8=bomb numbers
        move.b #10,gPaletteNextSwap ; gPaletteNextSwap=10; // >0 in frame
        move.b #1,gNextPalette ; gNextPalette=1; // 0=Palette 1=PaletteAlt
        move.b #0,gCursorMode ; gCursorMode=0;	
        move.b #0,gQte_auto_mode ; gQte_auto_mode=0;
		
        bsr InitNextAction ; InitNextAction(); // Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE  

;		// Initialize the song to be played
;#ifdef WITHMUSIC		
;		if (gLevelData->position==1)
;		{
;			asm("lda #0"); // SubTune 1
;			asm("jsr $0600"); // GoatTracker2 SID Init. (Dojo #3 sub tunes)
;		}
;		else
;		{
;	#ifdef CUTMUSICDURINGLOADING
;			// Setback volume
;			unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
;			filterandvolume = filterandvolume | 0x07; // volume to 15
;			POKE(0xd418,filterandvolume);
;	#endif
;		}
;#endif 		
        move.b #1,shouldloopingame;		shouldloopingame = 1; // We want to loop for game sequence

        bsr DisplayBanner ; DisplayBanner(); Use less on amiga
        ; // -- Intro text
        move.l gMessageIntro,a0
        bsr displayline2 ; displayline2(gMessageIntro);
        lea CurrentLevelName,a0
        cmp.b #'1',(a0)
        bne .noLevel11
        cmp.b #'1',1(a0)
        bne .noLevel11        
        move.b #9,gPaletteMode ; Special case, camera blinking
.noLevel11
        ;move.b #0,gIconLastDisplayedId ; gIconLastDisplayedId=0; // No icons displayed at start.
        
        bsr EraseLastIcon ; Erase last icon (for picture 2 and 3, sucess icon)

        rts
        
; --------------------------------------------------

;---------- Gestion du clavier ----------
; http://www.stashofcode.fr/routines-pour-demomaker-sur-amiga/#clavier
; From Yragael
; https://eab.abime.net/showthread.php?t=62322
PollKeyboard:
	movem.l d0-d2,-(sp)
    lea $DFF000,a5
	;Vérifier dans ICR que la requête est bien générée par le CIA A aur l'événement SP (bascule des 8 bits reçus du clavier dans SDR)
	btst #3,$BFED01
	beq _keyboardNotKeyboard
	;Lire les 8 bits dans SDR et détecter s'il s'agit de la pression ou du relâchement d'une touche
    move.b $BFEC01,d0
	btst #0,d0
	bne _keyboardKeyDown
;	move.w #$00F0,d1		;Touche relâchée : couleur vert
    move.b #0,keyboarddata
;	bra _keyboardKeyUp
    bra _keyboardkeyup
_keyboardKeyDown:
;	move.w #$0F00,d1		;Touche pressée : couleur rouge
;_keyboardKeyUp:
	;Changer la couleur de fond si la touche pressée est celle attendue (ESC)
	not.b d0
	lsr.b #1,d0
    move.b d0,keyboarddata
_keyboardkeyup:    
;	cmpi.b #$45,d0
;	bne _keyboardNotESC
;	move.w d1,COLOR00(a5)
;_keyboardNotESC:
	;Acquitter auprès du clavier en maintenant à 0 le signal sur sa ligne KDAT durant 85 us, ce qui s'effectue en positionnant SPMODE à 1 dans CRA ("software must pulse the line low for 85 microseconds to ensure compatibility with all keyboard models" et "the KDAT line is active low [...] a low level (0V) is interpreted as 1"). Pour rappel, une ligne raster, c'est 227,5 cycles de 280 ns, donc 63,7 us, ce qui signifie qu'il faut attendre que le raster ait parcouru deux lignes. Maintenant, ce n'est pas très élégant d'attendre que le raster se balade en se tournant les pouces...
	bset #6,$BFEE01
	move.l 4(a5),d0 ; VPOSR
	lsr.l #8,d0
	and.w #$01FF,d0
	moveq #2-1,d1
_keyboardWait85us:
	move.l 4(a5),d2 ; VPOSR
	lsr.l #8,d2
	and.w #$01FF,d2
	cmp.w d0,d2
	beq _keyboardWait85us
	move.w d2,d0
	dbf d1,_keyboardWait85us
	bclr #6,$BFEE01
_keyboardNotKeyboard:
	movem.l (sp)+,d0-d2
	rts

;-----------------------------------------------------------------------
; Return random number in d0.w
GetRandom:	
    Movem.l	d1-d2,-(sp)
	Move.w	SBPA,d0
	Move.w	d0,d1
	Move.w	d0,d2
	And.w	#%0000000001000000,d0	
	And.w	#%0000000000000001,d2	
	Lsr	#6,d0
	eor.w	d0,d2
	Lsl.w	#1,d1
	Or.w	d2,d1
	Move.w	d1,SBPA
    move.w  d1,d0
	Movem.l	(sp)+,d1-d2
	Rts
SBPA: ; Sequence Binaire Pseudo Aleatoire	
    Dc.w	$7fa5
;-----------------------------------------------------------------------
InitRandom:				; call once at init
	Move.b	$dff006,SBPA+1
	Rts
;-----------------------------------------------------------------------

; ---------------------------------------------------------
; Main Loop		
LoopGame: ;		while(shouldloopingame) // This is false when we change level

            move.b #0,gSoundGoodHaveBeenPlayedThisFrame ; gSoundGoodHaveBeenPlayedThisFrame=0;

            bsr UpdateTime ; UpdateTime(); ; // Manage Time
		
            bsr UpdatePaletteMode ; UpdatePaletteMode(); // Update palette effects

            ; -- Get info from keyboard (in keyboarddata) Raw key or 0
            bsr PollKeyboard
            ;move.b keyboarddata,$100

; Audio debug (keep)            
;            cmp.b #$21,keyboarddata
;            bne .nosoundplaydebug
;            bsr PlaySoundTest
;.nosoundplaydebug:       

            ; Save keys states
            move.b firepressed,d0
            move.b d0,firepressedlast ; firepressedlast = firepressed;
            move.b iskeyup,d0
            move.b d0,iskeyuplast ; firepressedlast = firepressed;
            move.b iskeydown,d0
            move.b d0,iskeydownlast ; firepressedlast = firepressed;
            move.b iskeyleft,d0
            move.b d0,iskeyleftlast ; firepressedlast = firepressed;
            move.b iskeyright,d0
            move.b d0,iskeyrightlast ; firepressedlast = firepressed;
            ; Reset values
            move.b #0,firepressed ; firepressed=0;
            move.b #0,iskeyup ;iskeyup=0; ; u8 iskeyup,iskeydown,iskeyleft,iskeyright;
            move.b #0,iskeydown ; iskeydown=0;
            move.b #0,iskeyleft ; iskeyleft=0;
            move.b #0,iskeyright ; iskeyright=0;
            ; Set new values
            btst	#6,$bfe001		; Left mouse button
            bne.w	.nomouse
            move.b #1,firepressed
.nomouse:      
            ; Test keyboard
;            cmp.b #$40,keyboarddata ; Space not used on Amiga
;            bne .nokeyspace
;            move.b #1,firepressed
;.nokeyspace:
            cmp.b #$4e,keyboarddata
            bne .nokeyright
            move.b #1,iskeyright
.nokeyright:
            cmp.b #$4f,keyboarddata
            bne .nokeyleft
            move.b #1,iskeyleft
.nokeyleft:    
            cmp.b #$4c,keyboarddata
            bne .nokeyup
            move.b #1,iskeyup
.nokeyup:
            cmp.b #$4d,keyboarddata
            bne .nokeydown
            move.b #1,iskeydown
.nokeydown:

            ;move.b iskeydown,$100
            ;move.b iskeydownlast,$101

            ; // Skip level ?
            cmp.b #0,gCheatEnable; if (gCheatEnable) // Cheat. Skip level
            beq .nogCheatEnable
            cmp.b #1,gSkippingLevel ; Avoid multiple detection of key 'n'
            beq .nogCheatEnable
            cmp.b #$36,keyboarddata ; if (c64keypressed=='n') 
            bne .nogCheatEnable
            cmp.b #0,gCheatEnableTimer
            bne .nogCheatEnable
                    ;add.b #1,$101
                    move.b #1,gSkippingLevel ; gSkippingLevel=1; // Do not play SFX while forcing all actions
                    move.b #200,gCheatEnableTimer
                    move.l gCurrentScenarioLine,a0
                    cmp.b #255,(a0) ; while ( gCurrentScenarioLine[0] != 255 )
                    beq .endwhile  
                        cmp.b #1,gGaugeInUse; if ( gGaugeInUse == 1 )
                        bne .nogauge
                            bsr EraseAndStopGauge; EraseAndStopGauge(0x00);	
.nogauge                            
                        cmp.b #251,(a0) ; if (gCurrentScenarioLine[0]==251) // QTE, skip 10 more bytes
                        bne .noskipextra
                            add.l #10,gCurrentScenarioLine ; gCurrentScenarioLine+=10;
.noskipextra                            
                        add.l #10,gCurrentScenarioLine ; gCurrentScenarioLine+=10;
                        bsr InitNextAction ; InitNextAction();
.endwhile                    
                    move.b #4,gamestate ; gamestate=4; // End, go to next level
                    move.b #1,firepressed ; firepressed=1; // Simulate fire press
                    move.b #0,firepressedlast ; firepressedlast=0;
                    bsr DisableSpriteCursor
                    bsr DisableSpriteJauge
                    bsr EraseLastIcon
                    bra .nogCheatEnable
.nogCheatEnable:
            cmp.b #0,gCheatEnableTimer
            beq .noreducetimer
            sub.b #1,gCheatEnableTimer ; to avoid multiple "n" press
.noreducetimer:

            ; -- TEST SOS Cheat code
            cmp.b #1,gCheatEnable; if (gCheatEnable) // Cheat. Skip level
            beq .noCheatSequence
            cmp.b #0,keyboarddata; else if (c64keypressed!=0)
            beq .noCheatSequence
            ; // is user activating the cheat code ?
            ; // Cheat code activation is "SOS"
            ; // gCheatSequenceCorrectLetters (b)
            ; // S=$21 O=$18
            ; TEST "S"
            cmp.b #0,gCheatSequenceCorrectLetters ; if ( gCheatSequenceCorrectLetters == 0 && (c64keypressed=='s' ) )
            bne .no0
            cmp.b #$21,keyboarddata
            bne .no0
                move.b #1,gCheatSequenceCorrectLetters ; gCheatSequenceCorrectLetters = 1;
            bra .noCheatSequence
.no0:
            cmp.b #1,gCheatSequenceCorrectLetters ; else if ( gCheatSequenceCorrectLetters == 1 && (c64keypressed=='s' ) )
            bne .no0b
            cmp.b #$21,keyboarddata
            bne .no0b
            ; 	// do nothing, wait that S stop beeing pressed
            bra .noCheatSequence
.no0b:        
            ; TEST O
            cmp.b #1,gCheatSequenceCorrectLetters ; if ( gCheatSequenceCorrectLetters == 1 && (c64keypressed=='o' ) )
            bne .no1
            cmp.b #$18,keyboarddata
            bne .no1
                move.b #2,gCheatSequenceCorrectLetters ; gCheatSequenceCorrectLetters = 1;
            bra .noCheatSequence
.no1:
            cmp.b #2,gCheatSequenceCorrectLetters ; else if ( gCheatSequenceCorrectLetters == 1 && (c64keypressed=='s' ) )
            bne .no1b
            cmp.b #$18,keyboarddata
            bne .no1b
            ; 	// do nothing, wait that O stop beeing pressed
            bra .noCheatSequence
.no1b: 
            ; TEST "S"
            cmp.b #2,gCheatSequenceCorrectLetters ; if ( gCheatSequenceCorrectLetters == 2 && (c64keypressed=='s' ) )
            bne .no2
            cmp.b #$21,keyboarddata
            bne .no2
                move.w #$fff,$dff180 ; 	bordercolor(COLOR_WHITE);
                move.b #1,gCheatEnable; 	gCheatEnable = 1;
                move.b #0,gCheatSequenceCorrectLetters; else gCheatSequenceCorrectLetters=0;
            bra .noCheatSequence
.no2:			
            move.b #0,gCheatSequenceCorrectLetters; else gCheatSequenceCorrectLetters=0;
.noCheatSequence:
;			// -- End cheat code skip level

            cmp.b #0,gamestate; if (gamestate!=0) // Disable Cursor if mode is not "playing" else keep it enabled
            bne .disablecursor
            bsr EnableSpriteCursor
            bra .next
.disablecursor:          
            bsr DisableSpriteCursor
.next:

            ; -- State playing ----------------------
StatePlaying:
            cmp.b #0,gamestate ; if (gamestate==0) // Playing
            bne MainNogamestatePlaying

            ; -- Manage all cursors modes
            ; // Special mode for cursor. 0=normal. 1=Slow and random (hit), 
            ; // 2=reversed (all) 3=reversed Up, 4=reversed horizontal 5=always falling
                clr.l d0
                move.b gCursorMode,d0
                
                add.b #1,gCursorAnimCount ; Cycling 8 bits counter

                cmp.b #0,d0
                beq CursorMode0 ; if (gCursorMode==0 || gCursorMode>=8) // Normal mode & "do not show zone" mode & 9 show only sometimes
                cmp.b #8,d0
                bmi CursorMode0End
CursorMode0:
                    ; if      ( iskeyright ) x+=4; 
                    ; else if ( iskeyleft ) x-=4; 
                    ; if      ( iskeyup  ) y-=4;
                    ; else if ( iskeydown ) y+=4;                
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyRight
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyLeft
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyUp
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyDown
.nodown
                bra EndCursorModeManagement
CursorMode0End:


                cmp.b #1,d0
                bne CursorMode1End ; if (gCursorMode==0 || gCursorMode>=8) // Normal mode & "do not show zone" mode & 9 show only sometimes
CursorMode1:        ; else if (gCursorMode==1) // 1=Slow and random (hit)
                    ;  u8 xrand,yrand;
                    ;  xrand = rand() % 4 ; // 0 1 2 3
                    ;  if ( xrand < 2 ) x += xrand;
                    ;  else x -= (xrand-2);
                    ;  yrand = rand() % 4 ; // 0 1 2 3
                    ;  if ( yrand < 2 ) y += xrand;
                    ;  else y -= (yrand-2);
                    ;  if      ( iskeyright ) x+=2; 
                    ;  else if ( iskeyleft ) x-=2; 
                    ;  if      ( iskeyup  ) y-=2;
                    ;  else if ( iskeydown ) y+=2;
                    ; -- Apply random motion
                    bsr GetRandom
                    and.w #$03,d0
                    cmp.w #0,d0
                    bne .no1
                    move.b #1,mouse_want_right
                    bra .no4 
.no1
                    cmp.w #1,d0
                    bne .no2
                    move.b #1,mouse_want_left
.no2
                    cmp.w #2,d0
                    bne .no3
                    move.b #1,mouse_want_down
.no3
                    cmp.w #3,d0
                    bne .no4
                    move.b #1,mouse_want_up
.no4 
 
                    ; Do regular management
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyRight
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyLeft
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyUp
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyDown
.nodown
                   bra EndCursorModeManagement
CursorMode1End:

                cmp.b #2,d0
                bne CursorMode2End 
CursorMode2:            ; else if (gCursorMode==2)  // 2=reversed (all)
                        ; if      ( iskeyright ) x-=4; 
                        ; else if ( iskeyleft ) x+=4; 
                        ; if      ( iskeyup  ) y+=4;
                        ; else if ( iskeydown ) y-=4;
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyLeft
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyRight
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyDown
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyUp
.nodown
                bra EndCursorModeManagement
CursorMode2End:

                cmp.b #3,d0
                bne CursorMode3End 
CursorMode3:        ; else if (gCursorMode==3) // 3=reversed Up
                    ;					if      ( iskeyright ) x+=4; 
                    ;					else if ( iskeyleft ) x-=4; 
                    ;					if      ( iskeyup  ) y+=4;
                    ;					else if ( iskeydown ) y-=4;
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyRight
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyLeft
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyDown
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyUp
.nodown
                bra EndCursorModeManagement
CursorMode3End:

                cmp.b #4,d0
                bne CursorMode4End 
CursorMode4:        ; else if (gCursorMode==4) // 4=reversed horizontal
                    ;					if      ( iskeyright ) x-=4; 
                    ;					else if ( iskeyleft ) x+=4; 
                    ;					if      ( iskeyup  ) y-=4;
                    ;					else if ( iskeydown ) y+=4;
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyLeft
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyRight
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyUp
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyDown
.nodown
                bra EndCursorModeManagement
CursorMode4End:

                cmp.b #5,d0
                bne CursorMode5End 
CursorMode5:        ; else if (gCursorMode==5) // 5=always falling
                    ;					if      ( iskeyright ) x+=4; 
                    ;					else if ( iskeyleft ) x-=4; 
                    ;					y+=2;
                    
                    
                    ;					if (y==(128 - SP_H) )
                    ;						y=0;   
                    ; if bottom reached, then go up
                    cmp.w	#MOUSE_CURSOR_TOP+(15*8),Spr_y	; compare with bottom
                    bne	.no_h2
                    move.w	#MOUSE_CURSOR_TOP,Spr_y	; jump top line
                    move.w	#0,Posy
.no_h2	  
                    
                    
                    cmp.b #1,mouse_want_right
                    bne .noright
                    bsr MouseApplyRight
.noright
                    cmp.b #1,mouse_want_left
                    bne .noleft
                    bsr MouseApplyLeft
.noleft
        
                    move.b gCursorAnimCount,d5
                    and.b #$03,d5
                    cmp.b #0,d5
                    bne .nodown
                    bsr MouseApplyDown
.nodown                    
                  
                bra EndCursorModeManagement
CursorMode5End:

                cmp.b #6,d0
                bne CursorMode6End 
CursorMode6:        ; else if (gCursorMode==6) // 6=always going right
                    ;					if      ( iskeyup  ) y-=4;
                    ;					else if ( iskeydown ) y+=4;
                    ;					x++;
                    
                    ; if ( x >= (96-SP_W) ) x = 0; 
                    cmp.w	#MOUSE_CURSOR_LEFT+(11*8),Spr_x	; compare with right limit
                    bne	.no_h2
                    move.w	#MOUSE_CURSOR_LEFT,Spr_x	; jump top ledt side
                    move.w	#0,Posx
.no_h2	                     
                    

                    move.b gCursorAnimCount,d5
                    and.b #$03,d5
                    cmp.b #0,d5
                    bne .noright
                    bsr MouseApplyRight
.noright
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyUp
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyDown
.nodown
 
                bra EndCursorModeManagement
CursorMode6End:

                cmp.b #7,d0
                bne CursorMode7End 
CursorMode7:        ; else if (gCursorMode==7) // 7=always going left
                    ;					if      ( iskeyup  ) y-=4;
                    ;					else if ( iskeydown ) y+=4;
                    ;					x--;
                    
                    ; if (x > 200 ) x=(96-SP_W); // 
                    cmp.w	#MOUSE_CURSOR_LEFT,Spr_x	; compare with right limit
                    bne	.no_h2
                    move.w	#MOUSE_CURSOR_LEFT+(11*8),Spr_x	; jump top ledt side
                    move.w	#21,Posx ; 11 cells wide, but cursor move half cells
.no_h2	                    
                    
                    move.b gCursorAnimCount,d5
                    and.b #$03,d5
                    cmp.b #0,d5
                    bne .noleft
                    bsr MouseApplyLeft
.noleft
                    cmp.b #1,mouse_want_up
                    bne .noup
                    bsr MouseApplyUp
.noup
                    cmp.b #1,mouse_want_down
                    bne .nodown
                    bsr MouseApplyDown
.nodown
  
                bra EndCursorModeManagement
CursorMode7End:
                ; // gCursorMode == 8 // Do not show color
                ; // gCursorMode == 9 // show only sometimes
                ; // Limit Cursor
                ; if (x > 200 ) x=0; // If x become negative (<0)
                ; else if ( x >= (96-SP_W) ) x = (96-SP_W);
                ; if (y > 200 ) y=0; // If x become negative (<0)
                ; else if ( y >= (128 - SP_H) ) y = (128 - SP_H);	
EndCursorModeManagement:
                ; // Save position and get pointed zone
                ; pointedzone (d0.b) = GetPointedZone(x/8,y/8);
                bsr GetPointedZone ; d0 = 0 no zone pointed (red) else green
                move.b d0,pointedzone ; OK ... 1 to max
                
                ; -- if (gCursorMode!=9 || ( ( rand() % 4 ) == 1 ) )
                move.w d0,-(a7) ; Save pointed zone
                cmp.b #9,gCursorMode
                bne .nomode9
                ; in mode 9, cursir us blinking, we can see it one out of 4.
                bsr GetRandom ; d0.w
                and.w #$03,d0 ;
                cmp.w #1,d0
                bne .noseecursor
.seecursor
                bsr EnableSpriteCursor
                bra .nomode9
.noseecursor            
                bsr DisableSpriteCursor
.nomode9
                move.w (a7)+,d0 ; Restore pointed zone
  
                ; Change cursor color
                lea CursorColor,a0
                cmp.b #8,gCursorMode ; Cursor mode 8, all is red
                beq .nothingpointed
                cmp.b #0,d0
                beq .nothingpointed
                move.w #$0f0,2(a0) ; green
                bra .nextpoint
.nothingpointed:                
                move.w #$f00,2(a0) ; red
.nextpoint:                

                ; // Do action key is pressed ?
                cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0 && pointedzone > 0) // Zone start at 1
                beq .noactionKeyPressed
                cmp.b #1,firepressedlast
                beq .noactionKeyPressed
                cmp.b #0,pointedzone
                beq .noactionKeyPressed
                    ; u8 actiontodo=0;
                    move.l gCurrentScenarioLine,a0
                    clr.w d0
                    move.b pointedzone,d0
                    move.b (a0,d0.w),actiontodo ; actiontodo = gCurrentScenarioLine[pointedzone]; // 0 is time
                    ;move.b actiontodo,$101
                    ; // Check diablo mode
                    cmp.b #6,gPaletteMode; if (gPaletteMode==6) // Diablo
                    bne .nodiablo
                     cmp.b #100,actiontodo ;  if (actiontodo<100 && gNextPalette == 0 )
                     bge .nodiablo
                     cmp.b #1,gNextPalette ; 1 == alt with diablo
                     bne .nodiablo
                        move.b #101,actiontodo ; actiontodo = 101; // Force death if diablo is displayed
.nodiablo:
                    move.b actiontodo,d0
                    
                    cmp.b #100,d0 ; if (actiontodo<100) // Win! // 1 to 100 are sucess text id
                    bge .noactionunder100
                        clr.l d0
                        move.b actiontodo,d0  ; int texid = gCurrentScenarioLine[1];
                        lea gMessageSucces,a0
                        sub.w #1,d0
                        lsl #2,d0
                        add.l d0,a0 
                        move.l (a0),a0 ; gMessageSucces[texid-1]
                        bsr displayline2 ; displayline2(gMessageSucces[actiontodo-1]);                        
                        ; // Disable all possible choices on the scenario line. Means values >0 and < 99.
                        ; // This allow multiple choices
                        ; for (loopzone=1; loopzone<10; loopzone++)
                        move.w #1,d0 ; loopzone
                        move.l gCurrentScenarioLine,a0
                        clr.l d1 ; value = 0
                        move.w #9-1,d5
.loopzone:                        
                            move.b (a0,d0.w),d1 ; u8 value = gCurrentScenarioLine[loopzone];
                            cmp.b #0,d1 ; 	if (value>0 && value<100)
                            beq .loopzonenext
                            cmp.w #100,d1
                            bge .loopzonenext
                                ;move.b d1,$100 ; ok
                                bsr MarkZoneAsUsed ; MarkZoneAsUsed(loopzone=d0.b);
.loopzonenext:                             
                            add.w #1,d0 ; loopzone++
                        dbra d5,.loopzone
                        cmp.b #1,gGaugeInUse ; if ( gGaugeInUse == 1 )
                        bne .nogGaugeInUse
                        move.w #0,d0 
                        bsr EraseAndStopGauge ; 	EraseAndStopGauge(0x00);
.nogGaugeInUse:						
                        add.l #10,gCurrentScenarioLine ; gCurrentScenarioLine+=10;
                        bsr InitNextAction ; InitNextAction();
                        bsr PlaySoundGood ; PlaySoundGood(); // Sound GOOD
                    bra .noactionKeyPressed
.noactionunder100:
                    ; -- Bad actions
                    clr.l d0
                    move.b actiontodo,d0
                    cmp.w #200,d0 ; w, else signed value
                    bge .noactionKeyPressed; else if (actiontodo < 200) // 101 to 200 are deaths
                        sub.b #100,d0 ; u8 message (d0.b)  = actiontodo-100; // first message is 101
                        ; // Death
                        sub.b #1,d0 ; u8 indexoftext (d0.b) = (message-1);
						;move.b d0,$102
                        add.b #1,gSfxDeathCount ; gSfxDeathCount++; // Increase nb deaths for SFX play
SFXDEATHNUMBERBEFOREPLAYSOUND=3 ;//#define SFXDEATHNUMBERBEFOREPLAYSOUND 4 // Each 4 death a digitalised voice is played
                        move.w #TEXTCOLORBAD,textboldcolor ; textboldcolor (b) = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
                        lea gMessageDeath,a0
                        lsl #2,d0 ; *4
                        add.l d0,a0 
                        move.l (a0),a0
                        bsr displayline2 ; displayline2(gMessageDeath[indexoftext]);
                        move.w #TEXTCOLORGOOD,textboldcolor ; textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)
                        ; //}
                        ; //else
                        ; //	gSfxTextDeathSave=gMessageDeath[indexoftext];
                        ; // Erase gauge is it was used
                        cmp.b #1,gGaugeInUse ; if ( gGaugeInUse == 1 )
                        bne .nogGaugeInUse2
                        move.w #0,d0 
                        bsr EraseAndStopGauge ; 	EraseAndStopGauge(0x00);
.nogGaugeInUse2:							
                        ; // Go to state = 1 (cursor will not be displayed again)
                        ; //if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
						move.b #1,gamestate ; gamestate=1
                        ; TODO
                        ; //else
                        ; //{
                        ; //	gamestate=6;
                        ; //	gSfxDeathCount=0;
						move.b #0,gPaletteMode ; //gPaletteMode=0; // Reset death
                        move.b #0,gCursorMode ; gCursorMode=0;
                        bsr DisplayCentralIconDeath ; DisplayCentralIconDeath();
                        cmp.b #SFXDEATHNUMBERBEFOREPLAYSOUND,gSfxDeathCount; //if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
                        bne .regulardeathsound
                        bsr PlayRandomSound
                        move.b #0,gSfxDeathCount ; Reset death counter
                        ;add.w #1,$100
                        bra .regulardeathsound_next
.regulardeathsound:                        
                        bsr PlaySoundBad ; PlaySoundBad(); //Sound BAD
.regulardeathsound_next
						add.w #1,gNbDeath ; gNbDeath++; // stats
.noactionKeyPressed:

                ; // Manage Gauge -----------------------------
                ; Test is Gauge is ready to start
                cmp.b #1,gGaugeReadyToStart
                bne .notestreadytostart
                move.b mouse_want_right,d0
                or.b mouse_want_left,d0
                or.b mouse_want_up,d0
                or.b mouse_want_down,d0
                cmp.b #1,d0
                bne .notestreadytostart 
                ; Mouse have moved, we are ready to start Gauge !!
                move.b #0,gGaugeReadyToStart
                move.b #1,gGaugeInUse
.notestreadytostart        
                cmp.b #1,gGaugeInUse; if ( gGaugeInUse == 1 )
                bne .nogGaugeInUse3
                    bsr UpdateAndDisplayGauge ; DisplayGauge(); 
                    cmp.b #1,gGaugeEnded; if (gGaugeEnded==1) // Death because time have ended
                    bne .nogGaugeEnded ; // Death Message
 						move.w #TEXTCOLORBAD,textboldcolor ; textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
                        clr.l d0
                        move.b gMessageFailTimeCurrent,d0
                        lsl #2,d0
                        lea gTimeUpDeathsPtr,a0
                        add.l d0,a0
                        move.l (a0),a0
                        bsr displayline2 ; displayline2(gTimeUpDeathsPtr[gMessageFailTimeCurrent]);
 						move.w #TEXTCOLORGOOD,textboldcolor ; textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)
                        add.b #1,gMessageFailTimeCurrent ; gMessageFailTimeCurrent++;
                        cmp.b #NBMESFAILTIME,gMessageFailTimeCurrent; if (gMessageFailTimeCurrent==NBMESFAILTIME)
                        bne .noresetfailmessage
                        move.b #0,gMessageFailTimeCurrent ; gMessageFailTimeCurrent=0;
.noresetfailmessage:                        
						move.b #1,gamestate ; gamestate=1; ; // Go to state = 1 (cursor will not be displayed again)
						move.b #0,gPaletteMode ; gPaletteMode=0; // Reset death
                        move.b #0,gCursorMode; gCursorMode=0;
                        bsr DisplayCentralIconDeath ; DisplayCentralIconDeath(); ; // Display dead cursor	
						bsr PlaySoundBad ; PlaySoundBad(); //Sound BAD
						add.w #1,gNbDeath ; gNbDeath++; // stats
.nogGaugeEnded:
.nogGaugeInUse3:

StateQTEAuto:
                ; // QTE auto mode ---------------------------------
                cmp.b #1,gQte_auto_mode; if (gQte_auto_mode==1)
                bne .noQTE
                    sub.b #1,gQte_next; gQte_next--;
                    cmp.b #0,gQte_next
                    bne .noQTE ; if (gQte_next==0)
                        ; // start random QTE
                        ; // Save Pointer
                        move.l gCurrentScenarioLine,a0 ; gCurrentScenarioLine_Save=gCurrentScenarioLine;
                        move.l a0,gCurrentScenarioLine_Save
                        ; // Init false cookie
                        lea gScenario_qte_live,a1 ; gCurrentScenarioLine=(u8*)gScenario_qte_live;
                        move.l a1,gCurrentScenarioLine ; pScript=gCurrentScenarioLine;
                        move.b #251,0(a1) ; pScript[0]=251
                        move.b #35,1(a1) ; pScript[1]=35; // Short time.
                        move.b #1,2(a1); pScript[2]=1;
                        move.b #101,3(a1); pScript[3]=101;
                        ; // -- 
                        bsr GetRandom ; d0.w
                        and.w #3,d0
                        add.b #1,d0
                        move.b d0,4(a1) ; pScript[4]=1 + (rand() % 4);
                        bsr GetRandom ; d0.w
                        and.w #3,d0
                        add.b #1,d0
                        move.b d0,5(a1)  ; pScript[5]=1 + (rand() % 4);
                        bsr GetRandom ; d0.w
                        and.w #3,d0
                        add.b #1,d0
                        move.b d0,6(a1) ; pScript[6]=1 + (rand() % 4);
                        bsr GetRandom ; d0.w
                        and.w #3,d0
                        add.b #1,d0
                        move.b d0,7(a1) ; pScript[7]=1 + (rand() % 4);
                        move.b #255,8(a1) ; pScript[8]=255;
                        move.b #1,gQTEForceNoWait ; gQTEForceNoWait=1; // QTE will not wait for a key
                        bsr InitNextAction ; InitNextAction();
                        move.b #0,gQTEForceNoWait ; gQTEForceNoWait=0;
.noQTE:
            bra LoopGameWhileTest ; } // -- END obf gamestate playing
            ; -----------------------------------------------
MainNogamestatePlaying:
StateDead:
            cmp.b #1,gamestate ; else if (gamestate==1) // -- Game state DEAD, waiting key to restart
            bne MainNogamestateDead
                cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
                beq .nopressed
                cmp.b #1,firepressedlast
                beq .nopressed
                    ;bsr DisplayPic ; DisplayPic();	
                    bsr EraseLastIcon ; Erase death icon
                    ; // -- Intro text
                    move.l gMessageIntro,a0
                    bsr displayline2 ; displayline2(gMessageIntro);
                    ; Camera blinking ?
                    lea CurrentLevelName,a0
                    cmp.b #'1',(a0)
                    bne .noLevel11
                    cmp.b #'1',1(a0)
                    bne .noLevel11 
                    move.b #9,gPaletteMode ; Special case, camera blinking
.noLevel11                    
					move.b #0,gamestate ; gamestate=0;	
                    move.l gScenario,a0
                    move.l a0,gCurrentScenarioLine ; gCurrentScenarioLine=gScenario;
                    bsr resetzones ; resetzones();
                    ; Level 51 display pic (reset picture)
                    lea CurrentLevelName,a0
                    cmp.b #'5',(a0)
                    bne .noLevel51
                    cmp.b #'1',1(a0)
                    bne .noLevel51 
                    bsr DisplayPic ; Reinit screen
.noLevel51      
                    
                    bsr InitNextAction ; InitNextAction();	
.nopressed:                    
            bra LoopGameWhileTest
            
MainNogamestateDead:
            ; -----------------------------------------------
StateText:
            cmp.b #2,gamestate ; else if (gamestate==2) // -- DISPLAY TEXT
            bne MainNogamestateText
                cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
                beq .nopressed
                cmp.b #1,firepressedlast
                beq .nopressed
                    ; // Display text
                    move.l gCurrentScenarioLine,a0
                    clr.l d0
                    move.b 1(a0),d0  ; int texid = gCurrentScenarioLine[1];
                    lea gMessageSucces,a0
                    sub.w #1,d0
                    lsl #2,d0
                    add.l d0,a0 
                    move.l (a0),a0 ; gMessageSucces[texid-1]
                    bsr displayline2 ; displayline2(gMessageSucces[texid-1]);
                    bsr PlaySoundClick
                    add.l #10,gCurrentScenarioLine ; gCurrentScenarioLine+=10
                    bsr InitNextAction ; InitNextAction();
.nopressed:
            bra LoopGameWhileTest
            ; -----------------------------------------------
MainNogamestateText:
StateBeforeQTE:
            cmp.b #7,gamestate ; else if (gamestate==7) // -- BEFORE QTE. WAIT FOR KEY
            bne MainNogamestateBeforeQTE
            cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
            beq .nopressed
            cmp.b #1,firepressedlast
            beq .nopressed
                move.b #3,gamestate ; gamestate=3; // Start QTE
.nopressed
            ; If any keyboard key pressed ?
            clr.b d0
            move.b iskeyup,d0
            or.b iskeydown,d0
            or.b iskeyleft,d0
            or.b iskeyright,d0
            cmp.b #0,d0
            bne .nopressedkeys
            clr.b d0
            move.b iskeyuplast,d0
            or.b iskeydownlast,d0
            or.b iskeyleftlast,d0
            or.b iskeyrightlast,d0
            cmp.b #1,d0
            bne .nopressedkeys
                move.b #3,gamestate ; gamestate=3; // Start QTE
.nopressedkeys
            bra LoopGameWhileTest
            
MainNogamestateBeforeQTE:
StateQTE:
            cmp.b #3,gamestate ; else if (gamestate==3) // -- QTE ----------------------------------------------------
            bne MainNogamestateQTE
                ; // gCurrentScenarioLine[] .. 20 values
                ; // QTEID(251) / (1) Time / (2) Text if win / (3) Text if lost / (4) sequence / 255
                ; // Start with QTEstate=1; // Start the QTE sequence
                ; // gTimePrecise // iskeyright
                cmp.b #1,QTEstate ; if (QTEstate==1) // Display START icon for 1s
                bne QTEState2
                cmp.b #50,gTimePrecise ; if (gTimePrecise>=50) // 1s
                bmi .no50
                        bsr EraseLastIcon ; EraseLastIcon(); // Erase center icon
                        move.b #0,gTimePrecise ; gTimePrecise=0;
						move.b #2, QTEstate ; QTEstate=2;
                bra QTEStateEnd
.no50:
QTEState2:
                cmp.b #2,QTEstate ; else if (QTEstate==2) // Wait 1s
                bne QTEState3
                cmp.b #50,gTimePrecise ; if (gTimePrecise>=50) // 1s
                bmi .no50
                        bsr DisplayCentralIconQTEStart ; DisplayCentralIconQTEStart(); // Icone for saying it is starting
                        bsr PlaySoundWarning ; PlaySoundWarning();
                        move.b #0,gTimePrecise ; gTimePrecise=0;
						move.b #3, QTEstate ; QTEstate=3;
                bra QTEStateEnd
.no50:
QTEState3:			
                cmp.b #3,QTEstate ; else if (QTEstate==3) // Display START icon for 1s
                bne QTEState4
                cmp.b #50,gTimePrecise ; if (gTimePrecise>=50) // 1s
                bmi .no50
                        bsr EraseLastIcon ; EraseLastIcon(); // Erase center icon
                        move.b #0,gTimePrecise ; gTimePrecise=0;
						move.b #4, QTEstate ; QTEstate=4;
                bra QTEStateEnd
.no50:
QTEState4:
                cmp.b #4,QTEstate ; else if (QTEstate==4) // Wait 1s
                bne QTEState5
                cmp.b #50,gTimePrecise ; if (gTimePrecise>=50) // 1s
                bmi .no50
                        bsr EraseLastIcon ; EraseLastIcon(); // Erase center icon
                        move.b #0,gTimePrecise ; gTimePrecise=0;
						move.b #5,QTEstate ; QTEstate=5; // Start sequence
                        ; // Display first icon
						move.b #0,QTEcurrentstep ; QTEcurrentstep = 0;
                        move.l gCurrentScenarioLine,a0
                        clr.w d0
                        move.b QTEcurrentstep,d0
                        add.b #4,d0
                        move.b (a0,d0.w),d0
                        bsr DisplayQTEIcon ; DisplayQTEIcon(gCurrentScenarioLine[4+QTEcurrentstep]); // 1=up, 2=down, 3=right, 4=left
                bra QTEStateEnd
.no50:
QTEState5:
                cmp.b #5,QTEstate ; else if (QTEstate==5) // Sequence
                bne QTEState6
                move.b #0,win ; u8 win=0;
                move.b #0,lost  ; u8 lost=0;
                move.l gCurrentScenarioLine,a0
                clr.w d0
                move.b QTEcurrentstep,d0
                add.b #4,d0
                move.b (a0,d0.w),wantedaction ; u8 wantedaction = gCurrentScenarioLine[4+QTEcurrentstep]; // 1=up, 2=sown, 3=right, 4=left
                ; // Test Pressed key
                move.b #0,keypressed; u8 keypressed=0; 
                lea keypressed,a0
                ; if (rightpressed && !rightpressedlast) keypressed=3; 
                move.b iskeyright,d0
                move.b iskeyrightlast,d1
                move.b #3,d2
                bsr CheckUpTransitionAndSetValue
                cmp.b #1,d0
                beq .havebeenset
                ; else if (leftpressed && !leftpressedlast)  keypressed=4; 
                move.b iskeyleft,d0
                move.b iskeyleftlast,d1
                move.b #4,d2
                bsr CheckUpTransitionAndSetValue 
                cmp.b #1,d0
                beq .havebeenset                
                ; else if (uppressed && !uppressedlast)    keypressed=1; 
                move.b iskeyup,d0
                move.b iskeyuplast,d1
                move.b #1,d2
                bsr CheckUpTransitionAndSetValue  
                cmp.b #1,d0
                beq .havebeenset                
                ; else if (downpressed && !downpressedlast)  keypressed=2; 
                move.b iskeydown,d0
                move.b iskeydownlast,d1
                move.b #2,d2
                bsr CheckUpTransitionAndSetValue  
.havebeenset:                
                ; if (keypressed!=0) // If something have been pressed
                cmp.b #0,keypressed
                beq .nokey
                    ;move.b #$ff,$101
                    move.b wantedaction,d0
                    ;move.b d0,$102
                    ;move.b keypressed,$103
                    cmp.b keypressed,d0 ; if (wantedaction==keypressed) 
                    bne .nowin
                        move.b #1,win ; win=1;
                        bra .nokey
.nowin:                            
                        move.b #1,lost ; else lost =1;
.nokey:
                    ; // Test win.
                    cmp.b #1,win ; if (win==1)
                    bne .nowin1
                        ; // Go to next step 
						add.b #1,QTEcurrentstep ; ; QTEcurrentstep++;
                        ; // Test end
                        move.l gCurrentScenarioLine,a0
                        clr.w d0
                        move.b QTEcurrentstep,d0
                        add.b #4,d0
                        move.b (a0,d0.w),d0                   
                        cmp.b #255,d0 ; if ( gCurrentScenarioLine[4+QTEcurrentstep] == 255 )
                        bne .no255
                            ; // QTE is won !
                            ; // Erase last icon
                            bsr EraseLastIcon ; EraseLastIcon();
                            bsr PlaySoundGood ; PlaySoundGood(); // Sound GOOD							
                            cmp.b #0,gQte_auto_mode ; if (gQte_auto_mode==0)
                            bne .nogQte_auto_mode
                                ; // Display win text
                                ; displayline2(gMessageSucces[(gCurrentScenarioLine[2]-1)]);
                                move.l gCurrentScenarioLine,a0 
                                clr.l d0
                                move.b 2(a0),d0
                                sub.l #1,d0
                                lea gMessageSucces,a0
                                lsl #2,d0
                                add.l d0,a0 
                                move.l (a0),a0
                                bsr displayline2
                                ; // And continue sequence
                                add.l #20,gCurrentScenarioLine ; gCurrentScenarioLine+=20; // Jump two lines of sequence.
                                bsr InitNextAction ; InitNextAction();
                            bra QTEStateEnd
.nogQte_auto_mode:	
                                ; else // Random QTE mode
                                move.l gCurrentScenarioLine_Save,a0 ; gCurrentScenarioLine=gCurrentScenarioLine_Save
                                move.l a0,gCurrentScenarioLine
                                bsr GetRandom ; d0.w
                                move.b #100,gQte_next ; gQte_next = 100 + (rand() % 64); // 100 + 64
                                and.w #64-1,d0
                                add.b d0,gQte_next
                                bsr InitNextAction ; InitNextAction();
                        bra QTEStateEnd
.no255:
                            ; else
                            ; // Go to next step of sequence.
                            ; // We add a small pause here.
                            ; // Erase last icon
                            bsr EraseLastIcon ; EraseLastIcon();
							move.b #6, QTEstate ; QTEstate=6;
                            move.b #0,gTimePrecise ; gTimePrecise=0;
                            bsr PlaySoundGood ; PlaySoundGood(); // Sound GOOD
                    bra QTEStateEnd
.nowin1:
                    clr.l d0
                    move.l gCurrentScenarioLine,a0
                    move.b 1(a0),d0 ; gCurrentScenarioLine[1]
                    cmp.b #1,lost ; else if (gTimePrecise>=gCurrentScenarioLine[1] || lost==1) // Death // If player have taken too much time 
                    beq .lost
                    ;move.b gTimePrecise,$100
                    ;move.b d0,$101
                    clr.w d2
                    move.b gTimePrecise,d2
                    cmp.w d2,d0 ; not lost if gTimePrecise smaller than d0
                    bpl QTEStateEnd
.lost:                  ; Lost by time over
                        ; u8 message;
                        ;move.b #$ff,$103
                        bsr EraseLastIcon ; EraseLastIcon(); // Erase last icons
                        ; // Display death text
                        clr.l d0
                        move.l gCurrentScenarioLine,a0
                        move.b 3(a0),d0
                        sub.b #100,d0 ; message = gCurrentScenarioLine[3]-100; // first message is 101
                        ; // Death
                        move.w #TEXTCOLORBAD,textboldcolor ; textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
                        sub.b #1,d0 ; message-1
                        lsl #2,d0
                        lea gMessageDeath,a0
                        add.l d0,a0
                        move.l (a0),a0
                        bsr displayline2 ; displayline2(gMessageDeath[(message-1)]);
                        move.w #TEXTCOLORGOOD,textboldcolor ; textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)						
                        ; // Go to state = 1 (cursor will not be displayed again)
						move.b #1,gamestate ; gamestate=1;
						move.b #0,gPaletteMode ; gPaletteMode=0; // Reset death
                        move.b #0,gCursorMode; gCursorMode=0;						
                        ; // Display death icon
                        bsr DisplayCentralIconDeath ; DisplayCentralIconDeath();	
						bsr PlaySoundBad ; PlaySoundBad(); //Sound BAD
						add.w #1,gNbDeath ; gNbDeath++; // stats
                bra QTEStateEnd
                
QTEState6:
                cmp.b #6,QTEstate ; else if (QTEstate==6) // pause after good answer. Very brief to have blank between two arrows
                bne QTEStateEnd
                cmp.b #8,gTimePrecise; if (gTimePrecise>=8) // Very short pause.
                bmi QTEStateEnd
                        move.l gCurrentScenarioLine,a0
                        clr.w d0
                        move.b QTEcurrentstep,d0
                        add.b #4,d0
                        move.b (a0,d0.w),d0
                        bsr DisplayQTEIcon ; DisplayQTEIcon(gCurrentScenarioLine[4+QTEcurrentstep]); // 1=up, 2=down, 3=right, 4=left
                        move.b #0,gTimePrecise ; gTimePrecise=0;	
						move.b #5, QTEstate ; QTEstate=5; // Back to QTE play state
 				
QTEStateEnd:				
;			// ------- END QTE -----------------------------------------------
            bra LoopGameWhileTest
            
MainNogamestateQTE:

StateEnd:
            cmp.b #4,gamestate ; else if (gamestate==4) // -- END, load next level
            bne MainNogamestateEnd
                ; // Display sucess message
                cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
                beq .nopressed
                cmp.b #1,firepressedlast
                beq .nopressed
                    move.l gNextLevelName,a0 ; u8 levelnumber=gNextLevelName[5]; // Ascii
                    lea CurrentLevelName,a1
                    ; // Set next level
                    move.b (a1),d0 ; store level name
                    move.b 1(a1),d1
                    move.b 5(a0),(a1) ; copy two bytes of level name.
                    move.b 6(a0),1(a1)
                    ; if "D1" then force to "61" Amiga do not have switch side level
                    cmp.b #'D',(a1)
                    bne .noD1
                    move.b #'6',(a1)
                    move.b #'1',1(a1)
.noD1:                  
                    cmp.b #'3',d1 ; For music changes, check if we are looping back to same level (credits)
                    bne .nocreditlooping
                    cmp.b (a1),d0
                    bne .nocreditlooping
                    move.b #1,gCreditLooping
.nocreditlooping:                    
					move.b #8,gamestate ; Do real loading. gamestate=8;
.nopressed:
            bra LoopGameWhileTest

MainNogamestateEnd:
StateDisplayStats:
            cmp.b #5,gamestate ; else if (gamestate==5) // -- DISPLAY STATS
            bne MainNogamestateDisplayStats
                    cmp.b #0,firepressed ; if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
                    beq .nopressed
                    cmp.b #1,firepressedlast
                    beq .nopressed
                    ; // Display text
                    bsr DisplayDeadStats ; DisplayDeadStats();
                    add.l #10,gCurrentScenarioLine ; gCurrentScenarioLine+=10;
                    bsr InitNextAction ; InitNextAction();
.nopressed:
            bra LoopGameWhileTest

MainNogamestateDisplayStats:

StateLevelChange:	
            cmp.b #8,gamestate ; else if (gamestate==8) // -- DO THE LEVEL CHANGE (so always start at beginning of a frame)
            bne MainNogamestateLevelChange
                move.b #0,gSkippingLevel ; gSkippingLevel=0;
                ; // Go to next level
                move.b #0,shouldloopingame ; shouldloopingame=0; // Exit main loop and do level load.
                bsr EraseBanner ; EraseBanner(); // Erase bottom of screen 
                bsr InitLevel ; Will use "CurrentLevelId" // Display floppy
                                    
            bra LoopGameWhileTest
MainNogamestateLevelChange:	

LoopGameWhileTest: 

            bsr wait1Frame

        cmp.b #1,shouldloopingame
        beq LoopGame ; while(shouldloopingame) // This is false when we change level

    rts
;---------------------------------------------------------------

	data_c

	cnop 0,4
    
;---------------------------------------------------------------
; Intro copper list
COPPIntro1:	
		dc.w	$0180,$000
        DC.L	$01000200,$01020000
        dc.l    $010a0000,$01080000 ; Modulo 0
        dc.l    $01040064 ; BPLCON2. All sprite in front.     
        dc.l	$01fc0000,$010c0011 ; Aga fix
IntroSpr:		
        dc.l	$01200000,$01220000 ; spr 0. Cursor. 4 colors
		dc.l	$01240000,$01260000 ; spr 1  Unused
        dc.l    $01280000,$012a0000 ; spr 2  Unused
		dc.l	$012c0000,$012e0000 ; spr 3  Unused
        dc.l    $01300000,$01320000 ; spr 4  Unused
		dc.l	$01340000,$01360000 ; spr 5  Unused
        dc.l    $01380000,$013a0000 ; spr 6  Unused
		dc.l	$013c0000,$013e0000 ; spr 7  Unused
        ; Sprite colors
        dc.l    $01a00000 ; color 16 ; Transparent
        dc.l    $01a20888 ; color 17 ; Cursor Color 1
        dc.l    $01a40000 ; color 18 ; Cursor Color 2
        dc.l    $01a60fff ; color 19 ; Cursor Color 3
        ; Color remaining (12)
        dc.l    $01a80000,$01aa0000,$01ac0000,$01ae0000
        dc.l    $01b00000,$01b20000,$01b40000,$01b60000
        dc.l    $01b80000,$01ba0000,$01bc0000,$01be0000
        
IntroPalettePtr:
 		dc.w    $0180,$0F00,$0182,$0FFF ; Dual playfield 1
		dc.w    $0184,$0888,$0186,$0F00
		dc.w    $0188,$00F0,$018A,$000F
		dc.w    $018C,$0F0F,$018E,$0FF0
        
		dc.w    $0190,$00FF,$0192,$0808 ; Dual playfield 2
		dc.w    $0194,$0008,$0196,$0FF8
		dc.w    $0198,$088F,$019A,$0F88
		dc.w    $019C,$08F8,$019E,$0F84
        
		dc.b $2b,$07,$ff,$fe ; First line 
        
IntroPlansPtr:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000  
        ; Playfield 2
		Dc.l	$00e40000,$00e60000
		Dc.l	$00ec0000,$00ee0000
		Dc.l	$00f40000,$00f60000          

        ; Background = Playfield 1
		; Start line-1. 07 is beginning of horizontal line.	
		dc.b $2c,$df,$ff,$fe ; First line 
		Dc.l    $01006600 ; Planes+HAM+Color
       
        dc.b $2c+32,$df,$ff,$fe       
IntroPlansPtr2:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000  

        dc.b $2c+(32*2),$df,$ff,$fe       
IntroPlansPtr3:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000  
        
        dc.b $2c+(32*3),$df,$ff,$fe       
IntroPlansPtr4:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000          
        
        dc.b $2c+(32*4),$df,$ff,$fe       
IntroPlansPtr5:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000   

        dc.b $2c+(32*5),$df,$ff,$fe       
IntroPlansPtr6:
		Dc.l	$00e00000,$00e20000 ; Playfield 1
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000 

        dc.b    $2c+200,$df,$ff,$fe ; 200 lines = end       
        DC.L	$01000200
         
		;dc.w 	$ffdf,$fffe
       
        ;dc.l 	$30dffffe,$018000F0,$01000200
		Dc.l	$fffffffe
    
	cnop 0,4
   
;---------------------------------------------------------------
; Empty copper list for transition
COPPEmpty:	
		dc.w	$0180,$000
        DC.L	$01000200,$01020000
        dc.l    $010a0000,$01080000 ; Modulo 0
		Dc.l	$fffffffe
    
	cnop 0,4
   
;---------------------------------------------------------------
; Main copper list
COPP1:	
		dc.w	$0180,$000
        DC.L	$01000200,$01020000
        dc.l    $010a00C8,$010800c8 ; Modulo 40*5
        dc.l    $01040024 ; BPLCON2. All sprite in front.     
        dc.l	$01fc0000,$010c0011 ; Aga fix
Spr:		
        dc.l	$01200000,$01220000 ; spr 0. Cursor. 4 colors
SprGauge:
		dc.l	$01240000,$01260000 ; spr 1  Gauge Sprite.
Spr16:
        dc.l    $01280000,$012a0000 ; spr 2  16 colors sprite Pixel 0 to 15
		dc.l	$012c0000,$012e0000 ; spr 3  16 colors sprite Pixel 0 to 15
        dc.l    $01300000,$01320000 ; spr 4  16 colors sprite Pixel 16 to 31
		dc.l	$01340000,$01360000 ; spr 5  16 colors sprite Pixel 16 to 31
        dc.l    $01380000,$013a0000 ; spr 6  Unused
		dc.l	$013c0000,$013e0000 ; spr 7  Unused
        ; Sprite colors
        dc.l    $01a00f0f ; color 16 ; Transparent
        dc.l    $01a20000 ; color 17 ; Black
CursorColor:
        dc.l    $01a40ff0 ; color 18 ; cursor Color (variable)
JaugeColor:        
        dc.l    $01a60ff0 ; color 19 ; jauge color (variable)
SpriteColors2:
        ; Color remaining (12)
        dc.l    $01a80000,$01aa0000,$01ac0000,$01ae0000
        dc.l    $01b00000,$01b20000,$01b40000,$01b60000
        dc.l    $01b80000,$01ba0000,$01bc0000,$01be0000
        
HAMPalettePtr:
 		dc.w    $0180,$0000,$0182,$0FFF ; HAM 16 colors
		dc.w    $0184,$0888,$0186,$0F00
		dc.w    $0188,$00F0,$018A,$000F
		dc.w    $018C,$0F0F,$018E,$0FF0
		dc.w    $0190,$00FF,$0192,$0808
		dc.w    $0194,$0008,$0196,$0FF8
		dc.w    $0198,$088F,$019A,$0F88
		dc.w    $019C,$08F8,$019E,$0F84
        
		dc.b $2b,$07,$ff,$fe ; First line 
        
HAMPlansPtr:
		Dc.l	$00e00000,$00e20000
		Dc.l	$00e40000,$00e60000
		Dc.l	$00e80000,$00ea0000
		Dc.l	$00ec0000,$00ee0000
		Dc.l	$00f00000,$00f20000
		Dc.l	$00f40000,$00f60000          

        ; ---------------- HAM --------------------------------
		; Start line-1. 07 is beginning of horizontal line.	
		dc.b $2c,$df,$ff,$fe ; First line 
		Dc.l    $01006A00 ; Planes+HAM+Color

        ; ---------------- BANNER --------------------------------
		dc.b $2c+144,$df,$ff,$fe ; End HAM, start Banner. Dual playfield 8 and 8 colors.
        Dc.l $01000200
        dc.w $0180,$0000
BannerDualPlayfieldPtr1:     ; Text
		Dc.l	$00e00000,$00e20000
        Dc.l	$00e80000,$00ea0000
        Dc.l	$00f00000,$00f20000
BannerDualPlayfieldPtr2:     ; Background 
		Dc.l	$00e40000,$00e60000
		Dc.l	$00ec0000,$00ee0000
		Dc.l	$00f40000,$00f60000    
BannerDualPlayfieldPalette1:
 		dc.w    $0182,$0000,$0182,$0F00 ; 8 colors (first is pink and transparent so forget about it)
		dc.w    $0184,$0888,$0186,$0F00
		dc.w    $0188,$00F0,$018A,$000F
		dc.w    $018C,$0F0F,$018E,$0FF0
BannerDualPlayfieldPalette2:		
		dc.w    $0190,$00F0,$0192,$00F0 ; colors
		dc.w    $0194,$00F0,$0196,$00F0
		dc.w    $0198,$00F0,$019A,$00F0
		dc.w    $019C,$00F0,$019E,$00F0		
        
        dc.l $010a0000,$01080000 ; Modulo 0        

		dc.b $2c+145,$df,$ff,$fe ; End HAM, start Banner. Dual playfield 8 and 8 colors.
        Dc.l $01006600 ; 6 planes+Dual playfield+color
        
        ; ---------------- DEBUG --------------------------------
		dc.b $2c+145+61,$df,$ff,$fe ; end Banner
		Dc.l $01000200
        dc.w $0180,$0000,$0182,$0000
        dc.l $010a0000,$01080000  
        
		dc.w 	$ffdf,$fffe
       
        dc.w 	$22df,$fffe
        if DISPLAYDEBUGMEMORY==1        
		;  -- Debug zone (to display mem. 12 lines)
		dc.w    $0180,$0000,$0182,$0FFF ; Color 1 is white
		dc.w    $0184,$0888,$0186,$0F00 ; Color 2 is grey, 3 is red
		dc.w    $0188,$00F0,$018A,$000F
		dc.w    $018C,$0F0F,$018E,$0FF0
		dc.w    $0190,$00FF,$0192,$0808
		dc.w    $0194,$0008,$0196,$0FF8
		dc.w    $0198,$088F,$019A,$0F88
		dc.w    $019C,$08F8,$019E,$0F84
        dc.w 	$23df,$fffe
P1DBG:	Dc.l	$00e00000,$00e20000
P2DBG:	Dc.l	$00e40000,$00e60000
P3DBG:	Dc.l	$00e80000,$00ea0000
P4DBG:	Dc.l	$00ec0000,$00ee0000
		Dc.l    $01004200		
		dc.l 	$30dffffe,$01800000,$01000200
        endc
        
		Dc.l	$fffffffe

	cnop 0,4

;---------------------------------------------------------------
; Sprites
;---------------------------------------------------------------
Sprite_p: ; in game cursor
	Dc.w	$225a,$3d00 ; control bits
    dc.w $0000,$e000 ; 16x16
    dc.w $4000,$b000
    dc.w $6000,$9800
    dc.w $7000,$cc00
    dc.w $7800,$c600
    dc.w $7c00,$e300
    dc.w $7e00,$e180
    dc.w $7f00,$f0c0
    dc.w $7f80,$f060
    dc.w $7e00,$f9e0
    dc.w $7c00,$f7e0
    dc.w $6600,$dd80
    dc.w $0200,$ff80
    dc.w $0300,$76c0
    dc.w $0100,$03c0
    dc.w $0000,$03c0
Sprite_nul:	
    Dc.l	0 

	cnop 0,4

Cursor_sprite: 
  dc.w	$225a,$3d00 ; control bits
  ; data
  dc.w $7e00,$0000 ; 8 lines
  dc.w $c300,$3c00
  dc.w $8100,$7e00
  dc.w $8100,$7e00
  dc.w $8100,$7e00
  dc.w $8100,$7e00
  dc.w $c300,$3c00
  dc.w $7e00,$0000
  
  	cnop 0,4
  
Gauge_sprite:
  dc.w	$225a,$3d00 ; control bits
  ; data
  dc.w $fc00,$0000 ; 130 lines
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$7800
  dc.w $fc00,$0000
  dc.w 0,0 ; stop 
  
  	cnop 0,4
  
; -------------------------------------------
; SPRITE data

	cnop 0,4

IconDeath_part1a:
  dc.w $225a,$3d00
  include "data/IconDeath_part1.asm"
  dc.w 0,0 ; stop
  
  	cnop 0,4
  
IconDeath_part1b:
  dc.w $225a,$3d00
  include "data/IconDeath_part1_second.asm"
  dc.w 0,0 ; stop 

	cnop 0,4  

IconDeath_part2a:
  dc.w $225a,$3d00
  include "data/IconDeath_part2.asm"
  dc.w 0,0 ; stop
  
  	cnop 0,4
  
IconDeath_part2b:
  dc.w $225a,$3d00
  include "data/IconDeath_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

; --

IconQTE1_part1a:
  dc.w $225a,$3d00
  include "data/IconQTE1_part1.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
IconQTE1_part1b:
  dc.w $225a,$3d00
  include "data/IconQTE1_part1_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
    
IconQTE1_part2a:
  dc.w $225a,$3d00
  include "data/IconQTE1_part2.asm"
  dc.w 0,0 ; stop

	cnop 0,4
    
IconQTE1_part2b:
  dc.w $225a,$3d00
  include "data/IconQTE1_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

; --

IconQTE2_part1a:
  dc.w $225a,$3d00
  include "data/IconQTE2_part1.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
IconQTE2_part1b:
  dc.w $225a,$3d00
  include "data/IconQTE2_part1_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
    
IconQTE2_part2a:
  dc.w $225a,$3d00
  include "data/IconQTE2_part2.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
IconQTE2_part2b:
  dc.w $225a,$3d00
  include "data/IconQTE2_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    

; --
IconQTE3_part1a:
  dc.w $225a,$3d00
  include "data/IconQTE3_part1.asm"
  dc.w 0,0 ; stop

	cnop 0,4  

IconQTE3_part1b:
  dc.w $225a,$3d00
  include "data/IconQTE3_part1_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTE3_part2a:
  dc.w $225a,$3d00
  include "data/IconQTE3_part2.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTE3_part2b:
  dc.w $225a,$3d00
  include "data/IconQTE3_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

; --

IconQTE4_part1a:
  dc.w $225a,$3d00
  include "data/IconQTE4_part1.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTE4_part1b:
  dc.w $225a,$3d00
  include "data/IconQTE4_part1_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTE4_part2a:
  dc.w $225a,$3d00
  include "data/IconQTE4_part2.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTE4_part2b:
  dc.w $225a,$3d00
  include "data/IconQTE4_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

; --

IconQTEAction_part1a:
  dc.w $225a,$3d00
  include "data/IconQTEAction_part1.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTEAction_part1b:
  dc.w $225a,$3d00
  include "data/IconQTEAction_part1_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

IconQTEAction_part2a:
  dc.w $225a,$3d00
  include "data/IconQTEAction_part2.asm"
  dc.w 0,0 ; stop

	cnop 0,4  

IconQTEAction_part2b:
  dc.w $225a,$3d00
  include "data/IconQTEAction_part2_second.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4

; --

IconSucess_part1a:
  dc.w $225a,$3d00
  include "data/IconSucess_part1.asm"
  dc.w 0,0 ; stop 
  
  	cnop 0,4
    
IconSucess_part1b:
  dc.w $225a,$3d00
  include "data/IconSucess_part1_second.asm"
  dc.w 0,0 ; stop

	cnop 0,4
    
IconSucess_part2a:
  dc.w $225a,$3d00
  include "data/IconSucess_part2.asm"
  dc.w 0,0 ; stop

	cnop 0,4
    
IconSucess_part2b:
  dc.w $225a,$3d00
  include "data/IconSucess_part2_second.asm"
  dc.w 0,0 ; stop 
  
  cnop 0,4
    
SpriteCursor:
  dc.w $225a,$3d00
  ; data
  dc.w $0000,$e000 ; 16x16
  dc.w $4000,$b000
  dc.w $6000,$9800
  dc.w $7000,$cc00
  dc.w $7800,$c600
  dc.w $7c00,$e300
  dc.w $7e00,$e180
  dc.w $7f00,$f0c0
  dc.w $7f80,$f060
  dc.w $7e00,$f9e0
  dc.w $7c00,$f7e0
  dc.w $6600,$dd80
  dc.w $0200,$ff80
  dc.w $0300,$76c0
  dc.w $0100,$03c0
  dc.w $0000,$03c0
  dc.w 0,0 ; stop     

	cnop 0,4
    
;--------------------------------------------------------------
BannerBack: ; 320x61x3 = 2440 for a plane.
    incbin  "data/Banner320_8colors.ami"
;--------------------------------------------------------------
; GfxconvFormat (ami)  
; word - numcolors
; word - bitplane-width in bytes
; word - bitplane-size in bytes
; long - image-size (all bitplanes) in bytes
; numcolors { word - palette }
;image-size { byte - bitplanes }  
;--------------------------------------------------------------

; Audio
; Audacity, set frequency at bottom (11 Khz) and export was "other uncompressed format" Signed 8bits PCM

 cnop 0,4
    
sound_good_raw:
    Incbin	"data/sounds/good.raw"	; 4175
    cnop 0,4
    sound_good_raw_end:
        
sound_bad_raw:
    Incbin	"data/sounds/bad.raw"	; 6797
    cnop 0,4
sound_bad_raw_end:
    
sound_click_raw:
    Incbin	"data/sounds/click.raw"	; 890   
    cnop 0,4
sound_click_raw_end:
        
sound_warning_raw:
    Incbin	"data/sounds/warning2.raw" ; 3524
    cnop 0,4
sound_warning_raw_end:
 
sound_shout1_raw:
    Incbin	"data/sounds/Shout_Kristof_3.raw"  ; short aie
    cnop 0,4 
sound_shout1_raw_end:    

sound_shout2_raw:
    ;Incbin	"data/sounds/Shout_Kristof_2.raw"  ; long dead
    Incbin	"data/sounds/Shout_Kristof_2_processed_shorten8.raw"  ; long dead
    cnop 0,4
sound_shout2_raw_end:       

sound_shout3_raw:
    Incbin	"data/sounds/Shout_Kristof_1.raw"  ; lond dead
    cnop 0,4
sound_shout3_raw_end:       

sound_shout4_raw:
    Incbin	"data/sounds/Shout_Kristof_4.raw"  ; short aieee
    cnop 0,4 
sound_shout4_raw_end:       

sound_shout5_raw:
    Incbin	"data/sounds/Shout_Kristof_5.raw"  ; uahhhhh
    cnop 0,4 
sound_shout5_raw_end:       

 
    ; ---------------
    ; -- DATA FAST --
	data_f
    
HAMPalette: ; Currently used 16 colors. Will be extracted from HAM Iff
    ds.l 16,0

    ; -----------------------------------
    ; Level identifications.
    ; Here we give info about all the levels 
    ; ID of level = two bytes "0" 1" Means Level01.bin. We only store two characters.
    ; ID of palette and set of background tiles. (start at 0)
    ; ID of music
    ; ID of disk ID (to load level data)

STARTINGDISKID=5 ; First level on floppy order 
LevelsInfos:
    dc.b "0","1", 0  , 0  , STARTINGDISKID+0
    dc.b "0","2", 0  , -1 , STARTINGDISKID+1
    dc.b "1","1", 1  , 16 , STARTINGDISKID+2
    dc.b "1","2", 1  , -1 , STARTINGDISKID+3
    dc.b "1","3", 1  , -1 , STARTINGDISKID+4
    dc.b "2","1", 2  , 22 , STARTINGDISKID+5
    dc.b "2","2", 2  , -1 , STARTINGDISKID+6
    dc.b "2","3", 2  , -1 , STARTINGDISKID+7 
    dc.b "3","1", 3  , 45 , STARTINGDISKID+8
    dc.b "3","2", 3  , -1 , STARTINGDISKID+9
    dc.b "3","3", 3  , -1 , STARTINGDISKID+10
    dc.b "4","1", 4  , 58 , STARTINGDISKID+11
    dc.b "4","2", 4  , -1 , STARTINGDISKID+12
    dc.b "4","3", 4  , -1 , STARTINGDISKID+13
    dc.b "5","1", 5  , 73 , STARTINGDISKID+14
    dc.b "5","2", 5  , -1 , STARTINGDISKID+15
    dc.b "5","3", 5  , -1 , STARTINGDISKID+16  
ChangeModule:    
    dc.b "6","1", 6  , 0 , STARTINGDISKID+17
    dc.b "6","2", 6  , -1 , STARTINGDISKID+18
    dc.b "6","3", 6  , -1 , STARTINGDISKID+19  
    dc.b "7","1", 7  , 14 , STARTINGDISKID+20
    dc.b "7","2", 7  , -1 , STARTINGDISKID+21
    dc.b "7","3", 7  , -1 , STARTINGDISKID+22
    dc.b "8","1", 8  , 25 , STARTINGDISKID+23
    dc.b "8","2", 8  , -1 , STARTINGDISKID+24
    dc.b "8","3", 8  , -1 , STARTINGDISKID+25
    dc.b "9","1", 9  , 43 , STARTINGDISKID+26
    dc.b "9","2", 9  , -1 , STARTINGDISKID+27
    dc.b "9","3", 9  , -1 , STARTINGDISKID+28  
    dc.b "C","1", 10  , 61 , STARTINGDISKID+29
    dc.b "C","2", 10  , -1 , STARTINGDISKID+30
    dc.b "C","3", 10  , -1 , STARTINGDISKID+31  
    dc.b $ff ; end marker
    even

; Module 1
; 00 ($00): Tutorial
; 16 ($10): Level 1
; 22 ($16): Level 2
; 45 ($2D): Level 3
; 58 ($3A): Level 4
; 73 ($49): Level 5
; 83 ($53): Title Screen

; Module 2
; 00 ($00): Level 6
; 14 ($10): Level 7
; 25 ($16): Level 8
; 43 ($2D): Level 9
; 61 ($53): Credit

; Background tiles. These data are copied in chip at Back1 adress. 
; Palette is also taken from here. 
    cnop 0,4
BackgroundDatas: ; Set of 4*308 Iff files = 1232 bytes
    ; Level 0
    incbin  "data/Level0_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level0_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level0_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level0_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 1. Id 0
    incbin  "data/Level1_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level1_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level1_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level1_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96  
    ; Level 2
    incbin  "data/Level2_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level2_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level2_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level2_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 3
    incbin  "data/Level3_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level3_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level3_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level3_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96 
    ; Level 4
    incbin  "data/Level4_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level4_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level4_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level4_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 5
    incbin  "data/Level5_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level5_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level5_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level5_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 6
    incbin  "data/Level6_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level6_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level6_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level6_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 7
    incbin  "data/Level7_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level7_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level7_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level7_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 8
    incbin  "data/Level8_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level8_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level8_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level8_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level 9
    incbin  "data/Level9_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/Level9_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/Level9_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/Level9_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96      
    ; Level c
    incbin  "data/LevelC_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    incbin  "data/LevelC_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    incbin  "data/LevelC_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192  
    incbin  "data/LevelC_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96 


;SucessIcon_Level0: ; 884b ; $74 header 
;    incbin  "data/BannerSucess_Level0_HAM6.iff"
;    ; TODO other versions (one for each palette)
    
IconSpritesPalettes16:
  include "data/IconDeath_part1_pal.asm"
  	
  cnop 0,4

;--------------------------------------------------------------
; 2 tiles 16x16, in 8 colors
IntroBack:
    incbin "../datas_source/IntroBack.ami"    

TextPalette: ; 8 colors
    incbin "data/Font_8Colors_pal.bin"
	
Font8: ; 8 colors , 3 colors. 
    incbin "data/Font_8Colors.bin" 
    
TextPaletteIntro: ; 8 colors
    incbin "data/Font_8ColorsIntro_pal.bin   "
	
Font8Intro: ; 8 colors , 3 colors. 
    incbin "data/Font_8ColorsIntro.bin"  

TimeUpOffset:
    incbin "../datas/TIMEUP.BIN"
TimeUpTexts = TimeUpOffset + (7*NUMBERLANGUAGE*2) 

TextsIntro: ; NUMBERLANGUAGE * 9 * 41
    incbin "data/INTROTEXT.BIN"
    
    cnop 0,4
    
IconFloppy: ; 8 colors 48*35. Same palette as text
    incbin "data/IconLoad.ami"
    
;--------------------------------------------------------------

	bss_c

;--------------------------------------------------------------
; All background data. This is 308 bytes x 4 files = 1232 bytes
; Each level have its own set. Data are copied here automatically
; Everything is divided into 8 pixels width slices, so first pixel is always fixed.
; There can be a frame around picture.
; Back1_Half1 8x16 pixels (16*6 bytes=96)
; Back1_Half2 8x16 pixels (16*6 bytes=96)
; Back2_Half1 8x16 pixels (16*6 bytes=96)
; Back2_Half2 8x16 pixels (16*6 bytes=96)
; -- Frame top line (left to right)
; Back1_Half1_TopLeft 8x16 (16*6 bytes=96)
; Top 8x4 pixels (4*6 bytes=32)
; Back1_Half1_TopRight 8x16 (16*6 bytes=96)
; -- Frame Side (top to bottom)
; Back1_Half1_SideLeft 8x16 (16*6 bytes=96)
; Back1_Half1_SideRight 8x16 (16*6 bytes=96)
; Back2_Half1_SideLeft 8x16 (16*6 bytes=96)
; Back2_Half1_SideRight 8x16 (16*6 bytes=96)
; -- Frame middle 
; TopMiddle 8x4 (4*6 bytes=24)
; SideMiddle 8x8 (8*6 bytes=48)
; -- Bottom left
; Back1_BottomLeft 16x8 (16*6 bytes=96) 
; Back1_BottomMiddle 16x8 (16*6 bytes=96)
; Back2_BottomMiddle 16x8 (16*6 bytes=96)
; Back1_BottomRight 16x8 (16*6 bytes=96)
; BottomMiddle 8x8 (8*6 bytes=48)
;
; Total : 1496 bytes
; 11 differents levels
; 16456 bytes.

Back1:
    ;incbin  "data/Level0_Back1_HAM6.iff" ; $74 header , then 2x16x6=192
    ds.b 308
;--------------------------------------------------------------
Back1_half:
    ;incbin  "data/Level0_Back1_half_HAM6.iff" ; $74 header , then 1x16x6=96
    ds.b 308
;--------------------------------------------------------------    
Back2:
    ;incbin  "data/Level0_Back2_HAM6.iff" ; $74 header , then 2(bytes)x16x6=192 
    ds.b 308    
;--------------------------------------------------------------    
Back2_half:
    ;incbin  "data/Level0_Back2_half_HAM6.iff" ; $74 header , then 1(bytes)x16x6=96 
    ds.b 308    
;--------------------------------------------------------------

start_planes:	

	;ds.b    32*40 ; Secure, to remove
    
HAMPLANS:
    ds.b    144*40*6 ; 34Kb

BannerTextsPlanes:
    ds.b    40*61*3; 320x61 8 colors ; 7K

    if DISPLAYDEBUGMEMORY==1
plansDebugMem
	ds.b	12*40*4 ; 4 planes
    endc
	
end_planes:

;--------------------------------------------------------------

	bss_f


;--------------------------------------------------------------
    
   code_f


   ; Put replay routine here, else some assembly problem
   ; I added padding to 4 bytes inside, to fix some strange alignment bugs.
   include "ptplayer.asm"   
    

