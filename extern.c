/*
 * global variable initializaton
 *
 * @(#)extern.c	4.82 (Berkeley) 02/05/99
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <curses.h>
#include "rogue.h"

bool after;				/* True if we want after daemons */
bool again;				/* Repeating the last command */
bool noscore;				/* Was a wizard sometime */
bool seenstairs;			/* Have seen the stairs (for lsd) */
bool amulet = FALSE;			/* He found the amulet */
bool door_stop = FALSE;			/* Stop running when we pass a door */
bool fight_flush = FALSE;		/* True if toilet input */
bool firstmove = FALSE;			/* First move after setting door_stop */
bool got_ltc = FALSE;			/* We have gotten the local tty chars */
bool has_hit = FALSE;			/* Has a "hit" message pending in msg */
bool in_shell = FALSE;			/* True if executing a shell */
bool inv_describe = TRUE;		/* Say which way items are being used */
bool jump = FALSE;			/* Show running as series of jumps */
bool kamikaze = FALSE;			/* to_death really to DEATH */
bool lower_msg = FALSE;			/* Messages should start w/lower case */
bool move_on = FALSE;			/* Next move shouldn't pick up items */
bool msg_esc = FALSE;			/* Check for ESC from msg's --More-- */
bool passgo = FALSE;			/* Follow passages */
bool playing = TRUE;			/* True until he quits */
bool q_comm = FALSE;			/* Are we executing a 'Q' command? */
bool running = FALSE;			/* True if player is running */
bool save_msg = TRUE;			/* Remember last msg */
bool see_floor = TRUE;			/* Show the lamp illuminated floor */
bool stat_msg = FALSE;			/* Should status() print as a msg() */
bool to_death = FALSE;			/* Fighting is to the death! */
bool tombstone = TRUE;			/* Print out tombstone at end */
#ifdef MASTER
bool wizard = FALSE;			/* True if allows wizard commands */
#endif
bool pack_used[26] = {			/* Is the character used in the pack? */
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE
};

char dir_ch;				/* Direction from last get_dir() call */
char file_name[MAXSTR];			/* Save file name */
char huh[MAXSTR];			/* The last message printed */
char *p_colors[MAXPOTIONS];		/* Colors of the potions */
char prbuf[2*MAXSTR];			/* buffer for sprintfs */
char *r_stones[MAXRINGS];		/* Stone settings of the rings */
char *release;				/* release number of program */
char runch;				/* Direction player is running */
char *s_names[MAXSCROLLS];		/* Names of the scrolls */
char take;				/* Thing she is taking */
char whoami[MAXSTR];			/* Name of player */
char *ws_made[MAXSTICKS];		/* What sticks are made of */
char *ws_type[MAXSTICKS];		/* Is it a wand or a staff */
#ifdef TIOCSLTC
char orig_dsusp;			/* Original dsusp char */
#endif
char fruit[MAXSTR] =			/* Favorite fruit */
		{ 'j', 'b', 'o', 'g', 'r', 'u', 't', 'e', '\0' };
char home[MAXSTR] = { '\0' };		/* User's home directory */
char *inv_t_name[] = {
	"galfi ciska",
	"masno ciska",
	"vimcu je ciska"
};
char l_last_comm = '\0';		/* Last last_comm */
char l_last_dir = '\0';			/* Last last_dir */
char last_comm = '\0';			/* Last command typed */
char last_dir = '\0';			/* Last direction given */
char *tr_name[] = {			/* Names of the traps */
	"lo lolvrogai",
	"lo celga'a te kavbu",
	"lo sipna gapci te kavbu",
	"lo batci te kavbu",
	"lo skumu'u te kavbu",
	"lo vindu danti te kavbu",
	"lo furtifa gasnu te kavbu",
        "lo se naljmi kavbu"
};


