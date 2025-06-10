#ifndef CARDS_H
#define CARDS_H

#include <stddef.h> // provides size_t

#define CARD_STR_LEN 4 // Max length of string to represent cards
#define STANDARD_DECK_SIZE 52
#define DECK_REP_LEN 13 // Limit cards per line when printing decks
#define HAND_REP_LEN 7 // Limit cards per line when printing hands
#define BLACKJACK_INITIAL_DEAL 2

/* The ranks of playing card. */
typedef enum rank {
	ACE = 1,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	TEN,
	JACK,
	QUEEN,
	KING
} Rank;

/* The suites of a playing card. */
typedef enum suit {
	SPADES, /* The spade suit (♠) */
	DIAMONDS, /* The diamond suit (♦) */
	CLUBS, /* The club suit (♣) */
	HEARTS /* The heart suit (♥) */
} Suit;

/* A playing card */
typedef struct card Card;
/* A deck containing playing cards */
typedef struct deck Deck;
/* A players hand containing playing cards */
typedef struct hand Hand;

/* Function prototypes. */
int card_rep(char *buffer, size_t buf_size, const Card *card);
int deck_rep(Deck *deck);
int hand_rep(Hand *hand);
Deck *deck_gen(int packs);
size_t deck_size(const Deck *deck);
int deck_shuffle(Deck *deck);
int deal(Deck *deck, Hand **hand);
int blackjack_value(Card *card);
int blackjack_score(Hand *hand);
int blackjack_turn(Deck *deck, Hand **hand, _Bool dealer);
int blackjack(void);
int unload_deck(Deck *deck);
int unload_hand(Hand *hand);

#endif // CARDS_H
