/*
 * Contains functions for dealing with things like potions, scrolls,
 * and other items.
 *
 * @(#)things.c	4.53 (Berkeley) 02/05/99
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <curses.h>
#ifdef	attron
#endif	/* attron */
#include <ctype.h>
#include "rogue.h"

/*
 * inv_name:
 *	Return the name of something as it would appear in an
 *	inventory.
 */
char *
inv_name(THING *obj, bool drop)
{
    char *pb;
    struct obj_info *op;
    char *sp;
    int which;

    pb = prbuf;
    which = obj->o_which;
    switch (obj->o_type)
    {
        case POTION:
	    nameit(obj, "makfa djacu", p_colors[which], &pot_info[which], nullstr);
	when RING:
	    nameit(obj, "makfa djine", r_stones[which], &ring_info[which], ring_num);
	when STICK:
	    nameit(obj, ws_type[which], ws_made[which], &ws_info[which], charge_str);
	when SCROLL:
	    if (obj->o_count == 1)
	    {
		strcpy(pb, "lo pa makfa papri ");
		pb = &prbuf[9];
	    }
	    else
	    {
		sprintf(pb, "lo %d makfa papri ", obj->o_count);
		pb = &prbuf[strlen(prbuf)];
	    }
	    op = &scr_info[which];
	    if (op->oi_know)
		sprintf(pb, "pe %s", op->oi_name);
	    else if (op->oi_guess)
		sprintf(pb, "pe me'e zo %s", op->oi_guess);
	    else
		sprintf(pb, "pe me'e zo %s", s_names[which]);
	when FOOD:
	    if (which == 1)
		if (obj->o_count == 1)
		    sprintf(pb, "lo pa %s", fruit);
		else
		    sprintf(pb, "lo %d %s", obj->o_count, fruit);
	    else
		if (obj->o_count == 1)
		    strcpy(pb, "lo xo'e cidja");
		else
		    sprintf(pb, "lo %d cidja klani", obj->o_count);
	when WEAPON:
	    sp = weap_info[which].oi_name;
	    if (obj->o_count > 1)
		sprintf(pb, "lo %d ", obj->o_count);
	    else
		sprintf(pb, "lo pa ");
	    pb = &prbuf[strlen(prbuf)];
	    if (obj->o_flags & ISKNOW)
		sprintf(pb, "%s %s", num(obj->o_hplus,obj->o_dplus,WEAPON), sp);
	    else
		sprintf(pb, "%s", sp);
	    if (obj->o_label != NULL)
	    {
		pb = &prbuf[strlen(prbuf)];
		sprintf(pb, " pe me'e zo %s", obj->o_label);
	    }
	when ARMOR:
	    sp = arm_info[which].oi_name;
	    if (obj->o_flags & ISKNOW)
	    {
		sprintf(pb, "lo %s %s [",
		    num(a_class[which] - obj->o_arm, 0, ARMOR), sp);
		    strcat(pb, "nilbandu fa li ");
		pb = &prbuf[strlen(prbuf)];
		sprintf(pb, "%d]", 10 - obj->o_arm);
	    }
	    else
		sprintf(pb, "lo %s", sp);
	    if (obj->o_label != NULL)
	    {
		pb = &prbuf[strlen(prbuf)];
		sprintf(pb, " pe me'e zo %s", obj->o_label);
	    }
	when AMULET:
	    strcpy(pb, "lo djine pe la .iendor.");
	when GOLD:
	    sprintf(prbuf, "lo solji be se la'u li %d", obj->o_goldval);
#ifdef MASTER
	otherwise:
	    debug("ba'o tolcri lo noi jai zdile vau %s", unctrl(obj->o_type));
	    sprintf(pb, "lo noi jai cizra vau %s", unctrl(obj->o_type));
#endif
    }
    if (inv_describe)
    {
	if (obj == cur_armor)
	    strcat(pb, " (ca'o se dasni)");
	if (obj == cur_weapon)
	    strcat(pb, " (ca'o se jgari)");
	if (obj == cur_ring[LEFT])
	    strcat(pb, " (ca'o zulxa'e se jgari)");
	else if (obj == cur_ring[RIGHT])
	    strcat(pb, " (ca'o prityxa'e se jgari)");
    }
    if (drop && isupper(prbuf[0]))
	prbuf[0] = tolower(prbuf[0]);
    else if (!drop && islower(*prbuf))
	*prbuf = toupper(*prbuf);
    prbuf[MAXSTR-1] = '\0';
    return prbuf;
}

/*
 * drop:
 *	Put something down
 */