int n_objs;				/* # items listed in inventory() call */
int ntraps;				/* Number of traps on this level */
int hungry_state = 0;			/* How hungry is he */
int inpack = 0;				/* Number of things in pack */
int inv_type = 0;			/* Type of inventory to use */
int level = 1;				/* What level she is on */
int max_hit;				/* Max damage done to her in to_death */
int max_level;				/* Deepest player has gone */
int mpos = 0;				/* Where cursor is on top line */
int no_food = 0;			/* Number of levels without food */
int a_class[MAXARMORS] = {		/* Armor class for each armor type */
	8,	/* LEATHER */
	7,	/* RING_MAIL */
	7,	/* STUDDED_LEATHER */
	6,	/* SCALE_MAIL */
	5,	/* CHAIN_MAIL */
	4,	/* SPLINT_MAIL */
	4,	/* BANDED_MAIL */
	3,	/* PLATE_MAIL */
};

int count = 0;				/* Number of times to repeat command */
int fd;					/* File descriptor for score file */
int food_left;				/* Amount of food in hero's stomach */
int lastscore = -1;			/* Score before this turn */
int no_command = 0;			/* Number of turns asleep */
int no_move = 0;			/* Number of turns held in place */
int purse = 0;				/* How much gold he has */
int quiet = 0;				/* Number of quiet turns */
int vf_hit = 0;				/* Number of time flytrap has hit */

long dnum;				/* Dungeon number */
long seed;				/* Random number seed */
long e_levels[] = {
        10L,
	20L,
	40L,
	80L,
       160L,
       320L,
       640L,
      1300L,
      2600L,
      5200L,
     13000L,
     26000L,
     50000L,
    100000L,
    200000L,
    400000L,
    800000L,
   2000000L,
   4000000L,
   8000000L,
	 0L
};

coord delta;				/* Change indicated to get_dir() */
coord oldpos;				/* Position before last look() call */
coord stairs;				/* Location of staircase */

PLACE places[MAXLINES*MAXCOLS];		/* level map */

THING *cur_armor;			/* What he is wearing */
THING *cur_ring[2];			/* Which rings are being worn */
THING *cur_weapon;			/* Which weapon he is weilding */
THING *l_last_pick = NULL;		/* Last last_pick */
THING *last_pick = NULL;		/* Last object picked in get_item() */
THING *lvl_obj = NULL;			/* List of objects on this level */
THING *mlist = NULL;			/* List of monsters on the level */
THING player;				/* His stats */
					/* restart of game */

WINDOW *hw = NULL;			/* used as a scratch window */

#define INIT_STATS { 16, 0, 1, 10, 12, "1x4", 12 }

struct stats max_stats = INIT_STATS;	/* The maximum for the player */

struct room *oldrp;			/* Roomin(&oldpos) */
struct room rooms[MAXROOMS];		/* One for each room -- A level */
struct room passages[MAXPASS] =		/* One for each passage */
{
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 },
    { {0, 0}, {0, 0}, {0, 0}, 0, ISGONE|ISDARK, 0, 0 }
};

