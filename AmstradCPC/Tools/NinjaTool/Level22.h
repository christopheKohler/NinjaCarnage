// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL22.BIN"

// -------------------------------------------------------
#define NBZONES 4

u8 leveldata[2] = { 2 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	0,11,3,12,1, // 
	2,8,5,9,2, // 
	4,4,7,5,3, // 
	3,1,8,2,4, // 
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
u8 scenario[13][SEQLINESIZE] =
{
	{ 199, 1  , 101, 101, 101, 0  , 0  , 0 , 0 , 0 }, // 
	{ 251, 45, 2,  102 , 1  ,3  , 4  , 2  ,1 , 2}, // QTE
	{ 4  ,255   , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },

	{ 199, 0  , 1  , 101, 101, 0  , 0  , 0 , 0 , 0 }, // 
	{ 251, 45, 3,  103 , 3  ,4  , 3  , 4  ,1 , 4}, // QTE
	{ 3  ,2   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },

    { 199, 0  , 0  , 1,   101, 0  , 0  , 0 , 0 , 0 }, // 
	{ 251, 45, 4,  104 , 2  ,4  , 3  , 2  ,4 , 1    }, // QTE
	{ 2  ,1   , 2  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },

	{ 199, 0  , 0  , 0  , 1  , 0  , 0  , 0 , 0 , 0 }, // 
	{ 251, 45, 5,  105 , 4  ,1  , 4  , 3  ,2 , 4}, // QTE
	{ 1  ,2   , 1  , 255  , 0  ,0  , 0  , 0 , 0 , 0 },

	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END

};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL23 BIN"; // Must be 11 characters

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
  "Putain �a \2GLISSE\1 ce toit pourri!"
 "Ils pourraient venir nettoyer la  "
"merde des pigeons de temps en temps."
 "Je vais devoir surveiller mon     "
  "�quilibre.                      "
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
  "Ben non \2CONNARD\1, c'est trop loin"
 "J'ai saut� et j'ai gliss�. Faut   "
"pas �tre bien malin pour voir qu'il "
 "faut y aller doucement l�. T'es   "
  "press� ou quoi babache ?        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102 QTE 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai perdu mon �quilibre � cause"
 "de toi. J'ai rebondi trois fois   "
"avant de toucher le sol. Mon corps  "
 "est pas beau � voir chiotte. Un si"
  "beau gosse ! \2RECOMMENCE\1.        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103 QTE 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai perdu mon �quilibre ! Je   "
 "suis \2TOMBE\1 devant la porte. J'ai  "
"�cras� un garde, mais son pote m'a  "
 "descendu. Va falloir faire gaffe  "
  "on veut avancer mon boulet.     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104 QTE 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai rat� mon saut et je suis   "
 "tomb� direct la t�te sur le sol.  "
"Je suis \2MORT\1, par ta faute abruti.  "
 "Je savais que �a se passerait mal."
  "J'aurai d� rester couch�.       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105 QTE 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BOUM\1, je me fracasse sur le sol "
 "A cause de toi j'ai perdu mon     "
"�quilibre. C'est pourtant pas dur   "
 "d'appuyer sur des boutons bordel !"
  "T'as �t� fini � la pisse ?      "
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
  "                                "
 "             \2ATTENTION\1            "
"                                    "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2YES\1. C'�tait juste mais j'ai pu "
 "passer sur ce bout d'toit. Au fait"
"c�est le gang Yakusa des \2Toa YuaiKai\1"
 "qu�on va se faire l�. Tout un tas "
  "de m�chants gar�ons.            "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUIII\1. Ca glisse mais j'avance. "
 "Ces cons rackettent et vendent    "
"de la drogue. Ca sera pas une grosse"
 "perte une fois que je les aurai   "
  "tous fum�s ces connards.        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. J'ai fait plus de la      "
 "moiti�. Je t'ai dit que j'avais   "
"la couille gauche qui me grattait ? "
 "Et toi �a va, t'as pas le cul qui "
  "te gratte sur ta chaise ?       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Je saute enfin sur le    "
 "haut du toit. On va pouvoir se    "
"faufiler par une ouverture.         "
 "mais attends, c'est quoi �a ??????"
  "                                "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x33, 0x33,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xc9,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x67,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x66, 0x8a,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x66, 0x86, 0x00,
	0x91, 0x91, 0x91, 0x91, 0x91, 0xcd, 0x0c, 0x00,
	0x62, 0x62, 0x62, 0x62, 0x63, 0xcf, 0x0c, 0x00,
	0x91, 0x91, 0x91, 0x91, 0xc7, 0x8f, 0x0c, 0x00,
	0x62, 0x62, 0x33, 0xc9, 0xcf, 0x45, 0x0c, 0x00,
	0x33, 0x33, 0xc9, 0xcf, 0x8f, 0xcf, 0x87, 0x45,
	0x91, 0x63, 0xcf, 0xe5, 0xe5, 0xa0, 0x0d, 0xce,
	0x66, 0xc7, 0xe5, 0xda, 0x0a, 0x05, 0x0a, 0x99,
	0xc9, 0xcf, 0xda, 0x0a, 0x00, 0x5a, 0x00, 0x62,
	0xcf, 0xda, 0x0a, 0x00, 0x50, 0x20, 0x00, 0x9b,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x66, 0x62, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x06, 0x99, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x0c, 0x99, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x0c, 0x13, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x0c, 0x13, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x0c, 0x13, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x0c, 0x99, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x49, 0x99, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x4e, 0x33, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x99, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x91, 0xc4, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x33, 0xc9, 0x99, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x66, 0x0e, 0x42, 0x62, 0x62, 0x62, 0x62, 0x62,
	0xcd, 0xa4, 0x13, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x66, 0xc7,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x66, 0xc7, 0xda,
	0x36, 0x36, 0x36, 0x36, 0x66, 0xcd, 0xa5, 0x45,
	0x33, 0x33, 0x33, 0x66, 0xc9, 0xda, 0xcf, 0x61,
	0x39, 0x39, 0x6c, 0xc9, 0xcf, 0xe5, 0xa5, 0xb0,
	0x66, 0x66, 0xc9, 0xda, 0xcf, 0xda, 0x00, 0x9a,
	0xcc, 0xc9, 0xda, 0x45, 0xda, 0x0a, 0x00, 0xcb,
	0xc9, 0xda, 0xcf, 0xda, 0x0a, 0x00, 0x05, 0x61,
	0x0a, 0xe5, 0xda, 0x0a, 0x00, 0x00, 0x00, 0x64,
	0x50, 0xda, 0x0a, 0x00, 0x00, 0x00, 0x05, 0xc6,
	0xa0, 0xa0, 0x00, 0x00, 0x00, 0x0a, 0x0a, 0xc6,
	0x05, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0xcf,
	0x0a, 0x00, 0x00, 0x0a, 0x0a, 0x0a, 0x0f, 0xcb,
	0x00, 0x00, 0x05, 0x05, 0x05, 0x8f, 0x8f, 0x61,
	0x00, 0x0a, 0x0a, 0x0f, 0x0f, 0x4f, 0xe5, 0xc6,
	0x05, 0x05, 0x05, 0x0f, 0x8f, 0xda, 0xda, 0xc6,
	0xa5, 0x00, 0x45, 0x00, 0x0a, 0x00, 0x00, 0x44,
	0x00, 0xcb, 0x6b, 0x25, 0x00, 0x0f, 0x00, 0x45,
	0x61, 0x6e, 0x30, 0xc7, 0x0a, 0x0f, 0x00, 0x0f,
	0x3a, 0x30, 0x61, 0x25, 0x8a, 0x00, 0x00, 0x0f,
	0xc6, 0x98, 0x35, 0x82, 0x00, 0x0a, 0x00, 0x45,
	0xcc, 0x64, 0x61, 0x25, 0x25, 0x0d, 0x00, 0x00,
	0xcc, 0x98, 0x35, 0xc7, 0x00, 0x00, 0x00, 0x45,
	0xcc, 0x64, 0x61, 0x20, 0x00, 0x0e, 0x0a, 0xce,
	0x98, 0x98, 0x61, 0x0a, 0x1a, 0x1d, 0x0a, 0x31,
	0xcc, 0x65, 0x61, 0xcf, 0x0f, 0x1d, 0x0a, 0x14,
	0x65, 0x10, 0x30, 0x45, 0x0a, 0x0e, 0x08, 0x11,
	0xcf, 0x64, 0x61, 0xcd, 0x0f, 0x0e, 0x2a, 0x14,
	0x98, 0x98, 0x30, 0xc6, 0x0d, 0x1d, 0x82, 0x11,
	0xcc, 0x65, 0x30, 0x65, 0x25, 0x0e, 0x86, 0x45,
	0x98, 0xcd, 0x30, 0x1a, 0x1a, 0x0a, 0x05, 0x45,
	0x65, 0x8a, 0x35, 0x61, 0x25, 0x8f, 0x0a, 0x25,
	0x8f, 0x0c, 0x13, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x50, 0x0c, 0x13, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x50, 0x0c, 0x9c, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x05, 0x49, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x00, 0x4e, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
	0x45, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x9c, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
	0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
	0xcd, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x3a, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
	0xc7, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39,
	0x0f, 0x0f, 0x0f, 0x4f, 0xe5, 0xe5, 0xe5, 0x45,
	0xa5, 0x8f, 0x8f, 0xda, 0xda, 0xcf, 0x8a, 0x8a,
	0x4f, 0x4f, 0xe5, 0xe5, 0xe5, 0xcf, 0x45, 0xcf,
	0xda, 0xda, 0xda, 0xcf, 0xcf, 0x8a, 0xa0, 0x8a,
	0xe5, 0xe5, 0xe5, 0xcf, 0xcf, 0x45, 0x50, 0x50,
	0xcf, 0xcf, 0xcf, 0x8a, 0xda, 0x8a, 0xa0, 0x0a,
	0xcf, 0xcf, 0xcf, 0x45, 0x45, 0x50, 0x05, 0x00,
	0xcf, 0xda, 0x8a, 0x8a, 0xa0, 0xa0, 0x0a, 0x05,
	0xe5, 0xe5, 0x50, 0x50, 0x05, 0x05, 0x00, 0x97,
	0xda, 0x8a, 0xa0, 0x8a, 0x0a, 0x0a, 0x45, 0xcc,
	0xe5, 0x50, 0x45, 0x05, 0x00, 0x00, 0x44, 0xce,
	0x8a, 0x8a, 0xa0, 0x8a, 0x00, 0x00, 0x45, 0xcd,
	0x45, 0xcf, 0x45, 0x00, 0x00, 0x00, 0x44, 0xce,
	0x8a, 0x8a, 0x0a, 0x00, 0x00, 0x05, 0x45, 0xcf,
	0x05, 0x50, 0x00, 0x00, 0x00, 0x0a, 0x5a, 0xcf,
	0xa0, 0x00, 0x00, 0x05, 0x05, 0x50, 0x45, 0xe5,
	0x45, 0x4f, 0x4b, 0x6b, 0x1a, 0x1a, 0x0f, 0x4f,
	0x8f, 0x8a, 0x1a, 0x92, 0x25, 0x30, 0x30, 0x30,
	0x4f, 0x45, 0x0f, 0x1a, 0x1a, 0x30, 0xce, 0x30,
	0x8a, 0x8a, 0x0f, 0x25, 0x25, 0x30, 0x25, 0x6b,
	0x50, 0x45, 0x1a, 0x4b, 0x3a, 0x1a, 0x30, 0x30,
	0x8a, 0x8f, 0x0f, 0x20, 0x8f, 0x30, 0xcd, 0x97,
	0x0f, 0x1a, 0x1a, 0x1a, 0x1a, 0x3a, 0x64, 0x3a,
	0x61, 0xc9, 0x25, 0x25, 0x92, 0x8f, 0x61, 0x65,
	0xcc, 0xc6, 0x92, 0x1a, 0x1a, 0x92, 0x30, 0x35,
	0xcd, 0x8f, 0x6b, 0x0f, 0x25, 0x25, 0x35, 0x98,
	0xce, 0x30, 0x1a, 0x1a, 0x0f, 0x1f, 0x3a, 0x64,
	0xcc, 0x8d, 0x0f, 0x0f, 0x25, 0x3f, 0x65, 0xcc,
	0xcc, 0x98, 0x1a, 0x1f, 0x30, 0x30, 0x64, 0xcc,
	0xcd, 0x8f, 0x25, 0x8f, 0x25, 0xcc, 0x92, 0xcb,
	0xcf, 0x10, 0x0a, 0x1a, 0x1f, 0x65, 0xcb, 0xcc,
	0xcf, 0x98, 0x25, 0x25, 0x65, 0xc6, 0xcc, 0x9d,
	0x30, 0xbc, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
	0x25, 0x34, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x30, 0x74, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
	0x61, 0x8d, 0xb9, 0x39, 0x39, 0x39, 0x39, 0x39,
	0x97, 0x25, 0x5e, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c,
	0xcd, 0x8d, 0x50, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0x6e, 0x07, 0x00, 0xf4, 0x3c, 0x3c, 0x3c, 0x3c,
	0xcc, 0x02, 0x00, 0x50, 0x7c, 0x7c, 0x7c, 0x3c,
	0xcc, 0x87, 0x05, 0x00, 0xbc, 0x3c, 0x3c, 0x3c,
	0x46, 0x82, 0x00, 0x0a, 0xb4, 0xbc, 0xbc, 0xbc,
	0xcc, 0x20, 0x05, 0x00, 0x1e, 0x3c, 0x3c, 0x3c,
	0xcc, 0x20, 0x00, 0x0a, 0x54, 0x7c, 0x7c, 0x7c,
	0xcc, 0x0a, 0x05, 0x00, 0x14, 0xbc, 0xbc, 0xbc,
	0x46, 0x0a, 0x00, 0x0a, 0x54, 0x7c, 0x7c, 0x7c,
	0x2b, 0x00, 0x00, 0x05, 0x54, 0xbc, 0xbc, 0xbc,
	0xc9, 0x0a, 0x00, 0x0a, 0x50, 0x7c, 0x7c, 0x7c,
	0x00, 0x00, 0x00, 0x0a, 0x0a, 0xa0, 0x5a, 0xcf,
	0x00, 0x05, 0x05, 0x05, 0x50, 0x50, 0x4f, 0xe5,
	0x0a, 0xa0, 0x0a, 0xa0, 0xe5, 0xe5, 0xf0, 0xcf,
	0x50, 0x5a, 0x50, 0x50, 0xda, 0xda, 0xe5, 0xe5,
	0xa5, 0xa5, 0xa0, 0xe5, 0xe5, 0xf0, 0xf0, 0xa5,
	0xda, 0xda, 0xda, 0xda, 0xda, 0x8f, 0xa5, 0x00,
	0xf0, 0xe5, 0xe5, 0xe5, 0xcf, 0x8f, 0x0a, 0x00,
	0xda, 0xda, 0xda, 0xcf, 0xcf, 0x8f, 0x00, 0x00,
	0xe5, 0xe5, 0xcf, 0xcf, 0xcf, 0x8f, 0x00, 0x00,
	0xda, 0xcf, 0xcf, 0xcf, 0xcf, 0x8f, 0x00, 0x05,
	0xcf, 0xe5, 0xcf, 0xcf, 0xcf, 0xda, 0x00, 0x4f,
	0xda, 0xcf, 0xcf, 0xcf, 0xcf, 0xda, 0x0f, 0xcf,
	0xcf, 0xe5, 0xcf, 0xcf, 0xcf, 0xcf, 0x4f, 0x64,
	0xda, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf,
	0xcf, 0xe5, 0xcf, 0xc7, 0xcd, 0xcd, 0xcd, 0xcc,
	0xda, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf,
	0xcf, 0x6b, 0x0a, 0x1a, 0xc6, 0xcc, 0xc9, 0xcf,
	0xcf, 0x9d, 0x0f, 0x3a, 0xc9, 0xcf, 0xcf, 0x9d,
	0xcf, 0x35, 0x0a, 0x4f, 0x9a, 0xc6, 0x6e, 0x6e,
	0xa5, 0x4b, 0x0f, 0xce, 0xcc, 0x9d, 0x9d, 0x9d,
	0x00, 0x1a, 0x1a, 0xcf, 0x9a, 0x4f, 0xcf, 0x30,
	0x00, 0x05, 0x0f, 0x61, 0x9d, 0x9d, 0x9d, 0xcc,
	0x00, 0x05, 0x1a, 0x64, 0x6e, 0x64, 0x6e, 0x6e,
	0x00, 0x00, 0x0f, 0x8a, 0xcf, 0x8b, 0x89, 0x89,
	0x05, 0x1a, 0x35, 0x9f, 0x64, 0x3a, 0x17, 0x6b,
	0x61, 0x3a, 0x9d, 0xce, 0xcc, 0xcd, 0xcc, 0x3a,
	0xcf, 0xcf, 0xce, 0x65, 0x9a, 0x9a, 0xce, 0x98,
	0x9d, 0x3f, 0xce, 0x3f, 0x3a, 0x98, 0x3a, 0x07,
	0xcf, 0xcf, 0x45, 0x44, 0xcd, 0x6e, 0x9d, 0x87,
	0xcc, 0x9d, 0xcc, 0x98, 0x9a, 0x8f, 0x35, 0x20,
	0x64, 0x64, 0xce, 0xcc, 0x3f, 0x6e, 0x0b, 0x20,
	0x98, 0x9a, 0xcd, 0x30, 0x61, 0x2b, 0x6b, 0x0a,
	0x30, 0x00, 0x00, 0x05, 0x50, 0xbc, 0xbc, 0xfc,
	0x3a, 0x00, 0x00, 0x00, 0x05, 0xfc, 0xfc, 0xfc,
	0x2f, 0x00, 0x00, 0x05, 0x05, 0x7c, 0x7c, 0x7c,
	0x07, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0xfc,
	0x82, 0x00, 0x00, 0x05, 0x00, 0xbc, 0xbc, 0xbc,
	0x20, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0xfc,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0xf4, 0x7c, 0x7c,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xfc, 0xfc,
	0x00, 0x0a, 0x00, 0x00, 0x00, 0x5a, 0xf8, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0xfc, 0xfc,
	0x00, 0x0a, 0x00, 0x00, 0x00, 0x54, 0xf4, 0xf4,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x5e, 0xfc, 0xfc,
	0x00, 0x0a, 0x00, 0x00, 0x00, 0x50, 0xf8, 0xf8,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x50, 0xfc, 0xfc,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0xf4, 0xf4,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0xf8, 0xf8,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xce, 0xce, 0xcc,
	0xda, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x8a,
	0xcf, 0xcf, 0xcb, 0xce, 0xce, 0xce, 0xce, 0xcc,
	0xcf, 0xcf, 0xcf, 0xcf, 0xc7, 0xc7, 0xc7, 0x92,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf,
	0xcf, 0xcf, 0xcb, 0xce, 0xce, 0xce, 0x98, 0x30,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf,
	0xcf, 0xc7, 0xc7, 0xcf, 0xcd, 0xcd, 0x9d, 0x3f,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x4f, 0x4f,
	0xcf, 0xcb, 0xce, 0xce, 0xce, 0xce, 0x9d, 0x98,
	0xcf, 0xcf, 0xcf, 0xcf, 0xc7, 0xc7, 0x64, 0x64,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x9a, 0x30, 0x30,
	0xc7, 0xc7, 0xcd, 0xcd, 0xcd, 0x9d, 0x3f, 0x3f,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x30, 0x30,
	0xcb, 0xce, 0xce, 0xce, 0xcc, 0x3f, 0x6e, 0xc9,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x61, 0x9d,
	0xcc, 0xcc, 0x3f, 0x3f, 0x97, 0x17, 0x3a, 0x0a,
	0xcf, 0x9a, 0x30, 0xcc, 0xcc, 0x3f, 0x98, 0x00,
	0x3f, 0x3f, 0xc9, 0x30, 0x9a, 0xcc, 0x07, 0x00,
	0x98, 0x9d, 0x3f, 0x3f, 0x6e, 0xce, 0x87, 0x00,
	0xcf, 0x64, 0x64, 0x3f, 0x3f, 0x6e, 0x20, 0x05,
	0xcc, 0x3f, 0x3f, 0x3a, 0xcc, 0x89, 0x20, 0x05,
	0x9a, 0x61, 0x3f, 0x3f, 0x3f, 0x6b, 0x0a, 0x05,
	0x9d, 0x3f, 0x98, 0xcc, 0x3f, 0x3a, 0x0a, 0x05,
	0x30, 0x97, 0x3f, 0x3a, 0xcc, 0x3a, 0x00, 0x00,
	0x98, 0x9a, 0x3f, 0x3f, 0x61, 0x8d, 0x00, 0x00,
	0x3f, 0x6e, 0x61, 0x3f, 0x3a, 0x07, 0x00, 0x05,
	0x30, 0x9d, 0x3f, 0x97, 0x3f, 0x82, 0x00, 0x04,
	0x6e, 0x64, 0x3f, 0x2b, 0x3f, 0x20, 0x00, 0x4b,
	0x9d, 0x3a, 0x35, 0x3f, 0x2b, 0x20, 0x00, 0x0c,
	0x30, 0x3f, 0x3f, 0x3f, 0x6b, 0x0a, 0x00, 0x49,
	0x3f, 0x30, 0x3f, 0x3f, 0x3a, 0x0a, 0x00, 0x49,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xf4,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x05, 0xf8, 0xf8,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4, 0xf4,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0xf4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xf0,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x50, 0xf8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xf0,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x54, 0xf4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xf0,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05, 0xf0,
	0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x05, 0xf0,
	0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf0,
	0x0a, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0xf0,
	0x0a, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x5a,
	0xc7, 0xc7, 0xc6, 0xcc, 0xcc, 0x6e, 0x92, 0x30,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0xcb, 0x9d, 0x3f,
	0xcf, 0xcd, 0xcd, 0xcc, 0xcc, 0xc7, 0xcf, 0x35,
	0xcb, 0xcb, 0xcf, 0xcf, 0x61, 0xcc, 0x3f, 0x9d,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x1a, 0x35, 0x3f,
	0xc6, 0xc6, 0xc6, 0xcc, 0xcc, 0x6e, 0x98, 0x9d,
	0xcf, 0xcf, 0xcf, 0xcf, 0xcf, 0x64, 0x3f, 0x35,
	0xc9, 0xcc, 0xcc, 0xcc, 0xcc, 0x98, 0x30, 0x6b,
	0xcf, 0xcf, 0xcf, 0xcf, 0xce, 0x9d, 0xc9, 0x35,
	0xcc, 0xcc, 0xcc, 0xcc, 0x98, 0x98, 0x3f, 0x3a,
	0xcf, 0xcf, 0x9a, 0x30, 0x35, 0x6e, 0x64, 0x3f,
	0xcc, 0xcc, 0xcc, 0x98, 0x30, 0x3f, 0x3a, 0x9d,
	0xcf, 0xcf, 0xcf, 0xce, 0x3f, 0x30, 0x3f, 0xcc,
	0xcc, 0xcc, 0x98, 0xcd, 0x30, 0x9d, 0x9d, 0x3f,
	0xcf, 0xcf, 0xce, 0x3f, 0x6b, 0x35, 0x3f, 0x3f,
	0xcf, 0xcf, 0xcd, 0x9a, 0x3f, 0x98, 0x9d, 0x35,
	0xcc, 0x6e, 0x97, 0x3f, 0x07, 0x00, 0x00, 0x4b,
	0x98, 0x3f, 0x6e, 0x3f, 0x87, 0x00, 0x00, 0x04,
	0x3f, 0x35, 0x3f, 0x3f, 0x20, 0x00, 0x05, 0x41,
	0x3f, 0x6e, 0x3f, 0x6b, 0x20, 0x00, 0x50, 0x04,
	0x3f, 0x3f, 0x9d, 0x3a, 0x00, 0x00, 0x05, 0x05,
	0x3f, 0x3f, 0x3f, 0x07, 0x00, 0x00, 0x50, 0x00,
	0x3f, 0x3f, 0x3f, 0x87, 0x00, 0x00, 0x0e, 0x0a,
	0x35, 0x3f, 0x3f, 0x20, 0x00, 0x00, 0x0e, 0xa0,
	0x6e, 0x3f, 0x2b, 0x0a, 0x00, 0x00, 0x0e, 0xa0,
	0x3f, 0x17, 0x6b, 0x00, 0x00, 0x00, 0x50, 0x0a,
	0x9d, 0x3f, 0x3a, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x2b, 0x3f, 0x07, 0x00, 0x00, 0x05, 0x50, 0x0a,
	0x3f, 0x3f, 0x82, 0x00, 0x00, 0x50, 0x05, 0xa0,
	0x9d, 0x3f, 0x20, 0x00, 0x00, 0x0e, 0x0a, 0x0a,
	0x3f, 0x6b, 0x0a, 0x00, 0x00, 0xb5, 0x05, 0x00,
	0x3f, 0x3a, 0x00, 0x00, 0x0a, 0xa4, 0x0a, 0x0a,
	0x08, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x5a,
	0x82, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x50,
	0x87, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x5a,
	0x87, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05,
	0x86, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05,
	0x49, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0xc3, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x05,
	0x49, 0x0a, 0x00, 0x00, 0x05, 0x0a, 0x00, 0x00,
	0x86, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00,
	0x49, 0x08, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00,
	0x0e, 0x0d, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00,
	0x0e, 0xa5, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0x04, 0x0d, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00,
	0x04, 0xa5, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0x05, 0x58, 0x0a, 0x00, 0x00, 0x05, 0x0a, 0x00,
	0x05, 0xa5, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00,
	0xcc, 0xcc, 0xcc, 0x98, 0x64, 0x3f, 0x64, 0x3f,
	0xcf, 0x9a, 0xcf, 0x98, 0x3a, 0x35, 0x3a, 0x3f,
	0xcc, 0xcc, 0x6e, 0x64, 0x35, 0x3a, 0x3f, 0x3f,
	0xcf, 0xcf, 0xcf, 0x9d, 0xcf, 0x9d, 0x9d, 0x3f,
	0xcc, 0xcc, 0x6e, 0x64, 0x9d, 0x64, 0x2b, 0x3f,
	0xcf, 0xcf, 0x35, 0x3a, 0x9d, 0x3f, 0x9d, 0x3f,
	0xcc, 0x9d, 0xcd, 0x35, 0x6e, 0x35, 0x35, 0x6b,
	0xcf, 0xcf, 0x9d, 0xcd, 0x9d, 0xcc, 0x3f, 0x12,
	0x6e, 0xcc, 0xcf, 0x35, 0x30, 0x3f, 0x3f, 0x87,
	0xcf, 0x9a, 0xcc, 0x98, 0x3f, 0x3f, 0x6b, 0x25,
	0x35, 0xcd, 0xce, 0x6e, 0x9f, 0x3f, 0x12, 0x00,
	0xcf, 0xce, 0xcc, 0x9d, 0x98, 0x3f, 0x87, 0x00,
	0xcc, 0x65, 0xce, 0x6e, 0xcc, 0x6b, 0x05, 0x05,
	0xcf, 0x9d, 0x3a, 0xcc, 0x9d, 0x82, 0x00, 0x25,
	0xcc, 0xce, 0xce, 0x64, 0x6b, 0x0a, 0x00, 0x00,
	0xcf, 0x9a, 0xcd, 0x9d, 0x87, 0x00, 0x00, 0x05,
	0x3f, 0x87, 0x00, 0x00, 0x00, 0x61, 0x0a, 0x00,
	0x3f, 0x20, 0x00, 0x00, 0x0a, 0x24, 0x0d, 0x00,
	0x6b, 0x0a, 0x00, 0x05, 0x00, 0x61, 0x0a, 0x0a,
	0x3a, 0x00, 0x00, 0x00, 0x0a, 0xb5, 0x0d, 0x00,
	0x87, 0x00, 0x00, 0x05, 0xa0, 0x1f, 0x0d, 0x0a,
	0x20, 0x00, 0x00, 0x50, 0x08, 0x41, 0x0d, 0x00,
	0x0a, 0x00, 0x00, 0x10, 0x08, 0x0e, 0x0f, 0x0a,
	0x00, 0x00, 0x00, 0x04, 0x08, 0x05, 0x0d, 0x00,
	0x0a, 0x00, 0x00, 0x04, 0x20, 0x10, 0x0f, 0x0a,
	0x0a, 0x00, 0x00, 0x04, 0xa0, 0x05, 0xa5, 0x00,
	0x0a, 0x00, 0x00, 0x10, 0x0a, 0x00, 0x0e, 0x0a,
	0x0a, 0x00, 0x05, 0x05, 0x00, 0x00, 0x05, 0x00,
	0x0a, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x05, 0x05, 0x00, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0f, 0x00,
	0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x5a, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x00,
	0x00, 0xa5, 0x0a, 0x00, 0x00, 0x05, 0x0a, 0x00,
	0x00, 0x0f, 0x0a, 0x00, 0x00, 0x00, 0x0f, 0x00,
	0x00, 0x05, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x00,
	0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00,
	0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05, 0x0a,
	0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x0a,
	0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0a,
	0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x05, 0x05, 0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x05, 0xa5, 0x00, 0x00, 0x00, 0x05,
	0x64, 0xcd, 0x64, 0xc9, 0x0a, 0x00, 0x05, 0xa0,
	0x98, 0xce, 0xcc, 0x87, 0x00, 0x50, 0xda, 0x00,
	0xcf, 0x6e, 0xc9, 0x20, 0x00, 0xe1, 0xa0, 0x00,
	0x98, 0xce, 0x92, 0x0a, 0x50, 0xd8, 0x00, 0x00,
	0x64, 0x98, 0x0f, 0x00, 0xe1, 0xa0, 0x00, 0x00,
	0xcc, 0x25, 0x0a, 0x50, 0x65, 0x00, 0x00, 0x00,
	0x3a, 0x00, 0x05, 0x1a, 0x8f, 0x00, 0x00, 0x00,
	0x25, 0x00, 0x0f, 0xcf, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x05, 0x0a, 0x8f, 0x00, 0x00, 0x00, 0x00,
	0x0f, 0x0f, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0xcf, 0x0a, 0x05, 0x05, 0x00, 0x00, 0x00, 0x00,
	0x8a, 0x05, 0x8a, 0xa0, 0x00, 0x00, 0x05, 0x00,
	0x00, 0x4f, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x8a, 0xa0, 0x0a, 0x00, 0x00, 0x00, 0x00,
	0x45, 0x50, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0a,
	0x8a, 0xa0, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x0f,
	0x00, 0x00, 0x0a, 0x05, 0x00, 0x00, 0x0a, 0x00,
	0x00, 0x05, 0x05, 0x00, 0x00, 0x05, 0x0a, 0x00,
	0x00, 0x0a, 0x0f, 0x05, 0x00, 0x05, 0xa0, 0x00,
	0x05, 0x05, 0x0f, 0x00, 0x0a, 0x50, 0x0f, 0x00,
	0x00, 0x5a, 0x0f, 0x05, 0x00, 0x05, 0x0d, 0x00,
	0x05, 0xa5, 0x0f, 0x00, 0x0a, 0x50, 0x5a, 0x0a,
	0x05, 0x5a, 0x0f, 0x00, 0x0a, 0x05, 0x0d, 0x00,
	0x05, 0xa5, 0xa5, 0x00, 0x0a, 0x0e, 0x0c, 0x0a,
	0x50, 0xf0, 0x0f, 0x0f, 0x05, 0x05, 0x0c, 0x0a,
	0x05, 0xb0, 0xa5, 0x0a, 0x0f, 0x0e, 0x0e, 0x0a,
	0x05, 0x58, 0x70, 0x0f, 0x05, 0xa4, 0x0d, 0x0a,
	0x05, 0xa4, 0xb0, 0xa5, 0x0f, 0x0e, 0x0e, 0x00,
	0x00, 0x5a, 0x58, 0x0f, 0x0f, 0xa5, 0x0c, 0x0a,
	0x05, 0xa5, 0xa4, 0xa5, 0x5a, 0xa4, 0x0d, 0x05,
	0x00, 0x5a, 0x5a, 0x5a, 0x0f, 0xa5, 0x0c, 0x00,
	0x05, 0x05, 0xa5, 0xa5, 0x5a, 0x5a, 0x0d, 0x05,
	0x00, 0x00, 0x00, 0x70, 0x0a, 0x00, 0x00, 0x05,
	0x00, 0x00, 0x05, 0xb0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x24, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xb0, 0x05, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0x0a, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x41, 0xa5, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x41, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x87, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x50, 0x0d, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x87, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x05, 0x58, 0x0a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x24, 0x05, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x58, 0x4f, 0x0a, 0x00,
	0x00, 0x00, 0x00, 0x0a, 0x0f, 0x05, 0x00, 0x00,
	0x00, 0x0a, 0x05, 0x8f, 0x0a, 0x0a, 0x0a, 0x00,
	0x05, 0x25, 0x00, 0x0f, 0x05, 0x00, 0x00, 0x00
};