drop()
{
    char ch;
    THING *obj;

    ch = chat(hero.y, hero.x);
    if (ch != FLOOR && ch != PASSAGE)
    {
	after = FALSE;
	msg("su'o da xa'o zvati");
	return;
    }
    if ((obj = get_item("cirko", 0)) == NULL)
	return;
    if (!dropcheck(obj))
	return;
    obj = leave_pack(obj, TRUE, !ISMULT(obj->o_type));
    /*
     * Link it into the level object list
     */
    attach(lvl_obj, obj);
    chat(hero.y, hero.x) = obj->o_type;
    flat(hero.y, hero.x) |= F_DROPPED;
    obj->o_pos = hero;
    if (obj->o_type == AMULET)
	amulet = FALSE;
    msg("ba'o cirko %s", inv_name(obj, TRUE));
}

/*
 * dropcheck:
 *	Do special checks for dropping or unweilding|unwearing|unringing
 */
bool
dropcheck(THING *obj)
{
    if (obj == NULL)
	return TRUE;
    if (obj != cur_armor && obj != cur_weapon
	&& obj != cur_ring[LEFT] && obj != cur_ring[RIGHT])
	    return TRUE;
    if (obj->o_flags & ISCURSED)
    {
	msg("na kakne .i si'au malmakfa");
	return FALSE;
    }
    if (obj == cur_weapon)
	cur_weapon = NULL;
    else if (obj == cur_armor)
    {
	waste_time();
	cur_armor = NULL;
    }
    else
    {
	cur_ring[obj == cur_ring[LEFT] ? LEFT : RIGHT] = NULL;
	switch (obj->o_which)
	{
	    case R_ADDSTR:
		chg_str(-obj->o_arm);
		break;
	    case R_SEEINVIS:
		unsee();
		extinguish(unsee);
		break;
	}
    }
    return TRUE;
}

/*
 * new_thing:
 *	Return a new thing
 */
THING *
new_thing()
{
    THING *cur;
    int r;

    cur = new_item();
    cur->o_hplus = 0;
    cur->o_dplus = 0;
    strncpy(cur->o_damage, "0x0", sizeof(cur->o_damage));
    strncpy(cur->o_hurldmg, "0x0", sizeof(cur->o_hurldmg));
    cur->o_arm = 11;
    cur->o_count = 1;
    cur->o_group = 0;
    cur->o_flags = 0;
    /*
     * Decide what kind of object it will be
     * If we haven't had food for a while, let it be food.
     */
    switch (no_food > 3 ? 2 : pick_one(things, NUMTHINGS))
    {
	case 0:
	    cur->o_type = POTION;
	    cur->o_which = pick_one(pot_info, MAXPOTIONS);
	when 1:
	    cur->o_type = SCROLL;
	    cur->o_which = pick_one(scr_info, MAXSCROLLS);
	when 2:
	    cur->o_type = FOOD;
	    no_food = 0;
	    if (rnd(10) != 0)
		cur->o_which = 0;
	    else
		cur->o_which = 1;
	when 3:
	    init_weapon(cur, pick_one(weap_info, MAXWEAPONS));
	    if ((r = rnd(100)) < 10)
	    {
		cur->o_flags |= ISCURSED;
		cur->o_hplus -= rnd(3) + 1;
	    }
	    else if (r < 15)
		cur->o_hplus += rnd(3) + 1;
	when 4:
	    cur->o_type = ARMOR;
	    cur->o_which = pick_one(arm_info, MAXARMORS);
	    cur->o_arm = a_class[cur->o_which];
	    if ((r = rnd(100)) < 20)
	    {
		cur->o_flags |= ISCURSED;
		cur->o_arm += rnd(3) + 1;
	    }
	    else if (r < 28)
		cur->o_arm -= rnd(3) + 1;
	when 5:
	    cur->o_type = RING;
	    cur->o_which = pick_one(ring_info, MAXRINGS);
	    switch (cur->o_which)
	    {
		case R_ADDSTR:
		case R_PROTECT:
		case R_ADDHIT:
		case R_ADDDAM:
		    if ((cur->o_arm = rnd(3)) == 0)
		    {
			cur->o_arm = -1;
			cur->o_flags |= ISCURSED;
		    }
		when R_AGGR:
		case R_TELEPORT:
		    cur->o_flags |= ISCURSED;
	    }
	when 6:
	    cur->o_type = STICK;
	    cur->o_which = pick_one(ws_info, MAXSTICKS);
	    fix_stick(cur);
#ifdef MASTER
	otherwise:
	    debug("ba'o tolcri lo mabla");
	    wait_for(' ');
#endif
    }
    return cur;
}

/*
 * pick_one:
 *	Pick an item out of a list of nitems possible objects
 */
int
pick_one(struct obj_info *info, int nitems)
{
    struct obj_info *end;
    struct obj_info *start;
    int i;

    start = info;
    for (end = &info[nitems], i = rnd(100); info < end; info++)
	if (i < info->oi_prob)
	    break;
    if (info == end)
    {
#ifdef MASTER
	if (wizard)
	{
	    msg("mabla me'oi pick_one: %d lo %d dacti", i, nitems);
	    for (info = start; info < end; info++)
		msg("%s: %d%%", info->oi_name, info->oi_prob);
	}
#endif
	info = start;
    }
    return info - start;
}

