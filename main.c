#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define DECK_SIZE 52
#define HAND_SIZE 8
#define PLAY_SIZE 5
#define NONE -1

#define PLAY_INITIAL -1
#define PLAY_DEBUG 0
#define PLAY_BLACKJACK 1
#define PLAY_POKER 2

#define BLACKJACK 21
#define DEALER_THRESHOLD 16

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
};

struct hand_type {
    enum rank rank;
    enum rank second_rank;
    enum suit suit;
    enum hands type;
};

struct card {
    int rank;
    int suit;
};

struct player_hand {
    struct card card;
    struct player_hand *next;
};

void debug_scan_hand(bool deck[MAX_RANKS][MAX_SUITS]);
void debug_print_deck(bool deck[MAX_RANKS][MAX_SUITS]);

int random_unsigned(int max);
int random_signed(int max);

void testing_hands(void);
void lets_go_gambling(void);

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

void play_blackjack(bool deck[MAX_RANKS][MAX_SUITS], int cash);
void print_blackjack_table(struct player_hand *dealer, struct player_hand *hand, int cash);
void print_dealer_showdown(struct player_hand *dealer, struct player_hand *hand);
char int_to_face(int n);
struct player_hand *create_hand_node(struct card card);
struct card draw_card(bool deck[MAX_RANKS][MAX_SUITS]);
int get_sum(struct player_hand *hand);


int main(void) {
    srand(time(NULL));

    int input = PLAY_INITIAL;
    printf("Input '0' to enter debug mode, input '1' for gambling\n");

    scanf(" %d", &input);

    if (input == PLAY_DEBUG) {      
        testing_hands();
    } else {
        lets_go_gambling();
    }
    return 0;
}

void lets_go_gambling(void) {
    printf("How much money you bringing?\n");
    int cash;
    scanf(" %d", &cash);
    bool master_deck[MAX_RANKS][MAX_SUITS];
    initialise_deck(master_deck);
    
    printf("What we playing?\n");
    printf("1 - Blackjack\n");
    printf("2 - Poker\n");

    int game_mode = 0;
    scanf(" %d", &game_mode);

    if (game_mode == PLAY_BLACKJACK) {
        printf("\n");
        play_blackjack(master_deck, cash);
    }
}

void testing_hands(void) {
    bool deck[MAX_RANKS][MAX_SUITS];
        initialise_deck(deck);

    while (1) {
        printf("force quit to leave debug mode\n");
        
        int hand_size = command();

        draw_hand(deck, hand_size);
        
        struct hand_type hand = find_strongest_hand(deck);

        print_strongest_hand(hand);

        debug_print_deck(deck);
        initialise_deck(deck);
    }
}

void play_blackjack(bool deck[MAX_RANKS][MAX_SUITS], int cash) {
    char more = '\0';
    while (more != 'n') {
        printf("How much are we betting?\n");
        int bet = 0;
        while (bet > cash || bet <= 0) {
            scanf(" %d", &bet);
            if (bet <= 0) {
                printf("We don't bet debts here.\n");
            } else if (bet > cash) {
                printf("You don't got that much nerd.\n");
            }
        }

        cash -= bet;

        struct player_hand *dealer = NULL;
        dealer = create_hand_node(draw_card(deck));
        dealer->next = create_hand_node(draw_card(deck));
        
        struct player_hand *hand = NULL;
        hand = create_hand_node(draw_card(deck));
        hand->next = create_hand_node(draw_card(deck));
        char input = '\0';

        while (get_sum(hand) < BLACKJACK && input != 'S') {
            print_blackjack_table(dealer, hand, cash);
            printf("Press 'H' to hit, or 'S' to stand.\n");

            input = '\0';
            while (input != 'H' && input != 'S') {
                scanf(" %c", &input);
            }

            if (input == 'H') {
                struct player_hand *curr = hand;
                while (curr->next != NULL) {
                    curr = curr->next;
                }
                curr->next = create_hand_node(draw_card(deck));
            } else {
                print_dealer_showdown(dealer, hand);
                while (get_sum(dealer) <= DEALER_THRESHOLD) {
                    struct player_hand *curr_dealer = dealer;
                    while (curr_dealer->next != NULL) {
                        curr_dealer = curr_dealer->next;
                    }
                    curr_dealer->next = create_hand_node(draw_card(deck));
                    print_dealer_showdown(dealer, hand);
                    sleep(2);
                }
            }
        }
        
        print_dealer_showdown(dealer, hand);
        if (get_sum(hand) > BLACKJACK) {
            printf("BUST!\n");
        } else if (get_sum(hand) == get_sum(dealer) && get_sum(dealer) == BLACKJACK) {
            printf("Push!\n");
        } else if (get_sum(hand) == BLACKJACK) {
            printf("BLACKJACK!\n");
            cash += 2 * bet;
        } else if (get_sum(dealer) > BLACKJACK) {
            printf("Nice hand!\n");
            cash += 2 * bet;   
        } else if (get_sum(hand) < get_sum(dealer)) {
            printf("Better luck next time!\n");
        } else if (get_sum(hand) > get_sum(dealer)) {
            printf("Nice hand!\n");
            cash += 2 * bet;
        } else {
            printf("Push!\n");
            cash += bet;
        }

        initialise_deck(deck);
        printf("Balance: %d\n", cash);
        printf("Play more? (y/n)\n");
        scanf(" %c", &more);
        if (cash <= 0 && more != 'n') {
            printf("You have no money bruh\n");
            printf("Press 'q' to quit.\n");
            char quit;
            scanf(" %c", &quit);
            break;
        }
    }
}

