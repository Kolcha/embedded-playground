#include "fonts.h"

#include "fonts/drdos8x6.h"
#include "fonts/drdos8x8.h"
#include "fonts/lat1-08.h"
#include "fonts/lat1-16.h"
#include "fonts/lat2-08.h"
#include "fonts/lat2-16.h"
#include "fonts/lat2-terminus16.h"

const bmp_font_t drdos8x6 = {
  .w = FONT_CHAR_W_DRDOS8X6,
  .h = FONT_CHAR_H_DRDOS8X6,
  .n = FONT_LENGTH_DRDOS8X6,
  .csz = FONT_CHARSZ_DRDOS8X6,
  .data = font_data_drdos8x6,
};

const bmp_font_t drdos8x8 = {
  .w = FONT_CHAR_W_DRDOS8X8,
  .h = FONT_CHAR_H_DRDOS8X8,
  .n = FONT_LENGTH_DRDOS8X8,
  .csz = FONT_CHARSZ_DRDOS8X8,
  .data = font_data_drdos8x8,
};

const bmp_font_t lat1_08 = {
  .w = FONT_CHAR_W_LAT1_08,
  .h = FONT_CHAR_H_LAT1_08,
  .n = FONT_LENGTH_LAT1_08,
  .csz = FONT_CHARSZ_LAT1_08,
  .data = font_data_lat1_08,
};

const bmp_font_t lat1_16 = {
  .w = FONT_CHAR_W_LAT1_16,
  .h = FONT_CHAR_H_LAT1_16,
  .n = FONT_LENGTH_LAT1_16,
  .csz = FONT_CHARSZ_LAT1_16,
  .data = font_data_lat1_16,
};

const bmp_font_t lat2_08 = {
  .w = FONT_CHAR_W_LAT2_08,
  .h = FONT_CHAR_H_LAT2_08,
  .n = FONT_LENGTH_LAT2_08,
  .csz = FONT_CHARSZ_LAT2_08,
  .data = font_data_lat2_08,
};

const bmp_font_t lat2_16 = {
  .w = FONT_CHAR_W_LAT2_16,
  .h = FONT_CHAR_H_LAT2_16,
  .n = FONT_LENGTH_LAT2_16,
  .csz = FONT_CHARSZ_LAT2_16,
  .data = font_data_lat2_16,
};

const bmp_font_t lat2_terminus16 = {
  .w = FONT_CHAR_W_LAT2_TERMINUS16,
  .h = FONT_CHAR_H_LAT2_TERMINUS16,
  .n = FONT_LENGTH_LAT2_TERMINUS16,
  .csz = FONT_CHARSZ_LAT2_TERMINUS16,
  .data = font_data_lat2_terminus16,
};
