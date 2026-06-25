#include <stdlib.h>
#include "registry.h"

ItemType random_item_type(void) {
    int roll = rand() % 100;
    if (roll < 55) {
        return ITEM_APPLE;
    } else if (roll < 90) {
        return ITEM_BANANA;
    }
    return ITEM_BOMB;
}
