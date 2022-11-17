#ifndef MAPS_INO_
#define MAPS_INO_
#define MAPCOUNT 8
// clang-format off
byte maps[8*MAPCOUNT] = {
                    /* B     */ FN_FIRE,
                    /* Y     */ FN_FIRE | FN_AUTO_FIRE,

                    /* ️️UP    */ FN_UP,
                    /* DOWN  */ FN_DOWN,
                    /* LEFT  */ FN_LEFT,
                    /* RIGHT */ FN_RIGHT,
                    /* A     */ FN_FIRE2,
                    /* X     */ FN_FIRE2 | FN_AUTO_FIRE,
                    /* L     */ FN_FIRE3 | FN_AUTO_FIRE,
                    /* R     */ FN_FIRE3,

                    /* B     */ FN_FIRE,
                    /* Y     */ FN_UP,
                    /* ️️UP    */ FN_UP,
                    /* DOWN  */ FN_DOWN,
                    /* LEFT  */ FN_LEFT,
                    /* RIGHT */ FN_RIGHT,
                    /* A     */ FN_FIRE | FN_AUTO_FIRE,
                    /* X     */ FN_FIRE2,
                    /* L     */ FN_FIRE3,
                    /* R     */ FN_FIRE3,
                    };
// clang-format on
#endif