;&2511 Drapeau

;&26FD - RESIS.VOX lg=&920
;&301E - NINJA.VOX lg=&330
;&334F - CARNAGE.VOX lg=&6B0
;&3A00 - code anim a remonter lg=&5AA (UK) lg=&6C7 (FR)  ATTENTION lgmax=&74F


;&4150 - SPRITES1.SPR logo resistance + corrections shuriken droit et gauche
;&5100 a &514F - &50 octets pour le sang qui degouline

;&5340 - SPRITES2.SPR - Fonte 2x8
;&5700 - ROUTZIC.BIN - routine musique on=&5700 off=&5703
;&6000 a &9FFF - NINJA.SCR
;&A000 a &A439 - NINJA.MUS lg&43A

;sprite RESISTANCE en &F1EC - 228x61 soit 57x61 - &C017
;sprite SHURIKEN droit correction en &F3AF - 32x23 soit 8x23 - &C00B
;sprite SHURIKEN gauche correction en &D378 - 40x35 soit 10x35 - &C000
;SPRITES1.SPR = lg &FAB

;fonte ligne 1, &E280 - 2x8x38caracteres
;fonte ligne 2, &C320 - 2x8x21caracteres
;SPRITES2.SPR = lg &3B0

ORG &3A00
write direct "code.prs"
ADR_ECRAN_DEPART	EQU	#C000
ADR_ECRAN_BANDE		EQU	#F050
ADR_ECRAN_HAUTEUR	EQU	14	;nb ligne entre ADR_ECRAN_DEPART et ADR_ECRAN_BANDE	
FIN_ROULEAU		EQU	199
ADR_FONTE		EQU	#5340
scroll_depart		EQU	#C780
scroll_largeur		EQU	#4F
matrice_largeur		EQU	2
matrice_hauteur		EQU	8


CALL encres_en_noires

XOR A
CALL &BC0E	;mode 0

CALL couleurs

LD A,25:CALL DELAY

CALL aff_sprite_ralentir_on
CALL logo_resistance
LD A,25:CALL DELAY
CALL VOIX_RESISTANCE
LD A,25:CALL DELAY
CALL aff_presents
CAll aff_sprite_ralentir_off

LD A,0
LD BC,0
LD DE,0
LD HL,0
LD IX,0
LD IY,0
CALL &5700 ;depart musique

LD A,75:CALL DELAY

CALL affichage_rouleau

LD A,100:CALL DELAY

;CALL encres_en_noires
CALL R6_25a10
CALL effacer_ninja_carnage
CALL R6_10a25
;CALL couleurs

CALL &BB03
CALL loopTouche

LD A,&F0 ;rouge
LD (remplir_avec_octet+1),A
CALL remplircouleur

CALL degoulinade_sang

LD A,15:CALL DELAY

XOR A ;noir
LD (remplir_avec_octet+1),A
CALL remplircouleur

CALL aff_text_now_loading
CALL degoulinade_sang

;CALL aff_sprite_ralentir_on
;CALL aff_text_now_loading

CALL &5703;musique off
LD A,15:CALL DELAY
CALL VOIX_NINJAC
CALL &BCA7
CALL &BB03

RET

VOIX_RESISTANCE
;adresse du son
LD HL,&26FD
;longueur
LD DE,&920
LD A,5
LD (voxvit+1),A
CALL LECTURE_SON
RET

VOIX_NINJAC
;adresse du son
LD HL,&301E
;longueur
LD DE,&330
LD A,5
LD (voxvit+1),A
CALL LECTURE_SON
LD A,7:CALL DELAY
;adresse du son
LD HL,&334F
;longueur
LD DE,&6B0
LD A,5
LD (voxvit+1),A
CALL LECTURE_SON
RET

txt_presents
	DEFB "PRESENTE"
	DB 0

txt_press_space
	;DEFB "PRESS SPACE TO START"
	 DEFB " APPUYEZ SUR ESPACE "
	DB 0

txt_now_loading1
	;DEFB "NOW LOADING, PLEASE WAIT"
	 DEFB "  CHARGEMENT EN COURS"
	DB 0
txt_now_loading2
	 DEFB "  VEUILLEZ  PATIENTER"
	DB 0

