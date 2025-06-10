/*
 * blackjack.c - Implementation of a game of Blackjack (21).
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cards.h"

int main(void)
{
	srand(time(NULL));
	_Bool play = 0;
	char buffer[3];
	do {
		blackjack();
		fputs("Play again y/n? ", stdout);
		fgets(buffer, 3, stdin);
		printf("\n");

		if (strcmp(buffer, "y\n") == 0) {
			play = 1;
		} else {
			play = 0;
		}

	} while (play);
	return EXIT_SUCCESS;
}
