/*
 * cards.c - Implementation of cards and decks.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cards.h"

/*
 * struct card - Represents a playing card.
 * @rank: Rank of the card (ACE to KING).
 * @suit: Suit of the card (SPADES, DIAMONDS, CLUBS, HEARTS).
 */
struct card {
	Rank rank;
	Suit suit;
};

/*
 * struct deck - Represents a deck of playing cards.
 * @cards: Pointer to the dynamically allocated array of cards in the deck.
 * @head: Index of the top card in the deck (next card to be dealt).
 * @tail: Index of the bottom card in the deck (final card in the deck).
 */
struct deck {
	Card *cards;
	size_t head;
	size_t tail;
};

/*
 * struct hand - Represents a player's hand of playing cards.
 * @card: The value of the playing card.
 * @next: Pointer to the next playing card in the hand.
 */
struct hand {
	Card card;
	Hand *next;
};

/*
 * card_rep - Write a human-readable string for a playing card.
 * @buffer: Buffer to write the string representation.
 * @buf_size: Size of the buffer in bytes.
 * @card: Card to represent.
 *
 * Writes a string like "AS" for Ace of Spades into @buffer.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int card_rep(char *buffer, size_t buf_size, const Card *card)
{
	if (buffer == NULL) {
		errno = EINVAL;
		return -1;
	}

	if (buf_size < CARD_STR_LEN) {
		errno = EINVAL;
		return -1;
	}
	char rank_str[3]; // Max "10" = '\0'
	char suit_str;
	// Rank
	switch (card->rank) {
	case ACE:
		strcpy(rank_str, " A");
		break;
	case JACK:
		strcpy(rank_str, " J");
		break;
	case QUEEN:
		strcpy(rank_str, " Q");
		break;
	case KING:
		strcpy(rank_str, " K");
		break;
	case TWO:
	case THREE:
	case FOUR:
	case FIVE:
	case SIX:
	case SEVEN:
	case EIGHT:
	case NINE:
		snprintf(rank_str, sizeof(rank_str), " %d", card->rank);
		break;
	case TEN:
		snprintf(rank_str, sizeof(rank_str), "%d", card->rank);
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	// Suit
	switch (card->suit) {
	case SPADES:
		suit_str = 'S';
		break;
	case DIAMONDS:
		suit_str = 'D';
		break;
	case CLUBS:
		suit_str = 'C';
		break;
	case HEARTS:
		suit_str = 'H';
		break;
	default:
		errno = EINVAL;
		return -1;
	}
	snprintf(buffer, CARD_STR_LEN, "%s%c", rank_str, suit_str);
	return 0;
}

/*
 * deck_rep - Write a human-readable string for a deck of cards.
 * @deck: Deck to print.
 *
 * Displays cards in deck with up to 26 cards per line.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int deck_rep(Deck *deck)
{
	if (deck == NULL) {
		errno = EINVAL;
		return -1;
	}
	char buffer[CARD_STR_LEN];
	size_t buf_size = sizeof(buffer);
	size_t index = deck->head;
	size_t length = 0;
	while (index <= deck->tail) {
		card_rep(buffer, buf_size, &deck->cards[index]);
		if (length >= DECK_REP_LEN) {
			if (printf("\n") < 0) {
				errno = EIO;
				return -1;
			}
			length = 0;
		}
		if (printf("%s ", buffer) < 0) {
			errno = EIO;
			return -1;
		}
		index++;
		length++;
	}
	printf("\n");
	return 0;
}

/*
 * hand_rep - Write a human-readable string for a hand of cards.
 * @deck: Hand to print.
 *
 * Displays cards in a hand, with up to 7 cards per line.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int hand_rep(Hand *hand)
{
	if (hand == NULL) {
		errno = EINVAL;
		return -1;
	}
	char buffer[CARD_STR_LEN];
	size_t buf_size = sizeof(buffer);
	size_t length = 0;
	for (Hand *ptr = hand; ptr != NULL; ptr = ptr->next) {
		card_rep(buffer, buf_size, &ptr->card);
		if (length >= HAND_REP_LEN) {
			if (printf("\n") < 0) {
				errno = EIO;
				return -1;
			}
			length = 0;
		}
		if (printf("%s ", buffer) < 0) {
			errno = EIO;
			return -1;
		}
		length++;
	}
	printf("\n");
	return 0;
}

/*
 * deck_gen - Generate a deck (well actually a shoe) of playing cards.
 * @packs: Number of standard 52-card packs to include.
 *
 * Allocates and initializes a deck with the specified number of packs,
 * each in USPCC new deck order.
 *
 * Return: Pointer to the new deck, or NULL on error with errno set.
 */