int get_sum(struct player_hand *hand) {
    struct player_hand *curr = hand;
    int sum = 0;
    int ace_count = 0;
    while (curr != NULL) {
        if (curr->card.rank > TEN) {
            sum += 10;
        } else if (curr->card.rank != ACE) {
            sum += curr->card.rank + 1;
        } else {
            ace_count++;
        }
        curr = curr->next;
    }
    while (ace_count > 0) {
        if (sum + 11 > BLACKJACK) {
            sum += 1;
        } else {
            sum += 11;
        }
        ace_count--;
    }

    return sum;
}

void print_dealer_showdown(struct player_hand *dealer, struct player_hand *hand) {
    struct player_hand *curr_dealer = dealer;
    printf("DEALER:");

    while (curr_dealer != NULL) {
        if (curr_dealer->card.rank > TEN || curr_dealer->card.rank == ACE) {
            printf(" [%c]", int_to_face(curr_dealer->card.rank));
        } else {
            printf(" [%d]", (int)curr_dealer->card.rank + 1); 
        }
        curr_dealer = curr_dealer->next;
    }
    printf("\n");

    struct player_hand *curr = hand;
    printf("YOU:");

    while (curr != NULL) {
        if (curr->card.rank > TEN || curr->card.rank == ACE) {
            printf(" [%c]", int_to_face(curr->card.rank));
        } else {
            printf(" [%d]", (int)curr->card.rank + 1); 
        }
        curr = curr->next;
    }
    printf("\n");
}

void print_blackjack_table(struct player_hand *dealer, struct player_hand *hand, int cash) {
    printf("---BLACKJACK---\n");
    struct player_hand *curr_dealer = dealer->next;
    printf("DEALER:");
    printf(" [X]");

    while (curr_dealer != NULL) {
        if (curr_dealer->card.rank > TEN || curr_dealer->card.rank == ACE) {
            printf(" [%c]", int_to_face(curr_dealer->card.rank));
        } else {
            printf(" [%d]", (int)curr_dealer->card.rank + 1); 
        }
        curr_dealer = curr_dealer->next;
    }
    printf("\n");

    struct player_hand *curr = hand;
    printf("YOU:");

    while (curr != NULL) {
        if (curr->card.rank > TEN || curr->card.rank == ACE) {
            printf(" [%c]", int_to_face(curr->card.rank));
        } else {
            printf(" [%d]", (int)curr->card.rank + 1); 
        }
        curr = curr->next;
    }
    printf("\n");
    printf("YOUR BALANCE = %d\n", cash);
}

char int_to_face(int n) {
    if (n == JACK) {
        return 'J';
    } else if (n == QUEEN) {
        return 'Q';
    } else if (n == KING) {
        return 'K';
    } else {
        return 'A';
    }
}

struct player_hand *create_hand_node(struct card card) {
    struct player_hand *n = malloc(sizeof(struct player_hand));
    n->card = card;
    n->next = NULL;
    return n;
}

struct card draw_card(bool deck[MAX_RANKS][MAX_SUITS]) {
    bool card_drawn = false;
    struct card card;
    while (!card_drawn) {
        int suit = random_unsigned(MAX_SUITS);
        int rank = random_unsigned(MAX_RANKS);
        if (!deck[rank][suit]) {
            deck[rank][suit] = true;
            card.suit = suit;
            card.rank = rank;
            card_drawn = true;
        }
    }

    return card;
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

    return hand_size;
}

struct hand_type find_strongest_hand(bool deck[MAX_RANKS][MAX_SUITS]) {
    struct hand_type hand;

    hand = is_straight_flush(deck);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 4);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_full_house(deck);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_flush(deck);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_straight(deck);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 3);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_two_pair(deck);
    if ((int)hand.type != NONE) {
        return hand;
    }

    hand = is_n_of_a_kind(deck, 2);
    if ((int)hand.type != NONE) {
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

        if (count >= PLAY_SIZE - 1 && deck[0][suit]) {
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
    int saved_count = 0;
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
    if ((int)hand.second_rank != NONE && (int)trips.rank != NONE) {
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
        if (rank == (int)excluded_rank) {
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
        if ((int)hand.rank != NONE) {
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
    if ((int)hand.second_rank != NONE && (int)pair.rank != NONE) {
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

// Returns random number [0, max).
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