txt_scrolling
	DEFB "             "
	DEFB " HISTOIRE ET PROGRAMMATION : ORIENS "
	DB &FE
	DEFB "     GRAPHISMES : CED     "
	DB &FE
	DEFB "  MUSIQUE : TOM AND JERRY "
	DB &FE
	DEFB "     INTRO : KUKULCAN     "
	DB &FE
	DEFB "        LOGO : FRA        "
	DB &FE
	DEFB " ECRAN TITRE : Y. KLAPSAKIS"
	DB &FE
	DEFB "       COPYRIGHT 2020 RESISTANCE"
	DB &FE
	DEFB "                   "
	DB &FF

;--- [ test touche - barre d'espace ou joystick tir ]---
loopTouche
	CALL scrolling
;Test Touches
	LD BC,&F40E
	OUT (C),C
	LD BC,&F6C0
	OUT (C),C
	XOR A
	OUT (C),A
	LD BC,&F792
	OUT (C),C
	LD BC,&F645	;ligne 5
	OUT (C),C
	LD B,&F4
	IN A,(C)
	LD BC,&F782
	OUT (C),C
	LD BC,&F600
	OUT (C),C
	RLA		;Charge dans A la valeur du bit 7 (touche ESPACE)
JR NC,SortirTouche
	LD BC,&F40E
	OUT (C),C
	LD BC,&F6C0
	OUT (C),C
	XOR A
	OUT (C),A
	LD BC,&F792
	OUT (C),C
	LD BC,&F649	;ligne 9
	OUT (C),C
	LD B,&F4
	IN A,(C)
	LD BC,&F782
	OUT (C),C
	LD BC,&F600
	OUT (C),C
	RLA		;Charge dans A la valeur du bit 7 (touche DEL)
	RLA		;Charge dans A la valeur du bit 6 (touche JOY_FIRE0)
	RLA		;Charge dans A la valeur du bit 5 (touche JOY_FIRE1 Z)
	RLA		;Charge dans A la valeur du bit 4 (touche JOY_FIRE2 X)
	JP C,loopTouche	;Si TIR du joystick n'a pas etait enfonce on boucle

SortirTouche
	RET

;--- [ rideau R6 ]---
R6_25a10
;animation sur R6 = 25 a 10
LD BC,&BC06
OUT (C),C
LD B,&BD

LD A,15
LD C,25
OUT (C),C

anim4bas
DEC C
OUT (C),C

PUSH AF
CALL &BD19
CALL &BD19
POP AF

DEC A
CP 0
JP NZ,anim4bas
RET

R6_10a25
;animation sur R6 = 10 a 25
LD BC,&BC06
OUT (C),C
LD B,&BD

LD A,15
LD C,10
OUT (C),C

anim4haut
INC C
OUT (C),C

PUSH AF
CALL &BD19
CALL &BD19
POP AF

DEC A
CP 0
JP NZ,anim4haut
RET

;--- [ scrolling ]---
scrolling
debtxtA
	LD HL,txt_scrolling
	LD A,(HL)	;caractere

	LD C,32
	SUB C	;A = A-32

	CP &FF-32
	CALL Z,ON_BOUCLE

	CP &FE-32
	CALL Z,ON_RALENTI

	;calcul position suivant caractere		
	LD L,A		;L = A
	LD H,0		;H = 0
	;HL contient A
	ADD HL,HL 	;*2
	ADD HL,HL 	;*4
	ADD HL,HL 	;*8
	ADD HL,HL 	;*16
	LD DE,ADR_FONTE
	ADD HL,DE	;HL contient l'adresse du tiles

	DEC HL		;position -1 (juste pour simplier la boucle)
	PUSH HL		;sauver HL

	;CALL FRAME

;colonne 0
	CALL FRAME
	CALL SCROLL_GAUCHE
	POP HL
	INC HL ;col +1
	PUSH HL
	CALL AFFICHAGE_COLONNE

;colonne 1
	CALL FRAME
	CALL SCROLL_GAUCHE
	POP HL
	INC HL ;col +1
	PUSH HL
	CALL AFFICHAGE_COLONNE

;colonne 2 (espace entre 2 caracteres)
	CALL FRAME
	CALL SCROLL_GAUCHE
	LD HL,ADR_FONTE ;espace
	CALL AFFICHAGE_COLONNE
	POP HL	;sert a rien, juste a ne pas saturer la PILE

;caractere suivant
	LD HL,(debtxtA+1)
	INC HL
	LD (debtxtA+1),HL
	RET

compteur	db 00

AFFICHAGE_COLONNE
	LD DE,scroll_depart+scroll_largeur ;position ecran

	XOR A
	LD (compteur),A
loop_position
	;ecrire la nouvelle valeur a l'ecran
	LD A,(HL)
	LD (DE),A

	;source suivante = HL + matrice_largeur
	LD BC,matrice_largeur
	ADD HL,BC

	;destination = DE + ligne_suivante
	PUSH HL				;sauver source
	EX DE,HL			;echanger DE et HL
	CALL conv_ligne_suivante_scroll	;calcul ligne suivante
	EX DE,HL			;echange DE et HL	
	POP HL				;recuperer source

	LD A,(compteur)
	INC A
	LD (compteur),A
	CP matrice_hauteur
	JR NZ,loop_position

	RET

SCROLL_GAUCHE
	XOR A			;compteur
	LD HL,scroll_depart	;Adresse de base (coin haut gauche)
loop
	PUSH AF			;sauver A
	PUSH HL			;sauver HL
	LD (transfert+4),HL	;destination
	INC HL
	LD (transfert+1),HL	;source
	CALL transfert		;transfert des donnees
	POP HL
	CALL conv_ligne_suivante_scroll
	POP AF
	INC A
	CP 8			;8 lignes
	JR NZ,loop
	RET

transfert
	LD HL,&0000
	LD DE,&0000
	LD BC,scroll_largeur
	;LDIR
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;10
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;20
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;30
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;40
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;50
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;60
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI ;70
	LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI:LDI     ;79
	RET

conv_ligne_suivante_scroll
	;HL (adresse ecran)
	LD A,H
	ADD A,#08
	LD H,A
	RET NC
	LD BC,#C050
	ADD HL,BC
	RET

ON_BOUCLE
	LD HL,txt_scrolling
	LD (debtxtA+1),HL
	LD A,0	;espace	
	RET

ON_RALENTI
	LD A,15
	CALL DELAY
	LD A,0 ;espace
	RET

;--- [ call &BD19 ]---
frame

	LD B,&F5 	; Se connecter au PPI/8255 port B accessible via &F5xx
.Balayage
	IN A,(C) 	; Lire le contenu
	RRA 		; On teste si le bit 0 = 1
	JP nc,Balayage 	; Si le bit 0 = 0 on boucle jusqu'a la fin du balayage
			; On est en haut de l'ecran, on peut continuer le programme
	RET

;--- [ affichage now loading ]---
aff_text_now_loading
	LD HL,txt_now_loading1
	LD DE,&63C4-&50-&50
	CALL directminiFIN

	LD HL,txt_now_loading2
	LD DE,&63C4
	CALL directminiFIN

	;FLAG
	LD DE,&6640
	LD HL,&2511
	LD A,8	;hauteur
	CALL spr_loopFLAG
	LD DE,&6640+&50
	LD HL,&2511+&70
	LD A,8	;hauteur
	CALL spr_loopFLAG
	LD DE,&6640+&50+&50
	LD HL,&2511+&70+&70
	LD A,8	;hauteur
	CALL spr_loopFLAG
	LD DE,&6640+&50+&50+&50
	LD HL,&2511+&70+&70+&70
	LD A,8	;hauteur
	CALL spr_loopFLAG
	LD DE,&6640+&50+&50+&50+&50
	LD HL,&2511+&70+&70+&70+&70
	LD A,3	;hauteur
	CALL spr_loopFLAG

	RET	

spr_loopFLAG
		PUSH AF
		PUSH DE
		LD BC,&000E	;largeur
		LDIR
		POP DE
		PUSH HL
		LD HL,&800
		ADD HL,DE
		EX DE,HL
		POP HL
		POP AF
		DEC A
		JR NZ, spr_loopFLAG
		RET

;--- [ fonte mini FIN ]---
directminiFIN
loop_affichage_mini_texteFIN
	;lecture du caractere a afficher
	LD A,(HL)
	CP 0
	RET Z
	
	LD C,32
	SUB C	;A = A-32
	
	PUSH HL	;sauver HL
	PUSH DE ;sauver DE
	LD L,A		;L = A
	LD H,0		;H = 0
	;HL contient A (A * 2*8=16)
	ADD HL,HL 	;*2
	ADD HL,HL 	;*4
	ADD HL,HL 	;*8
	ADD HL,HL 	;*16

adrfonteminiFIN
	LD DE,ADR_FONTE
	ADD HL,DE	;HL contient l'adresse du tiles	

	POP DE		;recuperer l'adresse d'affichage a l'ecran
	PUSH DE		;on resauve DE
	CALL spr_hauteurFIN	;affichage du sprite
	POP DE		;on recupere
	INC DE
	INC DE		;position suivante a l'ecran +2
	INC DE		;finalement +3 pour aerer

	POP HL 	;recuperer HL
	INC HL	;position caractere suivant
	
	JR loop_affichage_mini_texteFIN
	RET

spr_hauteurFIN
		LD A,8	;hauteur
spr_loopFIN
		PUSH AF
		PUSH DE
		LD BC,&0002	;largeur
		LDIR
		POP DE
		PUSH HL
		LD HL,&800
		ADD HL,DE
		EX DE,HL
		POP HL
		POP AF
		DEC A
		JR NZ, spr_loopFIN
		RET

;--- [ ralentir affichage sprite ]---
aff_sprite_ralentir_on
	LD A,&CD
	LD (spr_ralentir),A
	LD A,&19
	LD (spr_ralentir+1),A
	LD A,&BD
	LD (spr_ralentir+2),A
	RET
aff_sprite_ralentir_off
	XOR A
	LD (spr_ralentir),A
	LD (spr_ralentir+1),A
	LD (spr_ralentir+2),A
	RET

;--- [ affichage PRESENTS ] ---
aff_presents
	LD A,8:LD (spr_hauteur+1),A	;hauteur
	LD A,2:LD (spr_largeur+1),A ;largeur
	LD HL,txt_presents
	LD DE,&C51D ;affichage
	CALL directmini
	RET

;--- [ fonte mini ]---
directmini
	;lecture valeur hauteur
	LD A,8:LD (spr_hauteur+1),A
	;lecture valeur largeur
	LD A,2:LD (spr_largeur+1),A
loop_affichage_mini_texte
	;lecture du caractere a afficher
	LD A,(HL)
	CP 0
	RET Z
	
	LD C,32
	SUB C	;A = A-32
	
	PUSH HL	;sauver HL
	PUSH DE ;sauver DE
	;CALL &BB5A
	LD L,A		;L = A
	LD H,0		;H = 0
	;HL contient A (A * 2*8=16)
	ADD HL,HL 	;*2
	ADD HL,HL 	;*4
	ADD HL,HL 	;*8
	ADD HL,HL 	;*16

adrfontemini
	LD DE,ADR_FONTE
	ADD HL,DE	;HL contient l'adresse du tiles	

	POP DE		;recuperer l'adresse d'affichage a l'ecran
	PUSH DE		;on resauve DE
	CALL spr_hauteur	;affichage du sprite
	POP DE		;on recupere
	INC DE
	INC DE		;position suivante a l'ecran +2
	INC DE		;finalement +3 pour aerer

	POP HL 	;recuperer HL
	INC HL	;position caractere suivant
	
	JR loop_affichage_mini_texte
	RET

;--- [ logo_resistance ]----
logo_resistance
	LD A,61:LD (spr_hauteur+1),A	;hauteur
	LD A,57:LD (spr_largeur+1),A ;largeur
	LD HL,&4150	;data en memoire
	LD DE,&F1EC ;affichage
	CALL spr_hauteur

	RET

spr_hauteur
		LD A,&20	;hauteur
spr_loop
		PUSH AF
		PUSH DE
spr_largeur
		LD BC,&0000	;largeur
		LDIR
		POP DE
		;EX DE,HL
		;CALL &BC26
		CALL ligne_suivante_suivant_R1
spr_ralentir
		NOP
		NOP
		NOP

		;EX DE,HL
		POP AF
		DEC A
		JR NZ, spr_loop
		RET

.ligne_suivante_suivant_R1
		LD A,D
		ADD A,8
		LD D,A
		AND &38
		RET NZ
		LD A,D
		SUB &40
		LD D,A
		LD A,E
		ADD A,&50	;si R1=40 alors &50 si R1=41 alors &52 si R1=44 alors &58...
		LD E,A
		RET NC
		INC D
		LD A,D
		AND 7
		RET NZ
		LD A,D
		SUB 8
		LD D,A
		RET


;---- [ temporisation (0.05s to 12.75s) ] ---- 
DELAY		LD B,15			; 15 x 1/300th ms (HALT) = 50ms
DELAY_1		HALT			; Waits 1/300th s
		DJNZ DELAY_1		; Loop for 50ms
		DEC A        
		JR NZ,DELAY           ; Loop for A x 50ms
		RET


;--- [effacer Ninja Carnage et les lettres japonaises] ----
effacer_ninja_carnage
;&CB34 lg=42 hauteur=48 NINJA
		LD DE,&CB34
		LD B,48	;hauteur
		LD A,42
		LD (rect_larg_efface+1),A
		CALL rect_loop2_efface

;&C500 lg=80 hauteur=64 CARNAGE
		LD DE,&C500
		LD B,72	;hauteur
		LD A,80
		LD (rect_larg_efface+1),A
		CALL rect_loop2_efface

;&C459 lg=7 hauteur=24 BOUT A DROITE
		LD DE,&C459
		LD B,24	;hauteur
		LD A,7
		LD (rect_larg_efface+1),A
		CALL rect_loop2_efface

;&F412 lg=5 hauteur=18 3 BOUT A GAUCHE
		LD DE,&F412
		LD B,18	;hauteur
		LD A,5
		LD (rect_larg_efface+1),A
		CALL rect_loop2_efface

		LD DE,&DCB8
		LD B,6	;hauteur
		LD A,4
		LD (rect_larg_efface+1),A
		CALL rect_loop2_efface

		XOR A
		LD (&F4B7),A
		LD (&FCB7),A

;sprite SHURIKEN droit correction en &F3AF - 32x23 soit 8x23 - &C00B
		LD A,23:LD (spr_hauteur+1),A	;hauteur
		LD A,8:LD (spr_largeur+1),A ;largeur
		LD HL,&4EE5	;data en memoire
		LD DE,&F3AF ;affichage
		CALL spr_hauteur
;sprite SHURIKEN gauche correction en &D378 - 40x35 soit 10x35 - &C000
		LD A,35:LD (spr_hauteur+1),A	;hauteur
		LD A,10:LD (spr_largeur+1),A ;largeur
		LD HL,&4F9D	;data en memoire
		LD DE,&D378 ;affichage
		CALL spr_hauteur
;PRESS SPACE TO START
		LD A,8:LD (spr_hauteur+1),A	;hauteur
		LD A,2:LD (spr_largeur+1),A ;largeur
		LD HL,txt_press_space
		LD DE,&C5AB ;affichage
		CALL directmini		

		RET

rect_loop2_efface
		PUSH BC
		PUSH DE
rect_larg_efface
		LD B,19	;largeur
rect_loop1_efface
		LD A,&0	;valeur a ecrire a l'ecran
		LD (DE),A	;ecrire
		INC DE		;adresse ecran suivante
		DJNZ rect_loop1_efface
		POP DE
		CALL ligne_suivante_efface
		POP BC
		;CALL &BD19
		DJNZ rect_loop2_efface
		RET

ligne_suivante_efface
		LD A,D		;on recopie D dans A
		ADD A,#08	;on ajoute #08 a A
		LD D,A		;on recopie A dans D
				;DE contient la nouvelle adresse
		RET NC		;si pas de debordement alors on arrete la le calcul
				;et on revient juste apres le call
		;si debordement on continue donc ici et cela signifie qu'on doit ajouter #C050 a notre adresse
		EX HL,DE	;on a besoin que notre adresse soit dans HL pour pouvoir lui additionner
				;quelque chose
		;l'adresse est maintenant dans HL
		LD BC,#C050	;on met #C050 dans BC
		ADD HL,BC	;on additionne HL et BC
		;HL contient maintenant l'adresse de la ligne inferieure mais on la veut dans DE
		EX HL,DE	;on refait l'echange et DE contient donc l'adr
		RET 

;--- [degoulinade sanglante] ----
degoulinade_sang
	ld h,e
	ld b,#50
	ld hl,&5100
.la006
	ld (hl),#00
	inc hl
	djnz la006
	ld bc,#07d0
.la00e
	push bc
	ld a,r
.la011
	cp #50
	jr c,la019
	sub #40
	jr la011
.la019
	ld h,#51
	ld l,a
.la01c
	ld a,(hl)
	cp #19
	jr c,la02c
	ld a,l
	cp #4f
	jr c,la029
	ld hl,&50FF
.la029
	inc hl
	jr la01c
.la02c
	ld c,l
	inc a
	ld (hl),a
	ld hl,#67d0
	ld b,a
	ld de,#ffb0
.la036
	add hl,de
	djnz la036
	add hl,bc
	ld d,#c0
	ld e,c
	ld b,a
.la03e
	push bc
	ld b,#08
	push hl
	push de
.la043
	push bc
	ld bc,#0800
	ld a,(hl)
	ld (de),a
	add hl,bc
	ex de,hl
	add hl,bc
	ex de,hl
	pop bc
	djnz la043
	pop de
	pop hl
	ld bc,#0050
	add hl,bc
	ex de,hl
	add hl,bc
	ex de,hl
	pop bc
	djnz la03e
	pop bc
	dec bc
	ld a,b
	or c
	jr nz,la00e
	ret



;--- [remplir en rouge] ----
remplircouleur
	LD HL,&6000
	LD DE,&6001
	LD BC,&3FFF
remplir_avec_octet
	LD (HL),&F0 ;&F0 rouge vif (encre 5)
	LDIR
	RET

;---- [presentation sous forme de rouleau, image en &6000] ----
affichage_rouleau
		LD HL,ADR_ECRAN_DEPART
		LD DE,ADR_ECRAN_BANDE
		LD A,FIN_ROULEAU		
loopprs1:
		PUSH AF
		CALL &BD19
		CALL &BD19
		CALL sousprg1
		POP AF
		DEC A
		JR NZ,loopprs1
		RET

sousprg1:
		PUSH AF
		CP ADR_ECRAN_HAUTEUR
		JR C,sautsousprg1
		LD A,ADR_ECRAN_HAUTEUR
sautsousprg1:
		PUSH DE
		PUSH HL
		PUSH HL
		CALL sousprg2
		POP HL
		LD D,H
		LD E,L
		LD BC,#6000
		SBC HL,BC	;&C000 en &6000

		LD BC,#0050	;largeur ligne
		LDIR
		POP HL
		CALL &BC26	;ligne suivante
		POP DE
		POP AF
		CP ADR_ECRAN_HAUTEUR+1
		RET C
		EX DE,HL
		CALL &BC26	;ligne suivante
		EX DE,HL
		RET

sousprg2:
		OR A
		RET Z
loopsousprg2 :
		PUSH AF
		LD BC,#0050	;largeur ligne
		PUSH HL
		PUSH BC
		LD BC,#6000
		SBC HL,BC	;&C000 en &6000
		POP BC

		PUSH DE
		LDIR
		POP DE
		POP HL
		CALL &BC26	;ligne suivante
		EX DE,HL
		CALL &BC29	;ligne precedente
		EX DE,HL
		POP AF
		DEC A
		JR NZ,loopsousprg2
		RET

;- - - - - - [Couleurs] - - - - - -
couleurs
		CALL &BD19

		XOR A
		LD IX,donneescouleurs
loopcouleur	PUSH AF
		LD C,(IX+00)
		LD B,(IX+00)
		CALL #BC32
		INC IX
		POP AF
		INC A
		CP 16
		JR NZ,loopcouleur
		CALL &BD19
		RET
donneescouleurs	
		DEFB 0,1,2,3,4,6,9,11,12,13,15,16,17,24,25,26


;- - - - [ TOUTES LES ENCRES SONT NOIRES ] - - - - -
encres_en_noires
	LD BC,&0000
	CALL &BC38	;border 0

	;LES ENCRES
	XOR A
bouclecouleursnoires
	PUSH AF
	LD BC,0
	CALL &BC32
	CALL &BD19
	POP AF
	INC A
	CP 16
	JR NZ,bouclecouleursnoires
	RET

;------------------------------------------------------------------------------
; LECTURE SON
;------------------------------------------------------------------------------
LECTURE_SON

	DI
loopvox
	LD C,&08
	CALL Lecture
	INC HL
	DEC DE
	LD A,D
	OR E
	JR NZ,loopvox
	EI
	RET

Lecture
	PUSH DE
	LD E,(HL)
vox3
	SLA E
	LD A,0
	JR NC,vox1
voxvol
	LD A,15		;volume
vox1
	LD B,&F4
	LD D,9
	OUT (C),D
	LD D,A
	LD B,&F6
	IN A,(C)
	OR &C0
	OUT (C),A
	AND &3F
	OUT (C),A
	LD B,&F4
	OUT (C),D
	LD B,&F6
	LD D,A
	OR &80
	OUT (C),A
	OUT (C),D
voxvit
	LD A,5		;vitesse
vox2
	DEC A
	JR NZ,vox2
	DEC C
	JR NZ,vox3
	POP DE
	RET
