#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#define DECK_SIZE 52
#define HAND_SIZE 8
#define PLAY_SIZE 5
#define NONE -1

enum suit {
    HEARTS,
    DIAMONDS,
    SPADES,
    CLUBS,
    MAX_SUITS
};

enum rank {
    ACE,
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
    KING,
    MAX_RANKS
};

enum hands {
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH,
    TOTAL_HANDS
}

struct hand_type {
    enum rank rank;
    enum rank second_rank;
    enum suit suit;
    enum hands type;
};

int random_unsigned(int max);
int random_signed(int max);

int command(void);

void initialise_deck(bool deck[MAX_RANKS][MAX_SUITS]);
void draw_hand(bool deck[MAX_RANKS][MAX_SUITS], int hand_size);
struct hand_type find_strongest_hand(bool deck[MAX_RANKS][MAX_SUITS]);
void print_strongest_hand(struct hand_type hand);
char *suit_to_string(enum suit suit);
char *rank_to_string(enum rank rank);

struct hand_type is_straight_flush(bool deck[MAX_RANKS][MAX_SUITS]);
struct hand_type is_n_of_a_kind(bool deck[MAX_RANKS][MAX_SUITS], int n);
struct hand_type is_full_house(bool deck[MAX_RANKS][MAX_SUITS]);
struct hand_type is_flush(bool deck[MAX_RANKS][MAX_SUITS]);
struct hand_type is_straight(bool deck[MAX_RANKS][MAX_SUITS]);
struct hand_type is_two_pair(bool deck[MAX_RANKS][MAX_SUITS]);
struct hand_type is_high_card(bool deck[MAX_RANKS][MAX_SUITS]);

enum rank exclusion_pair_search(bool deck[MAX_RANKS][MAX_SUITS], enum rank excluded_rank);

void debug_print_deck(bool deck[MAX_RANKS][MAX_SUITS]) {
    printf(" |A|2|3|4|5|6|7|8|9|X|J|Q|K|\n");
    printf("H|");
    for (int i = 0; i < MAX_RANKS; i++) {
        if (deck[i][HEARTS] == true) {
            printf("X|");
        } else {
            printf(" |");
        }
    }
    printf("\n");

    printf("D|");
    for (int i = 0; i < MAX_RANKS; i++) {
        if (deck[i][DIAMONDS] == true) {
            printf("X|");
        } else {
            printf(" |");
        }
    }
    printf("\n");

    printf("S|");
    for (int i = 0; i < MAX_RANKS; i++) {
        if (deck[i][SPADES] == true) {
            printf("X|");
        } else {
            printf(" |");
        }
    }
    printf("\n");

    printf("C|");
    for (int i = 0; i < MAX_RANKS; i++) {
        if (deck[i][CLUBS] == true) {
            printf("X|");
        } else {
            printf(" |");
        }
    }
    printf("\n");
}   

void debug_scan_hand(bool deck[MAX_RANKS][MAX_SUITS]) {
    char string[53];
    fgets(string, 53, stdin);
    int i = 0;
    for (int suit = 0; suit < MAX_SUITS; suit++) {
        for (int rank = 0; rank < MAX_RANKS; rank++) {
            if (string[i] == '\0') {
                return;
            }
            if (string[i] == 'X') {
                deck[rank][suit] = true;
            } else {
                deck[rank][suit] = false;
            }
            i++;
        }
    }

    // for (int rank = 0; rank < MAX_RANKS; rank++) {
    //     for (int suit = 0; suit < MAX_SUITS; suit++) {
    //         if (string[i] == '\0') {
    //             return;
    //         }
    //         if (string[i] == 'X') {
    //             deck[rank][suit] = true;
    //         } else {
    //             deck[rank][suit] = false;
    //         }
    //         i++;
    //     }
    // }
}

int main(void) {
    srand(time(NULL));

    int hand_size = command();

    bool deck[MAX_RANKS][MAX_SUITS];
    initialise_deck(deck);

    draw_hand(deck, hand_size);

    debug_scan_hand(deck);
    
    struct hand_type hand = find_strongest_hand(deck);

    print_strongest_hand(hand);

    debug_print_deck(deck);

    return 0;
}

int command(void) {
    printf("Enter hand size: ");
    int hand_size = 0;

    while (hand_size <= 0 || hand_size > DECK_SIZE) {
        scanf(" %d", &hand_size);
        if (hand_size <= 0 || hand_size > DECK_SIZE) {
            printf("ERROR: Please enter a number between 0 and 52.\n");
        }
    }
}

struct hand_type find_strongest_hand(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;

    hand = is_straight_flush(deck);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 4);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_full_house(deck);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_flush(deck);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_straight(deck);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 3);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_two_pair(deck);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 2);
    if (hand.type != NONE) {
        return hand;
    }

    hand = is_high_card(deck);
    return hand;
}

