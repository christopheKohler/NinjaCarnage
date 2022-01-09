// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL21.BIN"

// -------------------------------------------------------
#define NBZONES 7

u8 leveldata[2] = { 1 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	8,13,9,14,1, // 
	2,9,3,10,2, // 
	3,4,4,5,3, //
	2,13,3,14,4, //
	8,9,9,10,5, // 
	7,4,8,5,6, // 
	5,0,6,2,7, // 
};

// -------------------------------------------------------
// Sequences
// -- Choice with cursor
// Time (in 10th of seconds 100=10s), for all zone, sucess or fails (fails = id + 100 )
//  { 100,101,101,101,1,0,0,0,0,0,0,0 } 
// -- Text. 250  Textid then 0
// 	{ 250,1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
// -- QTE
//    251 TIME WINTEXT ERRORTEXT sequence 1=haut, 2=bas, 3=droite, 4=gauche
// 	{ 251,25 , 8, 103, 1  , 1  , 2  ,2  , 0  , 0 },
//  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
// -- END
//  { 255,
// ----------------------------------------------------
#define SEQLINESIZE 10
u8 scenario[6][SEQLINESIZE] =
{
	{ 250, 1 ,0,0,0,0,0,0,0,0  }, // 
	{ 199, 2  ,101 ,101 , 102, 101, 101, 101, 0 , 0 }, // Going to 1
	{ 199, 0  , 3  , 101, 102, 102, 101, 101 , 0 , 0 }, // Going to 2
	{ 199, 0  , 0  , 4  , 102 ,102, 103, 101 , 0 , 0 }, // Going to 3
	{ 199, 0  , 0  , 0  , 102  ,102,103  , 5 , 0 , 0 }, // Going to 7
	{ 255, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END

};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL22 BIN"; // Must be 11 characters

// -------------------------------------------------------
/*
"                                      "; // 38
*/

// Text entering.
// I want to create texte that are displaying on 4 lines.
// Some words are in color.
// there is no real interest to center.
// A message will say if message is too long.
// 251 : couleur 1
// 252 : couleur 2 (positif)
// 253 : couleur 3 (negatif)
// 254 : Encode space + number of spaces
// 255 : next line

u8 messageintro[] = {
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je dois grimper en haut de cette"
 "fa�ade. J�ai pass� mon flocon en  "
"accrobranche option varappe. Nan je "
 "d�conne, j�ai des chaussons avec  "
  "des pointes...                  "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 3
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AIE\1. C'est bien trop haut. Je me"
 "r�tame la tronche par terre. Je   "
"suis pas une ch�vre des montagnes.  "
 "Trouve-moi un chemin plus s�r.    "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je saute sur cette poutre mais  "
 "j'ai le ventre \2TRANSPERCE\1 par des "
"pics. Je sentais bien qu'il y avait "
 "une couille avec cette fa�ade.    "
  "trouve-moi un meilleur chemin.  "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je saute sur la corniche. Mais  "
 "elle c�de sous mon poids et je    "
"\2TOMBE\1 comme une merde. J'ai grossi ?"
 "Je vais arr�ter la mayonnaise avec"
  "les sushis !                    "
}
};

#define NBMESSSUCESS 5 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Avec mes chaussures, je peux    "
 "monter sur n�importe quoi, m�me   "
"sur ta \2MERE\1. Bon soyons s�rieux. Ca "
 "sent le pi�ge � plein nez ce truc."
  "soyons sur nos gardes. On va o�?"
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Je m'accroche sur ce     "
 "premier point. C'est assez stable."
"Cette fa�ade est quand m�me bien    "
 "bizarre, il y a des trous dans le "
  "bois un peu partout.            "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je saute de l'autre c�t�.       "
 "\2BIEN VU\1. Jusque l� tout va bien.  "
"J'entends que �a s'agite dedans.    "
 "J'ai bien fait de passer par l�.  "
  "Je fais quoi maintenant ?       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2IMPECCABLE\1. Je bondis sur cette "
 "sorte de corniche. J'y suis       "
"presque. Ils vont pas me voir venir "
 "ces blaireaux. J'ai h�te de voir  "
  "leur gueule.                    "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Un dernier bond et voil�. Je    "
 "suis arriv� sur le toit. Ca me    "
