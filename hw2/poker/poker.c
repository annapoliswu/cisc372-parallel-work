#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef enum suit_s {DIAMONDS, CLUBS, HEARTS, SPADES} Suit;
const char* suits[4] = {"diamond", "clubs", "hearts", "spades"}; 

/***************************** CARD *******************************/

typedef struct card_s{
  int rank;
  Suit suit;
} *Card;

/* Note: Card refers to a pointer to struct card. Must malloc size of things inside card and not just pointer. Use struct card_s to refer to struct itself. */
Card createCard(){
  Card card = (Card) malloc(sizeof(struct card_s));
  card->rank = (rand() % 13) + 1;
  card->suit = rand() % 4;
  return card;
}

bool isEqual(Card card1, Card card2){
  return (card1->rank == card2->rank) && (card1->suit == card2->suit);
}

void printCard(Card card){
  printf("%d of %s\n", card->rank, suits[card->suit]);
}

void destroyCard( Card * card){
  free(*card); //note need pointer to struct to free
}

/***************************** HAND *******************************/


typedef struct hand_s{
  Card cards[5];
} *Hand;

Hand createHand(){
  Hand hand = (Hand) malloc(sizeof(struct hand_s));
  for(int i = 0; i < 5; i++){
    hand->cards[i] = createCard();
  }
  return hand;
}

void printHand(Hand hand){
  for(int i = 0; i < 5; i++){
    printf("%d) " , (i+1));
    printCard(hand->cards[i]);
  }
}

void destroyHand( Hand * hand){
  for(int i = 0; i < 5; i++){
    free((*hand)->cards[i]);
  }
  free(*hand);
}

bool sameSuitHand(Hand hand){
  if( (hand->cards[4]->suit == 1) &&
      (hand->cards[3]->suit == 13) &&
      (hand->cards[2]->suit == 12) &&
      (hand->cards[1]->suit == 11) &&
      (hand->cards[0]->suit == 10) ){
    return true;
  }
  
  for(int i = 0; i < 4; i++){
    if(hand->cards[i]->suit != hand->cards[i+1]->suit){
      return false;
    }
    return true;
  }
}

bool consecutiveRankHand(Hand hand){
  for(int i = 0; i < 4; i++){
    if(hand->cards[i]->rank != (hand->cards[i+1]->rank - 1)){
      return false;
    }
    return true;
  }

}

bool isStraightFlush(Hand hand){
  return (consecutiveRankHand(hand) && sameSuitHand(hand) );
}

/****************************************************************/


int main(int argc, char *argv[]){
  srand(time(NULL));
  /*
  Hand hand = createHand();
  printHand(hand);
  destroyHand(&hand);
  */
  int count = 0;
  int straightFlushes = 0;
  int numIt = atoi(argv[1]);
  while(count < numIt){
    count++;
    Hand hand = createHand();
    if ( consecutiveRankHand(hand) == true){
      straightFlushes++;
    }
    destroyHand(&hand);
  }
  double frac = (double) straightFlushes / (double) numIt;
  printf("Straight flushes: %d \nFraction: %f\n", straightFlushes, frac);
  
}