void print_strongest_hand(struct hand_type hand) {
    char *rank = rank_to_string(hand.rank);
    char *suit = suit_to_string(hand.suit);

    if (hand.type == ROYAL_FLUSH) {
        printf("Royal Flush of %s!\n", suit);
    } else if (hand.type == STRAIGHT_FLUSH) {
        printf ("%s-high Straight Flush of %s!\n", rank, suit);
    } else if (hand.type == FOUR_OF_A_KIND) {
        printf("Four of a Kind, %ss!\n", rank);
    } else if (hand.type == FULL_HOUSE) {
        printf("Full House, %ss over %ss!\n", rank, rank_to_string(hand.second_rank));
    } else if (hand.type == FLUSH) {
        printf("Flush of %s!\n", suit);
    } else if (hand.type == STRAIGHT) {
        printf("%s-high Straight!\n", rank);
    } else if (hand.type == THREE_OF_A_KIND) {
        printf("Three of a Kind, %ss!\n", rank);
    } else if (hand.type == TWO_PAIR) {
        printf("Two Pair, %ss and %ss!\n", rank, rank_to_string(hand.second_rank));
    } else if (hand.type == PAIR) {
        printf("Pair of %ss!\n", rank);
    } else if (hand.type == HIGH_CARD) {
        printf("%s High!\n", rank);
    } else {
        printf("How did this happen?\n");
    }
}

char *suit_to_string(enum suit suit) {
    if (suit == HEARTS) {
        return "Hearts";
    } else if (suit == DIAMONDS) {
        return "Diamonds";
    } else if (suit == SPADES) {
        return "Spades";
    } else if (suit == CLUBS) {
        return "Clubs";
    } else {
        return "ERROR IN SUIT_TO_STRING";
    }
}

char *rank_to_string(enum rank rank) {
    if (rank == ACE) {
        return "Ace";
    } else if (rank == TWO) {
        return "Two";
    } else if (rank == THREE) {
        return "Three";
    } else if (rank == FOUR) {
        return "Four";
    } else if (rank == FIVE) {
        return "Five";
    } else if (rank == SIX) {
        return "Six";
    } else if (rank == SEVEN) {
        return "Seven";
    } else if (rank == EIGHT) {
        return "Eight";
    } else if (rank == NINE) {
        return "Nine";
    } else if (rank == TEN) {
        return "Ten";
    } else if (rank == JACK) {
        return "Jack";
    } else if (rank == QUEEN) {
        return "Queen";
    } else if (rank == KING) {
        return "King";
    } else {
        return "ERROR IN RANK_TO_STRING";
    }
}

// ALL hand type detectors will return hand.type = NONE if the check failed. Otherwise, will return the card/suit.
struct hand_type is_straight_flush(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.rank = NONE;
    hand.suit = NONE;
    hand.type = NONE;
    for (int suit = 0; suit < MAX_SUITS; suit++) {
        int count = 0;
        for (int rank = 0; rank < MAX_RANKS; rank++) {
            if (deck[rank][suit]) {
                count++;
            } else {
                count = 0;
            }
            if (count >= 5 && (int)hand.rank < rank) {
                hand.type = STRAIGHT_FLUSH;
                hand.suit = suit;
                hand.rank = rank;
            }
        }

        if (count == PLAY_SIZE - 1 && deck[0][suit]) {
            hand.type = ROYAL_FLUSH;
            hand.suit = suit;
            hand.rank = ACE;
            return hand;
        }
    }
    return hand;
}

struct hand_type is_n_of_a_kind(bool deck[MAX_RANKS][MAX_SUITS], int n) {
    struct hand_type hand;
    hand.rank = NONE;
    hand.type = NONE;
    int saved_count;
    int count;
    for (int rank = 0; rank < MAX_RANKS; rank++) {
        count = 0;
        for (int suit = 0; suit < MAX_SUITS; suit++) {
            if (deck[rank][suit]) {
                count++;
            }
        }
        if (count == n && rank == ACE) {
            hand.rank = ACE;
            saved_count = count;
            break;
        } else if (count == n && (int)hand.rank < rank) {
            hand.rank = rank;
            saved_count = count;
        }
    }

    if (saved_count == 4) {
        hand.type = FOUR_OF_A_KIND;
    } else if (saved_count == 3) {
        hand.type = THREE_OF_A_KIND;
    } else if (saved_count == 2) {
        hand.type = PAIR;
    }

    return hand;
}