/*
 * discovered:
 *	list what the player has discovered in this game of a certain type
 */
static int line_cnt = 0;

static bool newpage = FALSE;

static char *lastfmt, *lastarg;


discovered()
{
    char ch;
    bool disc_list;

    do {
	disc_list = FALSE;
	addmsg("for ");
	addmsg("what type");
	    addmsg(" of object do you want a list");
	msg("? (* for all)");
	ch = readchar();
	switch (ch)
	{
	    case ESCAPE:
		msg("");
		return;
	    case POTION:
	    case SCROLL:
	    case RING:
	    case STICK:
	    case '*':
		disc_list = TRUE;
		break;
	    default:
		msg("ko catke su'o batke pe zo'oi %c%c%c%c (zo'oi ESCAPE te zu'e lo nu sisti)", POTION, SCROLL, RING, STICK);
	}
    } while (!disc_list);
    if (ch == '*')
    {
	print_disc(POTION);
	add_line("", NULL);
	print_disc(SCROLL);
	add_line("", NULL);
	print_disc(RING);
	add_line("", NULL);
	print_disc(STICK);
	end_line();
    }
    else
    {
	print_disc(ch);
	end_line();
    }
}

/*
 * print_disc:
 *	Print what we've discovered of type 'type'
 */

#define MAX4(a,b,c,d)	(a > b ? (a > c ? (a > d ? a : d) : (c > d ? c : d)) : (b > c ? (b > d ? b : d) : (c > d ? c : d)))


print_disc(char type)
{
    struct obj_info *info = NULL;
    int i, maxnum = 0, num_found;
    static THING obj;
    static short order[MAX4(MAXSCROLLS, MAXPOTIONS, MAXRINGS, MAXSTICKS)];

    switch (type)
    {
	case SCROLL:
	    maxnum = MAXSCROLLS;
	    info = scr_info;
	    break;
	case POTION:
	    maxnum = MAXPOTIONS;
	    info = pot_info;
	    break;
	case RING:
	    maxnum = MAXRINGS;
	    info = ring_info;
	    break;
	case STICK:
	    maxnum = MAXSTICKS;
	    info = ws_info;
	    break;
    }
    set_order(order, maxnum);
    obj.o_count = 1;
    obj.o_flags = 0;
    num_found = 0;
    for (i = 0; i < maxnum; i++)
	if (info[order[i]].oi_know || info[order[i]].oi_guess)
	{
	    obj.o_type = type;
	    obj.o_which = order[i];
	    add_line("%s", inv_name(&obj, FALSE));
	    num_found++;
	}
    if (num_found == 0)
	add_line(nothing(type), NULL);
}

/*
 * set_order:
 *	Set up order for list
 */

set_order(short *order, int numthings)
{
    int i, r, t;

    for (i = 0; i< numthings; i++)
	order[i] = i;

    for (i = numthings; i > 0; i--)
    {
	r = rnd(i);
	t = order[i - 1];
	order[i - 1] = order[r];
	order[r] = t;
    }
}

/*
 * add_line:
 *	Add a line to the list of discoveries
 */
/* VARARGS1 */
char
add_line(char *fmt, char *arg)
{
    WINDOW *tw, *sw;
    int x, y;
    char *prompt = "--kutybu'i catke--";
    static int maxlen = -1;

    if (line_cnt == 0)
    {
	    wclear(hw);
	    if (inv_type == INV_SLOW)
		mpos = 0;
    }
    if (inv_type == INV_SLOW)
    {
	if (*fmt != '\0')
	    if (msg(fmt, arg) == ESCAPE)
		return ESCAPE;
	line_cnt++;
    }
    else
    {
	if (maxlen < 0)
	    maxlen = strlen(prompt);
	if (line_cnt >= LINES - 1 || fmt == NULL)
	{
	    if (inv_type == INV_OVER && fmt == NULL && !newpage)
	    {
		msg("");
		refresh();
		tw = newwin(line_cnt + 1, maxlen + 2, 0, COLS - maxlen - 3);
		sw = subwin(tw, line_cnt + 1, maxlen + 1, 0, COLS - maxlen - 2);
                for (y = 0; y <= line_cnt; y++) 
                { 
                    wmove(sw, y, 0); 
                    for (x = 0; x <= maxlen; x++) 
                        waddch(sw, mvwinch(hw, y, x)); 
                } 
		wmove(tw, line_cnt, 1);
		waddstr(tw, prompt);
		/*
		 * if there are lines below, use 'em
		 */
		if (LINES > NUMLINES)
		    if (NUMLINES + line_cnt > LINES)
			mvwin(tw, LINES - (line_cnt + 1), COLS - maxlen - 3);
		    else
			mvwin(tw, NUMLINES, 0);
		touchwin(tw);
		wrefresh(tw);
		wait_for(' ');
                if (md_hasclreol())
		{
		    werase(tw);
		    leaveok(tw, TRUE);
		    wrefresh(tw);
		}
		delwin(tw);
		touchwin(stdscr);
	    }
	    else
	    {
		wmove(hw, LINES - 1, 0);
		waddstr(hw, prompt);
		wrefresh(hw);
		wait_for(' ');
		clearok(curscr, TRUE);
		wclear(hw);
#ifdef	attron
		touchwin(stdscr);
#endif	/* attron */
	    }
	    newpage = TRUE;
	    line_cnt = 0;
	    maxlen = strlen(prompt);
	}
	if (fmt != NULL && !(line_cnt == 0 && *fmt == '\0'))
	{
	    mvwprintw(hw, line_cnt++, 0, fmt, arg);
	    getyx(hw, y, x);
	    if (maxlen < x)
		maxlen = x;
	    lastfmt = fmt;
	    lastarg = arg;
	}
    }
    return ~ESCAPE;
}

