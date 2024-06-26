// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL32.BIN"

// -------------------------------------------------------
#define NBZONES 6

u8 leveldata[2] = { 2 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	0,12,3,15,1, // 
	0,0,3,3,2, // 
	7,14,9,15,3, // 
	6,8,9,9,4, //
	1,7,3,9,5, //
	6,4,8,5,6, //
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
u8 scenario[15][SEQLINESIZE] =
{
	{ 199, 1  , 101, 103, 101,102, 101, 0 , 0 , 0 }, // 
	{ 199, 0  , 2  , 103, 101,102, 101, 0 , 0 , 0 }, // 
    { 199, 0  , 0  , 103, 3  ,102, 106, 0 , 0 , 0 }, // 
	{ 251, 45 , 4,  104 , 3  ,2  , 1  , 2  ,1 , 2}, // QTE
	{ 3  ,4   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, 
    { 199, 0  , 0  , 105 , 0  ,102, 5  , 0 , 0 , 0 }, //
    { 250, 6  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // text 6
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL33 BIN"; // Must be 11 characters

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
  "Ah le con, il est en train de   "
 "bouffer ! J'avais bien dit que    "
"c'�tait des baltringues !!! Je vais "
 "lui d�foncer sa gueule � ce tas de"
  "chiasse crasseux...             "
};
u8* messageintro_compress;

#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 6
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai � peine le temps de bouger "
 "qu'il sort son \2TANTO\1 et me        "
"l'\2ENFONCE\1 dans le ventre. J'ai les  "
 "visc�res sur la table. Ah tiens   "
  "il est l� le Mac Do !           "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Tu penses que c'est l'heure de  "
 "manger cr�tin ? J'ai � peine pu   "
"avancer vers le bol que ce con m'a  "
 "\2TRANCHE\1 la gorge ! Putain t'es pas"
  "la moiti� d'un con toi !        " 
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'avance pour lui fracasser sa  "
 "jambe mais il sort un tanto et me "
"l'\2ENFONCE\1 dans le ventre. Il tourne "
 "et me regarde m'effondrer avec un "
  "regard sadique.                 "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai pas �t� assez rapide. J'ai "
 "pas bloqu� son couteau et il me   "
"\2TRANCHE\1 la gorge. Le sang gicle de  "
 "partout. Ma satisfaction c'est que"
  "sa soupe est immangeable !      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je le frappe � la jambe mais ce "
 "con me \2FRACASSE\1 son bol sur la    "
"t�te. Il utilise un morceau pour    "
 "me trancher la gorge. Tu l'avais  "
  "pas vu venir celle-l� ? Moi si !"
},
{
/*
"------------------------------------" 36 chars (Line 3) 106
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je m'approche pour l'�trangler  "
 "mais il sort son couteau et me    "
"\2TRANSPERCE\1 la t�te de haut en bas.  "
 "J'ai m�me pas eu le temps de te   "
  "dire que t'�tais un gros b�tard."
}

};

#define NBMESSSUCESS 6 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Un \2BON\1 coup de pied dans le     "
 "tabouret et il vole sur sa gueule "
"de con. Il l'a pas vu venir celui-l�"
 "il a m�me pas eu le temps de poser"
  "ses baguettes !                 "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Je lui jette une poign�e de"
 "farine qui tra�nait par l�. Il se "
"prend le nuage dans les yeux. Il    "
 "va lui falloir quelques secondes  "
  "pour voir clair � nouveau....   "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2ATTENTION\1 il va sortir le TANTO "
 "qu'il a cach� sous sa tenue. Je   "
"dois agir vite. Pr�pare-toi pour la "
 "s�quence...                       "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. J'ai juste eu le temps de"
 "lui bloquer la main avant qu'il ne"
"sorte son couteau. Je pense que je  "
 "peux le fracasser maintenant.     "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BLAM\1. Un bon gros coup dans sa  "
 "face de pet. Celui-l� il va       "
"retourner fister sa grand-m�re.     "
 "Il est pas pr�t de remanger solide"
  "avec les dents que j'ai p�t�es. "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Allez, on va chercher le passage"
 "secret pour descendre. Les petites"
"bites ont toujours des cachettes    "
 "pour se planquer. J'ouvre le      "
  "panneau et �a doit �tre l�...   "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0x39, 0x7c, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xbc, 0x62, 0x36, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0x39, 0xc0, 0x91, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0x62, 0xc0, 0xc0, 0x7c, 0xfc, 0xa8,
	0x54, 0xbc, 0x62, 0x91, 0x91, 0x76, 0xfc, 0xa8,
	0x54, 0xbc, 0xc0, 0xc0, 0x62, 0x36, 0xfc, 0xa8,
	0x54, 0xb9, 0xc0, 0x91, 0xb9, 0x91, 0x7c, 0xa8,
	0x54, 0x39, 0x62, 0x36, 0x36, 0x62, 0x36, 0xa8,
	0x54, 0x33, 0xc0, 0x91, 0x39, 0x91, 0x91, 0xa8,
	0x14, 0x76, 0x62, 0x62, 0x62, 0xc0, 0x62, 0x28,
	0x11, 0x33, 0x39, 0xc0, 0x91, 0x91, 0x91, 0x22,
	0x00, 0x00, 0x0a, 0x0a, 0x0a, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x05, 0x25,
	0x00, 0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x05, 0x0f,
	0x00, 0x0a, 0x0a, 0x0a, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x00, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x05, 0x05, 0x05, 0x0f, 0x0f, 0x00, 0x05, 0x0f,
	0x00, 0x00, 0x05, 0x0a, 0x0a, 0x00, 0x00, 0x00,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x0f, 0x0f, 0x0f, 0x1a, 0x1a, 0x1a, 0x30,
	0x25, 0x25, 0x25, 0x25, 0x25, 0x30, 0x30, 0x30,
	0x0f, 0x0f, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x30,
	0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x30,
	0x0f, 0x0f, 0x1a, 0x1a, 0x1a, 0x1a, 0x30, 0x30,
	0x0f, 0x25, 0x25, 0x25, 0x25, 0x25, 0x30, 0x30,
	0x0f, 0x0f, 0x0f, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a,
	0x0f, 0x0f, 0x25, 0x25, 0x25, 0x25, 0x30, 0x30,
	0x0f, 0x0f, 0x0f, 0x0f, 0x1a, 0x1a, 0x1a, 0x30,
	0x0f, 0x0f, 0x0f, 0x25, 0x25, 0x25, 0x25, 0x25,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x0f, 0x0f, 0x0f, 0x0f, 0x05, 0x00, 0x00, 0x00,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x40, 0x36, 0x62, 0x62, 0x62, 0x36, 0x62, 0x22,
	0x11, 0x91, 0x91, 0x91, 0x91, 0x39, 0x91, 0xa8,
	0x11, 0x62, 0x62, 0x36, 0x33, 0x62, 0x36, 0x22,
	0x11, 0x91, 0x91, 0x39, 0x91, 0x91, 0xb9, 0xa8,
	0x40, 0xc0, 0x33, 0x36, 0x62, 0x62, 0x36, 0x22,
	0x40, 0xc0, 0x91, 0x91, 0x91, 0x91, 0x39, 0x22,
	0x40, 0x62, 0x33, 0x62, 0x62, 0x36, 0x33, 0x28,
	0x11, 0x91, 0x91, 0xb9, 0x39, 0x33, 0x36, 0xa8,
	0x14, 0x62, 0x36, 0x36, 0x33, 0x36, 0xfc, 0xa8,
	0x54, 0x39, 0x33, 0x33, 0x36, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0x3c, 0x3c, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x54, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xa8,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0f, 0xf0, 0x30, 0x30, 0x00, 0x5a, 0x30,
	0x5a, 0x30, 0x30, 0x30, 0x30, 0x00, 0xf5, 0x90,
	0xb0, 0x30, 0x30, 0x30, 0x30, 0x00, 0x35, 0x90,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x35, 0x90,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x70, 0x20,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x75, 0x90,
	0x75, 0xff, 0xff, 0xff, 0xff, 0x00, 0x35, 0x90,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0x35, 0x90,
	0x75, 0x75, 0xff, 0xff, 0xff, 0x00, 0x70, 0x20,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0x75, 0x90,
	0x75, 0x75, 0x75, 0x75, 0xff, 0x00, 0x35, 0x90,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0x05, 0xba,
	0x75, 0x75, 0xff, 0xff, 0xff, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0a, 0x0f, 0xf0, 0x70, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x75, 0x90, 0xea, 0x75,
	0xea, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x35, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0xb0, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0xea, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x35, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x35, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0xb0, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0xea, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x35, 0x90, 0x6a, 0x75, 0x90, 0xea, 0x75,
	0x6a, 0x35, 0x90, 0x6a, 0x35, 0x90, 0xea, 0x75,
	0x30, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0x00, 0x00, 0x50, 0xf0, 0x10, 0x6a, 0x6a, 0x6a,
	0x0a, 0x00, 0x50, 0xf0, 0x10, 0x30, 0x75, 0x75,
	0x00, 0x00, 0x50, 0xf0, 0x10, 0x6a, 0x6a, 0x6a,
	0x0a, 0x00, 0x10, 0xf0, 0x10, 0x30, 0xba, 0xff,
	0x0a, 0x00, 0x50, 0xf0, 0x10, 0x6a, 0x6a, 0x6a,
	0xa0, 0x00, 0x10, 0xb0, 0x10, 0x75, 0x75, 0x75,
	0xa0, 0x00, 0x50, 0x70, 0x10, 0x6a, 0x6a, 0x6a,
	0x20, 0x00, 0x10, 0xb0, 0x10, 0xba, 0xff, 0xff,
	0x20, 0x00, 0x10, 0x70, 0x10, 0x6a, 0x6a, 0x6a,
	0xa0, 0x00, 0x10, 0x30, 0x10, 0x75, 0x75, 0xff,
	0x0a, 0x00, 0x10, 0x70, 0x10, 0x6a, 0x6a, 0x6a,
	0x00, 0x00, 0x10, 0x30, 0x10, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x10, 0x30, 0x10, 0x6a, 0x6a, 0x6a,
	0x00, 0x00, 0x10, 0x30, 0x10, 0x7f, 0x7f, 0x7f,
	0x00, 0x00, 0x10, 0x70, 0x10, 0x6a, 0x6a, 0x6a,
	0x00, 0x00, 0x10, 0x30, 0x10, 0xbf, 0xbf, 0xaf,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0x00, 0x00,
	0x75, 0x75, 0x75, 0x75, 0xff, 0x00, 0x00, 0x00,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0x00, 0x00,
	0x75, 0x75, 0x75, 0x75, 0x75, 0x00, 0x05, 0x00,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0xb0, 0xa0,
	0x75, 0x75, 0xff, 0xff, 0xff, 0x00, 0x75, 0x25,
	0x35, 0x95, 0x95, 0x95, 0x95, 0x00, 0xa1, 0x50,
	0x75, 0xff, 0xff, 0xff, 0xff, 0x00, 0x75, 0x70,
	0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0xa1, 0xfa,
	0x75, 0xff, 0xff, 0xff, 0xff, 0x00, 0xf5, 0xaf,
	0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0x5a, 0xa0,
	0x35, 0xbf, 0xbf, 0xbf, 0xbf, 0x00, 0x00, 0x51,
	0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0x00, 0xa2,
	0x75, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x51,
	0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x00, 0x00, 0x00,
	0x5f, 0x75, 0xff, 0x3f, 0x7f, 0x00, 0x00, 0x00,
	0xb5, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x5f, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x75,
	0x55, 0x35, 0x90, 0x6a, 0x35, 0x90, 0xea, 0x75,
	0x10, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0x05, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x00, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x35,
	0x00, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x75,
	0x00, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0x05, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x10, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x35,
	0x10, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x35,
	0x10, 0x50, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0x50, 0x75, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x05, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x35,
	0xa2, 0x35, 0x90, 0x6a, 0x35, 0x90, 0x6a, 0x35,
	0x00, 0x5a, 0x20, 0xb0, 0x50, 0x20, 0xb0, 0x50,
	0x00, 0x00, 0x50, 0x70, 0x10, 0x30, 0x30, 0x5a,
	0x00, 0x00, 0x10, 0xb0, 0x10, 0xbf, 0xba, 0x1a,
	0x00, 0x00, 0x05, 0x25, 0x50, 0x30, 0x30, 0xb0,
	0x00, 0x00, 0x10, 0x1a, 0x00, 0x5a, 0x30, 0xb0,
	0x00, 0x00, 0x05, 0xa5, 0x00, 0x00, 0x5a, 0xb0,
	0x00, 0x00, 0x05, 0x00, 0x0f, 0x00, 0x00, 0xb0,
	0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x00, 0xf0,
	0x00, 0x00, 0x05, 0x0f, 0xb0, 0xf0, 0xa5, 0x1a,
	0x00, 0x00, 0x00, 0x0a, 0x5a, 0x5a, 0x70, 0x5a,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0x0f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0a, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0a, 0x0a, 0x0a, 0x0a, 0x00, 0x5a, 0x70, 0x0a,
	0x00, 0x05, 0x00, 0x00, 0x1a, 0xbf, 0x95, 0xba,
	0x30, 0xb0, 0x30, 0x75, 0x3a, 0x00, 0x00, 0x51,
	0x70, 0x30, 0x30, 0x30, 0xfa, 0x00, 0x00, 0x00,
	0x30, 0xb0, 0x30, 0x30, 0x25, 0x00, 0x00, 0x00,
	0x70, 0x1a, 0x30, 0x30, 0xa0, 0x00, 0x00, 0x00,
	0x25, 0xb0, 0x30, 0x30, 0x0a, 0x00, 0x00, 0x00,
	0x70, 0x5a, 0x30, 0x70, 0x00, 0x00, 0x00, 0x00,
	0x25, 0x1a, 0x30, 0x25, 0x00, 0x00, 0x00, 0x00,
	0xa5, 0x5a, 0x70, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x25, 0x1a, 0xb0, 0xa0, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x5a, 0x70, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x0f, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xa0, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x90, 0xea, 0x75, 0x90, 0xea, 0x75,
	0x00, 0x00, 0x5a, 0xb0, 0x75, 0x90, 0xea, 0x75,
	0x00, 0xa2, 0x00, 0x00, 0x0f, 0xf0, 0x30, 0x30,
	0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x51, 0x05, 0xb0, 0xa5, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xf2, 0x30, 0x30, 0x70, 0x0a,
	0x00, 0x00, 0x51, 0xb2, 0x75, 0x75, 0x30, 0x30,
	0x00, 0x00, 0x00, 0xf2, 0x30, 0xba, 0xba, 0xff,
	0x00, 0x00, 0x51, 0xb2, 0x30, 0x75, 0x75, 0xff,
	0x0a, 0x00, 0x0a, 0x05, 0xbf, 0xc0, 0x6a, 0x95,
	0x05, 0x05, 0x00, 0x5f, 0x6a, 0x3f, 0x95, 0xc0,
	0x00, 0x00, 0x00, 0x35, 0x95, 0x6a, 0x6a, 0xc0,
	0x05, 0x05, 0x00, 0xea, 0x95, 0x95, 0x95, 0xc0,
	0x0a, 0x0a, 0x0a, 0x6a, 0x42, 0x42, 0xc0, 0xc0,
	0x0f, 0x0f, 0x05, 0xea, 0xab, 0x81, 0x81, 0xc0,
	0x5a, 0x5a, 0x0f, 0x60, 0x12, 0xea, 0xea, 0x2b,
	0x0f, 0xf0, 0xf0, 0xb5, 0xd5, 0x30, 0x30, 0xbf,
	0x5a, 0x5a, 0xf0, 0x5f, 0x6a, 0x95, 0x3f, 0xc0,
	0x0f, 0xa5, 0xa5, 0x50, 0xbf, 0xc0, 0xc0, 0x95,
	0x0f, 0x5a, 0xa5, 0x05, 0xf5, 0x6a, 0xc0, 0x7f,
	0x0f, 0xf0, 0xf0, 0x0a, 0x5a, 0xbf, 0x95, 0xaf,
	0x05, 0x5a, 0x5a, 0xa5, 0x00, 0x5f, 0x57, 0x50,
	0x00, 0x0f, 0xb0, 0xf0, 0xa5, 0x05, 0x05, 0xf0,
	0x05, 0x05, 0x5a, 0x70, 0x30, 0xf0, 0xf0, 0xb0,
	0x50, 0x0a, 0x0f, 0xb0, 0x70, 0x70, 0x70, 0x70,
	0xfa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x95, 0x0a, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0xa5, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00,
	0xc0, 0x20, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x95, 0x25, 0x00, 0xa2, 0x00, 0x00, 0x00, 0x00,
	0x7f, 0xa5, 0x51, 0x51, 0x00, 0x00, 0x00, 0x00,
	0x3a, 0xa5, 0x51, 0xf2, 0x00, 0x71, 0x00, 0x00,
	0xfa, 0x0a, 0x51, 0xf4, 0x00, 0xf4, 0x00, 0x00,
	0x0f, 0x25, 0x00, 0x5a, 0x70, 0x00, 0x00, 0x00,
	0xb0, 0xa0, 0x51, 0x1a, 0x57, 0x70, 0x00, 0x51,
	0x70, 0x0a, 0x00, 0x21, 0x3f, 0x12, 0xf1, 0xf2,
	0xb0, 0x00, 0x00, 0x1f, 0xc0, 0x7f, 0x74, 0xf4,
	0x30, 0x0a, 0x00, 0x5f, 0x6a, 0x81, 0x70, 0xf8,
	0xb0, 0x25, 0x00, 0x1a, 0xbf, 0x81, 0x74, 0xfc,
	0x30, 0x30, 0x0a, 0x10, 0x75, 0x2b, 0x70, 0xf0,
	0x00, 0x00, 0xa2, 0xf2, 0x30, 0x30, 0xba, 0xff,
	0x00, 0x00, 0x51, 0xb2, 0x75, 0x75, 0x75, 0xff,
	0x00, 0x00, 0xa2, 0xf2, 0x30, 0x30, 0xba, 0xff,
	0x00, 0x00, 0x51, 0xa7, 0xb0, 0x30, 0x75, 0x75,
	0x00, 0x00, 0xa2, 0xa7, 0xf0, 0x30, 0xba, 0xff,
	0x00, 0x51, 0x51, 0xa2, 0xf0, 0xb0, 0x30, 0x75,
	0x00, 0x00, 0xf3, 0xa2, 0x5a, 0xba, 0x30, 0xba,
	0x00, 0x51, 0xf3, 0xa2, 0x5f, 0xf5, 0x30, 0x75,
	0x00, 0xf3, 0xf1, 0xa2, 0x50, 0xff, 0xba, 0xba,
	0x51, 0xf0, 0xf2, 0xa2, 0x55, 0xf5, 0x7a, 0x75,
	0xf0, 0xf4, 0xf1, 0x05, 0x50, 0xff, 0xfa, 0x30,
	0xf0, 0xf8, 0xf2, 0xf2, 0x0f, 0x5f, 0x2f, 0x75,
	0xf4, 0xf4, 0xf1, 0x10, 0x5a, 0x35, 0xaf, 0x30,
	0xfc, 0xf8, 0x71, 0x10, 0xa5, 0x75, 0x2a, 0x75,
	0xf0, 0xf0, 0xf6, 0xb2, 0x25, 0x35, 0xaa, 0x30,
	0x30, 0x30, 0x12, 0xb0, 0x25, 0x35, 0x2a, 0x30,
	0x50, 0xa5, 0x05, 0x5a, 0x30, 0xb0, 0xb0, 0x30,
	0x50, 0xf0, 0x00, 0x0a, 0xb0, 0x30, 0x30, 0x30,
	0x50, 0xf5, 0x0a, 0x05, 0x50, 0x30, 0x30, 0x30,
	0x50, 0xf5, 0xa0, 0x00, 0x0a, 0x1a, 0xba, 0x30,
	0x50, 0xb5, 0xa0, 0x00, 0x05, 0x05, 0x30, 0x30,
	0x05, 0xf5, 0xa0, 0x00, 0x00, 0x0a, 0x1a, 0xba,
	0x50, 0x5a, 0x0a, 0x00, 0x00, 0x05, 0x05, 0x75,
	0x05, 0xf5, 0xa0, 0x00, 0x00, 0x00, 0x0a, 0xb0,
	0x05, 0x5a, 0x0a, 0x00, 0x00, 0x00, 0x05, 0x5a,
	0x05, 0x5f, 0xa0, 0x05, 0x05, 0x00, 0x00, 0x0f,
	0x05, 0x5a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x50,
	0x05, 0x5a, 0x0a, 0x05, 0x05, 0x05, 0x00, 0x05,
	0x05, 0x05, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x0a, 0x0a, 0x05, 0x05, 0x05, 0x05, 0x00,
	0x0f, 0x05, 0x00, 0x5a, 0x0a, 0x0a, 0x0a, 0x00,
	0x70, 0x0a, 0x00, 0xf0, 0x25, 0x05, 0x05, 0x05,
	0x30, 0x30, 0x30, 0x75, 0x30, 0x03, 0x75, 0x75,
	0xba, 0xba, 0xba, 0xba, 0xba, 0x2b, 0xba, 0xba,
	0x75, 0x30, 0x75, 0xff, 0xff, 0x03, 0x57, 0x75,
	0x30, 0xba, 0xab, 0xab, 0xab, 0x03, 0xba, 0xba,
	0x30, 0x75, 0x75, 0x75, 0x57, 0x57, 0x75, 0x75,
	0x75, 0x30, 0xba, 0xab, 0xba, 0xba, 0xba, 0xbf,
	0x30, 0x30, 0x75, 0x75, 0x75, 0x75, 0x75, 0x6a,
	0xba, 0x75, 0x30, 0xba, 0xba, 0xba, 0xbf, 0x95,
	0x75, 0x30, 0xba, 0x75, 0x75, 0x75, 0x6a, 0x6a,
	0xb0, 0xba, 0x75, 0x75, 0xba, 0x21, 0x95, 0x81,
	0x5a, 0x75, 0x30, 0xff, 0x30, 0x42, 0xc0, 0x7f,
	0xa5, 0xb0, 0xba, 0xba, 0x21, 0x6a, 0x95, 0x12,
	0x5a, 0xf0, 0x75, 0x75, 0x35, 0xc0, 0x2b, 0xba,
	0x05, 0xf0, 0xb0, 0x30, 0xbf, 0x95, 0x12, 0xba,
	0x00, 0x5a, 0x5a, 0x0f, 0x35, 0x2b, 0x1a, 0xba,
	0x05, 0x10, 0x05, 0x5a, 0x35, 0x12, 0x1a, 0xba,
	0x75, 0x75, 0x57, 0xb0, 0x70, 0x35, 0xaa, 0x30,
	0xba, 0x17, 0x57, 0xf2, 0x30, 0x35, 0xaa, 0x30,
	0x75, 0x2b, 0x12, 0x51, 0xb0, 0x35, 0x20, 0x30,
	0x17, 0x17, 0xfb, 0xf2, 0xf2, 0x35, 0x25, 0x30,
	0x3f, 0x7f, 0xf9, 0xb2, 0xf1, 0xf5, 0x25, 0x30,
	0x95, 0xba, 0xf3, 0xf3, 0xfa, 0xbf, 0x70, 0x30,
	0x3f, 0x71, 0xf6, 0xf3, 0x7f, 0x7f, 0x70, 0x75,
	0x7f, 0x51, 0xf9, 0xf9, 0x3f, 0xbf, 0x30, 0x30,
	0xaa, 0x00, 0xf6, 0xfc, 0x7f, 0x3f, 0x70, 0x75,
	0x0a, 0x00, 0xf9, 0xb8, 0xba, 0xbf, 0x30, 0xba,
	0x00, 0x00, 0xf6, 0xfc, 0x7f, 0xbf, 0x30, 0x75,
	0xa2, 0x00, 0xfc, 0xb8, 0xba, 0xea, 0x30, 0xba,
	0xa2, 0x00, 0xf6, 0x74, 0x30, 0xbf, 0x75, 0xff,
	0xa0, 0x00, 0xb8, 0xb8, 0xba, 0x6a, 0x30, 0xba,
	0xa0, 0x00, 0xf6, 0x30, 0x30, 0xea, 0x75, 0xff,
	0x20, 0x51, 0xb8, 0xb8, 0x30, 0x6a, 0xff, 0xff,
	0x30, 0xa0, 0x00, 0xb0, 0x70, 0x0a, 0x0a, 0x0f,
	0xba, 0x70, 0x0a, 0x1a, 0x30, 0xa5, 0x0f, 0xa5,
	0x57, 0xba, 0x25, 0x10, 0xba, 0x25, 0x5a, 0x5a,
	0xab, 0x7f, 0x70, 0x50, 0x75, 0x25, 0x0f, 0xb0,
	0x17, 0xba, 0xba, 0x0a, 0xb0, 0x3a, 0x5a, 0xf0,
	0x7f, 0x75, 0x30, 0xa0, 0x1a, 0xbf, 0x25, 0xb0,
	0xba, 0xba, 0x30, 0x25, 0xf0, 0x75, 0x3a, 0x5a,
	0x75, 0x30, 0x70, 0x30, 0x70, 0x30, 0x95, 0x25,
	0x10, 0x30, 0xb0, 0x30, 0xba, 0xb0, 0xea, 0x3a,
	0x50, 0x70, 0x30, 0x75, 0x75, 0x30, 0x75, 0x95,
	0x05, 0xf0, 0x30, 0xba, 0xba, 0xba, 0x30, 0xea,
	0x05, 0xb0, 0x75, 0x75, 0x7f, 0x75, 0x30, 0x75,
	0x50, 0x30, 0xba, 0xbf, 0xbf, 0xba, 0xba, 0x70,
	0x1a, 0x75, 0x75, 0x7f, 0x7f, 0x75, 0x75, 0x30,
	0xb0, 0xba, 0xbf, 0xbf, 0xba, 0xbf, 0xba, 0xba,
	0x75, 0x75, 0x7f, 0x7f, 0x75, 0x7f, 0x7f, 0x75,
	0x0f, 0x55, 0x05, 0xb0, 0x21, 0xba, 0x55, 0x3a,
	0xa5, 0x10, 0x05, 0x30, 0x75, 0x30, 0x5f, 0xba,
	0x70, 0x05, 0x25, 0xbf, 0x30, 0x30, 0x1a, 0x3a,
	0xb0, 0x0a, 0x7a, 0x60, 0x30, 0x30, 0x75, 0x3a,
	0x30, 0xa5, 0x25, 0xbf, 0x70, 0x30, 0x75, 0x3a,
	0xb0, 0x20, 0x5a, 0x60, 0x30, 0x30, 0x75, 0x3a,
	0x30, 0x25, 0x10, 0xbf, 0x70, 0x30, 0x75, 0x3a,
	0xb0, 0x20, 0x5a, 0xea, 0xba, 0x1a, 0x35, 0x3a,
	0x50, 0x25, 0x10, 0xea, 0x70, 0x05, 0x75, 0x3a,
	0x25, 0x20, 0x1a, 0xea, 0xba, 0xa0, 0x35, 0x3a,
	0x3a, 0xa5, 0x10, 0xea, 0x75, 0x70, 0x75, 0x3a,
	0x95, 0x70, 0x5f, 0xea, 0xbf, 0xba, 0x35, 0x90,
	0xea, 0x3a, 0xb0, 0xea, 0x75, 0x7f, 0x35, 0x90,
	0x5f, 0x95, 0x75, 0xea, 0xba, 0x95, 0xbf, 0xd5,
	0xa5, 0xea, 0xb0, 0xea, 0x7f, 0x7f, 0xea, 0x95,
	0x70, 0x5f, 0x75, 0x6a, 0xba, 0x95, 0xbf, 0xc0,
	0x20, 0x00, 0xf6, 0x30, 0x30, 0xea, 0x75, 0xff,
	0xa0, 0x51, 0xf9, 0x30, 0x30, 0x6a, 0xff, 0xbf,
	0xa0, 0x51, 0xf3, 0x30, 0x75, 0x6a, 0x7f, 0x7f,
	0xa0, 0x51, 0x51, 0x30, 0x30, 0xc0, 0xbf, 0xbf,
	0xf1, 0x00, 0xf3, 0x30, 0x75, 0x6a, 0x7f, 0x7f,
	0x71, 0x00, 0x50, 0x30, 0xbf, 0xc0, 0x95, 0xbf,
	0x70, 0x00, 0xf2, 0x30, 0x6a, 0x6a, 0x3f, 0x7f,
	0x70, 0x00, 0x10, 0x75, 0x95, 0xc0, 0x95, 0x3f,
	0xba, 0x00, 0xb2, 0x35, 0x6a, 0xc0, 0x6a, 0x3f,
	0x30, 0x51, 0xb2, 0xbf, 0xc0, 0xc0, 0x95, 0x95,
	0xba, 0x00, 0xb2, 0xea, 0x6a, 0xc0, 0x6a, 0x6a,
	0x3a, 0x51, 0xb0, 0x3f, 0xc0, 0xc0, 0xc0, 0x95,
	0xba, 0x00, 0xf5, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0,
	0x30, 0x51, 0xf5, 0xc0, 0xc0, 0xc0, 0x95, 0x95,
	0xfa, 0x51, 0x35, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0,
	0x7a, 0x51, 0x35, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xba, 0xbf, 0xbf, 0xba, 0xbf, 0xbf, 0xba, 0xba,
	0x75, 0x7f, 0x7f, 0x75, 0x7f, 0x7f, 0x75, 0x7f,
	0xbf, 0xbf, 0xba, 0xbf, 0xbf, 0xba, 0xbf, 0xbf,
	0x7f, 0x7f, 0x75, 0x7f, 0x7f, 0x75, 0x7f, 0x7f,
	0xbf, 0xba, 0xbf, 0xbf, 0xba, 0xbf, 0xbf, 0xba,
	0x75, 0x75, 0x7f, 0x7f, 0x75, 0x7f, 0x7f, 0x30,
	0xb0, 0xbf, 0xbf, 0xba, 0xbf, 0xbf, 0xba, 0x75,
	0x1a, 0x7f, 0x7f, 0x75, 0x7f, 0x7f, 0x30, 0xbf,
	0x50, 0xbf, 0xba, 0xbf, 0xbf, 0xba, 0x75, 0x3f,
	0x05, 0x75, 0x75, 0x7f, 0x7f, 0x30, 0xbf, 0x7f,
	0x00, 0xb0, 0xbf, 0xbf, 0xba, 0x75, 0x3f, 0x3f,
	0x00, 0x1a, 0x7f, 0x7f, 0x30, 0xbf, 0x7f, 0x7f,
	0x00, 0x50, 0xbf, 0xba, 0x75, 0x3f, 0x3f, 0xbf,
	0x00, 0x05, 0x75, 0x30, 0xbf, 0x3f, 0x7f, 0xff,
	0x00, 0x00, 0xb0, 0x75, 0x3f, 0xbf, 0xba, 0xba,
	0x05, 0x00, 0x1a, 0xbf, 0x7f, 0x7f, 0x75, 0xff,
	0xba, 0x50, 0x75, 0xea, 0x75, 0x6a, 0x6a, 0xc0,
	0x30, 0xa5, 0x75, 0x6a, 0x35, 0x95, 0x95, 0xc0,
	0xba, 0x20, 0x35, 0xea, 0x75, 0xc0, 0x6a, 0xc0,
	0x30, 0x25, 0x75, 0x6a, 0x30, 0xc0, 0x95, 0x95,
	0x75, 0x70, 0x35, 0x6a, 0x30, 0x6a, 0xc0, 0xd5,
	0xbf, 0x30, 0x75, 0x6a, 0xba, 0x95, 0x95, 0x90,
	0x3f, 0xba, 0x35, 0xc0, 0x7f, 0x3f, 0x6a, 0x90,
	0x6a, 0x7f, 0x75, 0x6a, 0x95, 0x7f, 0x6a, 0x90,
	0x3f, 0xbf, 0x35, 0xc0, 0x3f, 0xba, 0x6a, 0xd5,
	0x6a, 0x6a, 0xe0, 0x6a, 0x95, 0x2f, 0x6a, 0xd5,
	0x3f, 0x95, 0x35, 0xc0, 0x6a, 0x3a, 0xc0, 0x95,
	0x6a, 0xc0, 0xe0, 0xc0, 0xc0, 0xd5, 0x6a, 0x95,
	0x3f, 0x95, 0x35, 0xc0, 0x6a, 0x95, 0xc0, 0xc0,
	0x6a, 0x6a, 0xe0, 0xc0, 0xc0, 0xc0, 0x6a, 0xc0,
	0xbf, 0xc0, 0xe0, 0xc0, 0xc0, 0x95, 0xc0, 0xc0,
	0x6a, 0x6a, 0xe0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xd1, 0x50, 0xea, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0,
	0xd1, 0x50, 0xbf, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x2a, 0xb2, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xaa, 0xb2, 0x3f, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x20, 0xb0, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xa0, 0xb0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xa0, 0x30, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xa2, 0x30, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xa2, 0x30, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xa2, 0x30, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf1, 0x30, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf1, 0x30, 0xea, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x70, 0x30, 0xea, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xfa, 0x30, 0xea, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xba, 0x30, 0xea, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x3a, 0x30, 0xea, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0
};