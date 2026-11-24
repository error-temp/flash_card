#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base.h"

#include "card.h"

typedef struct {
  Card *cards;
  size_t number;
} Deck;

extern Arena string_buffer;

Deck DeckLoad(const char *path)  {
  Deck d = {};
  
  FILE *f = fopen(path, "r");
  if (!f) {
    fprintf(stderr, "Failed to open %s\n", path);
    exit(1);
  }
  defer { fclose(f); }

  u16 num = 0;
  // get number of lines in file
  for (; !feof(f); num += (fgetc(f) == '\n')? 1 : 0);
  fseek(f, 0, SEEK_SET);
  d.cards = malloc(num * sizeof(*d.cards));
  if (!d.cards) {
    fprintf(stderr, "Failed to allocate memory for cards\n");
    exit(1);
  }
  d.number = num;

  char temp_buf[1024];
  String temp_string;
  for (u16 i = 0; fgets(temp_buf, sizeof(temp_buf), f) && i < num; i++) {
    temp_string = S(temp_buf);
    StrTrimSpace(&temp_string);
    temp_string = S(ArenaAlloc(&string_buffer, temp_string.data, temp_string.length));
    d.cards[i] = CardCreate(&temp_string);
  }

  return d;
}

// fisher-yates algorithm
void DeckShuffle(Deck *d) {
  for (i32 i = d->number - 1; i >= 0; i--) {
    i32 j = rand() % (i + 1);
    Card temp = d->cards[i];
    d->cards[i] = d->cards[j];
    d->cards[j] = temp;

    d->cards[i].status = CARD_STATUS_UNREAD;
    d->cards[j].status = CARD_STATUS_UNREAD;
  }
}

void DeckPrint(Deck *d) {
  if (d->number <= 0) {
    printf("Deck is empty\n");
    return;
  }

  for (size_t i = 0; i < d->number; i++) {
    printf("front: " STR_FMT "\n", STR_ARG(d->cards[i].front));
    printf(" back: " STR_FMT "\n", STR_ARG(d->cards[i].back));
    printf("\n");
  }
}

void DeckUnload(Deck *d) {
  free(d->cards);
  d->number = 0;
}
