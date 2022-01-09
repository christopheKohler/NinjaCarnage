// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL61.BIN"

// -------------------------------------------------------
#define NBZONES 4

u8 leveldata[2] = { 1 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	4,6,7,7,1, // 
	8,9,9,10,2, // 
	4,10,5,11,3, // 
	6,12,7,13,4, // 

};

// -------------------------------------------------------
// Sequences
// -- Choice with cursor
// Time (in 10th of seconds 100=10s), for all zone, sucess or fails (fails = id + 100 )
//  { 100,101,101,101,1,0,0,0,0,0,0,0 } 
// -- Test. 250  Textid then 0
// 	{ 250,1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
// -- QTE
//    251 TIME WINTEXT ERRORTEXT sequence 1=haut, 2=bas, 3=droite, 4=gauche
// 	{ 251,25 , 8, 103, 1  , 1  , 2  ,2  , 0  , 0 },
//  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
// -- END
//  { 255,
// ----------------------------------------------------
#define SEQLINESIZE 10
u8 scenario[16][SEQLINESIZE] =
{
	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 199, 2  , 101, 101, 101,0  , 0  , 0 , 0 , 0 }, // Text
													{251, 45, 3,  102 , 1  ,4  , 1  , 4  ,2 , 3}, //
													{ 4  ,3   , 2  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 199, 0  , 4  , 101, 101,0  , 0  , 0 , 0 , 0 }, // Text
													{251, 45, 5,  103 , 1  ,2  , 3  , 4  , 3 , 2}, //
													{ 1  ,4   , 1  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 199, 0  , 0  , 6  , 101,0  , 0  , 0 , 0 , 0 }, // Text
													{251, 45, 7,  103 , 2  ,4  , 1  , 2  ,3 , 4}, //
													{ 1  ,3   , 2  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 199, 0  , 0  , 0  , 8  ,0  , 0  , 0 , 0 , 0 }, // Text
													{251, 45, 9,  103 , 4  ,1  , 3  , 2  ,3 , 4}, //
													{ 2  ,1   , 3  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 250, 10 , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 254, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Erase background
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL62 BIN"; // Must be 11 characters

// -------------------------------------------------------
/*
"                                      "; // 38
*/

u8 messageintro[] = {
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ah ben voil� autre chose ! La   "
 "seule issue semble �tre ce puits. "
"Ca glisse autant que du vomi de     "
 "moines Shaolin un lendemain de    "
  "cuite � la Asahi !              "
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
  "\2NON\1. C'est bien trop loin �a !  "
 "Je saute pour descendre plus vite "
"Eh bien c'est r�ussi ! J'ai tout    "
 "d�val� la t�te la premi�re et je  "
  "meurs 30 m�tres plus bas !      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ah \2NON\1 couillon. Je d�rape et je"
 "glisse. Impossible d'attraper     "
"quelque chose, et je m'�crase       "
 "30 m�tres plus bas. Et un ninja   "
  "tout plat !                     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1 Gros naze. C'est pourtant   "
 "pas difficile d'appuyer sur des   "
"fl�ches non ? En attendant je glisse"
 "et je me fracasse la t�te         "
  "30 m�tres plus bas...           "
}

};

#define NBMESSSUCESS 10 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Va falloir faire gaffe et y     "
 "aller doucement et progressivement"
"Tu vas devoir t'assurer que je ne   "
 "perde pas mon �quilibre. Ca a     "
  "l'air profond !                 "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2TRES BIEN\1. Je cale mon pied sur "
 "le premier trou.                  "
"                                    "
 "         \2FAIS LA SEQUENCE\1         "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2PARFAIT\1. J'ai gard� mon         "
 "�quilibre. Ces puits sont souvent "
"le rep�re d'esprits vengeurs.       "
 "J'esp�re que ce sera pas le cas   "
  "ici...                          "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OK\1. J'arrive sur le second trou."
 "Jusque-l� tout se passe bien.     "
"                                    "
 "         \2FAIS LA SEQUENCE\1         "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Tout est sous contr�le.  "
 "Au japon, il n'est pas rare qu'on "
"balance des corps dans des puits.   "
 "C'est pour �a que les esprits     "
  "vengeurs sont souvent l�.       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. C'est bon pour le         "
 "troisi�me trou...                 "
"                                    "
 "         \2FAIS LA SEQUENCE\1         "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 7
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2IMPEC\1. Allez, on a bient�t fait "
 "la premi�re partie de la descente."
"et pas la queue d'un \2ONRYO\1 pour le  "
 "moment. Pourtant je suis certain  "
  "qu'il y a des horreurs en bas.  "
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2PARFAIT\1. C'est bon pour le      "
 "dernier trou.                     "
"                                    "
 "         \2FAIS LA SEQUENCE\1         "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 9
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2YEAHHHHH\1. Tout s'est bien pass�."
 "Trankil wesh gros. On va descendre"
"comme �a. Je t'avais bien dit que   "
 "mes crampons pouvaient franchir   "
  "n'importe quoi !                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 10
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AHHH MERDE\1. Un gros bloc de     "
 "pierre se d�tache. Je tente de me "