struct hand_type is_full_house(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.type = NONE;
    hand.rank = NONE;
    hand.second_rank = NONE;

    struct hand_type trips = is_n_of_a_kind(deck, 3);
    hand.second_rank = exclusion_pair_search(deck, trips.rank);
    if (hand.second_rank != NONE && trips.rank != NONE) {
        hand.type = FULL_HOUSE;
        hand.rank = trips.rank;
    }
    
    return hand;
}

// Helper function for Full Houses and Two Pairs. Search for a pair while ignoring the input rank.
enum rank exclusion_pair_search(bool deck[MAX_RANKS][MAX_SUITS], enum rank excluded_rank) {
    int highest_rank = NONE;
    
    int count = 0;
    for (int rank = 0; rank < MAX_RANKS; rank++) {
        if (rank == excluded_rank) {
            rank++;
        }

        if (rank == MAX_RANKS) {
            break;
        }

        count = 0;
        for (int suit = 0; suit < MAX_SUITS; suit++) {
            if (deck[rank][suit]) {
                count++;
            }
        }

        if (count >= 2 && rank == ACE) {
            highest_rank = ACE;
            break;
        } else if (count >= 2 && highest_rank < rank) {
            highest_rank = rank;
        }
    }

    return highest_rank;
}

struct hand_type is_flush(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.rank = NONE;
    hand.suit = NONE;
    hand.type = NONE;

    int count;
    for (int suit = 0; suit < MAX_SUITS; suit++) {
        count = 0;
        for (int rank = 0; rank < MAX_RANKS; rank++) {
            if (deck[rank][suit]) {
                count++;
            }
            if (count >= PLAY_SIZE && deck[0][suit]) {
                hand.rank = ACE;
                hand.suit = suit;
                hand.type = FLUSH;
                return hand;
            } else if (count >= PLAY_SIZE && (int)hand.rank < rank) {
                hand.rank = rank;
            }
        }
        if (hand.rank != NONE) {
            hand.type = FLUSH;
            hand.suit = suit;
            return hand;
        }
    }


    return hand;
}
struct hand_type is_straight(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.rank = NONE;
    hand.type = NONE;

    bool straight_array[MAX_RANKS + 1] = {false};
    for (int rank = 0; rank < MAX_RANKS; rank++) {
        for (int suit = 0; suit < MAX_SUITS; suit++) {
            if (deck[rank][suit]) {
                straight_array[rank] = true;
                break;
            }
        }
    }

    if (straight_array[0]) {
        straight_array[KING + 1] = true;
    }

    int count = 0;
    for (int i = 0; i < MAX_RANKS + 1; i++) {
        if (straight_array[i]) {
            count++;
        } else {
            count = 0;
        }

        if (count >= PLAY_SIZE && straight_array[i]) {
            hand.type = STRAIGHT;
            hand.rank = i;
        }
    }

    if (hand.rank == KING + 1) {
        hand.rank = ACE;
    }

    return hand;
}
struct hand_type is_two_pair(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.type = NONE;
    hand.rank = NONE;
    hand.second_rank = NONE;

    struct hand_type pair = is_n_of_a_kind(deck, 2);
    hand.second_rank = exclusion_pair_search(deck, pair.rank);
    if (hand.second_rank != NONE && pair.rank != NONE) {
        hand.type = TWO_PAIR;
        hand.rank = pair.rank;
    }
    
    return hand;
}
struct hand_type is_high_card(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;
    hand.type = HIGH_CARD;
    hand.rank = NONE;

    for (int rank = 0; rank < MAX_RANKS; rank++) {
        for (int suit = 0; suit < MAX_SUITS; suit++) {
            if (deck[0][suit]) {
                hand.rank = ACE;
                return hand;
            } else if (deck[rank][suit]) {
                hand.rank = rank;
            }
        }
    }

    return hand;
}

void initialise_deck(bool deck[MAX_RANKS][MAX_SUITS]) {
    for (int i = 0; i < MAX_RANKS; i++) {
        for (int j = 0; j < MAX_SUITS; j++) {
            deck[i][j] = false;
        }
    }
}

void draw_hand(bool deck[MAX_RANKS][MAX_SUITS], int hand_size) {
    int i = 0;
    while (i < hand_size) {
        enum rank rank = random_unsigned(MAX_RANKS);
        enum suit suit = random_unsigned(MAX_SUITS);
        if (!deck[rank][suit]) {
            deck[rank][suit] = true;
            i++;
        }
    }
}


// Returns random number [0, max].
int random_unsigned(int max) {
    return rand() % max;
}

// Returns random number [-max, max]
int random_signed(int max) {
    int plus_minus = random_unsigned(2);
    if (plus_minus == 1) {
        return random_unsigned(max);
    } else {
        return -random_unsigned(max);
    }
}

