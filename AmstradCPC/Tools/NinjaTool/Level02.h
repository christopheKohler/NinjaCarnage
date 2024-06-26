// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL02.BIN"

// -------------------------------------------------------
#define NBZONES 4

u8 leveldata[2] = { 2 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 bytes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	3,0,5,2,1, // Head
	7,1,11,3,2, // 1
	8,13,11,15,3, // Rock
	5,9,10,10,4, // Arrow
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
u8 scenario[11][SEQLINESIZE] =
{
	{ 199, 101, 1  , 101, 101,0  , 0  , 0 , 0 , 0 }, // Zone 1
	{ 250, 2  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 3  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text Choose Rock
	{ 199, 102, 0  , 4  , 102,0  , 0  , 0 , 0 , 0 }, // Zone 3
	{ 250, 5  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 6  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text ARROW
	{ 199, 103, 0  , 0  , 7  ,0  , 0  , 0 , 0 , 0 }, // Zone 4
	{ 251, 150, 8,  103 , 1  ,2  , 3  , 4  ,255 , 0}, // QTE
	{ 0  ,0   , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 250, 9  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END

	/*
	{ 50 ,101,102 ,102 ,102,1   ,102 , 0 , 0 , 0 },
	{ 50 ,101, 2  ,104 ,103 ,0  ,104 , 0 , 0 , 0 },
	{ 250,4  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 40 ,101, 0  ,3   ,103 ,0  ,104 , 0 , 0 , 0 },
	{ 250,6  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 251,25 , 1  , 1  , 2  ,2  , 0  , 0 , 0 , 0 },
	{ 0  ,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 40 ,101, 0  ,0   ,103 ,0  ,4   , 0 , 0 , 0 },
	{ 255,0  , 0  ,0   ,0   ,0  ,0   , 0 , 0 , 0 },
	*/
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL11 BIN"; // Must be 11 characters

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
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Bon on va v�rifier que tu aies  "
 "compris. D�place le curseur avec  "
"\2DIRECTIONS\1 jusqu'� la zone marqu�e 1"
 "et appuie sur \2ACTION\1 pour valider."
  "                                "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 3
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3)
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Oh merde tu as rat�. Je sens    "
 "qu�on va y passer la nuit...      "
"Appuie sur \2ACTION\1 et on va          "
 "recommencer. Concentre-toi bien   "
  "mon poulet, allez allez...      "
},
{
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "\2NON\1. J�ai dit le \2CAILLOU\1 putain!"
 "C�est petit un caillou, un peu    "
"comme ton cerveau, et tout dur, ah, "
 "tiens comme ton cerveau aussi !   "
  "Recommence.                     "
},
{
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "\2PERDU!\1 Pourtant c��tait pas dur "
 "bordel ! Ca promet. Allez on      "
"recommence tout. Tu commences �     "
 "piger qu�il va falloir y mettre du"
  "tien pour qu�on avance.         "
}

};

#define NBMESSSUCESS 9 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "\2BRAVO\1. Je sais pas trop ce qu�on"
 "fait dans ces cas l�, on donne un "
"\2SUSUCRE\1 ? Sur une image, tu peux    "
 "avoir jusqu'� 9 zones maximum.    "
  "                                "
},
{
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Tu vas devoir trouver l�ordre   "
 "logique pour que je puisse        "
"progresser, zone apr�s zone. Chaque "
 "erreur me m�nera � la \2MORT\1.       "
  "Ca craint grave �a.             "
},
{
//-------------------------------------" 38 chars 3 ROCK
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "A chaque mort, on recommencera  "
 "tout. Donc sois concentr�, sinon  "
"�a va durer des lustres cette merde."
 "Allez maintenant va actionner le  "
  "\2CAILLOU\1 ma couille.             "
},
{
//-------------------------------------" 38 chars 4
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "\2OK\1 tu as r�ussi Einstein. Je    "
 "sens que �a ne vas pas �tre une   "
"partie de plaisir avec toi � mes    "
 "c�t�s. Allez on continue. Parfois "
  "je serai face � des pi�ges...   "
},
{
//-------------------------------------" 38 chars 5
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Je devrai les �viter avec       "
 "rapidit�. De ton c�t� tu auras une"
"s�quence de \2DIRECTIONS\1 qui va       "
 "appara�tre et tu devras la r�ussir"
  "pour que j'�vite les pi�ges.    "
},
{
//-------------------------------------" 38 chars 6
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Si tu foires, je meurs. Voyons  "
 "voir si tu as compris. Va         "
"actionner la partie qui ressemble � "
 "une \2FLECHE\1 et pr�pare-toi � faire "
  "la \2SEQUENCE\1 rythm�e.            "
},
{
//-------------------------------------" 38 chars 7
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Apr�s les deux ic�nes de d�part "
 "                                  "
" \2FAIS LA SEQUENCE AVEC LES\1          "
 " \2DIRECTIONS\1.                      "
  "                                "
},
{
//-------------------------------------" 38 chars 8
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "\2BRAVO\1 mon gars, tu vois que t�es"
 "pas si con. Je pense que tu as    "
"compris les bases, on va pouvoir y  "
 "aller. Maintenant c�est pour de   "
  "vrai !!!!!!!!                   "
}
,
{
//-------------------------------------" 38 chars 9
/*
//"--------------------------------" 32 chars (Line 1 & 5)
/"----------------------------------" 34 chars (Line 2 & 4)
"------------------------------------" 36 chars (Line 3)
*/
  "Alors d�conne pas, je te confie "
 "mon cul... Allez appuie sur       "
"\2ACTION\1 et on va commencer le jeu.   "
 "J'ai une connasse � aller buter ! "
  "             \2GOOOOOOO\1           "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0xba, 0xc0, 0xc0, 0xc0, 0x4a, 0xc0, 0xc0, 0x2b,
	0xba, 0x6a, 0xc0, 0xc0, 0xe0, 0xc0, 0xc0, 0x57,
	0xba, 0x42, 0xc0, 0xc0, 0x4a, 0xc0, 0x95, 0xff,
	0xba, 0xbf, 0xc0, 0xc0, 0xe0, 0xc0, 0x81, 0xba,
	0x30, 0x75, 0x17, 0xc0, 0xe0, 0xc0, 0x7f, 0x75,
	0x0a, 0x0f, 0x0f, 0xb0, 0x30, 0x30, 0x12, 0xba,
	0x05, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0xff, 0x30,
	0x0a, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0xba, 0x70,
	0x05, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0x75, 0x71,
	0x0a, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0xba, 0xa0,
	0x05, 0x0f, 0x0f, 0xb0, 0x30, 0x75, 0x75, 0xa2,
	0x0a, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0xba, 0x00,
	0x05, 0x0f, 0x0f, 0xb0, 0x30, 0x75, 0x70, 0x00,
	0x0a, 0x0f, 0x0f, 0x1a, 0x30, 0x30, 0xba, 0x00,
	0x05, 0x0f, 0x0f, 0xb0, 0x30, 0x75, 0x70, 0x00,
	0x0a, 0x0f, 0x5a, 0x1a, 0x30, 0x30, 0x70, 0x00,
	0xff, 0x75, 0x3f, 0x3f, 0x95, 0x95, 0xc0, 0xc0,
	0xff, 0xba, 0x17, 0x6a, 0x6a, 0x6a, 0xc0, 0xc0,
	0x75, 0x70, 0xbf, 0x3f, 0x95, 0xc0, 0xc0, 0xc0,
	0xba, 0xba, 0xa1, 0x6a, 0x6a, 0xc0, 0xc0, 0x95,
	0x30, 0x70, 0xf5, 0x3f, 0x95, 0x95, 0x57, 0x30,
	0x30, 0xf0, 0xf0, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xf1, 0xf3, 0xf0, 0x30, 0x70, 0x30, 0x30, 0x30,
	0xa2, 0x00, 0xf2, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x00, 0x00, 0x51, 0x30, 0x70, 0x30, 0x30, 0x00,
	0x00, 0x00, 0x00, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x00, 0x00, 0x00, 0xb0, 0x70, 0x30, 0x30, 0x15,
	0x00, 0x5a, 0x51, 0xb0, 0x25, 0x30, 0x30, 0x15,
	0x05, 0xf5, 0x5b, 0xb0, 0x70, 0x30, 0x30, 0x15,
	0x50, 0xfa, 0xaf, 0xb0, 0x25, 0x30, 0x30, 0x15,
	0x50, 0xbf, 0x75, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x50, 0xff, 0xff, 0xb0, 0x25, 0x30, 0x30, 0x15,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x2b, 0xba, 0x30,
	0xc0, 0xc0, 0xc0, 0x2b, 0xba, 0x30, 0xa5, 0x00,
	0xc0, 0x2b, 0xba, 0x30, 0xa5, 0x00, 0x00, 0x00,
	0x57, 0x30, 0x30, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x30, 0x30, 0x30, 0x20, 0x0a, 0x5a, 0x50, 0x10,
	0x30, 0x30, 0x30, 0x20, 0xa0, 0xb0, 0x10, 0x50,
	0x30, 0x30, 0x30, 0x20, 0x20, 0xf0, 0x50, 0x05,
	0x30, 0x30, 0x30, 0x20, 0x20, 0xa5, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05,
	0x3f, 0x3f, 0x95, 0x95, 0xc0, 0x80, 0x50, 0x50,
	0x3f, 0x3f, 0x6a, 0xc0, 0xc0, 0x80, 0x50, 0x50,
	0xc0, 0xc0, 0x95, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xc0, 0xd5, 0xc0, 0xc0, 0x80, 0x50, 0x50,
	0xc0, 0xc0, 0x7a, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xc0, 0xaa, 0xc0, 0xc0, 0x80, 0x50, 0x50,
	0xc0, 0x95, 0xa0, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0x05, 0x0f, 0x0f, 0xb0, 0x30, 0x75, 0x71, 0x00,
	0x0a, 0x0f, 0x5a, 0x1a, 0x30, 0xba, 0xf1, 0x00,
	0x05, 0x0f, 0x0f, 0xb0, 0x30, 0x75, 0x20, 0x00,
	0x0a, 0x0f, 0x5a, 0xb0, 0x30, 0xba, 0xa0, 0x00,
	0x05, 0x0f, 0xa5, 0xb0, 0x75, 0x74, 0xa2, 0x00,
	0x0a, 0x5a, 0x5a, 0x30, 0xff, 0xba, 0xa2, 0x00,
	0x05, 0x0f, 0xb0, 0x75, 0x7f, 0x74, 0xa2, 0x00,
	0x0f, 0x5a, 0x30, 0xff, 0xba, 0xfc, 0xa0, 0x00,
	0x05, 0xb0, 0x75, 0x75, 0x74, 0x7c, 0xf1, 0x00,
	0x5a, 0x30, 0xba, 0xba, 0xec, 0xfc, 0xf8, 0x00,
	0xb0, 0x75, 0x30, 0x74, 0x7e, 0x74, 0xf0, 0xa2,
	0x30, 0x30, 0xb8, 0xec, 0xb8, 0xf8, 0xf8, 0xa0,
	0x30, 0xf1, 0x74, 0x7c, 0x74, 0xf4, 0xf1, 0xa0,
	0x30, 0xa2, 0xf6, 0xfc, 0xf8, 0xf8, 0xf2, 0xf1,
	0x70, 0x51, 0xf1, 0xfc, 0xf4, 0xf1, 0xf1, 0xf1,
	0xf1, 0x50, 0xf0, 0xf2, 0xf8, 0xf2, 0xf2, 0xf1,
	0x05, 0x75, 0xff, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x00, 0xb0, 0xfa, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x00, 0x05, 0xf0, 0x30, 0xaf, 0x30, 0x30, 0x15,
	0x00, 0x00, 0x50, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x00, 0x00, 0xb2, 0x30, 0xaf, 0x30, 0x30, 0x15,
	0x00, 0x00, 0xb0, 0x30, 0x25, 0x30, 0x30, 0x15,
	0x00, 0x51, 0x30, 0x30, 0xaf, 0x30, 0x30, 0x40,
	0x00, 0x50, 0x30, 0xba, 0xaf, 0x30, 0x30, 0x15,
	0x00, 0xb2, 0x30, 0x30, 0xba, 0xba, 0x30, 0x40,
	0x00, 0xb0, 0x30, 0xba, 0xaf, 0x30, 0x30, 0x15,
	0x00, 0xb0, 0x30, 0x30, 0xba, 0xba, 0xba, 0x40,
	0x00, 0x30, 0x30, 0xba, 0xaf, 0x30, 0x30, 0x15,
	0x00, 0x30, 0x30, 0x30, 0xba, 0xba, 0xba, 0x40,
	0x00, 0x30, 0x30, 0xba, 0x3a, 0xba, 0x30, 0x40,
	0x00, 0x30, 0x30, 0xba, 0xba, 0xba, 0xba, 0x40,
	0x00, 0x30, 0x30, 0xba, 0x3a, 0xba, 0xba, 0x00,
	0xc0, 0xd5, 0x00, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xd0, 0x00, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0x80, 0x00, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xc0, 0x00, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xc0, 0x00, 0xc0, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xc0, 0x00, 0xc0, 0xc0, 0x80, 0xb0, 0x10,
	0xc0, 0x95, 0x00, 0x6a, 0xc0, 0x80, 0x1a, 0x10,
	0xc0, 0xba, 0x00, 0x75, 0xc0, 0x80, 0xb0, 0x10,
	0xc0, 0xa0, 0x00, 0x50, 0xc0, 0x80, 0xb0, 0x10,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0xb0, 0x10,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x75, 0x10,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0xb0, 0x1a,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x75, 0x10,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x30, 0x1a,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x75, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x1a,
	0xa2, 0xb2, 0xf0, 0xf1, 0xf1, 0xf1, 0xf3, 0xf1,
	0x51, 0x70, 0x70, 0x00, 0xf2, 0xf3, 0xf3, 0xa2,
	0xb2, 0xb0, 0xa0, 0x00, 0x51, 0xf3, 0xf3, 0xf1,
	0x30, 0x70, 0x00, 0x00, 0x00, 0xf3, 0xf3, 0xa2,
	0x70, 0xa2, 0x00, 0x00, 0x00, 0xf3, 0xf3, 0xa2,
	0xf1, 0x00, 0x00, 0x00, 0x00, 0x51, 0xf3, 0xa2,
	0xa2, 0x00, 0x00, 0x00, 0x00, 0x51, 0xf3, 0xa2,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0xa2,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0xa2, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00,
	0x00, 0x30, 0x30, 0xba, 0x3a, 0x3a, 0xba, 0x75,
	0x00, 0xb0, 0x30, 0x3a, 0x3a, 0xba, 0xba, 0x30,
	0x00, 0xb0, 0x30, 0xba, 0x3a, 0x3a, 0xba, 0x30,
	0x00, 0xb2, 0x30, 0x3a, 0x90, 0x3a, 0x3a, 0x75,
	0x51, 0x10, 0x30, 0xba, 0x90, 0x3a, 0xba, 0x35,
	0x50, 0x10, 0x30, 0x3a, 0x90, 0x90, 0x3a, 0x35,
	0x50, 0xb2, 0x30, 0x3a, 0x90, 0x3a, 0xba, 0x75,
	0x51, 0xb0, 0x30, 0x3a, 0x90, 0x90, 0x3a, 0x35,
	0x00, 0x30, 0x30, 0x90, 0x90, 0x3a, 0x7f, 0x35,
	0x00, 0xb0, 0x30, 0x3a, 0x90, 0x90, 0x3a, 0x35,
	0x00, 0x10, 0x30, 0x90, 0x90, 0x90, 0x7f, 0x60,
	0x00, 0x10, 0x30, 0x3a, 0x90, 0x90, 0x90, 0x60,
	0x00, 0x10, 0x30, 0x90, 0x90, 0x3a, 0x7f, 0x60,
	0x00, 0xb2, 0x30, 0x3a, 0x90, 0x90, 0xd5, 0xea,
	0x51, 0xb0, 0x30, 0x90, 0x90, 0x90, 0xd5, 0x60,
	0xf6, 0xb2, 0x30, 0x90, 0x90, 0x3a, 0xd5, 0xea,
	0x75, 0x75, 0x30, 0xba, 0xba, 0x30, 0x75, 0x1a,
	0x1a, 0x1a, 0x30, 0x30, 0x70, 0x30, 0x75, 0x5f,
	0xb0, 0xb0, 0x30, 0x30, 0x25, 0x30, 0x35, 0x1a,
	0x75, 0x75, 0x30, 0xba, 0xaf, 0x30, 0x75, 0x5f,
	0xb5, 0x35, 0x30, 0xba, 0xfa, 0x30, 0x35, 0x1a,
	0x1f, 0xb5, 0x30, 0x3a, 0x7a, 0x30, 0x35, 0x5f,
	0xb5, 0x1f, 0x30, 0xba, 0xba, 0x30, 0x35, 0x5f,
	0x35, 0x1f, 0x30, 0x3a, 0x7a, 0x75, 0x35, 0x5f,
	0x35, 0xe0, 0x75, 0x3a, 0x90, 0x30, 0x60, 0x1f,
	0x35, 0x35, 0x30, 0x3a, 0x3a, 0x75, 0x35, 0xf5,
	0x60, 0xe0, 0x75, 0x90, 0x90, 0xba, 0x60, 0x1f,
	0x60, 0x60, 0x75, 0x3a, 0x90, 0x75, 0x60, 0xb5,
	0x60, 0x60, 0x75, 0x90, 0x3a, 0xba, 0x60, 0x1f,
	0x60, 0x60, 0x35, 0x3a, 0x90, 0xff, 0x60, 0xb5,
	0x60, 0x60, 0x75, 0x90, 0x90, 0xba, 0x60, 0x35,
	0x60, 0x60, 0x35, 0x90, 0x90, 0x7f, 0x60, 0xb5,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xa2, 0xa2, 0x51,
	0x00, 0x00, 0x00, 0x00, 0x51, 0x51, 0x00, 0xf3,
	0x00, 0x00, 0x00, 0x00, 0xa2, 0xa2, 0x51, 0xf4,
	0x00, 0x00, 0x51, 0x51, 0x51, 0x00, 0xf2, 0xfc,
	0x00, 0x00, 0xa2, 0xa2, 0xa2, 0x05, 0xab, 0xf8,
	0x51, 0x51, 0x51, 0x51, 0x00, 0xb2, 0x17, 0xf9,
	0x00, 0xa2, 0xa2, 0xa2, 0x51, 0xa1, 0x6a, 0xa0,
	0x00, 0x00, 0x00, 0x00, 0xf2, 0xbd, 0x95, 0xa2,
	0xf3, 0x00, 0x00, 0x51, 0xf4, 0x21, 0x2b, 0x51,
	0x20, 0x00, 0xf2, 0xf4, 0xfc, 0x75, 0x57, 0xf8,
	0x00, 0x00, 0x51, 0xf3, 0xf4, 0x21, 0xab, 0x30,
	0x00, 0x00, 0x00, 0xa2, 0xf2, 0x75, 0xff, 0x75,
	0x00, 0x00, 0x00, 0x51, 0x51, 0xfd, 0xba, 0xba,
	0x00, 0x00, 0x51, 0x00, 0xa2, 0xb0, 0x30, 0xff,
	0x00, 0xf3, 0x00, 0x00, 0x51, 0xb0, 0x30, 0xab,
	0x51, 0xf8, 0xa2, 0x00, 0x00, 0xb2, 0xba, 0x17,
	0xf1, 0x10, 0x30, 0x90, 0x90, 0x90, 0xd5, 0xea,
	0xa2, 0x10, 0x30, 0x90, 0x90, 0x90, 0xd5, 0xea,
	0xa8, 0xb2, 0x30, 0x90, 0x90, 0x90, 0xd5, 0x6a,
	0xf1, 0xb0, 0x30, 0x90, 0x90, 0x90, 0xd5, 0xea,
	0xf3, 0xb0, 0x30, 0x90, 0x90, 0x90, 0xd5, 0x6a,
	0x51, 0x30, 0x30, 0x90, 0x90, 0x90, 0xd5, 0xea,
	0x50, 0x30, 0x30, 0x3a, 0x90, 0x90, 0xd5, 0x6a,
	0xb2, 0x30, 0x30, 0x90, 0x90, 0x90, 0x90, 0xc0,
	0xb0, 0x30, 0x30, 0x3a, 0x90, 0x90, 0x90, 0x6a,
	0x30, 0x30, 0x30, 0x90, 0x90, 0x90, 0x90, 0xea,
	0x30, 0x30, 0x30, 0x3a, 0x3a, 0x3a, 0x3a, 0x35,
	0x75, 0x30, 0x30, 0xba, 0xba, 0xba, 0xba, 0x75,
	0xba, 0x30, 0x30, 0x3a, 0x90, 0x90, 0x90, 0xbf,
	0x75, 0x30, 0x30, 0xba, 0x90, 0x90, 0x90, 0x6a,
	0xba, 0x30, 0x30, 0x30, 0x3a, 0x3a, 0x3a, 0xbf,
	0x57, 0x30, 0x30, 0xba, 0xba, 0xba, 0xba, 0x75,
	0x60, 0x60, 0x35, 0x90, 0x90, 0xff, 0x60, 0x35,
	0x60, 0x60, 0x60, 0x90, 0x90, 0x7f, 0x60, 0xb5,
	0x60, 0x60, 0x35, 0x90, 0x90, 0xff, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x90, 0x90, 0x7f, 0x60, 0x35,
	0x60, 0x60, 0x35, 0x90, 0x90, 0x7f, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x90, 0x90, 0x7f, 0x35, 0x60,
	0x60, 0x60, 0x60, 0x90, 0x90, 0x7f, 0x60, 0x60,
	0x60, 0x60, 0x60, 0x3a, 0x3a, 0x7f, 0xbf, 0x60,
	0x60, 0x60, 0x35, 0x90, 0x90, 0x7f, 0x35, 0x35,
	0x60, 0x60, 0x60, 0x3a, 0x3a, 0x7f, 0xbf, 0x60,
	0x35, 0x35, 0x35, 0xba, 0x3a, 0x7f, 0xbf, 0x35,
	0x75, 0x75, 0x75, 0x30, 0xba, 0xba, 0x75, 0x75,
	0x35, 0x35, 0x35, 0xba, 0x3a, 0x3a, 0x30, 0x30,
	0x60, 0x60, 0x60, 0x90, 0x90, 0x90, 0xbf, 0x75,
	0x35, 0x35, 0x60, 0x3a, 0x3a, 0x3a, 0x6a, 0x60,
	0x75, 0x75, 0x75, 0xba, 0xba, 0xba, 0xbf, 0x35,
	0x50, 0xfc, 0xf1, 0x00, 0x51, 0xf2, 0xba, 0x2b,
	0x51, 0xbc, 0xfc, 0xf1, 0x00, 0xf2, 0xff, 0x17,
	0x00, 0xf4, 0x3c, 0xfc, 0xf1, 0x51, 0x30, 0x3f,
	0x00, 0xf6, 0xbc, 0x3c, 0xf8, 0x51, 0xb0, 0x6a,
	0x00, 0x54, 0x3c, 0x9c, 0x7c, 0xa2, 0xf7, 0x3f,
	0x00, 0x50, 0xbc, 0x6c, 0x3c, 0xa0, 0x10, 0x6a,
	0x00, 0x51, 0xbc, 0x9c, 0x3c, 0xa8, 0xb2, 0x3f,
	0x00, 0x00, 0xf4, 0x6c, 0x7c, 0x79, 0xb0, 0x6a,
	0x00, 0x00, 0xf6, 0xbc, 0xbc, 0xf8, 0xb8, 0x17,
	0xa2, 0x00, 0x50, 0xfc, 0x7c, 0x7c, 0xf0, 0x6a,
	0xa2, 0x00, 0x51, 0xf4, 0xbc, 0xf8, 0xf9, 0x17,
	0x00, 0x00, 0x00, 0xf3, 0xf4, 0xfc, 0xa0, 0x42,
	0x51, 0x00, 0x00, 0x51, 0xf2, 0xfc, 0xa2, 0xbf,
	0x00, 0x00, 0x00, 0x00, 0xf3, 0xfc, 0x51, 0xbf,
	0x00, 0x00, 0x00, 0x00, 0x51, 0xf4, 0xf2, 0x35,
	0x00, 0xa2, 0x00, 0x00, 0x00, 0xf6, 0xf4, 0x35,
	0x3a, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x57, 0x30, 0x30, 0x30, 0x30, 0x70, 0x70, 0x70,
	0x7f, 0x75, 0x30, 0xba, 0xba, 0x30, 0x30, 0xb0,
	0x7f, 0x30, 0x75, 0x75, 0x75, 0x75, 0x30, 0x30,
	0x2b, 0x75, 0x75, 0xff, 0xba, 0xba, 0xba, 0xba,
	0x7f, 0x30, 0x75, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x81, 0x75, 0x75, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x2b, 0x75, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x95, 0x75, 0x75, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x2b, 0xff, 0xba, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x95, 0x57, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x2b, 0xff, 0xba, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x95, 0x57, 0xba, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x2b, 0x03, 0xba, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x95, 0x57, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x2b, 0x2b, 0xaa, 0xb4, 0xb4, 0xb4, 0x3c, 0x9c,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70,
	0xb0, 0xb0, 0xf0, 0xf0, 0xf0, 0xf0, 0xa5, 0xa5,
	0x30, 0x30, 0x30, 0x70, 0x70, 0x70, 0x5a, 0x0f,
	0xba, 0xba, 0xba, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x75, 0x75, 0x75, 0x75, 0x75, 0x30, 0x30, 0x30,
	0xff, 0xff, 0xba, 0xba, 0xba, 0xba, 0xba, 0xba,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x75, 0x75, 0x75,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xba, 0xba, 0xba,
	0xff, 0xff, 0xba, 0xff, 0xff, 0xff, 0xff, 0x75,
	0xff, 0xff, 0xaa, 0xf5, 0xff, 0xff, 0xba, 0xba,
	0xff, 0xff, 0xfb, 0x50, 0xff, 0xff, 0xff, 0x75,
	0xff, 0xff, 0xbe, 0xa2, 0x75, 0xff, 0xba, 0xba,
	0xff, 0xff, 0xbe, 0x79, 0x10, 0xff, 0xff, 0x75,
	0x00, 0x00, 0x14, 0x3c, 0xa2, 0x75, 0xff, 0xba,
	0x9c, 0xcc, 0xcc, 0x6c, 0x79, 0x10, 0xff, 0x75,
	0x00, 0xa2, 0x00, 0x00, 0x51, 0x50, 0xfc, 0xb5,
	0xa2, 0xf3, 0x00, 0x00, 0x00, 0xf6, 0xf8, 0xb7,
	0xf3, 0x50, 0x00, 0x00, 0x51, 0x50, 0xf9, 0x15,
	0xa0, 0x50, 0xa2, 0xa2, 0x00, 0xf2, 0xa8, 0xb7,
	0xf1, 0x51, 0xf1, 0x51, 0x51, 0x51, 0xa8, 0xb7,
	0xf2, 0x00, 0xf6, 0xa2, 0xa2, 0xf2, 0xa8, 0xb7,
	0xf2, 0xa2, 0x51, 0xf9, 0x51, 0xf3, 0xa8, 0xb7,
	0x51, 0xa0, 0xa2, 0xf6, 0xf3, 0xf3, 0xa0, 0xb7,
	0x00, 0xa8, 0xa2, 0x51, 0xf1, 0xf2, 0xa2, 0xb7,
	0x00, 0xf1, 0x51, 0xa2, 0xf2, 0xf3, 0x51, 0x15,
	0x00, 0xf2, 0x51, 0xf3, 0xf3, 0x00, 0xa2, 0x15,
	0x71, 0xf6, 0xa2, 0xf3, 0x51, 0x00, 0x51, 0x15,
	0xba, 0x54, 0xf1, 0x51, 0xa2, 0xa2, 0x00, 0x15,
	0x2b, 0xf0, 0xf8, 0xf3, 0x51, 0x51, 0x00, 0xb7,
	0x95, 0xf1, 0xf4, 0xf1, 0x00, 0xa2, 0x00, 0xb5,
	0x81, 0xf3, 0xf8, 0xf2, 0xa2, 0x00, 0x00, 0x35,
	0x95, 0x57, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x2b, 0x2b, 0xab, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x95, 0x57, 0x57, 0x57, 0x57, 0x57, 0x57, 0xff,
	0x3f, 0x2b, 0x03, 0xab, 0xab, 0xab, 0xab, 0xab,
	0x95, 0x17, 0x03, 0x03, 0x03, 0x57, 0x57, 0xff,
	0x3f, 0x2b, 0x2b, 0x2b, 0xab, 0xab, 0x03, 0xab,
	0xbf, 0x3f, 0x17, 0x17, 0x17, 0x17, 0x57, 0x57,
	0x21, 0x3f, 0x3f, 0x2b, 0x2b, 0x2b, 0x03, 0x2b,
	0xbf, 0x17, 0x3f, 0x3f, 0x3f, 0x17, 0x17, 0x17,
	0x3f, 0x3f, 0x95, 0x95, 0x3f, 0x3f, 0x2b, 0x2b,
	0xc0, 0x6a, 0x6a, 0x6a, 0x3f, 0x3f, 0x3f, 0x3f,
	0xc0, 0x95, 0x95, 0xc0, 0x95, 0x3f, 0x3f, 0x3f,
	0xc0, 0xc0, 0x6a, 0xc0, 0x6a, 0x3f, 0x3f, 0x3f,
	0x6a, 0x95, 0xc0, 0xc0, 0x95, 0x95, 0x95, 0x3f,
	0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a, 0x6a, 0x6a,
	0x3f, 0x6a, 0x6a, 0xc0, 0xc0, 0x95, 0x95, 0x95,
	0x00, 0x00, 0x50, 0xf0, 0xa2, 0xf5, 0xba, 0xba,
	0xff, 0xff, 0xfa, 0xf1, 0x50, 0xff, 0x30, 0x30,
	0xff, 0xff, 0xfa, 0xa2, 0x75, 0xff, 0x30, 0x30,
	0xff, 0xff, 0xfb, 0x10, 0xff, 0xff, 0xff, 0xba,
	0xff, 0x57, 0xaa, 0x75, 0xff, 0x57, 0x57, 0x57,
	0xab, 0xab, 0xba, 0xab, 0xab, 0xab, 0x03, 0x03,
	0x57, 0xff, 0x57, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0xab, 0xab, 0xab, 0xab, 0x03, 0xab, 0xab,
	0x03, 0x03, 0x03, 0x57, 0x03, 0x03, 0x03, 0x57,
	0x2b, 0x03, 0x03, 0x03, 0x2b, 0x2b, 0x2b, 0xab,
	0x17, 0x17, 0x17, 0x17, 0x03, 0x17, 0x17, 0x17,
	0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b,
	0x3f, 0x3f, 0x17, 0x17, 0x3f, 0x17, 0x17, 0x17,
	0x95, 0x95, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x3f, 0x3f,
	0xc0, 0xc0, 0x95, 0x95, 0x95, 0x95, 0x95, 0x3f,
	0xd5, 0xa2, 0xf4, 0xf1, 0x00, 0x00, 0x51, 0xbf,
	0x90, 0x00, 0xf2, 0xf3, 0xa2, 0x00, 0x50, 0x17,
	0x7a, 0x00, 0x51, 0xf1, 0x00, 0x00, 0x10, 0x3f,
	0x53, 0x00, 0x00, 0xa2, 0xa2, 0x00, 0xf7, 0x3f,
	0xaa, 0x00, 0x51, 0x51, 0x00, 0x00, 0xa1, 0x2b,
	0x71, 0x00, 0x00, 0xa2, 0x00, 0x51, 0x35, 0x2b,
	0xa0, 0x00, 0x00, 0x51, 0x00, 0xf2, 0xbf, 0x3f,
	0xa2, 0x00, 0x00, 0xa2, 0x00, 0xf5, 0x3f, 0x6a,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x3f, 0x95,
	0xa2, 0x00, 0xa2, 0x00, 0x51, 0xbf, 0x6a, 0x6a,
	0x00, 0x00, 0xa0, 0x00, 0x50, 0x17, 0x95, 0x95,
	0x00, 0x51, 0xa0, 0x00, 0xf7, 0x6a, 0x6a, 0x6a,
	0x00, 0x50, 0xa2, 0x00, 0xa1, 0x3f, 0x95, 0xc0,
	0x00, 0xa3, 0xa2, 0x00, 0x21, 0x6a, 0x6a, 0xc0,
	0x00, 0xb5, 0x00, 0x51, 0xbf, 0x3f, 0xc0, 0xc0,
	0x51, 0x57, 0x00, 0x50, 0x03, 0x6a, 0x6a, 0xc0,
	0x3f, 0x17, 0x95, 0x95, 0xc0, 0xc0, 0xc0, 0x6a,
	0x2b, 0x21, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0xc0,
	0x17, 0x12, 0x21, 0xbf, 0x17, 0x95, 0x95, 0x95,
	0x3f, 0x7f, 0x57, 0x75, 0x21, 0x3f, 0x3f, 0x3f,
	0x2b, 0x3f, 0x2b, 0x2b, 0x7f, 0x17, 0x17, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x17, 0x57, 0x3f, 0x2b,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x2b, 0x03, 0x3f,
	0x6a, 0x3f, 0x6a, 0x6a, 0x3f, 0x3f, 0x17, 0x17,
	0x95, 0x3f, 0x95, 0x95, 0x95, 0x3f, 0x2b, 0x2b,
	0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x3f, 0x3f,
	0x95, 0x95, 0xc0, 0x95, 0x3f, 0x95, 0x3f, 0x3f,
	0xc0, 0xc0, 0xc0, 0x6a, 0x6a, 0xc0, 0x3f, 0x6a,
	0xc0, 0xc0, 0xc0, 0x6a, 0xc0, 0x95, 0x95, 0x95,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a,
	0xc0, 0xc0, 0x6a, 0xc0, 0xc0, 0x6a, 0x6a, 0x3f,
	0xc0, 0x95, 0xbf, 0xc0, 0xc0, 0x95, 0x6a, 0x2b,
	0x95, 0x95, 0x21, 0x6a, 0x6a, 0x7f, 0x3f, 0x57,
	0x6a, 0x2b, 0x35, 0x17, 0x95, 0x3a, 0x17, 0x12,
	0x17, 0x95, 0xab, 0x3f, 0x3f, 0x07, 0x17, 0x2f,
	0xab, 0x3f, 0x17, 0x17, 0x2b, 0xba, 0x3f, 0x2f,
	0x17, 0x2b, 0x3f, 0x3f, 0x3f, 0x75, 0x3f, 0xba,
	0x2b, 0x3f, 0x16, 0xfc, 0x29, 0x2b, 0x3f, 0x57,
	0xbf, 0x16, 0xfc, 0xfc, 0xfc, 0x17, 0x3f, 0x3f,
	0x3f, 0x7c, 0xbc, 0xfc, 0xfc, 0xa9, 0x3f, 0x3f,
	0x2b, 0xfc, 0xbc, 0xfc, 0x7c, 0x7c, 0x17, 0x3f,
	0x3e, 0xfc, 0xfc, 0xbc, 0x3c, 0xbc, 0xbd, 0x3f,
	0x7e, 0xfc, 0x7c, 0x7c, 0x7c, 0x7c, 0x29, 0x3f,
	0x56, 0xfc, 0xbc, 0xbc, 0x3c, 0x9c, 0x7c, 0x3f,
	0x56, 0xfc, 0xfc, 0x7c, 0x6c, 0x6c, 0x7c, 0x17,
	0x7c, 0xfc, 0xbc, 0x9c, 0x3c, 0x9d, 0xdc, 0x17,
	0x00, 0x3a, 0x00, 0x10, 0x17, 0x3f, 0x95, 0x95,
	0x51, 0xd0, 0x00, 0xf7, 0x03, 0x2b, 0x6a, 0xc0,
	0x51, 0x7b, 0x00, 0xa1, 0x17, 0x03, 0x17, 0x95,
	0xf3, 0xaa, 0x00, 0xa1, 0x03, 0x2b, 0x3f, 0x3f,
	0xa2, 0x71, 0x00, 0xf5, 0x17, 0x17, 0x3f, 0x95,
	0xa2, 0xa0, 0x00, 0xb2, 0x03, 0x3f, 0x17, 0x6a,
	0xa2, 0xf3, 0x00, 0x50, 0xab, 0x17, 0x3f, 0x95,
	0xa2, 0xa2, 0xa2, 0x51, 0x75, 0x03, 0x3f, 0x3f,
	0xf3, 0x51, 0x51, 0x00, 0xb0, 0xab, 0x17, 0x6a,
	0xf3, 0xf3, 0xa2, 0xa2, 0xf2, 0x21, 0x2b, 0x3f,
	0x51, 0xf3, 0x51, 0x51, 0x51, 0xb0, 0x17, 0x17,
	0x0a, 0x00, 0x00, 0xa2, 0xa2, 0xa2, 0xab, 0x2b,
	0x30, 0xb0, 0xa5, 0x00, 0x00, 0x10, 0x75, 0x17,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0xab, 0xab,
	0x30, 0x30, 0x30, 0x75, 0x75, 0x75, 0x75, 0x03,
	0x30, 0x30, 0x30, 0xba, 0xba, 0xba, 0xff, 0xab,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95,
	0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0xc0, 0x6a, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95, 0x3f, 0x95,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a, 0x6a, 0x6a,
	0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0xc0, 0x95, 0x95,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95, 0x3f,
	0x3f, 0xc0, 0xc0, 0x6a, 0x95, 0x3f, 0x3f, 0x3f,
	0x95, 0x6a, 0x95, 0x3f, 0x3f, 0x95, 0x95, 0x3f,
	0xc0, 0x6a, 0x6a, 0x3f, 0x3f, 0x3f, 0x3f, 0x2b,
	0x95, 0x3f, 0x95, 0x3f, 0x3f, 0x3f, 0x3f, 0x12,
	0x3f, 0x6a, 0x3f, 0x17, 0x3f, 0x17, 0x2b, 0x25,
	0x2b, 0x3f, 0x3f, 0x3f, 0x17, 0x3f, 0x7f, 0xa0,
	0x17, 0x3f, 0x3f, 0x3f, 0x17, 0x3f, 0x7f, 0xa0,
	0x3f, 0x17, 0x3f, 0x17, 0x3f, 0x17, 0x2b, 0x25,
	0x17, 0x3f, 0x17, 0x3f, 0x2b, 0x03, 0x3f, 0x12,
	0x2b, 0x3f, 0x17, 0x2b, 0x3f, 0x17, 0x17, 0x2b,
	0x7c, 0xfc, 0x7c, 0x7c, 0x6c, 0x3f, 0xdc, 0x3d,
	0xfc, 0xfc, 0xbc, 0x9c, 0x9c, 0x9d, 0xdc, 0x3d,
	0xfc, 0xfc, 0x3c, 0x3c, 0x6c, 0x6c, 0x7c, 0x3d,
	0xf8, 0xbc, 0xbc, 0xbc, 0x9c, 0xbc, 0x78, 0x3d,
	0xfc, 0xfc, 0xfc, 0x3c, 0x7c, 0x3c, 0xf8, 0x3d,
	0xf8, 0xfc, 0xfc, 0xbc, 0x9c, 0xbc, 0xfc, 0x3d,
	0x7c, 0xf4, 0xf4, 0x7c, 0x3c, 0x3c, 0xf8, 0x3d,
	0x78, 0xfc, 0xf4, 0xbc, 0xbc, 0xbc, 0xfc, 0x3d,
	0x70, 0xf4, 0xfc, 0xfc, 0x7c, 0xfc, 0xb4, 0x17,
	0xa5, 0xf8, 0xf8, 0xbc, 0x3c, 0xfc, 0xb4, 0x17,
	0x00, 0xf4, 0xf4, 0xfc, 0x7c, 0xf8, 0xa9, 0x3f,
	0x00, 0x5a, 0xf8, 0xf8, 0xf8, 0xf8, 0x29, 0x3f,
	0x00, 0x05, 0xf4, 0xf0, 0xf4, 0xf4, 0x17, 0x3f,
	0x00, 0x00, 0x5a, 0xf8, 0xf8, 0xbc, 0x17, 0x3f,
	0xa5, 0x00, 0x05, 0xf0, 0xf4, 0x29, 0x3f, 0xbf,
	0xba, 0xa5, 0x00, 0x00, 0x14, 0x17, 0x3f, 0x6a
};