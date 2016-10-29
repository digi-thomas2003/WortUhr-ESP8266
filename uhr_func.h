#include <Arduino.h>


//------------------------------------------------------------------------------

void led_set(uint16_t i) {     // color of LED i
  pixelsR[i] = G.r;
  pixelsG[i] = G.g;
  pixelsB[i] = G.b;
  ledstrip.setPixelColor(i, pixelsR[i], pixelsG[i], pixelsB[i]);
}

//------------------------------------------------------------------------------

void led_set_bg(uint16_t i) {   // background color of LED i
  pixelsR[i] = G.rr;
  pixelsG[i] = G.gg;
  pixelsB[i] = G.bb;
  ledstrip.setPixelColor(i, pixelsR[i], pixelsG[i], pixelsB[i]);
}

//------------------------------------------------------------------------------

void led_clear() {    // all LEDs off resp. color black
  uint8_t i;
  for (i = 0; i < NUM_PIXELS; i++)
  {
    pixelsR[i] = 0;
    pixelsG[i] = 0;
    pixelsB[i] = 0;
    ledstrip.setPixelColor(i, pixelsR[i], pixelsG[i], pixelsB[i]);
  }
}

//------------------------------------------------------------------------------

void set_bg_color() {    // adjust bright background color

  unsigned int rr, gg, bb, zz;
  rr = G.bg_r;
  gg = G.bg_g;
  bb = G.bg_b;
  zz = rr + gg + bb;
  if (zz > 100) {
    zz = zz * 10 / 100;
    rr = (int)rr * 10 / zz;
    gg = (int)gg * 10 / zz;
    bb = (int)bb * 10 / zz;
  }
  G.rr = rr;
  G.gg = gg;
  G.bb = bb;
}

//------------------------------------------------------------------------------

#include "uhr_func_110.h"

//------------------------------------------------------------------------------

void test_led() {
  uint8_t i;

  for (i = 0; i < NUM_PIXELS; i++)
  {
    led_clear();
    pixelsR[i] = 100;
    pixelsG[i] = 100;
    pixelsB[i] = 100;
    ledstrip.setPixelColor(i, pixelsR[i], pixelsG[i], pixelsB[i]);
    ledstrip.show();
    delay(20);
  }
}

//------------------------------------------------------------------------------

void test_uhr(uint16_t d) {

  led_clear(); es_ist();   ledstrip.show();  delay(d);
  led_clear(); fuenf();    ledstrip.show();  delay(d);
  led_clear(); zehn();     ledstrip.show();  delay(d);
  led_clear(); zwanzig();  ledstrip.show();  delay(d);
  led_clear(); viertel();  ledstrip.show();  delay(d);
  led_clear(); nach();     ledstrip.show();  delay(d);
  led_clear(); vor();      ledstrip.show();  delay(d);
  led_clear(); halb();     ledstrip.show();  delay(d);
  led_clear(); h_zwoelf(); ledstrip.show();  delay(d);
  led_clear(); h_zwei();   ledstrip.show();  delay(d);
  led_clear(); eins();     ledstrip.show();  delay(d);
  led_clear(); h_ein();    ledstrip.show();  delay(d);
  led_clear(); h_sieben(); ledstrip.show();  delay(d);
  led_clear(); h_drei();   ledstrip.show();  delay(d);
  led_clear(); h_fuenf();  ledstrip.show();  delay(d);
  led_clear(); h_elf();    ledstrip.show();  delay(d);
  led_clear(); h_neun();   ledstrip.show();  delay(d);
  led_clear(); h_vier();   ledstrip.show();  delay(d);
  led_clear(); h_acht();   ledstrip.show();  delay(d);
  led_clear(); h_zehn();   ledstrip.show();  delay(d);
  led_clear(); h_sechs();  ledstrip.show();  delay(d);
  led_clear(); uhr();      ledstrip.show();  delay(d);

}

//------------------------------------------------------------------------------