"fait mal au cul de le dire, mais tu "
 "as \2ASSURE\1. Je dois trouver une    "
  "ouverture. Continuons de monter."
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x05, 0xa4, 0x5a, 0x58, 0x70, 0x0a, 0x00, 0x00,
	0x0f, 0x58, 0x0f, 0x58, 0xa5, 0x00, 0x00, 0x00,
	0x5a, 0xa5, 0x0f, 0xa4, 0xa5, 0x00, 0x00, 0x00,
	0x0e, 0x0f, 0x05, 0x0f, 0x0a, 0x0a, 0x00, 0x00,
	0xf0, 0x0f, 0x0a, 0x0f, 0x0a, 0x00, 0x00, 0x00,
	0x0d, 0x0f, 0x0f, 0x0f, 0x0a, 0x00, 0x00, 0x00,
	0xa5, 0x0f, 0x05, 0x0d, 0x05, 0x00, 0x00, 0x00,
	0x5a, 0x0f, 0x0f, 0xaf, 0x00, 0x0a, 0x00, 0x00,
	0x0f, 0x08, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0xa4, 0x0f, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x5a, 0x0d, 0x00, 0x0a, 0x00, 0x00, 0x00,
	0x0a, 0x0f, 0xa5, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x0f, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0xa5, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x5a, 0x0f, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x0f, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x5a, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xe1, 0xa0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x0c, 0x0d, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x50, 0x49, 0x58, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0e, 0x0c, 0x0c, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0xa4, 0x49, 0x0c, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x49, 0x0c, 0x08, 0x00, 0x00,
	0x00, 0x05, 0x0c, 0x49, 0x0c, 0x0d, 0x00, 0x00,
	0x00, 0x50, 0x0c, 0x49, 0x0c, 0x58, 0x00, 0x00,
	0x00, 0x04, 0x0c, 0x09, 0x0c, 0x0c, 0x00, 0x00,
	0x00, 0x0e, 0x0c, 0x49, 0x0c, 0x0c, 0x0a, 0x00,
	0x00, 0xa4, 0x0c, 0x83, 0x86, 0x0c, 0xa0, 0x00,
	0x00, 0x0c, 0x0c, 0x43, 0x06, 0x0c, 0x08, 0x00,
	0x05, 0x0c, 0x0c, 0x83, 0x86, 0x0c, 0x0d, 0x00,
	0x00, 0x00, 0x5a, 0x70, 0x58, 0x0e, 0xa5, 0x00,
	0x00, 0x00, 0x05, 0xf0, 0x0d, 0x5a, 0x0d, 0x0a,
	0x00, 0x00, 0x05, 0xa4, 0xa5, 0x0f, 0xf0, 0x0f,
	0x00, 0x00, 0x0a, 0x0f, 0x0f, 0x05, 0x0e, 0x0f,
	0x00, 0x00, 0x00, 0x0f, 0x0a, 0x0f, 0x5a, 0xa5,
	0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0d,
	0x00, 0x00, 0x05, 0x05, 0x0d, 0x05, 0x0f, 0xa5,
	0x00, 0x00, 0x0a, 0x05, 0xaf, 0x0f, 0x5a, 0x0e,
	0x00, 0x00, 0x00, 0x05, 0x0e, 0x0a, 0x0d, 0x0f,
	0x00, 0x00, 0x00, 0x05, 0x05, 0x0e, 0xa5, 0x0f,
	0x00, 0x00, 0x00, 0x0a, 0x05, 0x58, 0x0f, 0x0f,
	0x00, 0x00, 0x05, 0x00, 0x05, 0xa5, 0x0a, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x0e, 0x0f, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x0f, 0xa5, 0x05,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x5a, 0x0a,
	0x00, 0x00, 0x00, 0x0a, 0x00, 0x05, 0x0f, 0x05,
	0x0a, 0x0f, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x05, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0a, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
	0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e,
	0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4,
	0x0a, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0c, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0c, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c, 0x0c,
	0x50, 0x0c, 0x49, 0x17, 0x43, 0x0c, 0x58, 0x00,
	0x0e, 0x0c, 0x09, 0x2b, 0x2b, 0x0c, 0x0c, 0x0a,
	0x0e, 0x0c, 0x1d, 0x3f, 0x3f, 0x0c, 0x0c, 0x0a,
	0xa4, 0x0c, 0x1d, 0x3f, 0x3f, 0x0c, 0x0c, 0xa0,
	0x0c, 0x0c, 0x09, 0x3f, 0x2b, 0x0c, 0x0c, 0x0d,
	0x0c, 0x0c, 0x09, 0x2b, 0x2b, 0x0c, 0x0c, 0x58,
	0x0c, 0x0c, 0x49, 0x17, 0x43, 0x0c, 0x0c, 0x0c,
	0x0c, 0x49, 0x09, 0x2b, 0x2b, 0x49, 0x0c, 0x0c,
	0x0c, 0x83, 0x49, 0x43, 0x43, 0x09, 0x86, 0x0c,
	0x0c, 0x17, 0x0c, 0x86, 0x86, 0x1d, 0x06, 0x0c,
	0x0c, 0x3f, 0x0c, 0x49, 0x0c, 0x1d, 0x2e, 0x0c,
	0x0c, 0x6b, 0x0c, 0x86, 0x86, 0x49, 0x2e, 0x0c,
	0x0c, 0x86, 0x0c, 0x49, 0x0c, 0x0c, 0x86, 0x0c,
	0x49, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x49, 0x0c,
	0x1d, 0x86, 0xd2, 0x08, 0x58, 0x86, 0x97, 0x0c,
	0x1d, 0x86, 0x2e, 0x0f, 0x0e, 0x2e, 0x97, 0x0c,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0x0a, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0f, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x0a, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a,
	0xa0, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x0a,
	0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
	0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0c, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x0c, 0x49,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x1d,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x0c, 0x0c, 0x09,
	0x00, 0x00, 0x00, 0x00, 0x50, 0x0c, 0x0c, 0x49,
	0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c, 0x0c, 0x86,
	0x00, 0x00, 0x00, 0x00, 0xa4, 0x0c, 0x0c, 0x2e,
	0x00, 0x00, 0x00, 0x05, 0x0c, 0x0c, 0x86, 0x86,
	0x00, 0x00, 0x00, 0x50, 0x0c, 0x0c, 0x43, 0x0c,
	0x00, 0x00, 0x00, 0x0e, 0x0c, 0x0c, 0x6b, 0x48,
	0x00, 0x00, 0x00, 0xa4, 0x0c, 0x0c, 0x86, 0x49,
	0x00, 0x00, 0x05, 0x0c, 0x0c, 0x49, 0x0c, 0x0c,
	0x00, 0x00, 0x50, 0x0c, 0x0c, 0x1d, 0x0c, 0x86,
	0x00, 0x00, 0x0e, 0x0c, 0x49, 0x49, 0x0c, 0xc1,
	0x00, 0x00, 0xa4, 0x0c, 0x09, 0x86, 0x0c, 0xc2,
	0x00, 0x05, 0x0c, 0x0c, 0x1d, 0x86, 0x84, 0x0c,
	0x00, 0x5a, 0x0c, 0x49, 0x49, 0x0c, 0x6a, 0x0c,
	0x49, 0x0c, 0x84, 0x0c, 0x0c, 0x84, 0x49, 0x49,
	0x86, 0x0c, 0x1f, 0x0c, 0x1d, 0x0e, 0x0c, 0x97,
	0x86, 0xc1, 0x0d, 0x0c, 0x0d, 0x49, 0x84, 0x83,
	0x0c, 0xc2, 0x0c, 0x0c, 0x0c, 0x48, 0x86, 0x49,
	0x0c, 0x49, 0x2e, 0x0c, 0x0c, 0x6b, 0x0c, 0x0c,
	0x48, 0x86, 0x0e, 0x0c, 0x0c, 0x0e, 0xc2, 0x0c,
	0x48, 0xc1, 0x0d, 0x0c, 0x0d, 0x49, 0xc0, 0x0c,
	0x49, 0xc0, 0x0c, 0x0c, 0x0c, 0x48, 0xc1, 0x0c,
	0x86, 0x97, 0x0c, 0x0c, 0x0c, 0x1d, 0x86, 0xc2,
	0xc1, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x49, 0xc1,
	0x97, 0x86, 0xa4, 0x0c, 0x0c, 0xa4, 0x97, 0x86,
	0x49, 0x2e, 0x58, 0x0c, 0x58, 0x0c, 0x6b, 0x0c,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x49,
	0x6b, 0x58, 0x0c, 0x49, 0x0c, 0x58, 0x49, 0x6a,
	0x97, 0x0c, 0xa4, 0x83, 0x86, 0xa4, 0x1d, 0x86,
	0x0c, 0x0c, 0xa4, 0x17, 0x06, 0xa4, 0x0c, 0x0c,
	0x0c, 0x0c, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0c, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0c, 0x0c, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x86, 0x0c, 0x0c, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x2e, 0x0c, 0x0c, 0xa0, 0x00, 0x00, 0x00, 0x00,
	0x86, 0x86, 0x0c, 0x0d, 0x00, 0x00, 0x00, 0x00,
	0x49, 0x06, 0x0c, 0x58, 0x00, 0x00, 0x00, 0x00,
	0x49, 0x2e, 0x0c, 0x0c, 0x0a, 0x00, 0x00, 0x00,
	0x0c, 0x86, 0x0c, 0x0c, 0xa0, 0x00, 0x00, 0x00,
	0x0c, 0x49, 0x0c, 0x0c, 0x0d, 0x00, 0x00, 0x00,
	0x86, 0x1d, 0x0c, 0x0c, 0x58, 0x00, 0x00, 0x00,
	0x84, 0x49, 0x49, 0x0c, 0x0c, 0x0a, 0x00, 0x00,
	0x86, 0x0c, 0x83, 0x0c, 0x0c, 0xa0, 0x00, 0x00,
	0x0c, 0x84, 0x97, 0x0c, 0x0c, 0x0d, 0x00, 0x00,
	0x48, 0x2e, 0x49, 0x49, 0x0c, 0x58, 0x0a, 0x00,
	0x00, 0xa4, 0x0c, 0x09, 0x86, 0x0c, 0x0c, 0x84,
	0x05, 0x0c, 0x0c, 0x1d, 0x86, 0x84, 0x0c, 0x1d,
	0x50, 0x0c, 0x0c, 0x49, 0x0c, 0x1d, 0x0c, 0x0c,
	0x0e, 0x0c, 0x0c, 0x86, 0x0c, 0x0c, 0x06, 0x0c,
	0xa4, 0x0c, 0x49, 0x2e, 0x48, 0x0c, 0x0c, 0x0c,
	0x0c, 0x0c, 0x09, 0x86, 0x0c, 0x2e, 0x0c, 0xa4,
	0x0c, 0x0c, 0x97, 0x0c, 0x0c, 0x09, 0x0c, 0x0d,
	0x0c, 0x0c, 0x6b, 0x0c, 0x06, 0x0c, 0x0c, 0x0c,
	0x0c, 0x49, 0x86, 0x0c, 0x09, 0x0c, 0x58, 0x0c,
	0x0c, 0x1d, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0a,
	0x49, 0x49, 0x0c, 0x06, 0x0c, 0x58, 0x0c, 0x58,
	0x49, 0x0c, 0x0c, 0x09, 0x0c, 0x0c, 0x05, 0x0c,
	0x86, 0x0c, 0x0c, 0x0c, 0x58, 0x0c, 0x08, 0x5a,
	0x2e, 0x0c, 0x0c, 0x0c, 0x0c, 0x05, 0x0c, 0x0d,
	0x86, 0x04, 0x0c, 0x58, 0x0c, 0x08, 0x50, 0x0d,
	0x0c, 0x58, 0x0c, 0x0c, 0x05, 0x0c, 0x0f, 0x0e,
	0x0c, 0xa4, 0x0c, 0x3f, 0x2e, 0x0c, 0xa4, 0x0c,
	0x0c, 0x08, 0x0c, 0x3f, 0x2e, 0x08, 0x0c, 0x1d,
	0x0c, 0x08, 0x0c, 0x3f, 0x2e, 0x08, 0x0c, 0x0c,
	0xa4, 0x0c, 0xa4, 0x3f, 0x2e, 0xa4, 0x0c, 0xa4,
	0x08, 0xa4, 0x0c, 0x2b, 0x2e, 0x0c, 0xa0, 0x0c,
	0x0c, 0x04, 0xa4, 0x17, 0x06, 0xa4, 0x04, 0x0c,
	0x0c, 0x0c, 0x49, 0x2b, 0x6b, 0x0c, 0x0c, 0x0d,
	0x04, 0x0c, 0x09, 0x43, 0x03, 0x0c, 0x0c, 0x04,
	0x58, 0x0c, 0x83, 0x86, 0x83, 0x86, 0x58, 0x0c,
	0x0c, 0x58, 0x0c, 0x0c, 0x0c, 0x58, 0x0c, 0x08,
	0x0d, 0x0c, 0xa4, 0x0c, 0x0c, 0xa4, 0x0d, 0x58,
	0x08, 0x0e, 0x0d, 0x0c, 0x0d, 0x0c, 0x0a, 0x0c,
	0x0d, 0x05, 0x0e, 0x0c, 0x0c, 0x0f, 0x05, 0x58,
	0x0e, 0x0d, 0x00, 0x0c, 0x08, 0x05, 0x0c, 0x0f,
	0x00, 0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x08, 0x05,
	0x08, 0x00, 0x0e, 0x0c, 0x0c, 0x0a, 0x00, 0x0c,
	0x84, 0x0c, 0x0c, 0x83, 0x0c, 0x0c, 0xa0, 0x00,
	0x0c, 0x0c, 0x84, 0x97, 0x0c, 0x0c, 0x0d, 0x00,
	0x0c, 0x1d, 0x0c, 0x49, 0x0c, 0x0c, 0x58, 0x00,
	0x0c, 0x06, 0x0c, 0x0c, 0x86, 0x0c, 0x0c, 0x0a,
	0x0c, 0x0c, 0x48, 0x0c, 0x6b, 0x0c, 0x0c, 0xa0,
	0xa4, 0x0c, 0x2e, 0x0c, 0x83, 0x0c, 0x0c, 0x0d,
	0x0c, 0x09, 0x0c, 0x0c, 0x1d, 0x86, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0c, 0x06, 0x49, 0x2e, 0x0c, 0x0c,
	0x58, 0x0c, 0x09, 0x0c, 0x0c, 0xc3, 0x0c, 0x0c,
	0x0e, 0x0c, 0x0c, 0x0c, 0x0c, 0x1d, 0x0c, 0x0c,
	0x0c, 0x58, 0x0c, 0x0c, 0x06, 0x49, 0x49, 0x0c,
	0x0d, 0x04, 0x0c, 0x09, 0x0c, 0x0c, 0x49, 0x0c,
	0x0a, 0x0c, 0x58, 0x0c, 0x0c, 0x0c, 0x0c, 0x86,
	0x0c, 0x0d, 0x04, 0x0c, 0x0c, 0x0c, 0x0c, 0x2e,
	0x58, 0x00, 0x0c, 0x58, 0x0c, 0x0c, 0x04, 0xc3,
	0x0f, 0x0e, 0x0d, 0x04, 0x0c, 0x58, 0x0c, 0x1d,
	0x0d, 0x0c, 0x0c, 0xa4, 0x0d, 0x05, 0x0d, 0x05,
	0x0c, 0xa4, 0x0c, 0x08, 0x0e, 0x0d, 0x0f, 0x08,
	0xa4, 0x0c, 0x58, 0x0c, 0x00, 0x0c, 0x0a, 0xa4,
	0x08, 0x0c, 0x0c, 0x0e, 0x0d, 0x0f, 0x0d, 0x00,
	0x0c, 0xa0, 0x0c, 0x08, 0x04, 0x0a, 0x0e, 0x08,
	0x0c, 0x0c, 0x00, 0x0e, 0x0f, 0xa5, 0x00, 0x05,
	0x0c, 0xa4, 0x0d, 0x00, 0x0d, 0x0f, 0x0f, 0x00,
	0xa4, 0x08, 0x04, 0x58, 0x5a, 0x0a, 0x05, 0x0f,
	0x08, 0x0c, 0x0a, 0x0e, 0x0a, 0xf0, 0x0a, 0x00,
	0x0c, 0xa0, 0x0e, 0x0a, 0x0d, 0x00, 0x05, 0x0f,
	0x00, 0x0c, 0x05, 0x0a, 0x50, 0xa5, 0x00, 0x00,
	0x58, 0x00, 0x0d, 0x05, 0x0a, 0x00, 0x0f, 0x0a,
	0x04, 0x0d, 0x0f, 0x00, 0x0f, 0x0a, 0x00, 0x00,
	0x08, 0x05, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x0f, 0x0a, 0x00, 0x0f, 0x0a, 0x0a, 0x0a,
	0x0d, 0x0f, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0xa4, 0x0d, 0x00, 0x0c, 0x08, 0x05, 0x0c, 0xa5,
	0x00, 0x0c, 0x0d, 0x0c, 0x0d, 0x0c, 0x08, 0x00,
	0x0a, 0x00, 0x05, 0x0c, 0x0d, 0x00, 0x00, 0x0e,
	0x0e, 0x0f, 0x00, 0x0c, 0x08, 0x05, 0x0e, 0x0a,
	0x50, 0x0a, 0x00, 0x0c, 0x08, 0x00, 0x5a, 0x00,
	0x0d, 0x05, 0x00, 0x0d, 0x08, 0x05, 0x05, 0x0d,
	0x0a, 0x0a, 0x00, 0x0a, 0x0a, 0x00, 0x0a, 0x0a,
	0x25, 0x00, 0x00, 0x10, 0x00, 0x00, 0x05, 0x25,
	0x20, 0x00, 0x00, 0x61, 0x20, 0x00, 0x00, 0x20,
	0x0c, 0x00, 0x05, 0x97, 0x87, 0x00, 0x04, 0x0d,
	0x18, 0x00, 0x05, 0x3f, 0x2f, 0x00, 0x10, 0x08,
	0x41, 0x0a, 0x05, 0x6b, 0x2f, 0x00, 0x4b, 0x00,
	0x05, 0x20, 0x05, 0x86, 0x87, 0x00, 0x25, 0x00,
	0x05, 0x86, 0x05, 0x0c, 0x0d, 0x04, 0x87, 0x00,
	0x04, 0x0c, 0x00, 0x0c, 0x08, 0x04, 0x0c, 0x00,
	0x0e, 0x0c, 0x00, 0x0c, 0x08, 0x04, 0x0c, 0x0a,
	0x05, 0x0d, 0x05, 0x0c, 0xa4, 0x0c, 0x0d, 0x49,
	0x0d, 0x0f, 0x0c, 0x0a, 0x0c, 0x0c, 0xa4, 0x0c,
	0xa0, 0x0e, 0x08, 0x04, 0x58, 0x0c, 0x0c, 0xa5,
	0x05, 0x0d, 0x0f, 0x0c, 0x0e, 0x0c, 0x08, 0x0c,
	0x0c, 0x0a, 0x0e, 0x00, 0x0c, 0x08, 0xa4, 0x0c,
	0x00, 0x05, 0xa5, 0x0e, 0x0a, 0x04, 0x0c, 0x0c,
	0x05, 0x0f, 0x0f, 0x08, 0x05, 0x0c, 0xa4, 0x0c,
	0x0f, 0x00, 0x5a, 0x5a, 0x0c, 0x00, 0x0c, 0xa0,
	0x00, 0x5a, 0xa0, 0x0e, 0x0a, 0x0e, 0x08, 0x0c,
	0x0f, 0x00, 0x05, 0x08, 0x0e, 0x0a, 0xa4, 0x08,
	0x00, 0x05, 0xf0, 0x00, 0x0f, 0x04, 0x08, 0x04,
	0x0f, 0x0a, 0x00, 0x0f, 0x05, 0x08, 0x50, 0x0c,
	0x00, 0x00, 0x0f, 0x0a, 0x05, 0x0f, 0x0c, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x0f, 0x0f, 0x00, 0x0c,
	0x0a, 0x0a, 0x0f, 0x0a, 0x00, 0x0f, 0x0f, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x0f, 0x05, 0x0f, 0x0c,
	0x0f, 0x0f, 0x0a, 0x00, 0x0f, 0x05, 0x00, 0x50,
	0x0f, 0x0a, 0x00, 0x0f, 0x0a, 0x00, 0x00, 0xa4,
	0x0a, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x05, 0x0c,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x0c,
	0x00, 0x00, 0x00, 0x0f, 0x05, 0x00, 0xa4, 0x0c,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x50, 0x0c, 0xa4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x0d, 0x05,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x0c, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x0c, 0x0d, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0x0c, 0x0d, 0x0a, 0x05,
	0x00, 0x00, 0x05, 0x0e, 0x0c, 0x0e, 0x0f, 0x05,
	0x00, 0x00, 0x0e, 0x0f, 0x0d, 0x0e, 0x1a, 0x05,
	0x00, 0x05, 0x0f, 0x0e, 0x0c, 0xae, 0x0f, 0x08,
	0x00, 0x05, 0x0f, 0x0c, 0x75, 0x2f, 0x0e, 0x0a,
	0x00, 0x0f, 0x0f, 0x0f, 0xbf, 0xae, 0x0f, 0x0a,
	0x0c, 0x0c, 0x00, 0x0c, 0x08, 0x04, 0x0c, 0x58,
	0x0c, 0x0c, 0x00, 0x18, 0x08, 0x04, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0a, 0x30, 0x20, 0x0e, 0x0c, 0x0c,
	0x0d, 0x0c, 0x08, 0x24, 0x20, 0x0c, 0x0d, 0x0c,
	0x08, 0x0e, 0x0c, 0x49, 0x0c, 0x0c, 0x0a, 0x0c,
	0x08, 0x05, 0x0e, 0x97, 0x86, 0x0f, 0x00, 0x0c,
	0x08, 0x05, 0x4b, 0x3f, 0x6b, 0x0f, 0x00, 0x0d,
	0x0f, 0x0e, 0x1a, 0x6b, 0x3a, 0x0e, 0x0f, 0x0a,
	0x05, 0x0c, 0x0f, 0x86, 0x87, 0x0e, 0x0d, 0x00,
	0x0f, 0x0c, 0x0a, 0x0c, 0x08, 0x0e, 0x0d, 0x0a,
	0x0d, 0x0e, 0x00, 0x0c, 0x08, 0x04, 0x0f, 0x0d,
	0x0c, 0x04, 0x0a, 0x49, 0x08, 0x0e, 0x04, 0x0d,
	0x0d, 0x04, 0x0a, 0xc3, 0x82, 0x0e, 0x05, 0x0d,
	0x0f, 0x05, 0x08, 0x86, 0x82, 0x0d, 0x05, 0x0a,
	0x0d, 0x00, 0x0c, 0x49, 0x0c, 0x08, 0x05, 0x08,
	0x0e, 0x00, 0x04, 0x97, 0x86, 0x00, 0x04, 0x0a,
	0x00, 0x05, 0x05, 0x0a, 0x00, 0x0f, 0x0f, 0x0a,
	0xa0, 0x00, 0x00, 0x0f, 0x0a, 0x00, 0x0f, 0x0e,
	0x0d, 0x00, 0x00, 0x00, 0x05, 0x0f, 0x00, 0x0a,
	0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05,
	0x0c, 0xa0, 0x05, 0x05, 0x0a, 0x00, 0x00, 0x05,
	0xa4, 0x58, 0x00, 0x00, 0x00, 0x05, 0x05, 0x00,
	0x05, 0x0c, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0e, 0x0c, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x25, 0x0c, 0x0d, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0f, 0x0c, 0x0c, 0x0a, 0x00, 0x00, 0x00,
	0x05, 0x0e, 0x0e, 0x0c, 0x0f, 0x00, 0x00, 0x00,
	0x10, 0x0e, 0x0f, 0x0d, 0x0e, 0x0a, 0x00, 0x00,
	0x0d, 0x0e, 0xae, 0x0c, 0x0f, 0x0f, 0x00, 0x00,
	0x0e, 0x0f, 0x7f, 0x24, 0x0d, 0x0f, 0x00, 0x00,
	0x0f, 0x0e, 0xbf, 0xaf, 0x0f, 0x0f, 0x0a, 0x00,
	0x05, 0x0f, 0x0a, 0x1a, 0x6a, 0x7f, 0x0d, 0x00,
	0x0f, 0x0f, 0x00, 0x5f, 0xc0, 0x3a, 0x0d, 0x00,
	0x0f, 0x00, 0x0f, 0xb5, 0x95, 0xaf, 0x0f, 0x00,
	0x0a, 0x00, 0x0a, 0xe0, 0xd5, 0x25, 0x0f, 0x10,
	0x00, 0x05, 0x00, 0xb5, 0x30, 0x0f, 0x0f, 0x0d,
	0x00, 0x00, 0x05, 0x1a, 0x25, 0x0f, 0x48, 0x0d,
	0x00, 0x00, 0x0f, 0x5a, 0x05, 0x0e, 0xc0, 0x87,
	0x0a, 0x0a, 0x0a, 0x00, 0x05, 0xc2, 0xc0, 0x2f,
	0x0f, 0x00, 0x00, 0x05, 0x00, 0xc0, 0xd5, 0x0f,
	0x5a, 0x0a, 0x00, 0x05, 0x05, 0xc0, 0xae, 0x0f,
	0xba, 0x00, 0x00, 0x00, 0x50, 0x84, 0x0f, 0x0f,
	0x2f, 0x00, 0x00, 0x05, 0x50, 0x0f, 0x0f, 0x0a,
	0x0a, 0x00, 0x00, 0x05, 0x00, 0x05, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0f, 0x0a, 0x00, 0x00, 0x0a, 0x00, 0x00,
	0x05, 0x20, 0x0f, 0x00, 0x00, 0x0a, 0x00, 0x0f,
	0x04, 0x0a, 0x4b, 0x2e, 0x6b, 0x0a, 0x0e, 0x00,
	0x00, 0x0d, 0x1a, 0x0c, 0x18, 0x0f, 0x08, 0x00,
	0x00, 0x0e, 0x0a, 0x0c, 0x08, 0x0e, 0x0a, 0x00,
	0x20, 0x0e, 0x00, 0x0c, 0x08, 0x04, 0x0a, 0x30,
	0x0c, 0x04, 0x00, 0x0c, 0x08, 0x04, 0x04, 0x0d,
	0x0d, 0x04, 0x0a, 0x0c, 0x08, 0x0e, 0x05, 0x0d,
	0x0d, 0x05, 0x08, 0x0c, 0x08, 0x0d, 0x05, 0x0d,
	0x0d, 0x00, 0x0d, 0x0c, 0x0d, 0x08, 0x05, 0x0d,
	0x0c, 0x00, 0x04, 0x49, 0x0c, 0x00, 0x04, 0x0d,
	0x0c, 0x00, 0x04, 0xc3, 0x86, 0x00, 0x04, 0x0d,
	0x04, 0x0a, 0x1a, 0x86, 0x92, 0x0a, 0x0e, 0x05,
	0x05, 0x0e, 0x00, 0x0c, 0x08, 0x04, 0x0f, 0x00,
	0x05, 0x0e, 0x00, 0x0c, 0x08, 0x04, 0x0f, 0x00,
	0x00, 0x04, 0x00, 0x0c, 0x08, 0x04, 0x00, 0x00,
	0x00, 0x04, 0x0a, 0x08, 0x08, 0x0e, 0x00, 0x00,
	0x00, 0x04, 0x0a, 0x00, 0x00, 0x0e, 0x00, 0x05,
	0x05, 0x5d, 0x6a, 0x3a, 0x0a, 0x0f, 0x0f, 0x00,
	0x05, 0x18, 0x6a, 0xd5, 0x0a, 0x05, 0x0f, 0x0a,
	0x05, 0x0f, 0xbf, 0x95, 0xa5, 0x0a, 0x05, 0x0a,
	0x05, 0x0f, 0x75, 0xc0, 0xa0, 0x0a, 0x00, 0x0a,
	0x0d, 0x0f, 0x1a, 0x35, 0xa0, 0x05, 0x00, 0x05,
	0x48, 0x0d, 0x0f, 0x30, 0x0f, 0x00, 0x00, 0x05,
	0xc2, 0x84, 0x0f, 0x50, 0x0f, 0x0a, 0x00, 0x00,
	0x6a, 0xc0, 0x87, 0x00, 0x00, 0x0a, 0x0a, 0x0f,
	0x5f, 0xc0, 0x80, 0x05, 0x00, 0x00, 0x05, 0x4a,
	0x0e, 0xea, 0x85, 0x05, 0x00, 0x00, 0x5a, 0x4a,
	0x0f, 0x0e, 0xd0, 0x00, 0x00, 0x00, 0x10, 0xea,
	0x0f, 0x0f, 0x5a, 0x05, 0x00, 0x00, 0x05, 0x6a,
	0x00, 0x0f, 0x00, 0x05, 0x00, 0x00, 0x00, 0x4a,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x0a, 0x00, 0x00, 0x0f, 0x0a, 0x0a,
	0x0a, 0x00, 0x0a, 0x00, 0x05, 0x0a, 0x25, 0x05,
	0x1a, 0x25, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x60, 0x25, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0x0a, 0x0f, 0x00, 0x05, 0x0a, 0x00, 0x05,
	0xaf, 0x00, 0x05, 0x00, 0x05, 0x00, 0x00, 0x1a,
	0x0a, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0xf8,
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x05, 0x27,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x50, 0x85,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x11, 0x27,
	0x00, 0x05, 0x00, 0x05, 0x00, 0x00, 0x1b, 0xad,
	0x00, 0x0a, 0x00, 0x1a, 0x0a, 0x00, 0xf4, 0xa5,
	0x00, 0x00, 0x05, 0xbd, 0x05, 0x05, 0xf8, 0x0f,
	0x00, 0x00, 0x50, 0x62, 0x0f, 0x50, 0x0f, 0x0f,
	0x00, 0x5a, 0x50, 0x91, 0x0a, 0x0f, 0x0f, 0x0a,
	0x00, 0xd0, 0x54, 0x76, 0xa0, 0x0f, 0x0a, 0x05,
	0x05, 0x90, 0x54, 0xf8, 0x0f, 0x0f, 0x00, 0x0e,
	0x05, 0xd0, 0x54, 0xa0, 0x0f, 0x00, 0x0e, 0x0c,
	0x00, 0x0f, 0x00, 0x00, 0x00, 0x05, 0x0a, 0x00,
	0x05, 0x85, 0x00, 0x00, 0x00, 0x05, 0x85, 0x00,
	0x50, 0x26, 0x08, 0x05, 0x00, 0x0c, 0x72, 0x05,
	0x54, 0xae, 0x49, 0x5f, 0x4b, 0x0c, 0xfe, 0x10,
	0x54, 0x86, 0x0c, 0xae, 0xae, 0x0c, 0xd6, 0x50,
	0x50, 0x58, 0x0f, 0x08, 0x0d, 0x5a, 0x58, 0x05,
	0x50, 0x1a, 0xa0, 0x00, 0x00, 0xb0, 0x5a, 0x05,
	0x05, 0x1a, 0x25, 0x04, 0x05, 0x30, 0x0f, 0x05,
	0x05, 0x18, 0x87, 0x0c, 0x0d, 0x92, 0x0d, 0x05,
	0x0e, 0x1a, 0x2f, 0x0c, 0x0d, 0x3a, 0x0e, 0x0f,
	0x0f, 0x10, 0x87, 0x0c, 0x0d, 0x92, 0x05, 0x0f,
	0x00, 0x1a, 0x25, 0x0c, 0x0d, 0x30, 0x0a, 0x05,
	0x04, 0x58, 0x25, 0x0c, 0x0d, 0x70, 0x0c, 0x00,
	0x0c, 0x0d, 0x25, 0x0c, 0x0d, 0x25, 0x0c, 0x0d,
	0x0c, 0x0c, 0xa5, 0x0c, 0x0d, 0xa4, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x30, 0x0a,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0x60, 0x61,
	0x00, 0x00, 0x0f, 0x00, 0x05, 0x0a, 0x5f, 0x6a,
	0x0a, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05, 0xea,
	0xa8, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x5f,
	0x27, 0x00, 0x00, 0x05, 0x0a, 0x00, 0x00, 0x00,
	0xd0, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00,
	0xb9, 0x0a, 0x00, 0x05, 0x00, 0x05, 0x00, 0x00,
	0xf4, 0xa0, 0x00, 0x1a, 0x0a, 0x00, 0x0a, 0x00,
	0x5a, 0xad, 0x05, 0x15, 0xad, 0x00, 0x00, 0x00,
	0x0f, 0x5a, 0x05, 0x4a, 0x72, 0x00, 0x00, 0x00,
	0x0f, 0x0f, 0x0a, 0x1b, 0xd0, 0x50, 0x0a, 0x00,
	0x00, 0x0f, 0x0a, 0xf4, 0x76, 0x50, 0x80, 0x00,
	0x0a, 0x05, 0x0f, 0x5a, 0xfc, 0x10, 0x85, 0x00,
	0x0c, 0x0a, 0x05, 0x0a, 0xf4, 0x50, 0x85, 0x00
};