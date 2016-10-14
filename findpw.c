/*
 * print out an encrypted password on the standard output
 *
 * @(#)findpw.c	1.4 (Berkeley) 02/05/99
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */
#include <stdio.h>

main()
{
    char buf[80];

    fprintf(stderr, "Password: ");
    fgets(buf, 80, stdin);
    buf[strlen(buf) - 1] = '\0';
    printf("%s\n", md_crypt(buf, "mT"));
}