#define ___ 1
#define XX 10
struct monster monsters[26] =
    {
/* Name		 CARRY	FLAG    str, exp, lvl, amr, hpt, dmg */
{ "tirfurgau",     0,	ISMEAN,	{ XX, 20,   5,   2, ___, "0x0/0x0" } },
{ "bi'anfu",       0,	ISFLY,	{ XX,  1,   1,   3, ___, "1x2" } },
{ "kentauru",	  15,	0,	{ XX, 17,   4,   4, ___, "1x2/1x5/1x5" } },
{ "drakono",	 100,	ISMEAN,	{ XX,5000, 10,  -1, ___, "1x8/1x8/3x10" } },
{ "dromaiu",	   0,	ISMEAN,	{ XX,  2,   1,   7, ___, "1x2" } },
{ "diionaia",      0,	ISMEAN,	{ XX, 80,   8,   3, ___, "%%%x0" } },
	/* NOTE: the damage is %%% so that xstr won't merge this */
	/* string with others, since it is written on in the program */
{ "grifone",	  20,	ISMEAN|ISFLY|ISREGEN, { XX,2000, 13,   2, ___, "4x3/3x5" } },
{ "xobgobeli",	   0,	ISMEAN,	{ XX,  3,   1,   5, ___, "1x8" } },
{ "dujda'u",       0,	0,	{ XX,  5,   1,   9, ___, "0x0" } },
{ "djabervoki",   70,	0,	{ XX,3000, 15,   6, ___, "2x12/2x4" } },
{ "katcipni",	   0,	ISMEAN|ISFLY,	{ XX,  1,   1,   7, ___, "1x4" } },
{ "dincrida",	   0,	0,	{ XX, 10,   3,   8, ___, "1x1" } },
{ "rokybixygau",  40,	ISMEAN,	{ XX,200,   8,   2, ___, "3x4/3x4/2x5" } },
{ "ridrnimfe",	 100,	0,	{ XX, 37,   3,   9, ___, "0x0" } },
{ "orxo",	  15,	ISGREED,{ XX,  5,   1,   6, ___, "1x8" } },
{ "pruxi",	   0,	ISINVIS,{ XX,120,   8,   3, ___, "4x4" } },
{ "xirnku'aga",	   0,	ISMEAN,	{ XX, 15,   3,   3, ___, "1x5/1x5" } },
{ "desysince",     0,	ISMEAN,	{ XX,  9,   2,   3, ___, "1x6" } },
{ "since",	   0,	ISMEAN,	{ XX,  2,   1,   5, ___, "1x3" } },
{ "tro'olo",	  50,	ISREGEN|ISMEAN,{ XX, 120, 6, 4, ___, "1x8/1x8/2x6" } },
{ "pavyseljirna",  0,	ISMEAN,	{ XX,190,   7,  -2, ___, "1x9/1x9/2x9" } },
{ "vampire",	  20,	ISREGEN|ISMEAN,{ XX,350,   8,   1, ___, "1x10" } },
{ "ruxpre",	   0,	0,	{ XX, 55,   5,   4, ___, "1x6" } },
{ "fukcrida",	  30,	0,	{ XX,100,   7,   7, ___, "4x4" } },
{ "bisycrida",	  30,	0,	{ XX, 50,   4,   6, ___, "1x6/1x6" } },
{ "zo'ombi",	   0,	ISMEAN,	{ XX,  6,   2,   8, ___, "1x8" } }
    };
#undef ___
#undef XX

struct obj_info things[NUMTHINGS] = {
    { 0,			26 },	/* potion */
    { 0,			36 },	/* scroll */
    { 0,			16 },	/* food */
    { 0,			 7 },	/* weapon */
    { 0,			 7 },	/* armor */
    { 0,			 4 },	/* ring */
    { 0,			 4 },	/* stick */
};

