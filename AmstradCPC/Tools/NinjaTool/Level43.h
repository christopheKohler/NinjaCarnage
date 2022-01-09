// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL43.BIN"

// -------------------------------------------------------
#define NBZONES 5

u8 leveldata[2] = { 3 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	0,0,3,3,1, // 
	6,0,8,2,2, // 
	0,5,3,7,3, // 
	6,5,9,8,4, // 
	2,11,5,13,5, // 
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
	{ 250, 1  , 0  , 0  , 0  , 0 , 0  , 0 , 0 , 0 }, // Text
	{ 199, 105, 101, 101, 101, 2  , 0  , 0 , 0 , 0 }, // Dodge from bottom
	{ 252, 1  , 0  , 0 ,  0  , 0  , 0  , 0 , 0 , 0 }, // Cursor shake
	{ 125, 105, 3  , 101, 101,0  , 0  , 0 , 0 , 0 }, // Dodge from up
	{ 251, 40 , 4,  102 , 1  ,2  , 1  , 2  ,3 , 4}, // QTE
	{ 1  ,2   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 115, 105, 0  , 5  , 101,0  , 0  , 0 , 0 , 0 }, // Hit Hand
	{ 251, 40 , 6,  103 , 3  ,2  , 3  , 1  ,4 , 1}, // QTE
	{ 3  ,4   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 95 , 106, 0  , 0  , 7  ,0  , 0  , 0 , 0 , 0 }, // Hit Head
	{ 251, 40 , 8,  104 , 4  ,3  , 2  , 4  ,1 , 4}, // QTE
	{ 2  ,3   , 4  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 85 , 9  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // USe Chaine
	{ 250, 10 , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 252, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Cursor Normal
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL51 BIN"; // Must be 11 characters

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
  "Je me dirige vers la porte du   "
 "fond, et voyez-vous sur qui je    "
"tombe ? Ce cul de babouin, �a fait  "
 "longtemps que j'attendais de le   "
  "revoir !                        "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+6+1)

#define NBMESSDEATH 6
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BAM\1. Un coup rapide de nunchaku "
 "sur le cr�ne et il me le fend en  "
"deux. Je m'�croule � ses genoux.    "
 "C'est pas aujourd'hui que je vais "
  "venger qui que ce soit...       "
},

{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AIE\1, j'ai pas �t� assez rapide, "
 "j'ai pas �vit� le coup et il me   "
"fracasse le cr�ne. Ca pisse le sang "
 "de partout. Va falloir faire plus "
  "attention grosse br�le !        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1. J'ai pas �t� assez rapide. "
 "Ma frappe rate sa main, et je     "
"n'arrive pas � le d�sarmer. Il me   "
 "retourne un coup de nunchaku en   "
  "revers et me p�te les dents !   "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2MERDE\1. J'ai pas r�ussi � viser  "
 "sa t�te. Il me met un coup de     "
"pied dans le ventre, et quand je    "
 "suis � genoux, il me brise la     "
  "nuque.                          "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je lance ma cha�ne pour tenter  "
 "d'enrouler son nunchaku. Ca semble"
"marcher mais il me d�coche un coup  "
 "de pied me faisant \2VOMIR\1 mes      "
  "tripes. Il me brise la nuque !  "
},
{
/*
"------------------------------------" 36 chars (Line 3) 106
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je tente d'enrouler ma cha�ne   "
 "autour de son cou mais il me      "
"d�coche un coup de pied puissant    "
 "qui m'explose la rate. J'ai mal   "
  "mais il me \2BRISE\1 vite la nuque !"
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
  "Il en a mis pas mal dans le fion"
 "de mon pote le sumo vert ! Il ne  "
"s'en n'est toujours pas remis. Il   "
 "boit pour oublier. C'est le moment"
  "de le \2VENGER\1 !                  "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. J'ai juste le temps        "
 "d��viter le premier coup qui vient"
"d'en bas. Par contre il me frappe la"
 "t�te, je ne peux plus focaliser   "
  "sur lui, va falloir m'aider la !"
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Un coup rapide qui vient "
 "d'en haut maintenant. Fais la     "
"\2SEQUENCE\1 pour l'�viter...           "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2TRES BIEN\1. J'�vite le coup de   "
 "justesse. Il me fr�le la t�te.    "
"R�fl�chis bien, car avec lui j'ai   "
 "pas le droit � l'erreur !         "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2C'EST BIEN\1. Je lui tape dans la "
 "main. R�ussis la s�quence pour    "
"savoir si j'ai �t� assez pr�cis pour"
 "le d�sarmer....                   "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Je frappe pile sur sa main"
 "et �a lui fait l�cher son arme.   "
"Maintenant je vais pouvoir          "
 "l'approcher cette grosse merde... "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 7
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2DANS LA TETE\1. Une derni�re      "
 "frappe... R�ussis la s�quence pour"
"que la frappe touche...             "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ma frappe arrive dans sa face.  "
 "C'est � son tour d'�tre sonn�. Je "
"vais pouvoir l'\2ACHEVER\1.             "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 9
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Il est bal�ze le con. J'enroule "
 "mon \2MANRIKI GUSARI\1 autour de son  "
"coup et je l'�trangle. Son corps    "
 "tombe par terre. \2C'EST POUR TOI\1   "
  "\2MON POTE LE SUMO VERT\1...        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 10
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ca y est ma vue redevient nette."
 "Il m'en aura fait baver ce con.   "
"Y'a une porte au fond, c'est la     "
 "seule voie d'acc�s pour continuer."
  "J'aime pas trop ce calme...     "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0xc0, 0xc0, 0xdc, 0xcf, 0xe5, 0xda, 0xc8, 0xc0,
	0xc0, 0xc0, 0xf8, 0xcf, 0xe5, 0xe5, 0xec, 0xc0,
	0xc0, 0xc4, 0xcf, 0xf4, 0xc8, 0xdc, 0xda, 0xc0,
	0xc0, 0xd4, 0xcf, 0xec, 0xc0, 0xc4, 0xda, 0xc8,
	0xc0, 0xc5, 0xf4, 0xc8, 0xc0, 0xc0, 0xe5, 0xe8,
	0xc0, 0xcd, 0xe4, 0xc0, 0xc0, 0xc0, 0xed, 0xec,
	0xc0, 0xed, 0xca, 0xc0, 0xc0, 0xc0, 0xcd, 0xca,
	0xc0, 0xf0, 0xe8, 0xc0, 0xc0, 0xc0, 0xd8, 0xe4,
	0xc0, 0xcf, 0xc8, 0xc0, 0xc0, 0xc0, 0xcd, 0xce,
	0xc0, 0xcf, 0xc8, 0xc0, 0xc0, 0xc0, 0xc5, 0xce,
	0xc0, 0xf0, 0xc0, 0xc0, 0xc0, 0xc0, 0xd0, 0xce,
	0xc0, 0xcf, 0xc8, 0xc0, 0xc0, 0xc0, 0xc5, 0xe4,
	0xc0, 0xcf, 0xc8, 0xc0, 0xc0, 0xc0, 0xc5, 0xce,
	0xc0, 0xda, 0xe8, 0xc0, 0xc0, 0xc0, 0xd8, 0xca,
	0xc0, 0xe5, 0xca, 0xc0, 0xc0, 0xc0, 0xcd, 0xe4,
	0xc0, 0xed, 0xce, 0xc0, 0xc0, 0xc0, 0xed, 0xe8,
	0x00, 0x00, 0x50, 0x00, 0x00, 0xb0, 0xc3, 0x10,
	0x00, 0x00, 0x50, 0x00, 0x50, 0x61, 0x97, 0x82,
	0x00, 0x00, 0x50, 0xa0, 0xb0, 0xc3, 0x3f, 0x6b,
	0x00, 0x00, 0x50, 0xa0, 0xb0, 0xc3, 0xc3, 0x3f,
	0x00, 0x00, 0xa0, 0x70, 0xb0, 0x70, 0xb0, 0x97,
	0x00, 0x00, 0xa0, 0xb0, 0x10, 0xe1, 0x70, 0x61,
	0x00, 0x00, 0xa0, 0x41, 0x50, 0xb5, 0x82, 0xb0,
	0x00, 0x00, 0xa0, 0x10, 0xa0, 0x97, 0x7a, 0x50,
	0x00, 0x00, 0xa0, 0x50, 0x82, 0x97, 0x3a, 0x50,
	0x00, 0x00, 0xa0, 0x00, 0xd2, 0xbf, 0x6b, 0x10,
	0x00, 0x00, 0xa0, 0x00, 0xd2, 0x6a, 0x6b, 0x41,
	0x00, 0x00, 0xa0, 0x00, 0xe1, 0x3f, 0x3a, 0xb0,
	0x00, 0x00, 0x82, 0x00, 0x41, 0x6a, 0x7a, 0x70,
	0x00, 0x00, 0x08, 0x00, 0x41, 0x95, 0x3a, 0xd2,
	0x00, 0x00, 0x82, 0x00, 0x41, 0x6a, 0x6b, 0x30,
	0x00, 0x00, 0x08, 0x00, 0x1f, 0x95, 0x7a, 0x5a,
	0xc3, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x61, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x41, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6b, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6b, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x97, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x97, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf0, 0xf0, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0xcd, 0xe4, 0xc0, 0xc0, 0xc4, 0xe5, 0xc8,
	0xc0, 0xd0, 0xde, 0xc0, 0xc0, 0xd4, 0xda, 0xc0,
	0xc0, 0xd4, 0xe5, 0xc0, 0xc0, 0xf8, 0xde, 0xc0,
	0xc0, 0xc4, 0xda, 0xc8, 0xd4, 0xcf, 0xe4, 0xc0,
	0xc0, 0xc0, 0xf0, 0xe8, 0xd8, 0xcf, 0xe8, 0xc0,
	0xc0, 0xc0, 0xf8, 0xe4, 0xed, 0xf4, 0xc8, 0xc0,
	0xc0, 0xc0, 0xd8, 0xf4, 0xe5, 0xec, 0xc0, 0xc0,
	0xc0, 0xc0, 0xd4, 0xe5, 0xda, 0xc8, 0xc0, 0xc8,
	0xc0, 0xc0, 0xc4, 0xed, 0xda, 0xc8, 0xc0, 0xca,
	0xc0, 0xc0, 0xc0, 0xda, 0xda, 0xce, 0xc4, 0xca,
	0xc0, 0xc4, 0xed, 0xcf, 0xf0, 0xf4, 0xcd, 0xe8,
	0xc4, 0xed, 0xda, 0xde, 0xf8, 0xda, 0xcf, 0xe8,
	0xc5, 0xda, 0xcf, 0xec, 0xdc, 0xe5, 0xde, 0xc8,
	0xc5, 0xcf, 0xec, 0xc0, 0xc0, 0xdc, 0xec, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x00, 0x00, 0x82, 0x00, 0xb5, 0xc0, 0x87, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x35, 0x95, 0x25, 0x00,
	0x00, 0x00, 0x82, 0x00, 0x97, 0x95, 0xa0, 0x00,
	0x00, 0x00, 0x82, 0x05, 0x6a, 0x6b, 0x0a, 0x00,
	0xa0, 0x00, 0x82, 0x50, 0x6a, 0x3a, 0x00, 0x00,
	0xa0, 0x00, 0x08, 0x10, 0x6a, 0x7a, 0x00, 0x00,
	0xa0, 0x00, 0x82, 0x41, 0x6a, 0x2f, 0x00, 0x00,
	0x00, 0x00, 0xa0, 0x1f, 0x97, 0x82, 0x00, 0x00,
	0x00, 0x00, 0xa0, 0xe1, 0x3f, 0x20, 0x00, 0x00,
	0x00, 0x00, 0xa0, 0x35, 0x97, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x50, 0xc3, 0x6b, 0x0a, 0x00, 0x00,
	0xed, 0x00, 0xb0, 0xc3, 0x92, 0x00, 0x00, 0x00,
	0x76, 0x00, 0x41, 0x92, 0xa5, 0x00, 0x00, 0x00,
	0x33, 0x00, 0xe1, 0xd2, 0x0a, 0x00, 0x00, 0x00,
	0x33, 0x88, 0xe1, 0x25, 0x00, 0x00, 0x00, 0x00,
	0x33, 0x32, 0xd2, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x5a, 0xf0, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0a, 0x5a, 0xa5, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x05, 0x05, 0xf0, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0a, 0x5a, 0xa0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x05, 0xa5, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0a, 0x5a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x05, 0xf0, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x5a, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x05, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xf4, 0x28, 0x7c,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0xe4, 0x88, 0xcc,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xf4, 0x88, 0x66,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0xe4, 0x88, 0xcc,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xf4, 0x88, 0x66,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x22, 0x99,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x50,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x50,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x45, 0x50,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe1,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xc3,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0xba,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xc3, 0xa5,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x92, 0x0a,
	0x00, 0x00, 0x00, 0x00, 0x41, 0x3f, 0x25, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x35, 0x6a, 0x20, 0x00,
	0x00, 0x00, 0x00, 0x50, 0x3f, 0x95, 0xa0, 0x00,
	0x0a, 0x00, 0x00, 0x4b, 0x3f, 0x95, 0x0a, 0x00,
	0xa0, 0x00, 0x00, 0xb5, 0x6a, 0x6b, 0x00, 0x00,
	0x25, 0x00, 0x05, 0x97, 0xc0, 0x3a, 0x00, 0x00,
	0x33, 0x9d, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x66, 0x6b, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3f, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x97, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x82, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x99, 0xf8, 0x14, 0xa2, 0x00, 0x05, 0x00, 0x05,
	0x66, 0x32, 0x44, 0x71, 0x00, 0x0a, 0x0a, 0x00,
	0x99, 0x66, 0x44, 0x98, 0x50, 0xf0, 0xa5, 0x05,
	0x66, 0x33, 0x44, 0xcc, 0x4b, 0xc3, 0x58, 0x00,
	0x99, 0x33, 0x44, 0xcc, 0xe1, 0x7f, 0xd2, 0x05,
	0x44, 0x33, 0x44, 0xcc, 0xe1, 0xc3, 0xa5, 0x00,
	0x54, 0x66, 0x44, 0xcc, 0x50, 0x58, 0x0a, 0x05,
	0x00, 0x38, 0x10, 0xec, 0x00, 0xa4, 0xf0, 0xe1,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0xeb, 0xd7,
	0x00, 0x00, 0x00, 0x00, 0x50, 0xd7, 0x7f, 0x3f,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x5a, 0x88, 0x66,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x22, 0x33,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x5a, 0x88, 0x66,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0x00, 0x00,
	0x0a, 0x0a, 0x00, 0x00, 0x00, 0x5a, 0x88, 0x33,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x50, 0xa8, 0x33,
	0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x5a, 0x88, 0x33,
	0x05, 0x05, 0x05, 0x00, 0x00, 0x50, 0xa8, 0x99,
	0x0f, 0x0f, 0x0a, 0x00, 0x00, 0x5a, 0xa0, 0x66,
	0x05, 0xa5, 0x05, 0x00, 0x00, 0x50, 0xaa, 0x99,
	0x5a, 0x58, 0x0a, 0x00, 0x00, 0x50, 0x6b, 0x6e,
	0xa4, 0x58, 0x00, 0x00, 0x00, 0x50, 0xd7, 0x3f,
	0x1d, 0x58, 0x0a, 0x0a, 0x00, 0x50, 0x97, 0x3f,
	0x7f, 0x7a, 0x05, 0x05, 0x00, 0x50, 0xd7, 0x3f,
	0xbf, 0xae, 0x5a, 0x58, 0x00, 0x50, 0x97, 0x3f,
	0xfa, 0x0a, 0x50, 0x7f, 0x6a, 0x7a, 0x00, 0x00,
	0xba, 0xa0, 0x4b, 0xff, 0x95, 0x87, 0x00, 0x00,
	0x7f, 0x25, 0x35, 0xff, 0x95, 0x20, 0x00, 0x00,
	0xbf, 0xfa, 0x97, 0xff, 0x3f, 0xa0, 0x00, 0x00,
	0x75, 0x30, 0x3f, 0xff, 0x6b, 0x0a, 0x00, 0x00,
	0xba, 0x4b, 0x6a, 0xff, 0xba, 0x00, 0x00, 0x00,
	0x25, 0x97, 0x6a, 0xff, 0xd2, 0x00, 0x00, 0x00,
	0x4b, 0x3f, 0x95, 0xeb, 0xaf, 0x00, 0x00, 0x00,
	0x97, 0x3f, 0xd5, 0x86, 0x82, 0x00, 0x00, 0x00,
	0x3a, 0xc2, 0x3f, 0xc3, 0xaa, 0x00, 0x00, 0x00,
	0x6b, 0x60, 0x7f, 0xa4, 0x82, 0x00, 0x00, 0x00,
	0x3f, 0x95, 0xff, 0x49, 0x08, 0x00, 0x00, 0x00,
	0x97, 0x95, 0x6b, 0x0c, 0x82, 0x00, 0x00, 0x00,
	0x41, 0x3f, 0xeb, 0x0c, 0x08, 0x00, 0x00, 0x00,
	0x3a, 0x7f, 0x86, 0x58, 0x08, 0x00, 0x05, 0x00,
	0x3f, 0xbf, 0x0c, 0xa4, 0x82, 0x00, 0x00, 0x0a,
	0x11, 0x33, 0x11, 0x33, 0x04, 0xbf, 0xbf, 0x6a,
	0x11, 0x66, 0x11, 0x66, 0x04, 0xff, 0x3f, 0x95,
	0x11, 0x33, 0x11, 0x33, 0x04, 0xbf, 0x6a, 0x7f,
	0x11, 0x66, 0x11, 0x66, 0x50, 0x3f, 0xbf, 0xbf,
	0x11, 0x33, 0x11, 0x98, 0x50, 0xbf, 0x6b, 0x7f,
	0x11, 0x66, 0x11, 0x66, 0x50, 0x3f, 0xeb, 0xbf,
	0x11, 0x66, 0x11, 0x98, 0x05, 0x7f, 0x86, 0x3f,
	0x11, 0xcc, 0x44, 0x34, 0x05, 0xfa, 0x50, 0x95,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0xea, 0x7f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0x3f, 0xeb,
	0x00, 0x00, 0x00, 0x00, 0x05, 0xc9, 0x7f, 0xc3,
	0x00, 0x00, 0x00, 0xa0, 0x50, 0xc1, 0x49, 0x49,
	0x00, 0x00, 0x00, 0x50, 0x10, 0xc1, 0x50, 0x0c,
	0x00, 0x00, 0x00, 0xb4, 0x44, 0xd5, 0x49, 0xa4,
	0x00, 0x00, 0x00, 0x3c, 0x10, 0x37, 0x86, 0x0c,
	0x00, 0x00, 0x00, 0x78, 0x44, 0x66, 0xb0, 0x0c,
	0x7f, 0x86, 0xe1, 0x0d, 0x00, 0x50, 0x97, 0x3f,
	0xbf, 0xeb, 0x49, 0xa0, 0x00, 0x50, 0x49, 0x3f,
	0x7f, 0xd7, 0x86, 0xa5, 0x00, 0x50, 0x86, 0x97,
	0xeb, 0x49, 0xeb, 0xa0, 0x00, 0x50, 0x49, 0x6b,
	0x7f, 0xd7, 0x7f, 0x5a, 0xa0, 0x50, 0x86, 0x97,
	0xeb, 0x49, 0xff, 0xa4, 0x00, 0x50, 0x49, 0xeb,
	0x86, 0x0c, 0xd7, 0xa4, 0x00, 0xfa, 0xc3, 0x7f,
	0xeb, 0x49, 0x49, 0x58, 0x00, 0xeb, 0x49, 0xbf,
	0x86, 0x0c, 0xd7, 0x08, 0xa4, 0x7f, 0x0c, 0xd7,
	0x0c, 0x0c, 0x49, 0x49, 0x86, 0xbf, 0xd2, 0x97,
	0x86, 0x0c, 0x86, 0x49, 0x0c, 0x3f, 0x2e, 0xc3,
	0x49, 0x49, 0x0c, 0x86, 0xa4, 0x97, 0x6b, 0x1d,
	0x86, 0x0c, 0x58, 0x49, 0x49, 0x3f, 0x2e, 0x49,
	0x0c, 0x0c, 0xa4, 0x86, 0x86, 0xbf, 0xae, 0x0c,
	0x0c, 0x0c, 0xf0, 0x49, 0x49, 0x3f, 0x7f, 0x49,
	0x0c, 0x58, 0x5a, 0x97, 0x97, 0x3f, 0x3f, 0xbf,
	0x7f, 0x7f, 0x58, 0xe1, 0x08, 0x00, 0x05, 0x05,
	0xbf, 0xeb, 0xa0, 0xa4, 0xa0, 0x00, 0x00, 0x0a,
	0x7f, 0x86, 0xa0, 0x49, 0x0a, 0x00, 0x05, 0x05,
	0xbf, 0x0c, 0x0c, 0xc3, 0x00, 0x00, 0x00, 0x0a,
	0x7f, 0x86, 0x49, 0xd7, 0x0a, 0x00, 0x05, 0x05,
	0xc3, 0x0c, 0x49, 0xeb, 0xa0, 0x00, 0x00, 0x0a,
	0xfa, 0x86, 0xc3, 0xd7, 0x08, 0x00, 0x05, 0x05,
	0xb0, 0x49, 0x49, 0xff, 0xaa, 0x00, 0x00, 0x0a,
	0x44, 0x86, 0xd7, 0xd7, 0x2a, 0x00, 0x05, 0x05,
	0x11, 0x49, 0xc3, 0xbf, 0xaa, 0x00, 0x00, 0x0f,
	0x11, 0x86, 0xd7, 0xff, 0x2f, 0x00, 0x00, 0x05,
	0x44, 0x49, 0xc3, 0xbf, 0x2e, 0x00, 0x00, 0x0f,
	0x50, 0x24, 0xd7, 0xff, 0x7f, 0x0a, 0x00, 0x05,
	0x00, 0xb0, 0x49, 0xbf, 0xbf, 0x08, 0x00, 0x0f,
	0x00, 0x00, 0xd7, 0xff, 0x3f, 0xaf, 0x00, 0x05,
	0x44, 0xf8, 0x49, 0xbf, 0xbf, 0x2e, 0x00, 0x0a,
	0x00, 0x00, 0x00, 0xb4, 0x10, 0x33, 0x24, 0x86,
	0x00, 0x00, 0x00, 0xf0, 0x45, 0x66, 0x97, 0xc3,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0xe1, 0xc3,
	0x0a, 0x00, 0x00, 0x00, 0x50, 0xf8, 0x41, 0x0c,
	0x05, 0x00, 0x00, 0x50, 0x54, 0xdc, 0xa0, 0xf0,
	0x0a, 0x0a, 0x00, 0xb4, 0x44, 0xc4, 0xa8, 0x44,
	0x05, 0x00, 0x00, 0x38, 0x50, 0x99, 0x88, 0x40,
	0x0a, 0x0a, 0x00, 0x50, 0x00, 0x45, 0xa0, 0x40,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x40,
	0x0a, 0x0a, 0x00, 0x00, 0x00, 0x50, 0x00, 0x1b,
	0x0f, 0xa5, 0x00, 0x00, 0x00, 0x0a, 0x8a, 0x44,
	0x5a, 0x5a, 0x00, 0x00, 0x00, 0x50, 0x00, 0x1f,
	0xa5, 0xa5, 0x0a, 0x00, 0x00, 0x0a, 0x8a, 0x1f,
	0x5a, 0x5a, 0x0a, 0x00, 0x00, 0x50, 0x05, 0xf5,
	0xa5, 0xa5, 0xa5, 0x00, 0x00, 0x0a, 0x8a, 0xeb,
	0x5a, 0x5a, 0x5a, 0x00, 0x00, 0x50, 0x8d, 0xd7,
	0x0c, 0x0d, 0x50, 0x7f, 0x97, 0x3f, 0x3f, 0x3f,
	0x0c, 0xa0, 0x4b, 0x3f, 0x97, 0x3f, 0x3f, 0x3f,
	0x58, 0x0a, 0xf5, 0x6b, 0x7f, 0xd7, 0x3f, 0x3f,
	0xa5, 0x00, 0x35, 0x3f, 0x92, 0xb0, 0xbf, 0x3f,
	0xb0, 0xa0, 0x97, 0x6b, 0xa5, 0x5a, 0x97, 0x3f,
	0x9d, 0x25, 0xbf, 0xd2, 0x0a, 0x4b, 0xbf, 0x3f,
	0x66, 0xd2, 0x6b, 0xa0, 0x00, 0x61, 0x7f, 0x3f,
	0xc4, 0x3f, 0x87, 0x00, 0x05, 0x97, 0x97, 0x3f,
	0x66, 0x6b, 0x0a, 0x00, 0x50, 0x6b, 0x7f, 0x3f,
	0x9d, 0xd2, 0x00, 0x00, 0x4b, 0xbf, 0x97, 0x3f,
	0x3f, 0xa0, 0x00, 0x05, 0x75, 0x6b, 0x6b, 0x3f,
	0x7f, 0x0a, 0x00, 0x10, 0x97, 0xbf, 0x97, 0x3f,
	0xba, 0x00, 0x00, 0x4b, 0x6b, 0x3f, 0x6b, 0x3f,
	0xd2, 0x00, 0x00, 0xb5, 0x97, 0x97, 0x3f, 0xbf,
	0xaf, 0x00, 0x05, 0x61, 0x6b, 0x3f, 0x7f, 0x3f,
	0x20, 0x00, 0x50, 0x97, 0x3f, 0x97, 0x3f, 0xbf,
	0x11, 0xdc, 0x24, 0xd7, 0x7f, 0x7f, 0x0a, 0x05,
	0x11, 0x66, 0xe1, 0xeb, 0xbf, 0x3f, 0x08, 0x00,
	0x11, 0x99, 0x04, 0xd7, 0xd7, 0x3f, 0xaf, 0x05,
	0x11, 0x66, 0x50, 0x49, 0xbf, 0xbf, 0x2e, 0x00,
	0x11, 0x99, 0x05, 0xd7, 0x7f, 0x6a, 0x7f, 0x00,
	0x44, 0x66, 0x00, 0x49, 0xbf, 0x3f, 0x3f, 0x0a,
	0x50, 0xf8, 0x00, 0xd7, 0x3f, 0x6a, 0x3f, 0x08,
	0x00, 0x00, 0x00, 0x49, 0xbf, 0x95, 0x95, 0x87,
	0x00, 0x00, 0x00, 0xa4, 0x97, 0x6a, 0x6a, 0x2e,
	0x00, 0x00, 0x00, 0x4b, 0x97, 0x3f, 0xbf, 0x6b,
	0x00, 0x00, 0x00, 0x04, 0xd7, 0x3f, 0x7f, 0x3f,
	0x00, 0x00, 0x00, 0x50, 0x49, 0xbf, 0x3f, 0xbf,
	0x00, 0x00, 0xa0, 0x05, 0xd7, 0x7f, 0x7f, 0x3f,
	0x00, 0x00, 0x50, 0x00, 0x49, 0x97, 0xbf, 0xbf,
	0x00, 0x00, 0xa0, 0x00, 0xe1, 0xeb, 0x7f, 0x3f,
	0x00, 0x00, 0x70, 0x00, 0xa4, 0xd7, 0x97, 0xbf,
	0xa5, 0xa5, 0xa5, 0x0a, 0x00, 0x5a, 0x7a, 0xeb,
	0x5a, 0x5a, 0x5a, 0x0a, 0x00, 0x50, 0x3a, 0xc3,
	0x0f, 0xf0, 0xf0, 0xa0, 0x00, 0x5a, 0x98, 0xeb,
	0x5a, 0x5a, 0x70, 0x0a, 0x00, 0x50, 0xcb, 0x92,
	0x05, 0xf0, 0xf0, 0xa5, 0x00, 0x0a, 0xb0, 0x92,
	0x5a, 0xf0, 0x70, 0xa5, 0x05, 0x50, 0x41, 0xfa,
	0x05, 0xb0, 0xb0, 0xa5, 0x00, 0x0a, 0xb0, 0x87,
	0x05, 0xf0, 0x70, 0x70, 0x05, 0x50, 0x61, 0x20,
	0x00, 0xb0, 0xb0, 0xa5, 0x00, 0x1a, 0xd7, 0xa0,
	0x0a, 0xf0, 0x70, 0x70, 0x05, 0x61, 0xeb, 0x0a,
	0x08, 0x1a, 0xb0, 0xb0, 0x1a, 0x97, 0x86, 0x00,
	0xaf, 0x5a, 0x70, 0x70, 0x61, 0x6b, 0x58, 0x00,
	0x2e, 0x1a, 0xb0, 0xb0, 0x97, 0x86, 0x0d, 0x00,
	0x7f, 0x58, 0x70, 0x61, 0xeb, 0x0c, 0xa0, 0x00,
	0x3f, 0xae, 0xb0, 0x92, 0x86, 0x86, 0x0a, 0x00,
	0xbf, 0x6b, 0x70, 0x61, 0x61, 0x0c, 0x00, 0x00,
	0x20, 0x00, 0x10, 0xeb, 0x3f, 0x7f, 0x7f, 0x3f,
	0xa0, 0x00, 0x41, 0x97, 0x3f, 0x97, 0x3f, 0xbf,
	0x0a, 0x00, 0x4b, 0x6b, 0x3f, 0x6b, 0x3f, 0x3f,
	0x00, 0x00, 0xf5, 0x97, 0x3f, 0x97, 0x3f, 0xbf,
	0x00, 0x00, 0x61, 0x6b, 0x3f, 0x6b, 0x3f, 0x3f,
	0x00, 0x00, 0x5d, 0x97, 0x3f, 0x3f, 0x3f, 0x3f,
	0x00, 0x05, 0xc3, 0x6b, 0x3f, 0x7f, 0x3f, 0x3f,
	0x00, 0x50, 0x6b, 0x97, 0x3f, 0xbf, 0xbf, 0x3f,
	0x00, 0x10, 0xc3, 0xeb, 0x7f, 0x6b, 0x6b, 0x3f,
	0x00, 0x41, 0x6b, 0x97, 0x97, 0xbf, 0x97, 0x97,
	0x00, 0x1f, 0xc3, 0xc3, 0x7f, 0xd7, 0x6b, 0x6b,
	0x00, 0xe1, 0x6b, 0x49, 0x97, 0xeb, 0xbf, 0xc3,
	0x00, 0x35, 0xc3, 0x86, 0xd7, 0xd7, 0x6b, 0xc3,
	0x00, 0x49, 0x6b, 0x0c, 0x97, 0xeb, 0xbf, 0xc3,
	0x05, 0x97, 0xc3, 0x86, 0xd7, 0xd7, 0x6b, 0x6b,
	0x50, 0x6b, 0x6b, 0x49, 0xc3, 0xeb, 0xbf, 0x3f,
	0x00, 0x00, 0xd8, 0x00, 0x0e, 0x49, 0xeb, 0x7f,
	0x00, 0x00, 0xf8, 0x00, 0x5a, 0x0c, 0xd7, 0x97,
	0x00, 0x00, 0xd8, 0x00, 0x50, 0x49, 0x49, 0xeb,
	0x00, 0x45, 0xf8, 0x00, 0x05, 0x0c, 0xc3, 0x86,
	0x00, 0x8a, 0xd8, 0x00, 0x05, 0x0c, 0x49, 0xc3,
	0x45, 0x45, 0x78, 0x00, 0x00, 0xa4, 0x86, 0xc3,
	0x8a, 0x8a, 0xd8, 0x00, 0x00, 0x0e, 0x49, 0x49,
	0x45, 0x45, 0x78, 0x00, 0x00, 0x0e, 0x0c, 0x86,
	0x8a, 0x8a, 0xd8, 0x00, 0x00, 0x50, 0x0c, 0x49,
	0x45, 0x00, 0x70, 0x00, 0x00, 0x5a, 0x0c, 0x0c,
	0x8a, 0x8a, 0xf0, 0x00, 0x00, 0x50, 0x0c, 0x0c,
	0xe5, 0x00, 0x50, 0x00, 0x00, 0x50, 0x86, 0x0c,
	0x20, 0x8a, 0xa0, 0x00, 0x00, 0x50, 0x86, 0x0c,
	0xe5, 0x00, 0x50, 0x00, 0x00, 0x1e, 0x8c, 0x0c,
	0x20, 0x8a, 0xa0, 0x00, 0x00, 0xe4, 0xc9, 0x49,
	0xe5, 0x00, 0x50, 0x00, 0x05, 0x6c, 0x63, 0x0c,
	0x7f, 0x7f, 0x18, 0x92, 0x86, 0xd2, 0x00, 0x00,
	0xeb, 0xbf, 0x86, 0x61, 0xeb, 0x0d, 0x00, 0x00,
	0xc3, 0x97, 0x6b, 0x61, 0x7f, 0x70, 0x00, 0x05,
	0xc3, 0xd7, 0x3f, 0x61, 0x3f, 0xba, 0x0a, 0x50,
	0x49, 0xc3, 0x3f, 0x92, 0x3f, 0x7f, 0xa0, 0x1a,
	0xc3, 0xc3, 0xbf, 0xba, 0x3f, 0xbf, 0x82, 0x4b,
	0x49, 0x49, 0xd7, 0x3a, 0x3f, 0x3f, 0xaf, 0xb5,
	0x86, 0x86, 0xc3, 0xba, 0x3f, 0xbf, 0x3a, 0xd7,
	0x18, 0x49, 0x49, 0x92, 0x3f, 0xd7, 0x6b, 0xbf,
	0x0c, 0x24, 0xc3, 0x92, 0x3f, 0xeb, 0xeb, 0xd7,
	0x18, 0x0c, 0x49, 0x92, 0x7f, 0xd7, 0x6b, 0xbf,
	0x0c, 0x24, 0xc3, 0x61, 0xbf, 0xeb, 0xeb, 0xd7,
	0x0c, 0x18, 0x49, 0x75, 0x7f, 0xc3, 0xd2, 0xff,
	0x86, 0x30, 0x0c, 0x35, 0xff, 0x92, 0xa5, 0xc3,
	0x49, 0x18, 0x30, 0xd7, 0x7f, 0xd2, 0x0a, 0x86,
	0xc3, 0x86, 0x30, 0x97, 0xeb, 0x70, 0x05, 0x49,
	0x1a, 0x97, 0x86, 0x0c, 0xd7, 0xd7, 0xd7, 0x3f,
	0xe1, 0x7f, 0xc3, 0x49, 0xc3, 0xc3, 0xeb, 0xbf,
	0x35, 0x97, 0x86, 0x0c, 0xc3, 0xc3, 0xc3, 0xd7,
	0xc3, 0x7f, 0xc3, 0x49, 0xc3, 0xc3, 0xc3, 0x49,
	0x97, 0xbf, 0x86, 0xc3, 0x86, 0x86, 0x86, 0x86,
	0x7f, 0x7f, 0x86, 0x49, 0x49, 0x49, 0x49, 0x49,
	0x97, 0x3f, 0x86, 0x86, 0x0c, 0x86, 0xa4, 0xc3,
	0x7f, 0x7f, 0x49, 0x86, 0x0c, 0x58, 0x58, 0x49,
	0xbf, 0xeb, 0x86, 0x86, 0xa4, 0xf0, 0xa4, 0xc3,
	0x7f, 0x6b, 0x49, 0x58, 0x58, 0xf0, 0x58, 0x49,
	0xbf, 0x86, 0x86, 0xa4, 0xf0, 0xa4, 0xa4, 0xc3,
	0x6b, 0x49, 0x58, 0x5a, 0xf0, 0x49, 0x49, 0x49,
	0x86, 0x86, 0xa5, 0x05, 0xa4, 0xa4, 0x0c, 0x86,
	0x49, 0x49, 0x08, 0x05, 0xf0, 0x58, 0x58, 0x49,
	0x86, 0xa4, 0x82, 0x00, 0xf0, 0xf0, 0xf0, 0xa4,
	0x58, 0x4b, 0x0d, 0x00, 0x5a, 0xf0, 0xf0, 0x49,
	0x8a, 0x8a, 0x00, 0x00, 0x50, 0x99, 0xcc, 0x24,
	0x45, 0x00, 0x00, 0x00, 0x1e, 0x6c, 0x66, 0x24,
	0x8a, 0x8a, 0x00, 0x00, 0xf4, 0x9c, 0xdc, 0x24,
	0x45, 0x00, 0x00, 0x00, 0x6c, 0xec, 0x6c, 0x30,
	0x8a, 0x00, 0x00, 0x00, 0xdc, 0x9c, 0xd9, 0x70,
	0x45, 0x00, 0x00, 0x00, 0xec, 0xec, 0x6d, 0x25,
	0x8a, 0x00, 0x00, 0x00, 0x99, 0x9c, 0x88, 0x8f,
	0x45, 0x00, 0x00, 0x00, 0xec, 0x66, 0x45, 0x10,
	0x8a, 0x00, 0x00, 0x00, 0x99, 0xdc, 0x8a, 0x8a,
	0x45, 0x00, 0x00, 0x00, 0xec, 0xed, 0x45, 0x45,
	0x8a, 0x00, 0x00, 0x00, 0xdc, 0xa8, 0x00, 0x8a,
	0x45, 0x00, 0x00, 0x00, 0xec, 0x45, 0x45, 0x45,
	0x8a, 0x8a, 0x8a, 0x00, 0xfc, 0x8a, 0x00, 0x8a,
	0x45, 0x45, 0x00, 0x00, 0x6d, 0x00, 0x45, 0x45,
	0x8a, 0x8a, 0x8a, 0x00, 0x8a, 0x8a, 0x00, 0x8a,
	0x00, 0x00, 0x00, 0x00, 0x45, 0x00, 0x00, 0x45,
	0x49, 0x86, 0x61, 0xd7, 0xc3, 0xa0, 0x00, 0x86,
	0xc3, 0x18, 0xc3, 0xc3, 0x70, 0x00, 0x05, 0x58,
	0x86, 0x70, 0x04, 0x92, 0xa0, 0x00, 0x0a, 0xa0,
	0x18, 0xa0, 0x0a, 0xa5, 0x00, 0x05, 0x05, 0xf0,
	0x70, 0x05, 0x00, 0x00, 0x00, 0x0a, 0x5a, 0xa5,
	0xa5, 0x0a, 0x00, 0x00, 0x05, 0x05, 0x92, 0xf0,
	0x0f, 0x05, 0x00, 0x00, 0x00, 0x4b, 0x70, 0x70,
	0xa5, 0x0a, 0x00, 0x00, 0x05, 0x92, 0xb0, 0xf0,
	0xd2, 0x05, 0x00, 0x00, 0x5a, 0xd2, 0x5a, 0xf0,
	0x61, 0x0a, 0x0a, 0x05, 0x1a, 0xa5, 0x05, 0xf0,
	0xcb, 0xa5, 0x00, 0x5a, 0x70, 0x0a, 0x0a, 0x5a,
	0x10, 0x3f, 0xa5, 0x05, 0xa5, 0x05, 0x05, 0x50,
	0x8a, 0xc3, 0x70, 0x0a, 0x0a, 0x00, 0x00, 0x5a,
	0x45, 0x10, 0xb0, 0x05, 0x00, 0x00, 0x05, 0x05,
	0x8a, 0xde, 0xed, 0x0a, 0x00, 0x00, 0x00, 0x0a,
	0x00, 0x45, 0xed, 0x00, 0x00, 0x00, 0x05, 0x05,
	0xa0, 0xe1, 0xd2, 0x00, 0x5a, 0xf0, 0xa4, 0xa4,
	0x50, 0x0e, 0x86, 0x00, 0x50, 0xf0, 0xf0, 0x49,
	0xa5, 0xe1, 0xc3, 0x00, 0x05, 0xf0, 0xa4, 0x0c,
	0x5a, 0xa4, 0x86, 0x0a, 0x05, 0xf0, 0x0c, 0x49,
	0xa5, 0xa4, 0x6b, 0xa0, 0x00, 0xf0, 0xa4, 0xc3,
	0x5a, 0xf0, 0xc3, 0x08, 0x05, 0xf0, 0xf0, 0x49,
	0xf0, 0xa4, 0x6b, 0x82, 0x00, 0x5a, 0xa4, 0xa4,
	0xf0, 0xf0, 0x97, 0x87, 0x00, 0x5a, 0xf0, 0x58,
	0xf0, 0x5a, 0x6b, 0xd2, 0x00, 0x50, 0xf0, 0xa4,
	0xa5, 0x05, 0x97, 0x86, 0x00, 0x5a, 0x58, 0x58,
	0xf0, 0x0a, 0x49, 0x6b, 0x0a, 0x05, 0xa4, 0x0c,
	0xa5, 0x05, 0x97, 0xc3, 0xa0, 0x05, 0xf0, 0x58,
	0x5a, 0x0a, 0x1d, 0x6b, 0x08, 0x00, 0xf0, 0xa4,
	0xa5, 0x05, 0xb5, 0x97, 0x82, 0x05, 0xf0, 0x58,
	0x5a, 0x00, 0xe1, 0x6b, 0x08, 0x00, 0xf0, 0xa4,
	0x05, 0x00, 0xe1, 0x3f, 0x82, 0x05, 0xf0, 0xf0
};