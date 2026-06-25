#include <stdlib.h>
#include "registry.h"

ItemType random_item_type(void) {
    int roll = rand() % 100;
    if (roll < 35) return ITEM_APPLE;
    if (roll < 55) return ITEM_BANANA;
    if (roll < 65) return ITEM_CHERRY;
    if (roll < 75) return ITEM_STRAWBERRY;
    if (roll < 80) return ITEM_GRAPE;
    if (roll < 85) return ITEM_ORANGE;
    if (roll < 90) return ITEM_COOKIE;
    if (roll < 95) return ITEM_CANDY;
    if (roll < 98) return ITEM_PUMPKIN;
    return ITEM_BOMB;
}