void laufschrift(int r, int g, int b, int d) {
  // muß noch bearbeitet werden!!!

  for ( unsigned int ii = 0; ii < strlen (ls_txt) - 1; ii++) {

    d++;
    for (int i = 0; i < 5; i++) {
      for (int b = 0; b < 10; b++) {
        for (int a = 0; a < ROWS_MATRIX; a++) {
          pixelsR[matrix[a][b]] = pixelsR[matrix[a][b + 1]];
          pixelsG[matrix[a][b]] = pixelsG[matrix[a][b + 1]];
          pixelsB[matrix[a][b]] = pixelsB[matrix[a][b + 1]];
          ledstrip.setPixelColor(matrix[a][b], pixelsR[matrix[a][b]], pixelsG[matrix[a][b]], pixelsB[matrix[a][b]]);
        }
      }
      for (int h = 0; h < 8; h++) {
        if (font_7x5[ls_txt[ii]][i] & (1 << h)) {
          pixelsR[matrix[h + 1][10]] = r;
          pixelsG[matrix[h + 1][10]] = g;
          pixelsB[matrix[h + 1][10]] = b;
          ledstrip.setPixelColor(matrix[h + 1][10], pixelsR[matrix[h + 1][10]], pixelsG[matrix[h + 1][10]], pixelsB[matrix[h + 1][10]]);
        } else {
          pixelsR[matrix[h + 1][10]] = 0;
          pixelsG[matrix[h + 1][10]] = 0;
          pixelsB[matrix[h + 1][10]] = 0;
          ledstrip.setPixelColor(matrix[h + 1][10], pixelsR[matrix[h + 1][10]], pixelsG[matrix[h + 1][10]], pixelsB[matrix[h + 1][10]]);
        }
      }
      ledstrip.show();
      delay(d);
    }
    for (int b = 0; b < 10; b++) {
      for (int a = 0; a < ROWS_MATRIX; a++) {
        pixelsR[matrix[a][b]] = pixelsR[matrix[a][b + 1]];
        pixelsG[matrix[a][b]] = pixelsG[matrix[a][b + 1]];
        pixelsB[matrix[a][b]] = pixelsB[matrix[a][b + 1]];
        ledstrip.setPixelColor(matrix[a][b], pixelsR[matrix[a][b]], pixelsG[matrix[a][b]], pixelsB[matrix[a][b]]);
      }
    }
    for (int h = 0; h < 8; h++) {
      pixelsR[matrix[h + 1][10]] = 0;
      pixelsG[matrix[h + 1][10]] = 0;
      pixelsB[matrix[h + 1][10]] = 0;
      ledstrip.setPixelColor(matrix[h + 1][10], pixelsR[matrix[h + 1][10]], pixelsG[matrix[h + 1][10]], pixelsB[matrix[h + 1][10]]);
    }
    ledstrip.show();
    delay(d);
  }

}

//------------------------------------------------------------------------------

void zahlen(char d1, char d2) {
#ifdef DEBUG
  //    USE_SERIAL.printf("d1: %u %c \n", d1, d1);
  //    USE_SERIAL.printf("d2: %u %c \n", d2, d2);
#endif
  for (int i = 0; i < 5; i++) {
    for (int h = 0; h < 8; h++) {
      if (font_7x5[d1][i] & (1 << h))   {
        pixelsR[matrix[h + 1][i]] = G.r;
        pixelsG[matrix[h + 1][i]] = G.g;
        pixelsB[matrix[h + 1][i]] = G.b;
        ledstrip.setPixelColor(matrix[h + 1][i], pixelsR[matrix[h + 1][i]], pixelsG[matrix[h + 1][i]], pixelsB[matrix[h + 1][i]]);
      } else {
        pixelsR[matrix[h + 1][i]] = G.rr;
        pixelsG[matrix[h + 1][i]] = G.gg;
        pixelsB[matrix[h + 1][i]] = G.bb;
        ledstrip.setPixelColor(matrix[h + 1][i], pixelsR[matrix[h + 1][i]], pixelsG[matrix[h + 1][i]], pixelsB[matrix[h + 1][i]]);
      }
    }
  }
  for (int i = 0; i < 5; i++) {
    for (int h = 0; h < 8; h++) {
      if (font_7x5[d2][i] & (1 << h))   {
        pixelsR[matrix[h + 1][i + 6]] = G.r;
        pixelsG[matrix[h + 1][i + 6]] = G.g;
        pixelsB[matrix[h + 1][i + 6]] = G.b;
        ledstrip.setPixelColor(matrix[h + 1][i + 6], pixelsR[matrix[h + 1][i + 6]], pixelsG[matrix[h + 1][i + 6]], pixelsB[matrix[h + 1][i + 6]]);
      } else {
        pixelsR[matrix[h + 1][i + 6]] = G.rr;
        pixelsG[matrix[h + 1][i + 6]] = G.gg;
        pixelsB[matrix[h + 1][i + 6]] = G.bb;
        ledstrip.setPixelColor(matrix[h + 1][i + 6], pixelsR[matrix[h + 1][i + 6]], pixelsG[matrix[h + 1][i + 6]], pixelsB[matrix[h + 1][i + 6]]);
      }
    }
  }
  ledstrip.show();
}

//------------------------------------------------------------------------------

