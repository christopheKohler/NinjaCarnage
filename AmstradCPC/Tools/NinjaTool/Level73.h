// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL73.BIN"

// -------------------------------------------------------
#define NBZONES 7

u8 leveldata[2] = { 3 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	4,4,7,4,1, // 
	0,3,1,6,2, // 
	8,12,11,15,3, // 
	4,8,7,10,4, // 
	10,3,11,6,5, // 
	0,12,3,15,6, // 
	6,5,7,6,7, // 
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
u8 scenario[10][SEQLINESIZE] =
{
	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 252, 4  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Cursor X inversed

	{ 199, 104, 103, 102, 2  ,103,101,104, 0 , 0 }, // Hit arms 
	{ 199, 105, 3  , 102, 0  ,3  ,101,105, 0 , 0 }, // dodge
	{ 199, 105, 0  , 4  , 0  ,0  ,101,105, 0 , 0 }, // choose weapon
	{ 199, 5  , 0  , 0  , 0  ,0  ,101, 5 , 0 , 0 }, // thrown head or mouth
	{ 199, 0  , 0  , 0  , 0  ,0  , 6  , 0 , 0 , 0 }, // throw flower

	{ 250, 7  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 252, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Cursor normal
	{ 255, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL81 BIN"; // Must be 11 characters

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
  "Je rentre dans le caveau et je  "
 "tombe sur �a .... C'est un        "
"Kubikajiri, un Yokai qui mange les  "
 "t�tes des cadavres. c'est pour �a "
  "que �a pue autant !             "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 5
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2JE PENSE PAS\1 que ce soit le bon "
 "moment pour lui lancer une fleur  "
"il est bien trop concentr� sur ce   "
 "qu'il fait l�. Ah ben tiens, il me"
  "saute dessus et me \2BOUFFE\1.      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "C'est \2PAS LE BON MOMENT\1 pour �a,"
 "y'a plus important l�. Il est en  "
"train de bouffer une t�te de cadavre"
 "bordel de merde !                 "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je me d�cale sur le c�t�, et ce "
 "con m'attrape la t�te et commence "
"� la bouffer. C'est \2PAS COMME CA\1    "
 "qu'il faut s'y prendre d�bilos !  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Comment tu veux que je frappe sa"
 "t�te alors que je lui tourne le   "
"dos ? Le temps que je me retourne,  "
 "il m'a saut� dessus ce con et me  "
  "\2BOUFFE\1 la tronche !             "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1. Je me retourne pour lui    "
 "frapper la t�te mais il est trop  "
"v�n�re. Je lui ai shoot� son quatre "
 "heures ! Il m'attrape la t�te et  "
  "commence � la bouffer. Le chacal"
}

};

#define NBMESSSUCESS 7 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Il bouffe une t�te d'ailleurs ! "
 "BEURK ! Je dois pas le regarder en"
"face, �a va l'�nerver. Je vais me   "
 "retourner. Tu vas devoir me faire "
  "agir de dos !                   "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Je mets un coup de pied    "
 "au niveau de ses mains. Ca fait   "
"tomber la t�te qu'il est en train   "
 "de bouffer. Ca va l'�nerver mais  "
  "il me bouffera pas la t�te.     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Il est v�n�re, il essaie  "
 "de m'attraper. J'ai juste le temps"
"de l'esquiver.                      "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2TRES BIEN\1. Je sors mon poignard "
 "de lancer. Je vais bri�vement me  "
"retourner pour le d�gommer. Dis moi "
 "o� viser, moi je ne vois rien dans"
  "cette position.                 "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Je lui lance mon poignard "
 "au niveau de sa t�te. MAIS �a ne  "
"lui fait rien. Va falloir trouver   "
 "autre chose, une derni�re id�e ?  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ca peut para�tre con, mais la   "
 "fleur que je lui ai lanc� semble  "
"le calmer. Il la ramasse et puis    "
 "\2DISPARAIT\1. En fait il d�vore les  "
  "t�tes des gens qu'on oublie.    "
},
{
/*
"------------------------------------" 36 chars (Line 3) 7
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "La fleur lui a montr� que je    "
 "n'�tais pas avare d'offrandes et  "
"�a l'a satisfait. Bien jou� mon gars"
 "Qui aurait pu croire qu'une fleur "
  "ferait mieux que mon poignard ?!"
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x00, 0x00, 0x00, 0x1a, 0x7f, 0x30, 0x4a, 0xba,
	0x5a, 0x0a, 0x00, 0x1a, 0xba, 0x70, 0x1f, 0xba,
	0xb0, 0xa0, 0x00, 0x50, 0x70, 0x0a, 0x5f, 0x70,
	0x75, 0x25, 0x00, 0x00, 0x0a, 0x00, 0x10, 0x25,
	0xba, 0x70, 0x0a, 0x00, 0x50, 0xba, 0x10, 0xa0,
	0x75, 0x30, 0x0a, 0x00, 0x5f, 0x7f, 0x50, 0x00,
	0xbf, 0xba, 0xa0, 0x05, 0x35, 0xd5, 0xa0, 0x00,
	0x75, 0x3a, 0xa0, 0x5a, 0xea, 0x90, 0xa0, 0x0a,
	0xbf, 0xba, 0x0a, 0xb0, 0x6a, 0x7a, 0x5a, 0xf8,
	0x75, 0x70, 0x05, 0x75, 0xc0, 0xaf, 0x50, 0xdc,
	0x75, 0x25, 0x05, 0x75, 0x95, 0x20, 0x5a, 0xec,
	0x30, 0xa0, 0x05, 0xb0, 0x7f, 0xa0, 0x5a, 0xdc,
	0x70, 0x0a, 0x00, 0x0a, 0xba, 0x0a, 0x5a, 0xec,
	0x0f, 0x00, 0x00, 0x05, 0x25, 0x00, 0x54, 0xf4,
	0x00, 0x00, 0x00, 0x50, 0xd0, 0x00, 0x50, 0xec,
	0x0a, 0x00, 0x05, 0x10, 0x90, 0x00, 0x5e, 0xf4,
	0xa0, 0x00, 0x00, 0x00, 0x00, 0x05, 0x30, 0x70,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0xa5,
	0x00, 0x00, 0x05, 0xf0, 0xf8, 0x0a, 0x5a, 0x0a,
	0x00, 0x0f, 0xf4, 0xf4, 0xf4, 0xa5, 0x00, 0x00,
	0x05, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0x0a, 0x00,
	0x50, 0xf4, 0xf4, 0xf4, 0xdc, 0xdc, 0xa5, 0x00,
	0x5a, 0xf8, 0xf8, 0xf8, 0xec, 0xcc, 0xf8, 0x00,
	0x5e, 0xf4, 0xdc, 0xf4, 0xdc, 0x99, 0xf8, 0x0a,
	0x5e, 0xf8, 0xfc, 0xf8, 0xec, 0x33, 0xdc, 0xa0,
	0xf4, 0xf4, 0xdc, 0xf4, 0xdc, 0x66, 0xec, 0xf8,
	0xfc, 0xfc, 0xec, 0xf8, 0xec, 0x66, 0xdc, 0xdc,
	0xdc, 0xf4, 0x99, 0xfc, 0xfc, 0x66, 0xec, 0xec,
	0x66, 0xfc, 0xb9, 0xf8, 0xec, 0xdc, 0xf4, 0xfe,
	0x76, 0xfc, 0x99, 0xdc, 0xf4, 0xcc, 0xf8, 0xec,
	0x66, 0xfc, 0xb9, 0xfc, 0xec, 0xfc, 0xf4, 0xfe,
	0x99, 0xdc, 0x99, 0xdc, 0xf4, 0xdc, 0xfc, 0x7f,
	0x00, 0x1a, 0xbf, 0xba, 0x1a, 0x3f, 0xfa, 0x1a,
	0x00, 0x1a, 0x7f, 0x75, 0x1a, 0x7f, 0x70, 0x1a,
	0x00, 0x10, 0xbf, 0xba, 0x1a, 0xbf, 0xfa, 0x1a,
	0x00, 0x50, 0x75, 0x70, 0x50, 0x75, 0x70, 0x50,
	0x00, 0x05, 0x30, 0x25, 0x50, 0xba, 0xfa, 0x50,
	0x00, 0x00, 0x5a, 0x0a, 0x05, 0x75, 0x25, 0x05,
	0x00, 0x00, 0x05, 0x05, 0x00, 0xb0, 0x25, 0x05,
	0x00, 0x00, 0x0a, 0x0a, 0x05, 0x1a, 0xa0, 0x00,
	0x00, 0x00, 0x05, 0x05, 0x00, 0x5a, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xad, 0x00, 0x00, 0x0a, 0x0a, 0x00, 0x05, 0x00,
	0xf0, 0x00, 0x05, 0x05, 0x05, 0x00, 0x00, 0x0a,
	0xf8, 0x00, 0x0f, 0x0f, 0x0a, 0x0a, 0x05, 0x05,
	0xdc, 0x0a, 0x05, 0x1a, 0xa5, 0x00, 0x00, 0x0a,
	0xf8, 0x0a, 0x00, 0x5a, 0x30, 0xa5, 0x05, 0xa0,
	0x70, 0x0a, 0x1a, 0x5f, 0x7a, 0x00, 0xf0, 0xf8,
	0x30, 0xa0, 0xf5, 0x35, 0xaf, 0x05, 0xf4, 0xf4,
	0x75, 0x25, 0x35, 0xbf, 0x20, 0x5a, 0xec, 0xf8,
	0xbf, 0xaf, 0xea, 0x7f, 0x20, 0xf4, 0xdc, 0xfc,
	0xff, 0xaf, 0x6a, 0xba, 0x20, 0xf4, 0xec, 0xdc,
	0x75, 0x25, 0xea, 0x30, 0xa0, 0xf4, 0xdc, 0xcc,
	0xb0, 0xa5, 0x35, 0x70, 0x0a, 0xf4, 0xec, 0xec,
	0x5a, 0x0a, 0xf5, 0x25, 0x00, 0x5e, 0xfc, 0xcc,
	0x05, 0x00, 0x1a, 0xa0, 0x00, 0xf4, 0xfc, 0xec,
	0x00, 0x00, 0x50, 0x0a, 0x00, 0x5e, 0x74, 0xfc,
	0x5a, 0x0a, 0x05, 0x00, 0x00, 0x50, 0x30, 0xfc,
	0xb0, 0xa0, 0x00, 0x0f, 0x00, 0x05, 0x60, 0x74,
	0x75, 0x20, 0x5a, 0x30, 0x00, 0x05, 0xe0, 0x7a,
	0xbf, 0xaa, 0xb0, 0xba, 0x0a, 0x00, 0x1a, 0xd5,
	0xff, 0xaf, 0x35, 0xd5, 0x0a, 0x00, 0x50, 0x95,
	0xbf, 0xaf, 0x75, 0x7f, 0xa0, 0x00, 0x05, 0xff,
	0xb9, 0x66, 0xb9, 0x76, 0xec, 0xfc, 0xf4, 0x7e,
	0x99, 0xdc, 0xec, 0x76, 0xf4, 0xf8, 0xfd, 0xd5,
	0xec, 0x66, 0xec, 0x66, 0xec, 0xfc, 0xfc, 0x7f,
	0xfc, 0x99, 0xdc, 0xdc, 0xfc, 0xf8, 0xfd, 0xd5,
	0xec, 0xb9, 0xfc, 0xcc, 0xfc, 0xfc, 0xfe, 0x7e,
	0xfc, 0x99, 0xdc, 0xdc, 0xfc, 0x30, 0xff, 0xd5,
	0xdc, 0xec, 0x66, 0xb8, 0x75, 0x7f, 0x7f, 0x7e,
	0x66, 0xfc, 0xdc, 0x30, 0x1f, 0x95, 0xbf, 0xd5,
	0x99, 0xdc, 0xfc, 0x1a, 0x60, 0xc0, 0x7f, 0x7e,
	0xcc, 0xb8, 0x25, 0x05, 0xbf, 0x95, 0xbf, 0xfe,
	0xfc, 0x75, 0xba, 0x0f, 0xff, 0xff, 0xff, 0xfc,
	0xb8, 0xbf, 0x95, 0xaf, 0xbf, 0xbf, 0xff, 0xf8,
	0xfd, 0xea, 0xc0, 0x7f, 0xff, 0x7f, 0x7f, 0xfc,
	0xf5, 0xbf, 0x95, 0xd5, 0xbf, 0x95, 0x95, 0xf8,
	0x75, 0x7f, 0x7f, 0x7f, 0x7f, 0x6a, 0x3f, 0x25,
	0xbf, 0xd5, 0xbf, 0xff, 0xbf, 0x95, 0xbf, 0xaa,
	0xdc, 0xa0, 0x00, 0x05, 0xf5, 0x70, 0x05, 0x25,
	0xd8, 0xa0, 0x00, 0x00, 0x1a, 0xfa, 0x50, 0x70,
	0xdc, 0xa0, 0x00, 0x05, 0x50, 0xfa, 0x10, 0xba,
	0xec, 0xa8, 0x00, 0x00, 0x5a, 0x70, 0x55, 0x75,
	0xcc, 0xa5, 0x00, 0x00, 0x05, 0xa5, 0x10, 0xb0,
	0xec, 0xad, 0x00, 0x05, 0x0a, 0x00, 0x10, 0x1a,
	0xfc, 0xf8, 0x00, 0x5a, 0x70, 0x0a, 0x50, 0x50,
	0xec, 0xf8, 0x00, 0x05, 0xba, 0xa5, 0x05, 0x05,
	0xf4, 0xf0, 0x00, 0x00, 0xba, 0x70, 0x00, 0x00,
	0xf8, 0xf8, 0x00, 0x00, 0x75, 0x25, 0x00, 0x00,
	0xf4, 0xf0, 0x00, 0x00, 0xba, 0xa0, 0x05, 0xb0,
	0xf8, 0xf8, 0x00, 0x00, 0x70, 0x0a, 0x50, 0x30,
	0xf4, 0xa5, 0x00, 0x05, 0xa5, 0x00, 0x50, 0x70,
	0xf8, 0x0f, 0x00, 0x5a, 0x0a, 0x0a, 0x05, 0xa5,
	0x00, 0x00, 0x00, 0xa5, 0x05, 0x00, 0x00, 0x00,
	0x5a, 0xa5, 0x00, 0x5a, 0xa5, 0x00, 0x5a, 0x0a,
	0x7f, 0x25, 0x30, 0xff, 0xa0, 0x00, 0x00, 0x75,
	0xba, 0xa5, 0x75, 0x75, 0xa0, 0x00, 0x00, 0x1a,
	0x70, 0x0f, 0x30, 0xba, 0x0a, 0x05, 0xa5, 0x55,
	0x25, 0x05, 0xb0, 0x70, 0x0a, 0x50, 0x75, 0x1a,
	0xa0, 0x00, 0x5a, 0xfa, 0x0a, 0x1a, 0xbf, 0xaf,
	0x00, 0x00, 0x05, 0x25, 0x00, 0xf5, 0x7f, 0x2f,
	0x00, 0x00, 0x05, 0x25, 0x00, 0xb0, 0xbf, 0xd0,
	0xa5, 0x00, 0x00, 0xa0, 0x05, 0x75, 0x3f, 0x7a,
	0x70, 0x05, 0xa0, 0x00, 0x00, 0xba, 0xbf, 0xd0,
	0xba, 0x50, 0x25, 0xa5, 0x05, 0x75, 0x7f, 0x2f,
	0x75, 0x50, 0x70, 0x70, 0x50, 0xba, 0xbf, 0xaa,
	0xbf, 0x05, 0xb0, 0xba, 0x05, 0x75, 0x7f, 0x25,
	0x7f, 0x00, 0x1a, 0xba, 0x50, 0x30, 0xba, 0x0a,
	0xbf, 0x05, 0x10, 0x70, 0x10, 0xb0, 0x30, 0x05,
	0x75, 0x5a, 0x5a, 0x20, 0x5a, 0x70, 0x70, 0x0a,
	0xb0, 0x10, 0xa5, 0xa0, 0x1a, 0xba, 0xa5, 0x00,
	0xff, 0x7f, 0x6a, 0x7f, 0x6a, 0x6a, 0xd7, 0x2f,
	0xbf, 0xbf, 0xc0, 0xd5, 0xbf, 0x3a, 0x5d, 0x3a,
	0xff, 0x6a, 0x6a, 0x7f, 0x3f, 0xbf, 0xbf, 0xd5,
	0xff, 0xbf, 0xc0, 0x95, 0xbf, 0xba, 0x6a, 0x3f,
	0xff, 0xff, 0x6a, 0x7f, 0x7f, 0x5f, 0x3f, 0x95,
	0xf5, 0xaf, 0xbf, 0x3f, 0xbf, 0xbf, 0x6a, 0x7f,
	0x5f, 0xaf, 0xff, 0x5f, 0x3f, 0x7f, 0xbf, 0xd5,
	0x5f, 0x7f, 0x2f, 0x05, 0x95, 0xae, 0x0a, 0x3a,
	0x10, 0xbf, 0xaf, 0xfa, 0x6a, 0x7f, 0xaa, 0xaf,
	0x50, 0x6a, 0x95, 0xaf, 0xc0, 0x95, 0xaa, 0x20,
	0x05, 0xbf, 0x95, 0xf5, 0x6a, 0x7f, 0x20, 0x0a,
	0x00, 0xb5, 0xaf, 0xbf, 0x3f, 0xbf, 0x05, 0x00,
	0x00, 0x1a, 0x5f, 0x7f, 0x7f, 0x7f, 0x5a, 0x0a,
	0x00, 0x50, 0x25, 0xbf, 0xbf, 0xf0, 0xa5, 0x00,
	0x05, 0x05, 0x20, 0x75, 0xf0, 0x30, 0x5a, 0x0a,
	0x00, 0x0a, 0xa5, 0x50, 0x24, 0x24, 0xa5, 0x05,
	0x75, 0xba, 0x0a, 0x00, 0x0a, 0x0a, 0xb0, 0xa5,
	0xff, 0x7f, 0x25, 0x05, 0x05, 0x00, 0x75, 0x70,
	0x3f, 0xbf, 0xba, 0x0a, 0x5a, 0x00, 0x30, 0xba,
	0x6a, 0x7f, 0x75, 0xa0, 0x5a, 0x00, 0xa5, 0x30,
	0x95, 0x95, 0xba, 0xa0, 0x5a, 0x00, 0x0a, 0x70,
	0x6a, 0x95, 0x7f, 0x20, 0xa5, 0x00, 0x00, 0xa5,
	0x3f, 0x95, 0xbf, 0xa0, 0x25, 0x0a, 0x00, 0x0a,
	0xea, 0x3f, 0x7f, 0x0a, 0xaf, 0xa0, 0x05, 0x0a,
	0xbf, 0x95, 0xbf, 0x0a, 0xaa, 0x20, 0x0f, 0x00,
	0x35, 0x3f, 0x7f, 0xa0, 0x20, 0xa0, 0xa5, 0x05,
	0xf5, 0x3f, 0xbf, 0xa0, 0xa0, 0x0a, 0xa0, 0x50,
	0x5f, 0x7f, 0x7f, 0x20, 0x0a, 0x00, 0x0a, 0x10,
	0x50, 0xbf, 0xbf, 0xaa, 0x0a, 0x0a, 0x05, 0x55,
	0x05, 0x75, 0x7f, 0x25, 0x05, 0xa5, 0x50, 0x10,
	0x05, 0xf5, 0xff, 0xa5, 0x50, 0x30, 0x1a, 0x55,
	0x00, 0x5f, 0xba, 0x5a, 0x10, 0x70, 0x1a, 0x55,
	0x5a, 0x5f, 0x70, 0x0a, 0xf5, 0x70, 0x0a, 0x0a,
	0x05, 0x1f, 0xba, 0x05, 0x30, 0xaf, 0x05, 0x00,
	0x0a, 0x5f, 0x3a, 0x05, 0x75, 0xfa, 0x0a, 0x00,
	0xa5, 0x1f, 0xfa, 0x50, 0xba, 0xaf, 0x00, 0x0a,
	0x70, 0x5f, 0xaf, 0x10, 0x75, 0x20, 0x0a, 0xa0,
	0xb0, 0x5f, 0x20, 0x1a, 0xba, 0xa5, 0x05, 0x25,
	0x5a, 0x1a, 0xa0, 0xf5, 0x70, 0x0a, 0x5a, 0x25,
	0x05, 0x10, 0x0a, 0x30, 0xaf, 0x00, 0x1a, 0x25,
	0x00, 0x5a, 0x05, 0x75, 0x70, 0x00, 0x1a, 0xa0,
	0x00, 0x05, 0x05, 0x30, 0xba, 0x00, 0x5a, 0x0a,
	0x00, 0x00, 0x05, 0x75, 0x7f, 0x0a, 0x05, 0x00,
	0x00, 0x00, 0x05, 0x75, 0xbf, 0xa0, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xb0, 0x7f, 0x25, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x5a, 0xbf, 0xfa, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x75, 0x3a, 0xa5, 0x05,
	0x0f, 0x00, 0x00, 0x00, 0xb0, 0xbf, 0x70, 0x5a,
	0x05, 0x00, 0x0f, 0x10, 0x18, 0x18, 0x58, 0x0a,
	0x00, 0x0a, 0x05, 0x04, 0x24, 0xae, 0x25, 0x05,
	0x05, 0x05, 0x05, 0x1a, 0x5d, 0x57, 0x58, 0x0a,
	0x00, 0x5a, 0x0a, 0x0e, 0xab, 0x2b, 0x0d, 0x0f,
	0x05, 0x05, 0xa0, 0x05, 0x5d, 0x57, 0x58, 0x0a,
	0x00, 0x5a, 0xa5, 0x05, 0xa4, 0xae, 0xa5, 0x0f,
	0x00, 0x05, 0xf0, 0x00, 0x5a, 0x58, 0x0f, 0x0f,
	0x00, 0x5a, 0x70, 0x00, 0x05, 0x0f, 0x0f, 0x0f,
	0x00, 0x05, 0xa5, 0x00, 0x00, 0x00, 0x05, 0x5a,
	0x00, 0x00, 0x0a, 0x05, 0x00, 0x00, 0x05, 0xb0,
	0x00, 0x0a, 0x00, 0x00, 0x0f, 0x00, 0x5a, 0x75,
	0x50, 0x20, 0x0a, 0x00, 0x1a, 0xa0, 0x5a, 0x30,
	0x1a, 0x25, 0xa5, 0x00, 0x50, 0x2f, 0x05, 0xf0,
	0x1a, 0x70, 0x70, 0x00, 0x05, 0xbf, 0x0a, 0x00,
	0xf5, 0x30, 0x30, 0x0a, 0x0a, 0x60, 0xaa, 0x00,
	0x75, 0xff, 0x75, 0x0a, 0x0a, 0xe0, 0x2f, 0x05,
	0x00, 0x5a, 0x70, 0x1a, 0x05, 0x25, 0x50, 0x10,
	0x00, 0x05, 0xa5, 0xb0, 0x00, 0xa0, 0x05, 0x50,
	0x0a, 0x5a, 0x5a, 0x30, 0x0a, 0xa0, 0x00, 0x05,
	0x00, 0x05, 0xb0, 0xba, 0x0a, 0x0a, 0x5a, 0x0a,
	0x0a, 0x0f, 0x5f, 0x7f, 0xa0, 0xb0, 0x30, 0xa5,
	0x05, 0x0f, 0x75, 0xbf, 0xa0, 0x75, 0x30, 0xf0,
	0x0a, 0x5a, 0xff, 0x7f, 0x20, 0xba, 0xba, 0x1a,
	0xa5, 0x1a, 0xff, 0xbf, 0x20, 0xff, 0x70, 0x55,
	0x25, 0x1a, 0xff, 0x7f, 0x20, 0xba, 0x25, 0x55,
	0x25, 0xf5, 0xbf, 0xbf, 0x20, 0x70, 0x0a, 0x10,
	0x25, 0xb0, 0xff, 0x6a, 0xa0, 0xa5, 0x00, 0x05,
	0xa5, 0x75, 0xbf, 0x95, 0xa0, 0x00, 0x00, 0x05,
	0x0f, 0x75, 0x6a, 0x7f, 0x0a, 0x5a, 0x0a, 0x0a,
	0x1a, 0xbf, 0x95, 0xba, 0x0a, 0xf0, 0x00, 0x05,
	0x75, 0x3f, 0x7f, 0xfa, 0x05, 0x25, 0x00, 0x0f,
	0xbf, 0x95, 0xbf, 0xaf, 0x50, 0x20, 0x05, 0x50,
	0x70, 0x0a, 0x00, 0x00, 0x5a, 0x75, 0x7f, 0x30,
	0xba, 0xa0, 0x00, 0x00, 0x05, 0xb0, 0xbf, 0xbf,
	0xba, 0xa0, 0x00, 0x00, 0x00, 0x5a, 0x75, 0x7f,
	0xba, 0xa0, 0x05, 0x00, 0x00, 0x05, 0xb0, 0x95,
	0x70, 0x0a, 0x5a, 0x00, 0x00, 0x00, 0x5a, 0x6a,
	0xa5, 0x05, 0xb0, 0x0a, 0x00, 0x00, 0x50, 0xbf,
	0x0a, 0x50, 0x30, 0x0a, 0x00, 0x00, 0x05, 0x60,
	0x00, 0x10, 0xfa, 0x00, 0x00, 0x00, 0x05, 0xbf,
	0x00, 0x5f, 0x25, 0x05, 0xa0, 0x00, 0x00, 0x75,
	0x0a, 0x5f, 0x25, 0x50, 0x25, 0x00, 0x05, 0xbf,
	0xa0, 0x1a, 0xa0, 0x1a, 0x25, 0x00, 0x00, 0x75,
	0x20, 0x5a, 0x0a, 0x1a, 0x25, 0x00, 0x00, 0x30,
	0xaf, 0x05, 0x00, 0xf5, 0x25, 0x5a, 0x00, 0xf5,
	0xfa, 0x0a, 0x00, 0xb0, 0x25, 0xb0, 0x0a, 0xb0,
	0x25, 0xa0, 0x05, 0x75, 0xa5, 0x30, 0xa0, 0x1a,
	0xa0, 0x20, 0x5a, 0xba, 0xa5, 0x75, 0xa0, 0x1a,
	0xff, 0xff, 0x1a, 0x0a, 0x00, 0x1f, 0xd5, 0x75,
	0xff, 0x25, 0x0f, 0x0a, 0x0f, 0x55, 0x95, 0xbf,
	0x7f, 0x20, 0x0a, 0xa5, 0x0f, 0x15, 0xc0, 0x7f,
	0x95, 0xaf, 0x0a, 0xba, 0xa5, 0x5f, 0x6a, 0xbf,
	0x3f, 0xff, 0x0f, 0x75, 0x1a, 0xbf, 0x95, 0x7f,
	0xd5, 0x70, 0x87, 0x0f, 0x75, 0x75, 0x3f, 0xbf,
	0x3a, 0xa0, 0x0a, 0x50, 0x1a, 0xba, 0xff, 0x7f,
	0xfa, 0x05, 0x00, 0x05, 0xa0, 0x25, 0x75, 0xff,
	0x2a, 0x0f, 0x0f, 0x5a, 0x0a, 0x5a, 0xff, 0xff,
	0xaf, 0x0a, 0x0f, 0x75, 0x25, 0x1a, 0x75, 0xff,
	0x7a, 0x00, 0x5a, 0x1a, 0xaf, 0xb0, 0xff, 0xba,
	0xff, 0xa0, 0x0f, 0x0f, 0x70, 0x75, 0xff, 0xfa,
	0xff, 0xba, 0xa5, 0x5a, 0x1a, 0xba, 0xff, 0x25,
	0xfa, 0x75, 0xfa, 0x0f, 0x75, 0x25, 0x75, 0xa0,
	0xba, 0x05, 0x25, 0x1a, 0x1a, 0x0a, 0xf5, 0x0a,
	0xaf, 0x00, 0x05, 0xa5, 0x0f, 0x00, 0x30, 0x00,
	0x6a, 0x7f, 0x7f, 0x25, 0x10, 0xa5, 0x00, 0x1a,
	0xc0, 0xbf, 0xff, 0xa0, 0x50, 0x0a, 0x05, 0x5f,
	0x95, 0x7f, 0xba, 0x0a, 0x05, 0x05, 0x00, 0xf5,
	0xbf, 0xff, 0x25, 0x00, 0x00, 0x0a, 0x05, 0x75,
	0x7f, 0xba, 0x0a, 0x00, 0x5a, 0x25, 0x50, 0xba,
	0xff, 0xa5, 0x00, 0x00, 0xb0, 0xa0, 0x5a, 0xfa,
	0xba, 0x0a, 0x00, 0x00, 0x05, 0x00, 0x50, 0x25,
	0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa0,
	0xa0, 0x00, 0x00, 0x00, 0x00, 0x05, 0xf0, 0x0a,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xa5, 0x00,
	0x00, 0x00, 0x0a, 0x00, 0x00, 0x05, 0xa5, 0x00,
	0x00, 0x50, 0x25, 0x00, 0x00, 0x0f, 0xa0, 0x0a,
	0x00, 0x1a, 0x70, 0x00, 0x00, 0x5a, 0x05, 0xa5,
	0x00, 0x1a, 0xb0, 0x00, 0x00, 0x05, 0x50, 0x25,
	0x00, 0x50, 0x70, 0x00, 0x00, 0x00, 0x10, 0xfa,
	0x00, 0x05, 0xa0, 0x0a, 0x00, 0x00, 0x5f, 0x70,
	0x0a, 0xa0, 0x5a, 0xff, 0xa5, 0xba, 0xa0, 0x50,
	0x00, 0x0a, 0xf0, 0xba, 0x5a, 0x75, 0x0a, 0x50,
	0x00, 0x0a, 0xf0, 0x70, 0x50, 0xba, 0x0a, 0x4f,
	0x00, 0x05, 0x25, 0x25, 0x50, 0x70, 0x00, 0xf4,
	0x0a, 0x05, 0x20, 0xa0, 0x50, 0x25, 0x05, 0xde,
	0x0a, 0x5a, 0xaf, 0x00, 0x05, 0xa0, 0x4f, 0xfc,
	0xa0, 0x5a, 0x70, 0x00, 0x00, 0x0a, 0xf4, 0x7c,
	0xa0, 0xb0, 0xba, 0x00, 0x00, 0x05, 0xde, 0xed,
	0x20, 0xf5, 0x70, 0x00, 0x00, 0x50, 0xfc, 0x6d,
	0xa0, 0x30, 0xa5, 0x00, 0x00, 0x4f, 0xbc, 0xf8,
	0x20, 0x75, 0xa0, 0x00, 0x05, 0xde, 0x3c, 0xf8,
	0xaa, 0xb0, 0x0a, 0x00, 0x50, 0xde, 0x7c, 0x8f,
	0x20, 0xb0, 0x0a, 0x00, 0x4f, 0xbc, 0x7c, 0x8f,
	0xaa, 0x5a, 0x00, 0x05, 0xf4, 0x3c, 0xfc, 0xa0,
	0x20, 0x00, 0x00, 0x5a, 0xde, 0x6c, 0xed, 0xa0,
	0x20, 0x00, 0x05, 0xe5, 0xbc, 0x9c, 0xed, 0x0a,
	0xaf, 0x0a, 0x5a, 0x0f, 0x00, 0x0f, 0xfa, 0x00,
	0x20, 0x0f, 0x0f, 0xa5, 0x00, 0x0f, 0xaf, 0x00,
	0x25, 0x0f, 0x5a, 0x0f, 0x00, 0x1a, 0xaa, 0x00,
	0x20, 0x0a, 0x1a, 0xa5, 0x00, 0x1a, 0xa0, 0x00,
	0x8a, 0x05, 0x50, 0x0f, 0x05, 0x0f, 0x00, 0x00,
	0xa0, 0x00, 0x1a, 0xa5, 0x0f, 0x00, 0x00, 0x00,
	0xa0, 0x5a, 0x10, 0xa5, 0x0a, 0x00, 0x00, 0x00,
	0x0a, 0xe5, 0x10, 0xa5, 0x0a, 0x00, 0x00, 0x00,
	0x0a, 0xde, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xbc, 0x0a, 0x0f, 0x00, 0x00, 0x05, 0x00,
	0x05, 0xfc, 0x8a, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0xbc, 0xa8, 0x0a, 0x00, 0x00, 0x05, 0x00,
	0x50, 0x7c, 0xa8, 0x0a, 0x00, 0x00, 0x00, 0x0a,
	0x4f, 0xbc, 0xa8, 0xa0, 0x00, 0x00, 0x05, 0x00,
	0x54, 0x7c, 0xa8, 0x0a, 0x00, 0x00, 0x5a, 0x0a,
	0x5e, 0xbc, 0xa0, 0xa5, 0x00, 0x00, 0xf4, 0xa0,
	0x05, 0x00, 0x05, 0xa5, 0x00, 0x05, 0xf5, 0x70,
	0x0a, 0x00, 0x5a, 0xba, 0x00, 0x05, 0x75, 0xa5,
	0x05, 0x00, 0x05, 0x25, 0x00, 0x50, 0x30, 0x0a,
	0x0a, 0xa0, 0x00, 0x10, 0x0a, 0x50, 0x70, 0x05,
	0x50, 0x05, 0x00, 0x5f, 0x20, 0x05, 0x25, 0x50,
	0x0a, 0xa0, 0x00, 0x10, 0x0a, 0x00, 0xa0, 0x1a,
	0x50, 0xda, 0x00, 0x05, 0x20, 0x05, 0x0a, 0xf5,
	0x05, 0xe5, 0x0a, 0x10, 0xaf, 0x00, 0x0f, 0x30,
	0x00, 0xcf, 0xa0, 0x05, 0x30, 0x0a, 0x05, 0x70,
	0x00, 0xde, 0x8f, 0x00, 0x1a, 0x0a, 0x5a, 0xa5,
	0x00, 0xf4, 0xf8, 0x00, 0x05, 0x00, 0x05, 0xa5,
	0x00, 0x5e, 0xed, 0x0a, 0x00, 0x00, 0x0a, 0x0a,
	0x00, 0x54, 0x7c, 0xa0, 0x00, 0x00, 0x05, 0x05,
	0x00, 0x45, 0xbc, 0x8a, 0x00, 0x00, 0x00, 0x0a,
	0x00, 0x50, 0x3c, 0x8a, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x05, 0xbc, 0xa8, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x7f, 0xbf, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xd5, 0x0c, 0x5d, 0x6a, 0xc0,
	0xc0, 0xc0, 0xc0, 0xae, 0x58, 0xa4, 0x6a, 0xc0,
	0xc0, 0xc0, 0x95, 0x58, 0x0c, 0x58, 0xea, 0xc0,
	0xc0, 0xc0, 0x95, 0xa4, 0xbf, 0xfa, 0x1d, 0xc0,
	0xc0, 0xc0, 0xd5, 0x0c, 0x0c, 0xae, 0xf5, 0xc0,
	0xc0, 0xc0, 0x2e, 0xa4, 0xf0, 0x5d, 0xf5, 0xc0,
	0xc0, 0xc0, 0x7a, 0x0c, 0x0c, 0xf5, 0x0e, 0x6a,
	0xc0, 0xc0, 0xae, 0x58, 0x58, 0x5d, 0xa4, 0x6a,
	0xc0, 0xc0, 0x58, 0x0c, 0xaf, 0xa4, 0x0e, 0xea,
	0xc0, 0xc0, 0x58, 0x58, 0x7a, 0xf5, 0xa4, 0xea,
	0xc0, 0xc0, 0xaf, 0x0d, 0xae, 0x0c, 0x0e, 0xea,
	0xc0, 0xc0, 0xaf, 0x0d, 0x5d, 0x58, 0x0e, 0xea,
	0xc0, 0xc0, 0x2f, 0x58, 0x0e, 0xa5, 0x0c, 0x6a,
	0xc0, 0xc0, 0xd5, 0x0e, 0xf0, 0xa4, 0x5d, 0x6a,
	0x5e, 0x7c, 0x0a, 0xa0, 0x00, 0x00, 0xde, 0x8a,
	0xf4, 0xfc, 0x05, 0x8f, 0x00, 0x00, 0xfc, 0xa8,
	0xf4, 0xde, 0x00, 0x8f, 0x00, 0x05, 0xbc, 0xf8,
	0xcf, 0xf8, 0x05, 0xf8, 0x00, 0x05, 0xfc, 0xa8,
	0xde, 0xde, 0x05, 0xed, 0x00, 0x50, 0xbc, 0xf8,
	0xcf, 0xf8, 0x50, 0x6d, 0x00, 0x50, 0xbc, 0x6d,
	0xde, 0x8f, 0x50, 0xfc, 0x00, 0x45, 0xbc, 0xed,
	0xed, 0xa0, 0x4f, 0x7c, 0x00, 0x45, 0xec, 0x7c,
	0xde, 0x8a, 0x4f, 0xfc, 0x00, 0x45, 0xbc, 0xdc,
	0xed, 0xa0, 0xf4, 0x7c, 0x00, 0x45, 0x6c, 0x7c,
	0xcf, 0xa0, 0xb4, 0x6d, 0x00, 0x54, 0xbc, 0xdc,
	0xed, 0xa5, 0xde, 0x6d, 0x00, 0x5e, 0x6c, 0x76,
	0xcf, 0x8f, 0x9e, 0x78, 0x00, 0x54, 0xbc, 0xdc,
	0xed, 0xf0, 0xde, 0x78, 0x00, 0x5e, 0x6c, 0x76,
	0xcf, 0xda, 0xbc, 0x2d, 0x00, 0x1e, 0x39, 0xdc,
	0xed, 0xe5, 0xde, 0x78, 0x00, 0xf4, 0x6c, 0x76,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x91, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xd4, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x94, 0xe8, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x91, 0xe8, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xb9, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x76, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x36, 0x62, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x94, 0xe8, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x91, 0x39, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x7c, 0xc0, 0xb9, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x36, 0x62, 0x36, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x36, 0xe8, 0x76, 0x62, 0xc0, 0xc0,
	0xc0, 0xc0, 0x94, 0x39, 0xd4, 0x62, 0xc0, 0xc0,
	0xc0, 0xc0, 0x91, 0x7c, 0xd4, 0x68, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc4, 0x5f, 0x5d, 0x0c, 0x5d, 0xc0,
	0xc0, 0xc0, 0xc4, 0x0e, 0xae, 0xae, 0xbf, 0xc0,
	0xc8, 0xc0, 0xd5, 0x75, 0x5d, 0x0c, 0x6a, 0xc0,
	0x75, 0xcc, 0xba, 0x9d, 0xff, 0xbf, 0xc0, 0xc0,
	0xba, 0xba, 0x30, 0xc8, 0xc0, 0xc0, 0xc0, 0xc0,
	0x75, 0x75, 0x30, 0xee, 0xba, 0xee, 0xc0, 0xc0,
	0xba, 0xba, 0x30, 0x30, 0x75, 0xc8, 0xc0, 0xc0,
	0x30, 0x30, 0x8a, 0xe5, 0x64, 0xc0, 0xc0, 0xc0,
	0x30, 0x65, 0x45, 0x9a, 0xea, 0xc0, 0xc0, 0xc0,
	0x30, 0x8a, 0x50, 0x9a, 0xea, 0xc0, 0xc0, 0xc0,
	0xda, 0x45, 0x65, 0x65, 0xc8, 0xc0, 0xc0, 0xc0,
	0x45, 0x9a, 0x9a, 0x9a, 0xc8, 0xc0, 0xc0, 0xc0,
	0xcf, 0x65, 0x30, 0x30, 0xc8, 0xc0, 0xc0, 0xc0,
	0xcf, 0x9a, 0x30, 0x30, 0xee, 0xc0, 0xc0, 0xc0,
	0xe5, 0xe5, 0x30, 0x30, 0x75, 0xc8, 0xc0, 0xc0,
	0x50, 0x00, 0xb0, 0xee, 0xc4, 0xcc, 0xc0, 0xc0,
	0xde, 0xcf, 0xbc, 0x2d, 0x00, 0x1e, 0x99, 0xdc,
	0xed, 0xe5, 0x3c, 0x2d, 0x00, 0xb4, 0x6c, 0xdc,
	0xfc, 0xde, 0xbc, 0xa8, 0x00, 0x9e, 0xcc, 0x78,
	0xed, 0xfc, 0x3c, 0xad, 0x00, 0xf4, 0x9c, 0xad,
	0xfc, 0xbc, 0x3c, 0xa8, 0x00, 0x9e, 0xdc, 0xa0,
	0xed, 0xfc, 0x3c, 0x8a, 0x00, 0xde, 0x6d, 0x0a,
	0xfc, 0xbc, 0x3c, 0xa8, 0x00, 0x9e, 0xf8, 0x00,
	0xfc, 0x3c, 0x7c, 0x8a, 0x00, 0xf4, 0x8f, 0x00,
	0xfc, 0xbc, 0x9c, 0xa0, 0x00, 0xf4, 0xa0, 0x00,
	0xfc, 0x3c, 0x7c, 0x8a, 0x00, 0x4f, 0x0a, 0x00,
	0xbc, 0x3c, 0x9c, 0xa0, 0x00, 0x4f, 0x00, 0x00,
	0xfc, 0x6c, 0x7c, 0x0a, 0x00, 0x50, 0x0a, 0x00,
	0xbc, 0x99, 0x9c, 0xa0, 0x00, 0x50, 0x00, 0x00,
	0x3c, 0xcc, 0x7c, 0x0a, 0x00, 0x05, 0x00, 0x00,
	0xbc, 0x99, 0x7c, 0x0a, 0x00, 0x05, 0x00, 0x00,
	0x3c, 0xcc, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0xc0, 0xc0, 0x7c, 0x76, 0x68, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0x36, 0x7c, 0x68, 0xc0, 0xc0,
	0xc0, 0xc0, 0xb9, 0x94, 0xfc, 0x68, 0xc0, 0xc0,
	0xc0, 0xc0, 0x7c, 0x94, 0xfc, 0x62, 0xc0, 0xc0,
	0xc0, 0xc0, 0x7c, 0xd4, 0xf9, 0x62, 0xc0, 0xc0,
	0xc0, 0xc0, 0x76, 0xfc, 0xa8, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x94, 0xfc, 0xa2, 0x68, 0xc0, 0xc0,
	0xc0, 0xc0, 0x91, 0x39, 0x28, 0xe2, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x73, 0x14, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0x51, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x91, 0xa2, 0x68, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x28, 0xb3, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x73, 0x14, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0x51, 0x68,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91, 0xa2, 0xb3,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x79, 0x11
};