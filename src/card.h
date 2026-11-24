#pragma once

#include "base.h"

#define CARD_FRONT 1
#define CARD_BOTH 0
#define CARD_BACK -1

typedef enum {
  CARD_STATUS_UNREAD,
  CARD_STATUS_READ,
} CardStatus;

typedef struct {
  String front, back;
  CardStatus status;
} Card;

Card CardCreate(String *s) {
  Card c = { .front = *s, };
  c.back = StrSplit(&c.front, '|');
  StrTrimSpace(&c.front);
  StrTrimSpace(&c.back);

  return c;
}

void CardPrint(Card *c, i8 side) {
  if (side >= 0) printf(STR_FMT "\n", STR_ARG(c->front));
  if (side <= 0) printf(STR_FMT "\n", STR_ARG(c->back));
}