void laufen(unsigned int d, byte aktion) {

  unsigned int t, a;

  if (aktion == 0) {
    for (t = 0; t < NUM_SMATRIX; t++) {
      for (a = NUM_SMATRIX; a > 1; a--)
      {
        pixelsR[smatrix[a - 1]] = pixelsR[smatrix[a - 2]];
        pixelsG[smatrix[a - 1]] = pixelsG[smatrix[a - 2]];
        pixelsB[smatrix[a - 1]] = pixelsB[smatrix[a - 2]];
        ledstrip.setPixelColor(smatrix[a - 1], pixelsR[smatrix[a - 1]], pixelsG[smatrix[a - 1]], pixelsB[smatrix[a - 1]]);
      }
      pixelsR[smatrix[0]] = G.rr;
      pixelsG[smatrix[0]] = G.gg;
      pixelsB[smatrix[0]] = G.bb;
      ledstrip.setPixelColor(smatrix[0], pixelsR[smatrix[0]], pixelsG[smatrix[0]], pixelsB[smatrix[0]]);
      ledstrip.show();
      delay(d);
    }
  }
  if (aktion == 1) {
    for (t = 0; t < NUM_PIXELS; t++) {
      for (a = NUM_PIXELS; a > 1; a--)
      {
        //led1[a-1].r= led1[a-2].r;
      }
      //led1[0].r = led[G.anz_leds - t - 1].r;
      ledstrip.show();
      delay(d);
    }
  }
}

//------------------------------------------------------------------------------

void wischen(byte r, byte g, byte b, unsigned int d) {

  unsigned int t, u;

  for (t = 0; t < ROWS_MATRIX; t++)
  {
    for (u = 0; u < 11; u++)
    {
      pixelsR[matrix[t][u]] = r;
      pixelsG[matrix[t][u]] = g;
      pixelsB[matrix[t][u]] = b;
      ledstrip.setPixelColor(matrix[t][u], pixelsR[matrix[t][u]], pixelsG[matrix[t][u]], pixelsB[matrix[t][u]]);
    }
    if (t > 0) {
      for (u = 0; u < 11; u++)
      {
        pixelsR[matrix[t - 1][u]] = G.rr;
        pixelsG[matrix[t - 1][u]] = G.gg;
        pixelsB[matrix[t - 1][u]] = G.bb;
        ledstrip.setPixelColor(matrix[t - 1][u], pixelsR[matrix[t - 1][u]], pixelsG[matrix[t - 1][u]], pixelsB[matrix[t - 1][u]]);
      }
    }
    ledstrip.show();
    delay(d);
  }
  for (unsigned int u = 0; u < 11; u++)
  {
    pixelsR[matrix[t - 1][u]] = G.rr;
    pixelsG[matrix[t - 1][u]] = G.gg;
    pixelsB[matrix[t - 1][u]] = G.bb;
    ledstrip.setPixelColor(matrix[t - 1][u], pixelsR[matrix[t - 1][u]], pixelsG[matrix[t - 1][u]], pixelsB[matrix[t - 1][u]]);
  }
  ledstrip.show();
}

//------------------------------------------------------------------------------

void schieben(int d, byte aktion) {

  unsigned int t, a, b;

  if (aktion == 0) {
    for (t = 0; t < ROWS_MATRIX; t++)
    {
      for (a = ROWS_MATRIX - 1; a > 0; a--)
      {
        for (b = 0; b < 11; b++)
        {
          pixelsR[matrix[a][b]] = pixelsR[matrix[a - 1][b]];
          pixelsG[matrix[a][b]] = pixelsG[matrix[a - 1][b]];
          pixelsB[matrix[a][b]] = pixelsB[matrix[a - 1][b]];
          ledstrip.setPixelColor(matrix[a][b], pixelsR[matrix[a][b]], pixelsG[matrix[a][b]], pixelsB[matrix[a][b]]);
        }
      }
      for (b = 0; b < 11; b++)
      {
        pixelsR[matrix[a][b]] = G.rr;
        pixelsG[matrix[a][b]] = G.gg;
        pixelsB[matrix[a][b]] = G.bb;
        ledstrip.setPixelColor(matrix[a][b], pixelsR[matrix[a][b]], pixelsG[matrix[a][b]], pixelsB[matrix[a][b]]);
      }
      ledstrip.show();
      delay(d);
    }
  }
}

//------------------------------------------------------------------------------

void set_bg(void) {     // set baclground color to all LEDs

  for (int t = 0; t < ROWS_MATRIX; t++) {
    for (int b = 0; b < 11; b++) {
      led_set_bg(matrix[t][b]);
    }
  }
}

//------------------------------------------------------------------------------