Deck *deck_gen(int packs)
{
	if (packs < 1) {
		errno = EINVAL;
		return NULL;
	}
	size_t num_cards = STANDARD_DECK_SIZE * packs;
	struct card *cards = malloc(num_cards * sizeof(struct card));
	if (cards == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	Deck *deck = malloc(sizeof(Deck));
	if (deck == NULL) {
		free(cards);
		errno = ENOMEM;
		return NULL;
	}
	size_t index = 0;
	for (size_t i = 0; i < (size_t)packs; i++) {
		for (Suit suit = SPADES; suit <= HEARTS; suit++) {
			for (Rank rank = ACE; rank <= KING; rank++) {
				cards[index].rank = rank;
				cards[index].suit = suit;
				index++;
			}
		}
	}
	deck->cards = cards;
	deck->head = 0;
	deck->tail = num_cards - 1;
	return deck;
}

/*
 * deck_size - Calculate the number of playing cards in a deck.
 * @deck: Pointer to the deck.
 *
 * Return: Number of cards in the deck, or maximum size_t value on error with
 * errno set.
 */
size_t deck_size(const Deck *deck)
{
	if (deck == NULL || deck->cards == NULL) {
		errno = EINVAL;
		return (size_t)-1; // Maximum value to represent error
	}
	if (deck->head > deck->tail)
		return 0; // Deck is empty
	size_t size = deck->tail - deck->head + 1;
	return size;
}

/**
 * deck_shuffle - Shuffle a deck of playing cards.
 * @deck: Pointer to the deck to shuffle.
 *
 * Shuffles the cards in the deck using the Fisher-Yates algorithm.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int deck_shuffle(Deck *deck)
{
	if (deck == NULL || deck->cards == NULL) {
		errno = EINVAL;
		return -1;
	}
	for (size_t i = deck->tail; i > deck->head; i--) {
		size_t random_card = rand() % (i + 1);
		struct card tmp_card = deck->cards[i];
		deck->cards[i] = deck->cards[random_card];
		deck->cards[random_card] = tmp_card;
	}
	return 0;
}

/*
 * deal - Deal a card from a deck to a hand.
 * @deck: Pointer to the deck to deal from.
 * @hand: Pointer to the pointer of the hand to deal to.
 *
 * Deals a card from the deck by incrementing the head and adding the card to the hand.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int deal(Deck *deck, Hand **hand)
{
	// Ensure deck exists
	if (deck == NULL || deck->cards == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (deck->head > deck->tail) {
		errno = ENODATA;
		return -1;
	}
	// Get card and increment deck head
	struct card tmp_card = deck->cards[deck->head];
	deck->head += 1;
	// Add card to hand
	Hand *player_hand = malloc(sizeof(Hand));
	if (player_hand == NULL) {
		errno = ENOMEM;
		return -1;
	}
	player_hand->card = tmp_card;
	player_hand->next = *hand;
	*hand = player_hand;
	return 0;
}

/*
 * blackjack_score - Calculate the score of a blackjack hand.
 * @hand: Hand to score.
 *
 * Calculates the score of a hand in a game of Blackjack.
 *
 * Return: Score of hand, or 22 for a Blackjack and 0 for a bust, -1 on error
 * with errno set.
 */
int blackjack_score(Hand *hand)
{
	if (hand == NULL) {
		errno = EINVAL;
		return -1;
	}
	size_t score = 0;
	size_t cards = 0;
	size_t aces = 0;
	for (Hand *ptr = hand; ptr != NULL; ptr = ptr->next) {
		int card_score = blackjack_value(&ptr->card);
		cards++;
		if (card_score == 11) {
			score += card_score;
			aces++;
		} else {
			score += card_score;
		}

		if (score > 21 && aces > 0) {
			for (int i = aces; i > 0; i--) {
				score -= 10;
				aces--;
			}
		}

		if (score > 21) {
			return 0;
		}
	}
	if (cards == 2 && score == 21) {
		return 22;
	} else {
		return score;
	}
}

/*
 * blackjack_value - Value of a card in a game of a blackjack.
 * @card: Card to score.
 *
 * Calculates the score of a hand in a game of Blackjack.
 *
 * Return: Score of card, Ace is high, -1 on error
 * with errno set.
 */
int blackjack_value(Card *card)
{
	switch (card->rank) {
	case ACE:
		return 11;
	case JACK:
	case QUEEN:
	case KING:
		return 10;
	case TWO:
	case THREE:
	case FOUR:
	case FIVE:
	case SIX:
	case SEVEN:
	case EIGHT:
	case NINE:
	case TEN:
		return card->rank;
	default:
		errno = EINVAL;
		return -1;
	}
}

/*
 * blackjack_turn - A players turn in a game of blackjack.
 * @deck: Pointer to the game deck.
 * @hand: Pointer to the players hand.
 * @dealer: If the player is the dealer or not.
 *
 * Return: Players score when they stick, -1 on error with errno set.
 */
int blackjack_turn(Deck *deck, Hand **hand, _Bool dealer)
{
	if (deck == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (hand == NULL) {
		errno = EINVAL;
		return -1;
	}
	int score = blackjack_score(*hand);
	if (dealer) {
		printf("Dealers hand: \n");
		hand_rep(*hand);

		while (score > 0 && score < 17) {
			if (deal(deck, hand) < 0) {
				return -1;
			}
			sleep(2);
			score = blackjack_score(*hand);
			printf("Dealer hits: ");
			hand_rep(*hand);
			printf("\n");
		}
		if (score == 0) {
			printf("Dealer busts: ");
			hand_rep(*hand);
			printf("\n");

		} else {
			sleep(2);
			printf("Dealer sticks: ");
			hand_rep(*hand);
			printf("\n");
		}
	} else {
		_Bool stick = 0;
		char buffer[3];

		printf("Hand: ");
		hand_rep(*hand);
		do {
			fputs("(H)it or (S)tick: ", stdout);
			fgets(buffer, 3, stdin);
			printf("\n");
			if (strcmp(buffer, "h\n") == 0) {
				deal(deck, hand);
				score = blackjack_score(*hand);
			} else if (strcmp(buffer, "s\n") == 0) {
				stick = 1;
			} else {
				printf("H or S required\n");
			}
			printf("Hand: ");
			hand_rep(*hand);
		} while (!stick && score > 0);
		if (score == 0) {
			printf("Bust!\n");
		} else {
			printf("Stick: %d\n", score);
		}
	}
	return score;
}

/*
 * unload_deck - Free memory of cards in the deck and the deck itself.
 * @deck: Pointer to the deck to free.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int unload_deck(Deck *deck)
{
	if (deck == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (deck->cards != NULL) {
		free(deck->cards);
		deck->cards = NULL;
	}
	free(deck);
	return 0;
}

int blackjack(void)
{
	Deck *shoe = deck_gen(6);
	if (shoe == NULL) {
		perror("deck_gen");
		return -1;
	}
	puts("Welcome to Blackjack\n");
	if (deck_shuffle(shoe) < 0) {
		perror("deck_shuffle");
		unload_deck(shoe);
		return -1;
	}
	Hand *dealer = NULL;
	Hand *player = NULL;
	// Deal hand to dealer and player
	for (size_t i = 0; i < BLACKJACK_INITIAL_DEAL; i++) {
		if (deal(shoe, &dealer) < 0) {
			perror("deal:dealer");
			unload_deck(shoe);
			unload_hand(dealer);
			unload_hand(player);
			return -1;
		}
		if (deal(shoe, &player) < 0) {
			perror("deal:player");
			unload_deck(shoe);
			unload_hand(dealer);
			unload_hand(player);
			return -1;
		}
	}
	printf("Dealer: ");
	hand_rep(dealer);
	printf("\n");
	int player_score = blackjack_turn(shoe, &player, 0);
	int dealer_score = blackjack_turn(shoe, &dealer, 1);

	if (player_score > dealer_score) {
		printf("Player wins with ");
		if (player_score < 22) {
			printf("%d!\n", player_score);
		} else {
			printf("Blackjack!\n");
		}
	}
	else if (dealer_score > player_score) {
		printf("Dealer wins with ");
		if (dealer_score < 22) {
			printf("%d!\n", dealer_score);
		} else {
			printf("Blackjack!\n");
		}
	} else {
		printf("Draw!\n");
	}

	// Free memory
	if (unload_deck(shoe) < 0) {
		perror("unload_deck");
		return -1;
	}
	if (unload_hand(dealer) < 0) {
		perror("unload_hand:dealer");
		return -1;
	}
	if (unload_hand(player) < 0) {
		perror("unload_hand:player");
		return -1;
	}
	return 0;
}

/*
 * unload_hand - Free memory of cards in a hand.
 * @hand: Pointer to the hand to free.
 *
 * Return: 0 on success, -1 on error with errno set.
 */
int unload_hand(Hand *hand)
{
	if (hand == NULL) {
		return 0; // Nothing to free
	}
	Hand *ptr = hand;
	while (ptr != NULL) {
		Hand *next = ptr->next;
		free(ptr);
		ptr = next;
	}
	return 0;
}