struct obj_info arm_info[MAXARMORS] = {
    { "lo skapi taxfu",		 20,	 20, NULL, FALSE },
    { "lo jimdjine taxfu",	 15,	 25, NULL, FALSE },
    { "lo jinme skapi taxfu",	 15,	 20, NULL, FALSE },
    { "lo jimtapla taxfu",	 13,	 30, NULL, FALSE },
    { "lo linsi taxfu",		 12,	 75, NULL, FALSE },
    { "lo tirse taxfu",		 10,	 80, NULL, FALSE },
    { "lo tilju taxfu",		 10,	 90, NULL, FALSE },
    { "lo tijytce taxfu",	  5,	150, NULL, FALSE },
};
struct obj_info pot_info[MAXPOTIONS] = {
    { "cfipu",			 7,   5, NULL, FALSE },
    { "xumsne",			 8,   5, NULL, FALSE },
    { "vindu",			 8,   5, NULL, FALSE },
    { "tsali zenba",		13, 150, NULL, FALSE },
    { "narselviska'e viska",	 3, 100, NULL, FALSE },
    { "kanro zenba",		13, 130, NULL, FALSE },
    { "monsuta ganse",		 6, 130, NULL, FALSE },
    { "makfa ganse",		 6, 105, NULL, FALSE },
    { "certu zenba",		 2, 250, NULL, FALSE },
    { "kanro zenba mutce",	 5, 200, NULL, FALSE },
    { "sutra zenba",		 5, 190, NULL, FALSE },
    { "kanro xrugau",		13, 130, NULL, FALSE },
    { "narviska'egau",		 5,   5, NULL, FALSE },
    { "volgau",			 6,  75, NULL, FALSE },
};
struct obj_info ring_info[MAXRINGS] = {
    { "bandu",			 9, 400, NULL, FALSE },
    { "tsali jmina",		 9, 400, NULL, FALSE },
    { "tsali ranji",		 5, 280, NULL, FALSE },
    { "sisku",			10, 420, NULL, FALSE },
    { "narselviska'e viska",	10, 310, NULL, FALSE },
    { "jadni",			 1,  10, NULL, FALSE },
    { "monsuta fanza",		10,  10, NULL, FALSE },
    { "xancre",			 8, 440, NULL, FALSE },
    { "xrani jmina",		 8, 400, NULL, FALSE },
    { "ka'orbi'o",		 4, 460, NULL, FALSE },
    { "norxagji ranji",		 9, 240, NULL, FALSE },
    { "skumu'u",		 5,  30, NULL, FALSE },
    { "stealth",		 7, 470, NULL, FALSE },
    { "taxfu nurgau",		 5, 380, NULL, FALSE },
};
struct obj_info scr_info[MAXSCROLLS] = {
    { "monsuta cfipu",			 7, 140, NULL, FALSE },
    { "magic mapping",			 4, 150, NULL, FALSE },
    { "monsuta muvstigau",		 2, 180, NULL, FALSE },
    { "sipna",				 3,   5, NULL, FALSE },
    { "taxfu xagzengau",		 7, 160, NULL, FALSE },
    { "maflitki facki",			11,  80, NULL, FALSE },
    { "papri facki",			10,  80, NULL, FALSE },
    { "xarci facki",			 6,  80, NULL, FALSE },
    { "taxfu facki",			 7, 100, NULL, FALSE },
    { "djine ja grana facki",		10, 115, NULL, FALSE },
    { "monsuta terpygau",		 3, 200, NULL, FALSE },
    { "cidja ganse",			 2,  60, NULL, FALSE },
    { "skumu'u",			 5, 165, NULL, FALSE },
    { "xarci xagzengau",		 8, 150, NULL, FALSE },
    { "monsuta klami'e",		 4,  75, NULL, FALSE },
    { "maldimna vimcu",			 7, 105, NULL, FALSE },
    { "monsuta fanza",			 3,  20, NULL, FALSE },
    { "taxfu nurgau",			 2, 250, NULL, FALSE },
};
struct obj_info weap_info[MAXWEAPONS + 1] = {
    { "bolga'axa'i",			11,   8, NULL, FALSE },
    { "cladaxi",			11,  15, NULL, FALSE },
    { "bagyce'a",			12,  15, NULL, FALSE },
    { "celga'a",			12,   1, NULL, FALSE },
    { "dakyxa'i",			 8,   3, NULL, FALSE },
    { "relxandakyxa'i",			10,  75, NULL, FALSE },
    { "danti",				12,   2, NULL, FALSE },
    { "jindydanti",			12,   5, NULL, FALSE },
    { "kilga'a",			12,   5, NULL, FALSE },
    { NULL, 0 },	/* DO NOT REMOVE: fake entry for dragon's breath */
};
struct obj_info ws_info[MAXSTICKS] = {
    { "gusni",			12, 250, NULL, FALSE },
    { "narselviska'egau",	 6,   5, NULL, FALSE },
    { "lindi",		 3, 330, NULL, FALSE },
    { "fagri",			 3, 330, NULL, FALSE },
    { "bisli",			 3, 330, NULL, FALSE },
    { "tamga'i",		15, 310, NULL, FALSE },
    { "makfa danti",		10, 170, NULL, FALSE },
    { "monsuta sutygau",	10,   5, NULL, FALSE },
    { "monsuta snogau",		11, 350, NULL, FALSE },
    { "nilka'o lebna",		 9, 300, NULL, FALSE },
    { "narplixau",		 1,   5, NULL, FALSE },
    { "to'o zei skumu'u",	 6, 340, NULL, FALSE },
    { "fa'a zei skumu'u",	 6,  50, NULL, FALSE },
    { "mafyvi'u",		 5, 280, NULL, FALSE },
};

