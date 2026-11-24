#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "base.h"

#include "deck.h"

#define KEY_KNOW 'y'
#define KEY_DONT 'n'
#define KEY_QUIT 'q'
#define KEY_HELP '?'

Arena string_buffer;

i32 main(int argc, char **argv) {
  if (argc <= 1) {
    fprintf(stderr, "No input file\n");
    return 1;
  }

  srand(time(NULL));

  string_buffer = ArenaInit(MB(1));
  defer { ArenaFree(&string_buffer); }

  Deck d = DeckLoad(argv[1]);
  defer { DeckUnload(&d); }

  if (!d.number) {
    fprintf(stderr, "No cards in deck\n");
    return 1;
  }

  bool active = true;
  while (active) {
    DeckShuffle(&d);

    i32 num_read = 0;
    for (i32 i = 0; num_read < d.number && active; i %= d.number) {
      Card *cur = &d.cards[i];

      if (cur->status == CARD_STATUS_READ) {
        i++;
        continue;
      }

      CardPrint(cur, CARD_FRONT);

      char input;
      printf("> ");
      if (scanf(" %c", &input) == EOF) {
        printf("\n");
        active = false;
        break;
      }
      switch (input) {
        case KEY_KNOW: {
          cur->status = CARD_STATUS_READ;
          printf("\n");
          num_read++;
          i++;
        } break;
        case KEY_DONT: {
          CardPrint(cur, CARD_BACK);
          printf("\n");
          i++;
        } break;
        case KEY_QUIT: active = false; break;
        default: printf("'%c' is an invalid input\n\n", input);
      }
    }

    if (num_read == d.number && active) {
      char retry;
      printf("retry? ");
      scanf(" %c", &retry);
      if ((retry | 32) != 'y') break;
      printf("\n");
    }
  }

  return 0;
}