void set_minute(byte m) {

  while (m > 4) {
    m -= 5;
  }
  if ( m > 0) {      // Minute 1 
    led_set(111);    // Minute-LEDs at the corners
  }
  if ( m > 1) {      // Minute 2
    led_set(112);
  }
  if ( m > 2) {      // Minute 3
    led_set(113);
  }
  if ( m > 3) {      // Minute 4
    led_set(110);
  }
}

//------------------------------------------------------------------------------

void set_stunde(byte _stunde, byte voll) {

  switch (_stunde) {
    case 0:  uhrzeit |= ((uint32_t)1 << H_ZWOELF);  break;
    case 1:
      if (voll == 1) {
        uhrzeit |= ((uint32_t)1 << H_EIN);
      }
      else          {
        uhrzeit |= ((uint32_t)1 << EINS);
      }
      break;
    case 2:  uhrzeit |= ((uint32_t)1 << H_ZWEI);  break;
    case 3:  uhrzeit |= ((uint32_t)1 << H_DREI);  break;
    case 4:  uhrzeit |= ((uint32_t)1 << H_VIER);  break;
    case 5:  uhrzeit |= ((uint32_t)1 << H_FUENF); break;
    case 6:  uhrzeit |= ((uint32_t)1 << H_SECHS); break;
    case 7:  uhrzeit |= ((uint32_t)1 << H_SIEBEN);  break;
    case 8:  uhrzeit |= ((uint32_t)1 << H_ACHT);  break;
    case 9:  uhrzeit |= ((uint32_t)1 << H_NEUN);  break;
    case 10: uhrzeit |= ((uint32_t)1 << H_ZEHN);  break;
    case 11: uhrzeit |= ((uint32_t)1 << H_ELF);   break;
    case 12: uhrzeit |= ((uint32_t)1 << H_ZWOELF);  break;
    case 13:
      if (voll == 1) {
        uhrzeit |= ((uint32_t)1 << H_EIN);
      }
      else          {
        uhrzeit |= ((uint32_t)1 << EINS);
      }
      break;
    case 14: uhrzeit |= ((uint32_t)1 << H_ZWEI);  break;
    case 15: uhrzeit |= ((uint32_t)1 << H_DREI);  break;
    case 16: uhrzeit |= ((uint32_t)1 << H_VIER);  break;
    case 17: uhrzeit |= ((uint32_t)1 << H_FUENF); break;
    case 18: uhrzeit |= ((uint32_t)1 << H_SECHS); break;
    case 19: uhrzeit |= ((uint32_t)1 << H_SIEBEN);  break;
    case 20: uhrzeit |= ((uint32_t)1 << H_ACHT);  break;
    case 21: uhrzeit |= ((uint32_t)1 << H_NEUN);  break;
    case 22: uhrzeit |= ((uint32_t)1 << H_ZEHN);  break;
    case 23: uhrzeit |= ((uint32_t)1 << H_ELF);   break;
    case 24: uhrzeit |= ((uint32_t)1 << H_ZWOELF);  break;
  }
}

//------------------------------------------------------------------------------

