#include "display.h"
#include "globals.h"
#include "symboldefs.h"

void define_symbols() {
  
#if DISPLAY_TYPE == DISPLAY_CURSES
  symboldef[CHAR_a] = 97;
  symboldef[CHAR_b] = 98;
  symboldef[CHAR_c] = 99;
  symboldef[CHAR_d] = 100;
  symboldef[CHAR_e] = 101;
  symboldef[CHAR_f] = 102;
  symboldef[CHAR_g] = 103;
  symboldef[CHAR_h] = 104;
  symboldef[CHAR_i] = 105;
  symboldef[CHAR_j] = 106;
  symboldef[CHAR_k] = 107;
  symboldef[CHAR_l] = 108;
  symboldef[CHAR_m] = 109;
  symboldef[CHAR_n] = 110;
  symboldef[CHAR_o] = 111;
  symboldef[CHAR_p] = 112;
  symboldef[CHAR_q] = 113;
  symboldef[CHAR_r] = 114;
  symboldef[CHAR_s] = 115;
  symboldef[CHAR_t] = 116;
  symboldef[CHAR_u] = 117;
  symboldef[CHAR_v] = 118;
  symboldef[CHAR_w] = 119;
  symboldef[CHAR_x] = 120;
  symboldef[CHAR_y] = 121;
  symboldef[CHAR_z] = 122;

  symboldef[CHAR_A] = 65;
  symboldef[CHAR_B] = 66;
  symboldef[CHAR_C] = 67;
  symboldef[CHAR_D] = 68;
  symboldef[CHAR_E] = 69;
  symboldef[CHAR_F] = 70;
  symboldef[CHAR_G] = 71;
  symboldef[CHAR_H] = 72;
  symboldef[CHAR_I] = 73;
  symboldef[CHAR_J] = 74;
  symboldef[CHAR_K] = 75;
  symboldef[CHAR_L] = 76;
  symboldef[CHAR_M] = 77;
  symboldef[CHAR_N] = 78;
  symboldef[CHAR_O] = 79;
  symboldef[CHAR_P] = 80;
  symboldef[CHAR_Q] = 81;
  symboldef[CHAR_R] = 82;
  symboldef[CHAR_S] = 83;
  symboldef[CHAR_T] = 84;
  symboldef[CHAR_U] = 85;
  symboldef[CHAR_V] = 86;
  symboldef[CHAR_W] = 87;
  symboldef[CHAR_X] = 88;
  symboldef[CHAR_Y] = 89;
  symboldef[CHAR_Z] = 90;

  symboldef[CHAR_0] = 48;
  symboldef[CHAR_1] = 49;
  symboldef[CHAR_2] = 50;
  symboldef[CHAR_3] = 51;
  symboldef[CHAR_4] = 52;
  symboldef[CHAR_5] = 53;
  symboldef[CHAR_6] = 54;
  symboldef[CHAR_7] = 55;
  symboldef[CHAR_8] = 56;
  symboldef[CHAR_9] = 57;

  symboldef[CHAR_SPACE] = 32;
  symboldef[CHAR_EXCLAIM] = 33;
  symboldef[CHAR_QUOTE] = 34;
  symboldef[CHAR_HASH] = 35;
  symboldef[CHAR_DOLLAR] = 36;
  symboldef[CHAR_PERCENT] = 37;
  symboldef[CHAR_AMPERS] = 38;
  symboldef[CHAR_APOSTROPHE] = 39;
  symboldef[CHAR_OPAREN] = 40;
  symboldef[CHAR_CPAREN] = 41;
  symboldef[CHAR_ASTER] = 42;
  symboldef[CHAR_PLUS] = 43;
  symboldef[CHAR_COMMA] = 44;
  symboldef[CHAR_DASH] = 45;
  symboldef[CHAR_PERIOD] = 46;
  symboldef[CHAR_FSLASH] = 47;
  symboldef[CHAR_COLON] = 58;
  symboldef[CHAR_SEMICOLON] = 59;
  symboldef[CHAR_OANBRACKET] = 60;
  symboldef[CHAR_EQUAL] = 61;
  symboldef[CHAR_CANBRACKET] = 62;
  symboldef[CHAR_QMARK] = 63;
  symboldef[CHAR_AT] = 64;
  symboldef[CHAR_OSQBRACKET] = 91;
  symboldef[CHAR_BSLASH] = 92;
  symboldef[CHAR_CSQBRACKET] = 93;
  symboldef[CHAR_CARAT] = 94;
  symboldef[CHAR_UNDERSCORE] = 95;
  symboldef[CHAR_OBRACE] = 123;
  symboldef[CHAR_PIPE] = 124;
  symboldef[CHAR_CBRACE] = 125;
  symboldef[CHAR_TILDE] = 126;

  symboldef[CHAR_DOT] = 249;
  symboldef[CHAR_LSHADE] = 176;
  symboldef[CHAR_MSHADE] = 177;
  symboldef[CHAR_DSHADE] = 178;
  symboldef[CHAR_FILLED] = 219;

#elif DISPLAY_TYPE == DISPLAY_WCURSES

  symboldef[CHAR_a] = 97;
  symboldef[CHAR_b] = 98;
  symboldef[CHAR_c] = 99;
  symboldef[CHAR_d] = 100;
  symboldef[CHAR_e] = 101;
  symboldef[CHAR_f] = 102;
  symboldef[CHAR_g] = 103;
  symboldef[CHAR_h] = 104;
  symboldef[CHAR_i] = 105;
  symboldef[CHAR_j] = 106;
  symboldef[CHAR_k] = 107;
  symboldef[CHAR_l] = 108;
  symboldef[CHAR_m] = 109;
  symboldef[CHAR_n] = 110;
  symboldef[CHAR_o] = 111;
  symboldef[CHAR_p] = 112;
  symboldef[CHAR_q] = 113;
  symboldef[CHAR_r] = 114;
  symboldef[CHAR_s] = 115;
  symboldef[CHAR_t] = 116;
  symboldef[CHAR_u] = 117;
  symboldef[CHAR_v] = 118;
  symboldef[CHAR_w] = 119;
  symboldef[CHAR_x] = 120;
  symboldef[CHAR_y] = 121;
  symboldef[CHAR_z] = 122;

  symboldef[CHAR_A] = 65;
  symboldef[CHAR_B] = 66;
  symboldef[CHAR_C] = 67;
  symboldef[CHAR_D] = 68;
  symboldef[CHAR_E] = 69;
  symboldef[CHAR_F] = 70;
  symboldef[CHAR_G] = 71;
  symboldef[CHAR_H] = 72;
  symboldef[CHAR_I] = 73;
  symboldef[CHAR_J] = 74;
  symboldef[CHAR_K] = 75;
  symboldef[CHAR_L] = 76;
  symboldef[CHAR_M] = 77;
  symboldef[CHAR_N] = 78;
  symboldef[CHAR_O] = 79;
  symboldef[CHAR_P] = 80;
  symboldef[CHAR_Q] = 81;
  symboldef[CHAR_R] = 82;
  symboldef[CHAR_S] = 83;
  symboldef[CHAR_T] = 84;
  symboldef[CHAR_U] = 85;
  symboldef[CHAR_V] = 86;
  symboldef[CHAR_W] = 87;
  symboldef[CHAR_X] = 88;
  symboldef[CHAR_Y] = 89;
  symboldef[CHAR_Z] = 90;

  symboldef[CHAR_0] = 48;
  symboldef[CHAR_1] = 49;
  symboldef[CHAR_2] = 50;
  symboldef[CHAR_3] = 51;
  symboldef[CHAR_4] = 52;
  symboldef[CHAR_5] = 53;
  symboldef[CHAR_6] = 54;
  symboldef[CHAR_7] = 55;
  symboldef[CHAR_8] = 56;
  symboldef[CHAR_9] = 57;

  symboldef[CHAR_SPACE] = 32;
  symboldef[CHAR_EXCLAIM] = 33;
  symboldef[CHAR_QUOTE] = 34;
  symboldef[CHAR_HASH] = 35;
  symboldef[CHAR_DOLLAR] = 36;
  symboldef[CHAR_PERCENT] = 37;
  symboldef[CHAR_AMPERS] = 38;
  symboldef[CHAR_APOSTROPHE] = 39;
  symboldef[CHAR_OPAREN] = 40;
  symboldef[CHAR_CPAREN] = 41;
  symboldef[CHAR_ASTER] = 42;
  symboldef[CHAR_PLUS] = 43;
  symboldef[CHAR_COMMA] = 44;
  symboldef[CHAR_DASH] = 45;
  symboldef[CHAR_PERIOD] = 46;
  symboldef[CHAR_FSLASH] = 47;
  symboldef[CHAR_COLON] = 58;
  symboldef[CHAR_SEMICOLON] = 59;
  symboldef[CHAR_OANBRACKET] = 60;
  symboldef[CHAR_EQUAL] = 61;
  symboldef[CHAR_CANBRACKET] = 62;
  symboldef[CHAR_QMARK] = 63;
  symboldef[CHAR_AT] = 64;
  symboldef[CHAR_OSQBRACKET] = 91;
  symboldef[CHAR_BSLASH] = 92;
  symboldef[CHAR_CSQBRACKET] = 93;
  symboldef[CHAR_CARAT] = 94;
  symboldef[CHAR_UNDERSCORE] = 95;
  symboldef[CHAR_OBRACE] = 123;
  symboldef[CHAR_PIPE] = 124;
  symboldef[CHAR_CBRACE] = 125;
  symboldef[CHAR_TILDE] = 126;

  symboldef[CHAR_DOT] = 183;
  symboldef[CHAR_LSHADE] = 9617;
  symboldef[CHAR_MSHADE] = 9618;
  symboldef[CHAR_DSHADE] = 9619;
  symboldef[CHAR_FILLED] = 9608;

#endif

}

symbol_code get_symbol(character c) {

  if (c < CHAR_MAX) {
    return symboldef[c];
  }

#if DISPLAY_TYPE == DISPLAY_CURSES || DISPLAY_TYPE == DISPLAY_WCURSES
  return 0;
#else
  return 0;
#endif
}