/*
 * end_line:
 *	End the list of lines
 */

end_line()
{
    if (inv_type != INV_SLOW)
	if (line_cnt == 1 && !newpage)
	{
	    mpos = 0;
	    msg(lastfmt, lastarg);
	}
	else
	    add_line((char *) NULL, NULL);
    line_cnt = 0;
    newpage = FALSE;
}

/*
 * nothing:
 *	Set up prbuf so that message for "nothing found" is there
 */
char *
nothing(char type)
{
    char *sp, *tystr = NULL;

    sprintf(prbuf, "ba'o facki tu'a no da");
    if (type != '*')
    {
	sp = &prbuf[strlen(prbuf)];
	switch (type)
	{
	    case POTION: tystr = "makfa djacu";
	    when SCROLL: tystr = "makfa papri";
	    when RING: tystr = "makfa djine";
	    when STICK: tystr = "makfa grana";
	}
	sprintf(sp, " je srana su'o %s", tystr);
    }
    return prbuf;
}

/*
 * nameit:
 *	Give the proper name to a potion, stick, or ring
 */

nameit(THING *obj, char *type, char *which, struct obj_info *op,
    char *(*prfunc)(THING *))
{
    char *pb;

    if (op->oi_know || op->oi_guess)
    {
	if (obj->o_count == 1)
	    sprintf(prbuf, "lo pa %s ", type);
	else
	    sprintf(prbuf, "lo %d %s ", obj->o_count, type);
	pb = &prbuf[strlen(prbuf)];
	if (op->oi_know)
	    sprintf(pb, "pe %s%s(%s)", op->oi_name, (*prfunc)(obj), which);
	else if (op->oi_guess)
	    sprintf(pb, "pe me'e zo %s%s(%s)", op->oi_guess, (*prfunc)(obj), which);
    }
    else if (obj->o_count == 1)
	sprintf(prbuf, "lo pa %s %s", which, type);
    else
	sprintf(prbuf, "lo %d %s %ss", obj->o_count, which, type);
}

/*
 * nullstr:
 *	Return a pointer to a null-length string
 */
char *
nullstr(THING *ignored)
{
    return "";
}

# ifdef	MASTER
/*
 * pr_list:
 *	List possible potions, scrolls, etc. for wizard.
 */

pr_list()
{
    int ch;

    msg(".au liste lo ro cmima be mo'oi klesi? ");
    ch = readchar();
    switch (ch)
    {
	case POTION:
	    pr_spec(pot_info, MAXPOTIONS);
	when SCROLL:
	    pr_spec(scr_info, MAXSCROLLS);
	when RING:
	    pr_spec(ring_info, MAXRINGS);
	when STICK:
	    pr_spec(ws_info, MAXSTICKS);
	when ARMOR:
	    pr_spec(arm_info, MAXARMORS);
	when WEAPON:
	    pr_spec(weap_info, MAXWEAPONS);
	otherwise:
	    return;
    }
}

/*
 * pr_spec:
 *	Print specific list of possible items to choose from
 */

pr_spec(struct obj_info *info, int nitems)
{
    struct obj_info *endp;
    int i, lastprob;

    endp = &info[nitems];
    lastprob = 0;
    for (i = '0'; info < endp; i++)
    {
	if (i == '9' + 1)
	    i = 'a';
	sprintf(prbuf, "%c: %%s (%d%%%%)", i, info->oi_prob - lastprob);
	lastprob = info->oi_prob;
	add_line(prbuf, info->oi_name);
	info++;
    }
    end_line();
}
# endif	/* MASTER */