void set_zeit(byte animation) {

  byte m;

  if (uhrzeit & ((uint32_t)1 << ESIST))   {
    es_ist();
  }
  if (uhrzeit & ((uint32_t)1 << VOR))     {
    vor();
  }
  if (uhrzeit & ((uint32_t)1 << NACH))    {
    nach();
  }
  if (uhrzeit & ((uint32_t)1 << UHR))     {
    uhr();
  }
  if (uhrzeit & ((uint32_t)1 << FUENF))   {
    fuenf();
  }
  if (uhrzeit & ((uint32_t)1 << ZEHN))    {
    zehn();
  }
  if (uhrzeit & ((uint32_t)1 << VIERTEL)) {
    viertel();
  }
  if (uhrzeit & ((uint32_t)1 << DREIVIERTEL)) {
    dreiviertel();
  }
  if (uhrzeit & ((uint32_t)1 << ZWANZIG)) {
    zwanzig();
  }
  if (uhrzeit & ((uint32_t)1 << HALB))    {
    halb();
  }
  if (uhrzeit & ((uint32_t)1 << EINS))    {
    eins();
  }
  if (uhrzeit & ((uint32_t)1 << H_EIN ))  {
    h_ein();
  }
  if (uhrzeit & ((uint32_t)1 << H_ZWEI))  {
    h_zwei();
  }
  if (uhrzeit & ((uint32_t)1 << H_DREI )) {
    h_drei();
  }
  if (uhrzeit & ((uint32_t)1 << H_VIER )) {
    h_vier();
  }
  if (uhrzeit & ((uint32_t)1 << H_FUENF)) {
    h_fuenf();
  }
  if (uhrzeit & ((uint32_t)1 << H_SECHS)) {
    h_sechs();
  }
  if (uhrzeit & ((uint32_t)1 << H_SIEBEN)) {
    h_sieben();
  }
  if (uhrzeit & ((uint32_t)1 << H_ACHT))  {
    h_acht();
  }
  if (uhrzeit & ((uint32_t)1 << H_NEUN))  {
    h_neun();
  }
  if (uhrzeit & ((uint32_t)1 << H_ZEHN))  {
    h_zehn();
  }
  if (uhrzeit & ((uint32_t)1 << H_ELF))   {
    h_elf();
  }
  if (uhrzeit & ((uint32_t)1 << H_ZWOELF)) {
    h_zwoelf();
  }

  m = _minute;
  set_minute(m);
  if (m > 0) {
    animation = 99;
  }
  /*
    switch (animation) {
    case 0: schieben(80, 0);  laufen(20, 1); break;
    case 1: wischen(100, 0, 0, 80);    break;
    case 2: schieben(50, 0);  break;
    case 3: wischen(100, 0, 0, 50);    break;
    case 4: schieben(50, 0);  break;
    case 5: wischen(100, 0, 0, 50);    break;
    case 6: schieben(50, 0);  break;
    case 7: wischen(100, 0, 0, 50);    break;
    case 8: schieben(50, 0);  break;
    case 9: wischen(100, 0, 0, 50);    break;
    case 10:wischen(100, 0, 0, 50);    break;
    case 11:schieben(50, 0);  break;
    }
  */
}

//------------------------------------------------------------------------------

void set_uhrzeit() {

  unsigned int letters = 0;
  uhrzeit = 0;
  uhrzeit |= ((uint32_t)1 << ESIST);

  letters = _minute / 5;
  switch (letters) {
    case 0: // volle Stunde
      uhrzeit |= ((uint32_t)1 << UHR);
      set_stunde(_stunde, 1);
      break;
    case 1: // 5 nach
      uhrzeit |= ((uint32_t)1 << FUENF);
      uhrzeit |= ((uint32_t)1 << NACH);
      set_stunde(_stunde, 0);
      break;
    case 2: // 10 nach
      uhrzeit |= ((uint32_t)1 << ZEHN);
      uhrzeit |= ((uint32_t)1 << NACH);
      set_stunde(_stunde, 0);
      break;
    case 3: // viertel (hochdt. viertel nach)
      uhrzeit |= ((uint32_t)1 << VIERTEL);
      //uhrzeit |= ((uint32_t)1 << NACH);
      set_stunde(_stunde + 1, 0);
      break;
    case 4: // 20 nach
      uhrzeit |= ((uint32_t)1 << ZWANZIG);
      uhrzeit |= ((uint32_t)1 << NACH);
      set_stunde(_stunde, 0);
      break;
    case 5: // 5 vor halb
      uhrzeit |= ((uint32_t)1 << FUENF);
      uhrzeit |= ((uint32_t)1 << VOR);
      uhrzeit |= ((uint32_t)1 << HALB);
      set_stunde(_stunde + 1, 0);
      break;
    case 6: // halb
      uhrzeit |= ((uint32_t)1 << HALB);
      set_stunde(_stunde + 1, 0);
      break;
    case 7: // 5 nach halb
      uhrzeit |= ((uint32_t)1 << FUENF);
      uhrzeit |= ((uint32_t)1 << NACH);
      uhrzeit |= ((uint32_t)1 << HALB);
      set_stunde(_stunde + 1, 0);
      break;
    case 8: // 20 vor
      uhrzeit |= ((uint32_t)1 << ZWANZIG);
      uhrzeit |= ((uint32_t)1 << VOR);
      set_stunde(_stunde + 1, 0);
      break;
    case 9: // dreiviertel (hochdt. viertel vor)
      uhrzeit |= ((uint32_t)1 << DREIVIERTEL);
      //uhrzeit |= ((uint32_t)1 << VOR);
      set_stunde(_stunde + 1, 0);
      break;
    case 10: // 10 vor
      uhrzeit |= ((uint32_t)1 << ZEHN);
      uhrzeit |= ((uint32_t)1 << VOR);
      set_stunde(_stunde + 1, 0);
      break;
    case 11: // 5 vor
      uhrzeit |= ((uint32_t)1 << FUENF);
      uhrzeit |= ((uint32_t)1 << VOR);
      set_stunde(_stunde + 1, 0);
      break;
  }
  set_zeit(letters);
}

//------------------------------------------------------------------------------