"rattraper sur ce que je peux, mais  "
 "impossible. Je tombe dans le puits"
  "�a va �tre chaud l� !!!!        "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0xa5, 0x5a, 0x70, 0x20, 0x70, 0x70, 0xf0, 0x30,
	0xf0, 0xb0, 0xb0, 0x0a, 0xb0, 0xb0, 0xb0, 0xb0,
	0xa5, 0xf0, 0x70, 0x20, 0xf0, 0x70, 0x70, 0x30,
	0xf0, 0xf0, 0xb0, 0x0a, 0xb0, 0xb0, 0xb0, 0xb0,
	0xa5, 0xf0, 0x70, 0x0a, 0x70, 0x70, 0x70, 0x30,
	0x5a, 0x5a, 0xb0, 0x0a, 0xf0, 0x30, 0x30, 0x30,
	0x0f, 0xf0, 0x70, 0x0a, 0x70, 0x30, 0x30, 0x92,
	0x0f, 0x5a, 0xb0, 0x0a, 0xf0, 0xb0, 0x61, 0x61,
	0x0f, 0xf0, 0x70, 0x0a, 0x70, 0x30, 0x30, 0x92,
	0x0f, 0x5a, 0xb0, 0x0a, 0xf0, 0xb0, 0x30, 0x61,
	0x0f, 0x5a, 0xf0, 0x0a, 0x70, 0x70, 0x30, 0x30,
	0x0f, 0x5a, 0xb0, 0x0a, 0x70, 0xb0, 0xb0, 0x30,
	0x0a, 0x5a, 0xf0, 0x0a, 0x70, 0x70, 0x30, 0x30,
	0x0f, 0x5a, 0xb0, 0x0a, 0x70, 0xf0, 0xb0, 0xb0,
	0x0a, 0x5a, 0xf0, 0x0a, 0x70, 0xf0, 0xf0, 0x70,
	0x0f, 0x5a, 0xb0, 0x0a, 0xf0, 0xb0, 0xf0, 0xb0,
	0x30, 0x92, 0x70, 0x61, 0xc3, 0xc3, 0x30, 0x70,
	0xb0, 0x30, 0xb0, 0xc3, 0x92, 0x92, 0x92, 0xb0,
	0x30, 0x30, 0x70, 0x61, 0x61, 0xc3, 0x70, 0xd2,
	0x92, 0x92, 0xb0, 0xc3, 0x92, 0x92, 0x92, 0xe1,
	0x61, 0x30, 0x30, 0x61, 0x30, 0xc3, 0x70, 0x70,
	0x92, 0x92, 0xb0, 0x92, 0x30, 0x92, 0x30, 0xf0,
	0x61, 0x30, 0x61, 0x30, 0x61, 0x70, 0x70, 0xf0,
	0x30, 0x30, 0xb0, 0x92, 0x30, 0x92, 0xb0, 0xf0,
	0x92, 0x70, 0x61, 0x61, 0x61, 0xc3, 0x70, 0xf0,
	0x61, 0x30, 0xb0, 0x92, 0x92, 0xb0, 0xb0, 0xf0,
	0x92, 0x70, 0xf0, 0x61, 0x70, 0x61, 0x70, 0xf0,
	0x30, 0xb0, 0xb0, 0xc3, 0xf0, 0xb0, 0x92, 0xf0,
	0x70, 0x70, 0x70, 0x61, 0x70, 0x30, 0x61, 0x70,
	0xf0, 0xb0, 0x30, 0x92, 0x92, 0xb0, 0xb0, 0xb0,
	0x70, 0x70, 0x70, 0xe1, 0xc3, 0x61, 0x70, 0x30,
	0xb0, 0xb0, 0xb0, 0x1a, 0x92, 0xb0, 0x92, 0xb0,
	0x61, 0x50, 0x70, 0xd2, 0x70, 0x70, 0x70, 0x5a,
	0xb0, 0x05, 0xe1, 0xb0, 0xb0, 0xf0, 0xf0, 0x4b,
	0xe1, 0x05, 0x70, 0x70, 0xf0, 0xf0, 0xf0, 0x50,
	0xf0, 0x05, 0xb0, 0xf0, 0x5a, 0xf0, 0xf0, 0x1a,
	0xf0, 0x0a, 0x30, 0x70, 0xf0, 0x5a, 0x5a, 0x50,
	0xf0, 0x0a, 0xb0, 0xb0, 0xf0, 0xf0, 0xf0, 0x10,
	0xf0, 0x0a, 0x70, 0x70, 0xf0, 0x70, 0x70, 0x50,
	0xf0, 0xa0, 0xb0, 0xb0, 0xb0, 0xb0, 0xf0, 0x5a,
	0xf0, 0x0a, 0x70, 0x70, 0x70, 0x61, 0x70, 0x50,
	0xf0, 0xa0, 0x30, 0x92, 0x92, 0xb0, 0x92, 0x1a,
	0xf0, 0xa0, 0x61, 0x70, 0x70, 0xc3, 0x70, 0x50,
	0xf0, 0xa0, 0x92, 0xb0, 0x92, 0x92, 0x92, 0x1a,
	0xf0, 0xa0, 0x61, 0x61, 0x61, 0xc3, 0x70, 0x1a,
	0xf0, 0xa0, 0x92, 0x92, 0xb0, 0x92, 0x92, 0x1a,
	0x70, 0x0a, 0x61, 0x70, 0x70, 0x61, 0x70, 0x10,
	0xf0, 0xa0, 0xb0, 0x92, 0x92, 0x92, 0x70, 0x1a,
	0x0a, 0x5a, 0xf0, 0x0a, 0x70, 0x70, 0x70, 0x70,
	0x05, 0x5a, 0xb0, 0x0a, 0xf0, 0xb0, 0xf0, 0xf0,
	0x0a, 0x5a, 0xf0, 0x0a, 0x70, 0x70, 0xf0, 0x70,
	0x05, 0x5a, 0xb0, 0x0a, 0xa5, 0xb0, 0xf0, 0x05,
	0x0a, 0x5a, 0x5a, 0x0a, 0x5a, 0x0a, 0x0a, 0x00,
	0x05, 0x0f, 0xa5, 0x0a, 0xa5, 0x05, 0x05, 0xa5,
	0x0f, 0x5a, 0x5a, 0x0a, 0x0a, 0x0a, 0x5a, 0xf0,
	0x05, 0x0f, 0x0f, 0x00, 0x05, 0x05, 0xf0, 0x49,
	0x0f, 0x5a, 0x0a, 0x0a, 0x0a, 0x5a, 0xc3, 0x57,
	0x05, 0x0f, 0x05, 0x05, 0x0f, 0xe1, 0x83, 0xab,
	0x0f, 0x0a, 0x0a, 0x0f, 0x5a, 0xd7, 0x12, 0xb0,
	0x0f, 0x05, 0x05, 0x0f, 0xe1, 0xab, 0x70, 0x70,
	0x0a, 0x00, 0x5a, 0x5a, 0x57, 0x92, 0xa4, 0xa4,
	0x0a, 0x0f, 0xf0, 0xa1, 0xeb, 0x24, 0x49, 0x58,
	0x05, 0x5a, 0xf5, 0x52, 0x92, 0xc3, 0x86, 0x0f,
	0x0f, 0xf0, 0xab, 0x86, 0xe1, 0xeb, 0xa5, 0x5a,
	0xf0, 0x70, 0xb0, 0x70, 0x70, 0xf0, 0x61, 0x70,
	0xb0, 0xf0, 0xb0, 0xa5, 0x0a, 0x0f, 0xb0, 0xb0,
	0xa5, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xf0,
	0x00, 0x05, 0x05, 0x05, 0xb0, 0xa5, 0x0a, 0x05,
	0x0a, 0x0a, 0x0a, 0x0f, 0xf0, 0xf0, 0xf0, 0x0a,
	0xf0, 0xf0, 0xa5, 0x1a, 0xb0, 0xf0, 0xf0, 0xf0,
	0x86, 0x86, 0x86, 0xa4, 0x86, 0x86, 0xf0, 0xa5,
	0x83, 0x83, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x58,
	0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0x43,
	0xab, 0xab, 0xab, 0xab, 0xab, 0x83, 0xab, 0xab,
	0xb0, 0xf0, 0x58, 0x58, 0x58, 0x58, 0x92, 0xd7,
	0x70, 0xa4, 0xa4, 0xa4, 0xa4, 0xf0, 0xf0, 0x24,
	0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0x0c, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x24, 0x24, 0x24,
	0x0f, 0x58, 0x18, 0x58, 0x58, 0x58, 0x58, 0xd2,
	0x58, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x18,
	0xf0, 0x0a, 0x70, 0x70, 0x61, 0x30, 0x70, 0x50,
	0xb0, 0xa0, 0xb0, 0xb0, 0x92, 0x30, 0xf0, 0x1a,
	0x70, 0x0a, 0xf0, 0x70, 0x61, 0x70, 0x70, 0x50,
	0xa5, 0x05, 0xf0, 0xb0, 0xb0, 0xb0, 0x30, 0x50,
	0x00, 0x05, 0xf0, 0x30, 0x70, 0x70, 0x70, 0x05,
	0xa5, 0x0a, 0x1a, 0xb0, 0xb0, 0xf0, 0xf0, 0x05,
	0xa5, 0x0f, 0x0f, 0x05, 0xf0, 0x70, 0xf0, 0x50,
	0xb0, 0xa5, 0xb0, 0x00, 0x0f, 0xf0, 0xf0, 0x1a,
	0x86, 0xd2, 0x5a, 0xa0, 0x05, 0x0f, 0xa5, 0xf0,
	0xab, 0xc3, 0x1a, 0x5a, 0x0a, 0x00, 0x5a, 0x5a,
	0x57, 0xff, 0x52, 0x5a, 0x0f, 0x0a, 0x0f, 0xf0,
	0x61, 0x83, 0xab, 0xd2, 0xb0, 0x0f, 0x0f, 0x0f,
	0xb0, 0xc3, 0x57, 0x43, 0xf0, 0xf0, 0x0f, 0x05,
	0xd2, 0xb0, 0x49, 0xab, 0xd2, 0xf0, 0xa5, 0x00,
	0x0c, 0x58, 0xb0, 0xc3, 0x06, 0xa5, 0x5a, 0x0a,
	0xb0, 0x49, 0x58, 0x61, 0xc3, 0x58, 0xa5, 0x0f,
	0x0f, 0xa4, 0x06, 0x58, 0xd7, 0xd2, 0x0f, 0xa4,
	0x1a, 0xa1, 0x49, 0xa1, 0xeb, 0x0d, 0xf0, 0x49,
	0x5a, 0xeb, 0x0c, 0x57, 0x06, 0xa5, 0x58, 0x0c,
	0x4b, 0x0c, 0xa1, 0xa1, 0xc3, 0x58, 0x49, 0x49,
	0x24, 0xd2, 0x57, 0x43, 0x86, 0x86, 0x86, 0x86,
	0x0c, 0x83, 0xeb, 0xc3, 0x49, 0x49, 0x49, 0x09,
	0xc3, 0x57, 0xc3, 0x86, 0x86, 0x86, 0xc3, 0x17,
	0x83, 0xeb, 0xc3, 0xc3, 0x49, 0x49, 0x83, 0x83,
	0x57, 0xc3, 0xc3, 0x86, 0x86, 0xc3, 0x43, 0x75,
	0xeb, 0xc3, 0xc3, 0x0c, 0x49, 0xab, 0x92, 0x18,
	0xc3, 0xc3, 0xc3, 0x49, 0xc3, 0xc3, 0x24, 0xf5,
	0xc3, 0xc3, 0x0c, 0x49, 0xc3, 0x92, 0xfa, 0x1a,
	0xc3, 0x86, 0x86, 0xc3, 0xc3, 0x75, 0x70, 0x5a,
	0xc3, 0x0c, 0x49, 0x09, 0xc3, 0xba, 0xba, 0x0f,
	0xc3, 0x49, 0x0c, 0xc3, 0x86, 0xf5, 0x70, 0x05,
	0xc3, 0x86, 0x49, 0x83, 0x58, 0xb0, 0xa5, 0x0f,
	0xa4, 0x86, 0x86, 0x86, 0x86, 0x86, 0x86, 0xc3,
	0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49, 0x49,
	0x86, 0x86, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x86,
	0x49, 0x49, 0xc3, 0xeb, 0xeb, 0xeb, 0xeb, 0xeb,
	0xd7, 0xd7, 0x57, 0x57, 0xd7, 0x57, 0x57, 0x57,
	0x7f, 0x2b, 0x2b, 0x83, 0xeb, 0xff, 0xab, 0xab,
	0x17, 0x57, 0x86, 0x86, 0x86, 0xc3, 0xd7, 0xff,
	0xab, 0xeb, 0x0c, 0x0c, 0x0c, 0x0c, 0x49, 0xff,
	0x61, 0xa4, 0xf0, 0xf0, 0x58, 0x0c, 0x0c, 0x49,
	0xd2, 0x58, 0x0f, 0x0f, 0x25, 0xa5, 0xb0, 0x0c,
	0x24, 0xa5, 0x0f, 0x0f, 0x5a, 0x0f, 0x0e, 0x24,
	0xb0, 0x0f, 0x0f, 0x0f, 0xa5, 0xa5, 0x1a, 0x18,
	0x5a, 0x0f, 0x0f, 0x0f, 0x5a, 0x25, 0xf0, 0x24,
	0x0f, 0x0f, 0x0f, 0xa5, 0xf0, 0x5a, 0xa5, 0xb0,
	0x05, 0x0f, 0x5a, 0xf0, 0xf0, 0x5a, 0xa5, 0xa4,
	0x0f, 0x0f, 0xb0, 0xb0, 0xf0, 0x70, 0xa5, 0x0e,
	0x86, 0xa4, 0x49, 0xf0, 0x49, 0x86, 0x70, 0xa5,
	0x49, 0x49, 0xa4, 0x49, 0xa4, 0xc3, 0xf0, 0x0f,
	0x86, 0x86, 0x86, 0xa4, 0x58, 0xc3, 0xd2, 0xa5,
	0xeb, 0x49, 0x49, 0x49, 0x0c, 0xe1, 0xc3, 0xf0,
	0xd7, 0xc3, 0xc3, 0x86, 0x86, 0xd2, 0xc3, 0xf0,
	0xeb, 0xeb, 0xc3, 0xc3, 0x0c, 0x58, 0x49, 0x58,
	0xd7, 0xd7, 0xc3, 0xc3, 0x86, 0x86, 0xa4, 0x86,
	0xeb, 0xeb, 0xc3, 0xc3, 0xc3, 0x0c, 0x58, 0x49,
	0xd7, 0xd7, 0xc3, 0xc3, 0xc3, 0x86, 0x58, 0xc3,
	0xc3, 0xeb, 0xeb, 0xc3, 0xc3, 0xc3, 0x0c, 0xe1,
	0x49, 0xd7, 0xd7, 0xc3, 0xc3, 0x86, 0x86, 0xa4,
	0x0c, 0xc3, 0xeb, 0xeb, 0xc3, 0xc3, 0x0c, 0x58,
	0x0c, 0x49, 0xc3, 0xd7, 0xc3, 0xc3, 0x86, 0x49,
	0x0c, 0x86, 0xc3, 0xc3, 0x92, 0xc3, 0xc3, 0x0c,
	0x24, 0x49, 0x49, 0xc3, 0xc3, 0x61, 0xc3, 0x86,
	0x18, 0x0c, 0x0c, 0xc3, 0xc3, 0x92, 0xc3, 0x49,
	0x86, 0x86, 0xd7, 0x43, 0xa5, 0x5a, 0x25, 0x5a,
	0x49, 0x49, 0xc3, 0x86, 0x0f, 0xa5, 0xa5, 0xa5,
	0x86, 0xc3, 0xeb, 0xd2, 0x5a, 0x5a, 0xf0, 0xf0,
	0x49, 0x09, 0xc3, 0xa5, 0x0f, 0xb0, 0xa5, 0xf0,
	0x86, 0xd7, 0x06, 0x0f, 0x5a, 0xf0, 0x70, 0xf0,
	0x49, 0xeb, 0xd2, 0x0f, 0xf0, 0xb0, 0xa5, 0xe1,
	0xc3, 0x43, 0x58, 0x0f, 0x5a, 0xf0, 0x0f, 0x35,
	0x83, 0x83, 0x0d, 0x0f, 0xb0, 0xa5, 0x50, 0xf5,
	0xc3, 0x43, 0xa5, 0x5a, 0xf0, 0x0a, 0x50, 0xbf,
	0x83, 0x86, 0x0f, 0x0f, 0xb0, 0x0a, 0x1a, 0x6a,
	0x43, 0xd2, 0x0f, 0x5a, 0x70, 0x05, 0xf5, 0x95,
	0xeb, 0x0d, 0x0f, 0xf0, 0xa5, 0x0f, 0xb5, 0xc0,
	0x43, 0xa5, 0x0f, 0x5a, 0x25, 0x5a, 0xff, 0x6a,
	0x86, 0x0f, 0x05, 0xb0, 0xa5, 0x5f, 0xf5, 0xbf,
	0xd2, 0x0f, 0x5a, 0xf5, 0x25, 0x5a, 0xbf, 0xc0,
	0x0d, 0x0f, 0x05, 0xb0, 0x0f, 0x75, 0xea, 0x95,
	0x5a, 0x5a, 0xeb, 0xff, 0x24, 0x25, 0x1a, 0x0e,
	0xa5, 0x75, 0xf5, 0x2b, 0x92, 0x18, 0xf0, 0x0e,
	0x5a, 0xf0, 0xbf, 0x3f, 0x95, 0x95, 0xfa, 0x24,
	0xf5, 0xba, 0x7f, 0x6a, 0xc0, 0x6a, 0x7f, 0xba,
	0x97, 0x85, 0xbf, 0x95, 0xc0, 0x95, 0xbf, 0x7a,
	0x6a, 0x2f, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x3a,
	0x95, 0xfa, 0xff, 0xff, 0xfa, 0xd2, 0xff, 0xff,
	0x7f, 0xf5, 0x6a, 0x7f, 0xa4, 0xb0, 0xf5, 0x3f,
	0x95, 0xbf, 0xc0, 0x95, 0xd2, 0x0f, 0x5f, 0x6a,
	0xc0, 0x6a, 0x7f, 0x6b, 0x25, 0x0f, 0x5f, 0x6a,
	0x95, 0xbf, 0xbf, 0x92, 0x5a, 0xa5, 0x75, 0x6a,
	0xff, 0xf5, 0xff, 0xf0, 0xf5, 0xb0, 0xf5, 0xea,
	0x95, 0x70, 0xff, 0x61, 0x70, 0xd7, 0x57, 0xbf,
	0xc0, 0xba, 0xf0, 0x1a, 0x1a, 0xf5, 0xff, 0xba,
	0xc0, 0x7f, 0x25, 0x70, 0xff, 0x75, 0xbf, 0xbf,
	0x18, 0xfa, 0xb0, 0xf5, 0xba, 0xf0, 0xf5, 0x6a,
	0x0c, 0x0d, 0x0c, 0x49, 0xc3, 0x61, 0xc3, 0x86,
	0x0c, 0x0c, 0x0e, 0x86, 0x92, 0x92, 0xc3, 0xc3,
	0x0c, 0x0c, 0x0f, 0x0c, 0x49, 0x61, 0x61, 0x61,
	0x0c, 0x0c, 0x0d, 0x0e, 0xc3, 0x92, 0x92, 0x92,
	0x49, 0x0c, 0x0d, 0x0f, 0x49, 0xc3, 0x61, 0x61,
	0x49, 0x0c, 0x0c, 0x0f, 0x0e, 0xc3, 0x92, 0x92,
	0xc3, 0x86, 0x86, 0x0d, 0x4b, 0x49, 0x61, 0x61,
	0xbf, 0x83, 0xc3, 0x0d, 0x0e, 0xc3, 0x92, 0x92,
	0xc0, 0x6b, 0x43, 0x86, 0x0e, 0x49, 0xc3, 0x61,
	0xc0, 0xc1, 0x83, 0x0c, 0xa5, 0x0c, 0x92, 0x92,
	0xc0, 0x6b, 0xc3, 0x86, 0xa5, 0x49, 0xc3, 0x61,
	0x95, 0xab, 0x83, 0xc3, 0x18, 0x0e, 0x86, 0x92,
	0x7f, 0xd2, 0x43, 0x06, 0xb0, 0x0e, 0x49, 0x49,
	0xb0, 0xf5, 0x83, 0xc3, 0x1a, 0xa5, 0x0c, 0x92,
	0xff, 0xd7, 0xff, 0xff, 0x5a, 0xf0, 0x0e, 0x49,
	0xc0, 0x7f, 0x7f, 0xba, 0xf0, 0xb0, 0x0e, 0x92,
	0x0d, 0x0a, 0x5a, 0x75, 0xa5, 0xf5, 0x75, 0xfa,
	0xa5, 0x0f, 0xb0, 0xba, 0x0f, 0xb0, 0xb0, 0xb0,
	0xa5, 0x0a, 0x5a, 0xfa, 0x0f, 0x70, 0xf0, 0x75,
	0xa5, 0x05, 0xb0, 0xba, 0x0f, 0xb0, 0xf0, 0xff,
	0x0f, 0x5a, 0xf0, 0xfa, 0x0f, 0x70, 0xd7, 0xbf,
	0xa5, 0x05, 0xf5, 0xb0, 0x5a, 0xf0, 0xf5, 0x3f,
	0x0f, 0x5a, 0xf5, 0xfa, 0x5a, 0xd2, 0xf5, 0xbf,
	0x0f, 0x50, 0xf5, 0xba, 0x5a, 0x7f, 0xe1, 0xb0,
	0x0a, 0x5a, 0x75, 0x70, 0xf0, 0x95, 0x70, 0xfa,
	0x0f, 0xb0, 0xf5, 0xf0, 0x18, 0xc0, 0xff, 0xeb,
	0x0a, 0x5a, 0xff, 0x70, 0xc3, 0x6a, 0x6b, 0xfa,
	0x0f, 0xb0, 0xba, 0xb0, 0xff, 0xea, 0x90, 0xba,
	0x0a, 0xf0, 0xfa, 0xf0, 0xff, 0x6a, 0xd5, 0xff,
	0x05, 0xb0, 0xba, 0xf0, 0xbf, 0xc0, 0x3a, 0xe1,
	0x0a, 0xf0, 0x75, 0x70, 0x97, 0xc0, 0xfa, 0xf0,
	0x05, 0xf0, 0xb0, 0xa5, 0xe0, 0x95, 0xf5, 0xf0,
	0x70, 0x75, 0xff, 0x6a, 0x7f, 0x25, 0x35, 0x95,
	0xb0, 0xf5, 0xff, 0xba, 0xf0, 0x1a, 0xff, 0x6a,
	0xfa, 0xd2, 0x70, 0xff, 0xff, 0x70, 0x35, 0x95,
	0xbf, 0xff, 0x3f, 0xd5, 0x3f, 0xff, 0xff, 0x6a,
	0x3f, 0xbf, 0x6a, 0x7a, 0xff, 0xff, 0x7a, 0xbf,
	0x7f, 0xbf, 0xff, 0xff, 0x7f, 0xb0, 0xff, 0xff,
	0xfa, 0x5a, 0xbf, 0x3f, 0x95, 0x95, 0xfa, 0x70,
	0x0f, 0xf5, 0x3f, 0x3f, 0x3f, 0x6a, 0xc0, 0x7f,
	0xa5, 0x70, 0xbf, 0xeb, 0x97, 0x95, 0x95, 0xbf,
	0x0f, 0xb0, 0xf5, 0xba, 0xe1, 0xea, 0x7f, 0x3a,
	0xf0, 0xd2, 0xbf, 0x7a, 0xf0, 0xf5, 0xff, 0xaf,
	0xff, 0xa5, 0xb0, 0xb0, 0xa5, 0x5a, 0xba, 0xba,
	0x70, 0x70, 0xd2, 0xd2, 0x0f, 0x0f, 0x25, 0x70,
	0xff, 0xff, 0xf0, 0x0f, 0x0f, 0x0f, 0xb0, 0x0f,
	0x25, 0x0f, 0xa5, 0x0a, 0x5a, 0x0f, 0x05, 0x0f,
	0x0f, 0x0f, 0x0f, 0x05, 0x0f, 0x0f, 0x00, 0x0f,
	0x95, 0xbf, 0xbf, 0xfa, 0xfa, 0xf0, 0x58, 0x49,
	0x95, 0xff, 0xba, 0xb0, 0x90, 0xb0, 0xa5, 0xa4,
	0xc0, 0x7f, 0xc3, 0x70, 0xd5, 0xf0, 0x5a, 0x5a,
	0x6a, 0xc0, 0x7f, 0xf5, 0xc0, 0xb0, 0xf0, 0xa4,
	0x95, 0xc0, 0x95, 0x70, 0xea, 0x70, 0x70, 0x5a,
	0xff, 0xff, 0x7f, 0x92, 0xea, 0xf0, 0xf5, 0xa4,
	0xd2, 0x61, 0xfa, 0xfa, 0xea, 0xfa, 0x70, 0x70,
	0xb0, 0xe1, 0xb0, 0xb0, 0xe0, 0xf5, 0xb0, 0xa5,
	0xff, 0xfa, 0xd2, 0xfa, 0xd7, 0xfa, 0x70, 0xfa,
	0x18, 0xff, 0xb0, 0xf5, 0xb0, 0xba, 0xb0, 0xaf,
	0x5a, 0x5a, 0x70, 0x70, 0xfa, 0xfa, 0x70, 0x70,
	0xa5, 0x0f, 0xf5, 0xba, 0x90, 0xba, 0xb0, 0xaf,
	0xf0, 0xa5, 0x75, 0xff, 0xfa, 0xfa, 0x75, 0x25,
	0x0f, 0xf0, 0xf5, 0xff, 0xb0, 0xb0, 0xb0, 0xa5,
	0x0a, 0x0f, 0x75, 0xd5, 0x70, 0x70, 0x70, 0x70,
	0x00, 0x05, 0xb0, 0xff, 0xb0, 0xba, 0xf5, 0xba,
	0x0a, 0xf0, 0x70, 0xa5, 0x35, 0x95, 0x61, 0x70,
	0x05, 0xf0, 0xf0, 0x0f, 0xb5, 0xd5, 0xba, 0xa5,
	0x0a, 0xf0, 0x70, 0x0f, 0xf5, 0xd5, 0xd7, 0x25,
	0x05, 0xb0, 0xf0, 0x0f, 0xe1, 0x95, 0xff, 0xa0,
	0x0a, 0xf0, 0x70, 0x5a, 0x58, 0x95, 0xff, 0xa5,
	0x05, 0xb0, 0xf0, 0x1a, 0xf5, 0xd5, 0xba, 0x0a,
	0x0f, 0xf0, 0x70, 0x5a, 0xfa, 0x7f, 0xfa, 0x0a,
	0x05, 0xb0, 0xf0, 0x5f, 0xb0, 0xff, 0xaf, 0x00,
	0x0f, 0xf0, 0x70, 0x5a, 0x75, 0x70, 0x87, 0x00,
	0x05, 0xb0, 0xf0, 0x4b, 0xb5, 0xaf, 0xa5, 0x0a,
	0x0f, 0xf0, 0x70, 0x5a, 0x97, 0x7a, 0xaf, 0x05,
	0x05, 0xb0, 0xf0, 0x1a, 0xbf, 0x7f, 0xfa, 0x0f,
	0x0f, 0xf0, 0x70, 0x0f, 0xbf, 0x7f, 0xb0, 0x05,
	0x05, 0xb0, 0xa5, 0x1a, 0xbf, 0x7f, 0xb0, 0x0f,
	0x0f, 0xf0, 0x70, 0x0f, 0xab, 0x7f, 0xd2, 0x05,
	0x05, 0xb0, 0xa5, 0x0f, 0x35, 0x7f, 0xa5, 0x0f,
	0x0f, 0x0f, 0x0f, 0x0a, 0x5a, 0x0f, 0x00, 0x05,
	0x0f, 0x05, 0x00, 0x05, 0xb0, 0xa5, 0x0a, 0x00,
	0x0f, 0x00, 0x00, 0x0f, 0x5a, 0x0f, 0x0f, 0x00,
	0x0f, 0x0f, 0xb0, 0xa5, 0xa5, 0xa5, 0x0a, 0x0a,
	0xa0, 0x5a, 0x0a, 0x05, 0x5a, 0x5a, 0x0f, 0x05,
	0x00, 0x00, 0x00, 0x05, 0xb0, 0xb0, 0xa5, 0x0a,
	0x00, 0x00, 0x05, 0x0f, 0x5a, 0x5a, 0x0f, 0x05,
	0x00, 0x05, 0x0f, 0x0f, 0x05, 0x5a, 0x0f, 0x0a,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x0f, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x0a, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x05, 0x00, 0x00,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00,
	0x0a, 0x00, 0x00, 0x00, 0x0a, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0f, 0xf0, 0xfa, 0x75, 0xd5, 0xff, 0xfa,
	0x0f, 0x05, 0xa5, 0xb0, 0xf5, 0xff, 0xff, 0xba,
	0x05, 0x05, 0xf0, 0x75, 0xff, 0x75, 0x75, 0x70,
	0x00, 0x00, 0xf0, 0x5f, 0xf5, 0xf5, 0xf5, 0xb0,
	0x05, 0x05, 0x25, 0xf5, 0xfa, 0xfa, 0x75, 0x70,
	0x0a, 0x05, 0xa5, 0xb0, 0xd5, 0xd0, 0xf5, 0xb0,
	0x00, 0x0f, 0x5a, 0x70, 0x60, 0xd0, 0xf0, 0xfa,
	0x00, 0x05, 0xb0, 0xf0, 0xe0, 0x90, 0x1a, 0xba,
	0x00, 0x0a, 0x75, 0x70, 0x75, 0xff, 0x5a, 0xfa,
	0x00, 0x05, 0xb0, 0xb0, 0xf5, 0xea, 0xb0, 0xba,
	0x00, 0x0f, 0x70, 0x5a, 0x70, 0x75, 0xff, 0x70,
	0x05, 0x05, 0xba, 0x0f, 0xb0, 0xb0, 0xff, 0xb0,
	0x00, 0x5a, 0x70, 0x25, 0x70, 0x70, 0xfa, 0x70,
	0x05, 0x1a, 0xb0, 0xa5, 0xb0, 0xe0, 0xba, 0xf0,
	0x00, 0x5a, 0x75, 0x25, 0x5a, 0xea, 0xff, 0x70,
	0x05, 0x1a, 0xf5, 0xa5, 0xb0, 0xe0, 0xb0, 0xb0,
	0x0f, 0xf0, 0x70, 0x0f, 0xab, 0x7a, 0x87, 0x0f,
	0x05, 0xb0, 0xa5, 0x0f, 0x35, 0x12, 0xa5, 0x05,
	0x0f, 0xf0, 0x70, 0x0f, 0x21, 0x7a, 0xa5, 0x0f,
	0x05, 0xb0, 0xa5, 0xa5, 0xb5, 0x52, 0x0f, 0x05,
	0x0f, 0xf0, 0x70, 0x5a, 0x21, 0x2f, 0x25, 0x0a,
	0x05, 0xb0, 0xa5, 0xa5, 0xb5, 0x07, 0xa5, 0x05,
	0x0f, 0x5a, 0x70, 0x5a, 0x21, 0x07, 0x0f, 0x00,
	0x05, 0xf0, 0xf0, 0xa5, 0xa1, 0x52, 0x0f, 0x00,
	0x0f, 0x5a, 0x70, 0x0f, 0xa1, 0xba, 0x0f, 0x0a,
	0x05, 0xf0, 0xf0, 0xa5, 0x5f, 0x57, 0xa5, 0x00,
	0x0f, 0x5a, 0x70, 0xa5, 0x0b, 0xba, 0xa5, 0x0a,
	0x05, 0x5a, 0xf0, 0xa5, 0x5f, 0xff, 0x25, 0x05,
	0x0f, 0x5a, 0xf0, 0xa5, 0x1a, 0xba, 0xa5, 0x0a,
	0x05, 0x0f, 0xf0, 0xa5, 0x5f, 0x75, 0x25, 0x0f,
	0x0f, 0x5a, 0xf0, 0xa5, 0x1a, 0xba, 0x70, 0x0a,
	0x05, 0x0f, 0xf0, 0xa5, 0x5a, 0x75, 0x25, 0x0f,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x5a, 0x5a, 0x70, 0x5a, 0x75, 0xfa, 0x70,
	0x05, 0x05, 0x1a, 0xba, 0x5a, 0xf5, 0xf5, 0xb0,
	0x00, 0x0a, 0x5a, 0xaf, 0x5a, 0x70, 0xfa, 0x70,
	0x00, 0x05, 0x10, 0xa5, 0x10, 0xba, 0xb0, 0xf0,
	0x00, 0x00, 0x40, 0x30, 0x10, 0xff, 0x70, 0x70,
	0x00, 0x05, 0x10, 0xa5, 0x10, 0x75, 0xb0, 0xf0,
	0x00, 0x00, 0x05, 0x20, 0x5a, 0x70, 0x70, 0x70,
	0x00, 0x05, 0x10, 0x0a, 0xb0, 0xf0, 0xb0, 0xf0,
	0x00, 0x00, 0x05, 0x20, 0x30, 0x70, 0x70, 0x70,
	0x00, 0x00, 0x00, 0x0a, 0x1a, 0x30, 0xb0, 0xa5,
	0x00, 0x00, 0x00, 0x05, 0x5a, 0x70, 0xf0, 0xf0,
	0x00, 0x00, 0x05, 0x05, 0xb0, 0xb0, 0xf0, 0xa5,
	0x00, 0x00, 0x05, 0x50, 0x70, 0x70, 0x70, 0x5a,
	0x00, 0x00, 0x00, 0x5a, 0xb0, 0xb0, 0xf0, 0xa5,
	0x00, 0x00, 0x05, 0xa5, 0x70, 0x70, 0xf0, 0x0f,
	0x00, 0x00, 0x10, 0x0f, 0xb0, 0xb0, 0xa5, 0xa5,
	0x0a, 0x0f, 0xf0, 0xa5, 0x5a, 0xba, 0x70, 0x0f,
	0x05, 0x0f, 0xf0, 0xa5, 0x0f, 0x30, 0x25, 0x0f,
	0x0a, 0x0f, 0x0f, 0xf0, 0x0f, 0x30, 0x70, 0x0f,
	0x05, 0x05, 0x0f, 0xf0, 0x5a, 0xb0, 0xa5, 0x0f,
	0x0a, 0x0a, 0x0f, 0xf0, 0xf0, 0x70, 0x70, 0x0f,
	0x05, 0x05, 0x0f, 0x5a, 0xf0, 0xf0, 0xb0, 0x0f,
	0x0a, 0x0a, 0x0f, 0x5a, 0xf0, 0x70, 0x70, 0x1a,
	0x05, 0x05, 0x05, 0x5a, 0xf0, 0xb0, 0xb0, 0xa5,
	0x0a, 0x0a, 0x0f, 0x0f, 0xf0, 0x70, 0x70, 0xf0,
	0x05, 0x05, 0x05, 0x0f, 0xb0, 0xf0, 0xf0, 0xa5,
	0x0a, 0x0a, 0x0a, 0x0f, 0xf0, 0x70, 0x70, 0xa5,
	0x05, 0x05, 0x05, 0x0f, 0x5a, 0xb0, 0xb0, 0xa5,
	0x0a, 0x0a, 0x0a, 0x0f, 0x5a, 0xf0, 0x70, 0x70,
	0x05, 0x05, 0x05, 0x05, 0x5a, 0xb0, 0xa5, 0xb0,
	0x0a, 0x0a, 0x0a, 0x0f, 0x5a, 0xf0, 0xa5, 0xf0,
	0x05, 0x05, 0x05, 0x05, 0x0f, 0x0f, 0x0f, 0x5a,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x00,
	0x0f, 0x0f, 0x0f, 0x5a, 0xa5, 0x05, 0x05, 0x1a,
	0xa5, 0x0f, 0xf0, 0x30, 0x70, 0x5a, 0x0f, 0xf0,
	0xa5, 0x0f, 0xf0, 0xb0, 0x30, 0x30, 0x30, 0xb0,
	0x0f, 0x0f, 0xa5, 0x75, 0x75, 0x75, 0x70, 0x70,
	0x0f, 0x0f, 0x5a, 0xb0, 0xba, 0xba, 0xb0, 0xb0,
	0x0f, 0x0f, 0x5a, 0x70, 0x30, 0x70, 0x70, 0x70,
	0x25, 0xa5, 0x5a, 0xb0, 0xb0, 0xb0, 0xb0, 0xf0,
	0xf0, 0x0f, 0x0f, 0x70, 0x70, 0x70, 0x70, 0x70,
	0xb0, 0x0f, 0x0f, 0xb0, 0xb0, 0xb0, 0xb0, 0xa5,
	0xf0, 0x0f, 0x5a, 0x5a, 0x70, 0x70, 0x70, 0xa5,
	0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0x0f, 0x0f,
	0x00, 0x00, 0x5a, 0x5a, 0x70, 0x70, 0xf0, 0x0f,
	0x00, 0x05, 0x0f, 0xb0, 0xb0, 0xf0, 0xa5, 0x0f,
	0x00, 0x0f, 0x5a, 0x70, 0x70, 0x70, 0x5a, 0x0a,
	0x05, 0xb0, 0xb0, 0xb0, 0xb0, 0xf0, 0xa5, 0x0f,
	0x1a, 0x70, 0x70, 0x70, 0x70, 0x70, 0x0f, 0x0a,
	0xb0, 0xb0, 0xb0, 0xf0, 0xb0, 0xa5, 0xa5, 0x05,
	0x30, 0x70, 0x70, 0x70, 0x70, 0x70, 0x0f, 0x0a,
	0xb0, 0xb0, 0xf0, 0xf0, 0xb0, 0xa5, 0x0f, 0x05,
	0xf0, 0x70, 0xf0, 0xf0, 0xf0, 0xf0, 0x0a, 0x0a,
	0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xa5, 0x0f, 0x05,
	0xf0, 0xf0, 0xf0, 0x5a, 0x5a, 0x0f, 0x0a, 0x0a,
	0x5a, 0xa5, 0xa5, 0xf0, 0xa5, 0x0f, 0x05, 0x05,
	0xf0, 0x5a, 0x5a, 0x5a, 0x0f, 0x0a, 0x0a, 0x0a,
	0xa5, 0x0f, 0xa5, 0x0f, 0x0f, 0x05, 0x05, 0x05,
	0x0f, 0x0f, 0x0f, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a,
	0x0f, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x05
};