struct h_list helpstr[] = {
    '?',	"	se sidju",				TRUE,
    '/',	"	facki fi lo ekra lerfu",		TRUE,
    'h',	"	zunle stapa",				TRUE,
    'j',	"	cnita stapa",				TRUE,
    'k',	"	gapru stapa",				TRUE,
    'l',	"	pritu stapa",				TRUE,
    'y',	"	gapru je zunle stapa",			TRUE,
    'u',	"	gapru je pritu stapa",			TRUE,
    'b',	"	cnita je zunle stapa",			TRUE,
    'n',	"	cnita je pritu stapa",			TRUE,
    'H',	"	zunle bajra",				TRUE,
    'J',	"	cnita bajra",				TRUE,
    'K',	"	gapru bajra",				TRUE,
    'L',	"	pritu bajra",				TRUE,
    'Y',	"	gapru je zunle bajra",			TRUE,
    'U',	"	gapru je pritu bajra",			TRUE,
    'B',	"	cnita je zunle bajra",			TRUE,
    'N',	"	cnita je pritu bajra",			TRUE,
    CTRL('H'),	"	zunle ke jundi bajra",			FALSE,
    CTRL('J'),	"	cnita ke jundi bajra",			FALSE,
    CTRL('K'),	"	gapru ke jundi bajra",			FALSE,
    CTRL('L'),	"	pritu ke jundi bajra",			FALSE,
    CTRL('Y'),	"	gapru je zunle ke jundi bajra",		FALSE,
    CTRL('U'),	"	gapru je pritu ke jundi bajra",		FALSE,
    CTRL('B'),	"	cnita je zunle ke jundi bajra",		FALSE,
    CTRL('N'),	"	cnita je pritu ke jundi bajra",		FALSE,
    '\0',	"	<SHIFT><dir>: bajra",			TRUE,
    '\0',	"	<CTRL><dir>: jundi bajra",		TRUE,
    'f',	"<dir>	gunta co'u lo nu tcezilcro",		TRUE,
    't',	"<dir>	renro",					TRUE,
    'm',	"<dir>	move onto without picking up",		TRUE,
    'z',	"<dir>	zap a wand in a direction",		TRUE,
    '^',	"<dir>	identify trap type",			TRUE,
    's',	"	search for trap/secret door",		TRUE,
    '>',	"	go down a staircase",			TRUE,
    '<',	"	go up a staircase",			TRUE,
    '.',	"	rest for a turn",			TRUE,
    ',',	"	pick something up",			TRUE,
    'i',	"	inventory",				TRUE,
    'I',	"	inventory single item",			TRUE,
    'q',	"	quaff potion",				TRUE,
    'r',	"	read scroll",				TRUE,
    'e',	"	eat food",				TRUE,
    'w',	"	wield a weapon",			TRUE,
    'W',	"	wear armor",				TRUE,
    'T',	"	take armor off",			TRUE,
    'P',	"	put on ring",				TRUE,
    'R',	"	remove ring",				TRUE,
    'd',	"	drop object",				TRUE,
    'c',	"	call object",				TRUE,
    'a',	"	repeat last command",			TRUE,
    ')',	"	print current weapon",			TRUE,
    ']',	"	print current armor",			TRUE,
    '=',	"	print current rings",			TRUE,
    '@',	"	print current stats",			TRUE,
    'D',	"	recall what's been discovered",		TRUE,
    'o',	"	examine/set options",			TRUE,
    CTRL('R'),	"	redraw screen",				TRUE,
    CTRL('P'),	"	repeat last message",			TRUE,
    ESCAPE,	"	cancel command",			TRUE,
    'S',	"	save game",				TRUE,
    'Q',	"	quit",					TRUE,
    '!',	"	shell escape",				TRUE,
    'F',	"<dir>	fight till either of you dies",		TRUE,
    'v',	"	print version number",			TRUE,
    0,		NULL
};

#ifdef TIOCGLTC
struct ltchars ltc;		/* needed to change ^Y to not be suspchar */
#endif /* TIOCGLTC */
