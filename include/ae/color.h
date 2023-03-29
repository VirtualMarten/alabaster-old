#ifndef __COLOR_H__
#define __COLOR_H__

#include <limits.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

typedef struct {
    uint8_t r, g, b, a;
} color_t;

#define _GET_COLOR(_1, _2, _3, _4, NAME, ...) NAME
#define color(...) _GET_COLOR(__VA_ARGS__, _COLOR4, _COLOR3, _COLOR3, _COLOR1)(__VA_ARGS__)
#define _COLOR1(hex) (color_t){ (hex & 0xff000000) >> 24, (hex & 0x00ff0000) >> 16, (hex & 0x0000ff00) >> 8, hex & 0x000000ff }
#define _COLOR3(r, g, b) (color_t){ r, g, b }
#define _COLOR4(r, g, b, a) (color_t){ r, g, b, a }

#define _GET_COLORF(_1, _2, _3, _4, NAME, ...) NAME
#define colorf(...) _GET_COLORF(__VA_ARGS__, _COLORF4, _COLORF3, _COLORF3)(__VA_ARGS__)
#define _COLORF1(hex) (color_t){ (hex & 0xff000000) >> 24, (hex & 0x00ff0000) >> 16, (hex & 0x0000ff00) >> 8, hex & 0x000000ff }
#define _COLORF3(r, g, b) (color_t){ r, g, b }
#define _COLORF4(r, g, b, a) (color_t){ r, g, b, a }

static inline color_t rgb(float r, float g, float b) {
    return (color_t){ r * UINT8_MAX, g * UINT8_MAX, b * UINT8_MAX, UINT8_MAX };
    // return 0x000000ff |
    //     ((uint8_t)(b * UINT8_MAX) << 24) |
    //     ((uint8_t)(g * UINT8_MAX) << 16) |
    //     (uint8_t)(r * UINT8_MAX) << 8;
}

static inline color_t rgba(float r, float g, float b, float a) {
    return (color_t){ r * UINT8_MAX, g * UINT8_MAX, b * UINT8_MAX, a * UINT8_MAX };
    // return ((uint8_t)(r * UINT8_MAX) << 24) |
    //     ((uint8_t)(g * UINT8_MAX) << 16) |
    //     ((uint8_t)(b * UINT8_MAX) << 8) |
    //     (uint8_t)(a * UINT8_MAX);
}

static inline color_t rgb8(uint8_t r, uint8_t g, uint8_t b) {
    return (color_t){ r, g, b, UINT8_MAX };
    //return 0x000000ff | (r << 24) | (g << 16) | b << 8;
}

static inline color_t rgba8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return (color_t){ r, g, b, a };
    //return (r << 24) | (g << 16) | (b << 8) | a;
}

// static inline uint8_t red8(color_t c) {
//     return (c & 0xff000000) >> 24;
// }

// static inline uint8_t green8(color_t c) {
//     return (c & 0x00ff0000) >> 16;
// }

// static inline uint8_t blue8(color_t c) {
//     return (c & 0x0000ff00) >> 8;
// }

// static inline uint8_t alpha8(color_t c) {
//     return c & 0x000000ff;
// }

#define unpack_rgb8(c) c.r,c.g,c.b
#define unpack_rgba8(c) c.r,c.g,c.b,c.a
#define unpack_rgb(c) c.r/(float)UINT8_MAX,c.g/(float)UINT8_MAX,c.b/(float)UINT8_MAX
#define unpack_rgba(c) c.r/(float)UINT8_MAX,c.g/(float)UINT8_MAX,c.b/(float)UINT8_MAX,c.a/(float)UINT8_MAX

//#define unpack_rgb8(c) ((c&0xff000000)>>24),((c&0x00ff0000)>>16),((c&0x0000ff00)>>8)
//#define unpack_rgba8(c) ((c&0xff000000)>>24),((c&0x00ff0000)>>16),((c&0x0000ff00)>>8),(c&0x000000ff)
//#define unpack_rgb(c) ((c&0xff000000)>>24)/(float)UINT8_MAX,((c&0x00ff0000)>>16)/(float)UINT8_MAX,((c&0x0000ff00)>>8)/(float)UINT8_MAX
//#define unpack_rgba(c) ((c&0xff000000)>>24)/(float)UINT8_MAX,((c&0x00ff0000)>>16)/(float)UINT8_MAX,((c&0x0000ff00)>>8)/(float)UINT8_MAX,(c&0x000000ff)/(float)UINT8_MAX

// static inline float red(color_t c) {
//     return ((c & 0xff000000) >> 24) / (float)UINT8_MAX;
// }

// static inline float green(color_t c) {
//     return ((c & 0x00ff0000) >> 16) / (float)UINT8_MAX;
// }

// static inline float blue(color_t c) {
//     return ((c & 0x0000ff00) >> 8) / (float)UINT8_MAX;
// }

// static inline float alpha(color_t c) {
//     return (c & 0x000000ff) / (float)UINT8_MAX;
// }

#define C_BLACK                   rgb8(0, 0, 0)//0x000000FF
#define C_NAVY                    rgb8(0, 0, 128)//0x000080FF
#define C_DARK_BLUE               rgb8(0, 0, 139)//0x00008BFF
#define C_MEDIUM_BLUE             rgb8(0, 0, 205)//0x0000CDFF
#define C_BLUE                    rgb8(0, 0, 255)//0x0000FFFF
#define C_DARK_GREEN              rgb8(0, 100, 0)//0x006400FF
#define C_GREEN                   rgb8(0, 128, 0)//0x008000FF
#define C_TEAL                    rgb8(0, 128, 128)//0x008080FF
#define C_DARK_CYAN               rgb8(0, 139, 139)//0x008B8BFF
#define C_DEEP_SKY_BLUE           rgb8(0, 191, 255)//0x00BFFFFF
#define C_DARK_TURQUOISE          rgb8(0, 206, 209)//0x00CED1FF
#define C_MEDIUM_SPRING_GREEN     rgb8(0, 250, 154)//0x00FA9AFF
#define C_LIME                    rgb8(0, 255, 0)//0x00FF00FF
#define C_SPRING_GREEN            rgb8(0, 255, 127)//0x00FF7FFF
#define C_AQUA                    rgb8(0, 255, 255)//0x00FFFFFF
#define C_CYAN                    rgb8(0, 255, 255)//0x00FFFFFF
#define C_MIDNIGHT_BLUE           rgb8(25, 25, 112)//0x191970FF
#define C_DODGER_BLUE             rgb8(30, 144, 255)//0x1E90FFFF
#define C_LIGHT_SEA_GREEN         rgb8(32, 178, 170)//0x20B2AAFF
#define C_FOREST_GREEN            rgb8(34, 139, 34)//0x228B22FF
#define C_SEA_GREEN               rgb8(46, 139, 87)//0x2E8B57FF
#define C_DARK_SLATE_GRAY         rgb8(47, 79, 79)//0x2F4F4FFF
#define C_DARK_SLATE_GREY         rgb8(47, 79, 79)//0x2F4F4FFF
#define C_LIME_GREEN              rgb8(50, 205, 50)//0x32CD32FF
#define C_MEDIUM_SEA_GREEN        rgb8(60, 179, 113)//0x3CB371FF
#define C_TURQUOISE               rgb8(64, 224, 208)//0x40E0D0FF
#define C_ROYAL_BLUE              rgb8(65, 105, 225)//0x4169E1FF
#define C_STEEL_BLUE              rgb8(70, 130, 180)//0x4682B4FF
#define C_DARK_SLATE_BLUE         rgb8(72, 61, 139)//0x483D8BFF
#define C_MEDIUM_TURQUOISE        rgb8(72, 209, 204)//0x48D1CCFF
#define C_INDIGO                  rgb8(75, 0, 130)//0x4B0082FF
#define C_DARK_OLIVE_GREEN        rgb8(85, 107, 47)//0x556B2FFF
#define C_CADET_BLUE              rgb8(95, 158, 160)//0x5F9EA0FF
#define C_CORNFLOWER_BLUE         rgb8(100, 149, 237)//0x6495EDFF
#define C_REBECCA_PURPLE          rgb8(102, 51, 153)//0x663399FF
#define C_MEDIUM_AQUA_MARINE      rgb8(102, 205, 170)//0x66CDAAFF
#define C_DIM_GRAY                rgb8(105, 105, 105)//0x696969FF
#define C_DIM_GREY                rgb8(105, 105, 105)//0x696969FF
#define C_SLATE_BLUE              rgb8(106, 90, 205)//0x6A5ACDFF
#define C_OLIVE_DRAB              rgb8(107, 142, 35)//0x6B8E23FF
#define C_SLATE_GRAY              rgb8(112, 128, 144)//0x708090FF
#define C_SLATE_GREY              rgb8(112, 128, 144)//0x708090FF
#define C_LIGHT_SLATE_GRAY        rgb8(119, 136, 153)//0x778899FF
#define C_LIGHT_SLATE_GREY        rgb8(119, 136, 153)//0x778899FF
#define C_MEDIUM_SLATE_BLUE       rgb8(123, 104, 238)//0x7B68EEFF
#define C_LAWN_GREEN              rgb8(124, 252, 0)//0x7CFC00FF
#define C_CHARTREUSE              rgb8(127, 255, 0)//0x7FFF00FF
#define C_AQUAMARINE              rgb8(127, 255, 212)//0x7FFFD4FF
#define C_MAROON                  rgb8(128, 0, 0)//0x800000FF
#define C_PURPLE                  rgb8(128, 0, 128)//0x800080FF
#define C_OLIVE                   rgb8(128, 128, 0)//0x808000FF
#define C_GRAY                    rgb8(128, 128, 128)//0x808080FF
#define C_GREY                    rgb8(128, 128, 128)//0x808080FF
#define C_SKY_BLUE                rgb8(135, 206, 235)//0x87CEEBFF
#define C_LIGHT_SKY_BLUE          rgb8(135, 206, 250)//0x87CEFAFF
#define C_BLUE_VIOLET             rgb8(138, 43, 226)//0x8A2BE2FF
#define C_DARK_RED                rgb8(139, 0, 0)//0x8B0000FF
#define C_DARK_MAGENTA            rgb8(139, 0, 139)//0x8B008BFF
#define C_SADDLE_BROWN            rgb8(139, 69, 19)//0x8B4513FF
#define C_DARK_SEA_GREEN          rgb8(143, 188, 143)//0x8FBC8FFF
#define C_LIGHT_GREEN             rgb8(144, 238, 144)//0x90EE90FF
#define C_MEDIUM_PURPLE           rgb8(147, 112, 219)//0x9370DBFF
#define C_DARK_VIOLET             rgb8(148, 0, 211)//0x9400D3FF
#define C_PALE_GREEN              rgb8(152, 251, 152)//0x98FB98FF
#define C_DARK_ORCHID             rgb8(153, 50, 204)//0x9932CCFF
#define C_YELLOW_GREEN            rgb8(154, 205, 50)//0x9ACD32FF
#define C_SIENNA                  rgb8(160, 82, 45)//0xA0522DFF
#define C_BROWN                   rgb8(165, 42, 42)//0xA52A2AFF
#define C_DARK_GRAY               rgb8(169, 169, 169)//0xA9A9A9FF
#define C_DARK_GREY               rgb8(169, 169, 169)//0xA9A9A9FF
#define C_LIGHT_BLUE              rgb8(173, 216, 230)//0xADD8E6FF
#define C_GREEN_YELLOW            rgb8(173, 255, 47)//0xADFF2FFF
#define C_PALE_TURQUOISE          rgb8(175, 238, 238)//0xAFEEEEFF
#define C_LIGHT_STEEL_BLUE        rgb8(176, 196, 222)//0xB0C4DEFF
#define C_POWDER_BLUE             rgb8(176, 224, 230)//0xB0E0E6FF
#define C_FIRE_BRICK              rgb8(178, 34, 34)//0xB22222FF
#define C_DARK_GOLDEN_ROD         rgb8(184, 134, 11)//0xB8860BFF
#define C_MEDIUM_ORCHID           rgb8(186, 85, 211)//0xBA55D3FF
#define C_ROSY_BROWN              rgb8(188, 143, 143)//0xBC8F8FFF
#define C_DARK_KHAKI              rgb8(189, 183, 107)//0xBDB76BFF
#define C_SILVER                  rgb8(192, 192, 192)//0xC0C0C0FF
#define C_MEDIUM_VIOLET_RED       rgb8(199, 21, 133)//0xC71585FF
#define C_INDIAN_RED              rgb8(205, 92, 92)//0xCD5C5CFF
#define C_PERU                    rgb8(205, 133, 63)//0xCD853FFF
#define C_CHOCOLATE               rgb8(210, 105, 30)//0xD2691EFF
#define C_TAN                     rgb8(210, 180, 140)//0xD2B48CFF
#define C_LIGHT_GRAY              rgb8(211, 211, 211)//0xD3D3D3FF
#define C_LIGHT_GREY              rgb8(211, 211, 211)//0xD3D3D3FF
#define C_THISTLE                 rgb8(216, 191, 216)//0xD8BFD8FF
#define C_ORCHID                  rgb8(218, 112, 214)//0xDA70D6FF
#define C_GOLDEN_ROD              rgb8(218, 165, 32)//0xDAA520FF
#define C_PALE_VIOLET_RED         rgb8(219, 112, 147)//0xDB7093FF
#define C_CRIMSON                 rgb8(220, 20, 60)//0xDC143CFF
#define C_GAINSBORO               rgb8(220, 220, 220)//0xDCDCDCFF
#define C_PLUM                    rgb8(221, 160, 221)//0xDDA0DDFF
#define C_BURLY_WOOD              rgb8(222, 184, 135)//0xDEB887FF
#define C_LIGHT_CYAN              rgb8(224, 255, 255)//0xE0FFFFFF
#define C_LAVENDER                rgb8(230, 230, 250)//0xE6E6FAFF
#define C_DARK_SALMON             rgb8(233, 150, 122)//0xE9967AFF
#define C_VIOLET                  rgb8(238, 130, 238)//0xEE82EEFF
#define C_PALE_GOLDEN_ROD         rgb8(238, 232, 170)//0xEEE8AAFF
#define C_LIGHT_CORAL             rgb8(240, 128, 128)//0xF08080FF
#define C_KHAKI                   rgb8(240, 230, 140)//0xF0E68CFF
#define C_ALICE_BLUE              rgb8(240, 248, 255)//0xF0F8FFFF
#define C_HONEY_DEW               rgb8(240, 255, 240)//0xF0FFF0FF
#define C_AZURE                   rgb8(240, 255, 255)//0xF0FFFFFF
#define C_SANDY_BROWN             rgb8(244, 164, 96)//0xF4A460FF
#define C_WHEAT                   rgb8(245, 222, 179)//0xF5DEB3FF
#define C_BEIGE                   rgb8(245, 245, 220)//0xF5F5DCFF
#define C_WHITE_SMOKE             rgb8(245, 245, 245)//0xF5F5F5FF
#define C_MINT_CREAM              rgb8(245, 255, 250)//0xF5FFFAFF
#define C_GHOST_WHITE             rgb8(248, 248, 255)//0xF8F8FFFF
#define C_SALMON                  rgb8(250, 128, 114)//0xFA8072FF
#define C_ANTIQUE_WHITE           rgb8(250, 235, 215)//0xFAEBD7FF
#define C_LINEN                   rgb8(250, 240, 230)//0xFAF0E6FF
#define C_LIGHT_GOLDEN_ROD_YELLOW rgb8(250, 250, 210)//0xFAFAD2FF
#define C_OLD_LACE                rgb8(253, 245, 230)//0xFDF5E6FF
#define C_RED                     rgb8(255, 0, 0)//0xFF0000FF
#define C_FUCHSIA                 rgb8(255, 0, 255)//0xFF00FFFF
#define C_MAGENTA                 rgb8(255, 0, 255)//0xFF00FFFF
#define C_DEEP_PINK               rgb8(255, 20, 147)//0xFF1493FF
#define C_ORANGE_RED              rgb8(255, 69, 0)//0xFF4500FF
#define C_TOMATO                  rgb8(255, 99, 71)//0xFF6347FF
#define C_HOT_PINK                rgb8(255, 105, 180)//0xFF69B4FF
#define C_CORAL                   rgb8(255, 127, 80)//0xFF7F50FF
#define C_DARK_ORANGE             rgb8(255, 140, 0)//0xFF8C00FF
#define C_LIGHT_SALMON            rgb8(255, 160, 122)//0xFFA07AFF
#define C_ORANGE                  rgb8(255, 165, 0)//0xFFA500FF
#define C_LIGHT_PINK              rgb8(255, 182, 193)//0xFFB6C1FF
#define C_PINK                    rgb8(255, 192, 203)//0xFFC0CBFF
#define C_GOLD                    rgb8(255, 215, 0)//0xFFD700FF
#define C_PEACH_PUFF              rgb8(255, 218, 185)//0xFFDAB9FF
#define C_NAVAJO_WHITE            rgb8(255, 222, 173)//0xFFDEADFF
#define C_MOCCASIN                rgb8(255, 228, 181)//0xFFE4B5FF
#define C_BISQUE                  rgb8(255, 228, 196)//0xFFE4C4FF
#define C_MISTY_ROSE              rgb8(255, 228, 225)//0xFFE4E1FF
#define C_BLANCHED_ALMOND         rgb8(255, 235, 205)//0xFFEBCDFF
#define C_PAPAYA_WHIP             rgb8(255, 239, 213)//0xFFEFD5FF
#define C_LAVENDER_BLUSH          rgb8(255, 240, 245)//0xFFF0F5FF
#define C_SEA_SHELL               rgb8(255, 245, 238)//0xFFF5EEFF
#define C_CORNSILK                rgb8(255, 248, 220)//0xFFF8DCFF
#define C_LEMON_CHIFFON           rgb8(255, 250, 205)//0xFFFACDFF
#define C_FLORAL_WHITE            rgb8(255, 250, 240)//0xFFFAF0FF
#define C_SNOW                    rgb8(255, 250, 250)//0xFFFAFAFF
#define C_YELLOW                  rgb8(255, 255, 0)//0xFFFF00FF
#define C_LIGHT_YELLOW            rgb8(255, 255, 224)//0xFFFFE0FF
#define C_IVORY                   rgb8(255, 255, 240)//0xFFFFF0FF
#define C_WHITE                   rgb8(255, 255, 255)//0xFFFFFFFF

// #define black                   = 0x000000ff;//rgb8(0, 0, 0);
// #define navy                    = rgb8(0, 0, 128);
// #define dark_blue               = rgb8(0, 0, 139);
// #define medium_blue             = rgb8(0, 0, 205);
// #define blue                    = rgb8(0, 0, 255);
// #define dark_green              = rgb8(0, 100, 0);
// #define green                   = rgb8(0, 128, 0);
// #define teal                    = rgb8(0, 128, 128);
// #define dark_cyan               = rgb8(0, 139, 139);
// #define deep_sky_blue           = rgb8(0, 191, 255);
// #define dark_turquoise          = rgb8(0, 206, 209);
// #define medium_spring_green     = rgb8(0, 250, 154);
// #define lime                    = rgb8(0, 255, 0);
// #define spring_green            = rgb8(0, 255, 127);
// #define aqua                    = rgb8(0, 255, 255);
// #define cyan                    = rgb8(0, 255, 255);
// #define midnight_blue           = rgb8(25, 25, 112);
// #define dodger_blue             = rgb8(30, 144, 255);
// #define light_sea_green         = rgb8(32, 178, 170);
// #define forest_green            = rgb8(34, 139, 34);
// #define sea_green               = rgb8(46, 139, 87);
// #define dark_slate_gray         = rgb8(47, 79, 79);
// #define dark_slate_grey         = rgb8(47, 79, 79);
// #define lime_green              = rgb8(50, 205, 50);
// #define medium_sea_green        = rgb8(60, 179, 113);
// #define turquoise               = rgb8(64, 224, 208);
// #define royal_blue              = rgb8(65, 105, 225);
// #define steel_blue              = rgb8(70, 130, 180);
// #define dark_slate_blue         = rgb8(72, 61, 139);
// #define medium_turquoise        = rgb8(72, 209, 204);
// #define indigo                  = rgb8(75, 0, 130);
// #define dark_olive_green        = rgb8(85, 107, 47);
// #define cadet_blue              = rgb8(95, 158, 160);
// #define cornflower_blue         = rgb8(100, 149, 237);
// #define rebecca_purple          = rgb8(102, 51, 153);
// #define medium_aqua_marine      = rgb8(102, 205, 170);
// #define dim_gray                = rgb8(105, 105, 105);
// #define dim_grey                = rgb8(105, 105, 105);
// #define slate_blue              = rgb8(106, 90, 205);
// #define olive_drab              = rgb8(107, 142, 35);
// #define slate_gray              = rgb8(112, 128, 144);
// #define slate_grey              = rgb8(112, 128, 144);
// #define light_slate_gray        = rgb8(119, 136, 153);
// #define light_slate_grey        = rgb8(119, 136, 153);
// #define medium_slate_blue       = rgb8(123, 104, 238);
// #define lawn_green              = rgb8(124, 252, 0);
// #define chartreuse              = rgb8(127, 255, 0);
// #define aquamarine              = rgb8(127, 255, 212);
// #define maroon                  = rgb8(128, 0, 0);
// #define purple                  = rgb8(128, 0, 128);
// #define olive                   = rgb8(128, 128, 0);
// #define gray                    = rgb8(128, 128, 128);
// #define grey                    = rgb8(128, 128, 128);
// #define sky_blue                = rgb8(135, 206, 235);
// #define light_sky_blue          = rgb8(135, 206, 250);
// #define blue_violet             = rgb8(138, 43, 226);
// #define dark_red                = rgb8(139, 0, 0);
// #define dark_magenta            = rgb8(139, 0, 139);
// #define saddle_brown            = rgb8(139, 69, 19);
// #define dark_sea_green          = rgb8(143, 188, 143);
// #define light_green             = rgb8(144, 238, 144);
// #define medium_purple           = rgb8(147, 112, 219);
// #define dark_violet             = rgb8(148, 0, 211);
// #define pale_green              = rgb8(152, 251, 152);
// #define dark_orchid             = rgb8(153, 50, 204);
// #define yellow_green            = rgb8(154, 205, 50);
// #define sienna                  = rgb8(160, 82, 45);
// #define brown                   = rgb8(165, 42, 42);
// #define dark_gray               = rgb8(169, 169, 169);
// #define dark_grey               = rgb8(169, 169, 169);
// #define light_blue              = rgb8(173, 216, 230);
// #define green_yellow            = rgb8(173, 255, 47);
// #define pale_turquoise          = rgb8(175, 238, 238);
// #define light_steel_blue        = rgb8(176, 196, 222);
// #define powder_blue             = rgb8(176, 224, 230);
// #define fire_brick              = rgb8(178, 34, 34);
// #define dark_golden_rod         = rgb8(184, 134, 11);
// #define medium_orchid           = rgb8(186, 85, 211);
// #define rosy_brown              = rgb8(188, 143, 143);
// #define dark_khaki              = rgb8(189, 183, 107);
// #define silver                  = rgb8(192, 192, 192);
// #define medium_violet_red       = rgb8(199, 21, 133);
// #define indian_red              = rgb8(205, 92, 92);
// #define peru                    = rgb8(205, 133, 63);
// #define chocolate               = rgb8(210, 105, 30);
// #define tan                     = rgb8(210, 180, 140);
// #define light_gray              = rgb8(211, 211, 211);
// #define light_grey              = rgb8(211, 211, 211);
// #define thistle                 = rgb8(216, 191, 216);
// #define orchid                  = rgb8(218, 112, 214);
// #define golden_rod              = rgb8(218, 165, 32);
// #define pale_violet_red         = rgb8(219, 112, 147);
// #define crimson                 = rgb8(220, 20, 60);
// #define gainsboro               = rgb8(220, 220, 220);
// #define plum                    = rgb8(221, 160, 221);
// #define burly_wood              = rgb8(222, 184, 135);
// #define light_cyan              = rgb8(224, 255, 255);
// #define lavender                = rgb8(230, 230, 250);
// #define dark_salmon             = rgb8(233, 150, 122);
// #define violet                  = rgb8(238, 130, 238);
// #define pale_golden_rod         = rgb8(238, 232, 170);
// #define light_coral             = rgb8(240, 128, 128);
// #define khaki                   = rgb8(240, 230, 140);
// #define alice_blue              = rgb8(240, 248, 255);
// #define honey_dew               = rgb8(240, 255, 240);
// #define azure                   = rgb8(240, 255, 255);
// #define sandy_brown             = rgb8(244, 164, 96);
// #define wheat                   = rgb8(245, 222, 179);
// #define beige                   = rgb8(245, 245, 220);
// #define white_smoke             = rgb8(245, 245, 245);
// #define mint_cream              = rgb8(245, 255, 250);
// #define ghost_white             = rgb8(248, 248, 255);
// #define salmon                  = rgb8(250, 128, 114);
// #define antique_white           = rgb8(250, 235, 215);
// #define linen                   = rgb8(250, 240, 230);
// #define light_golden_rod_yellow = rgb8(250, 250, 210);
// #define old_lace                = rgb8(253, 245, 230);
// #define red                     = rgb8(255, 0, 0);
// #define fuchsia                 = rgb8(255, 0, 255);
// #define magenta                 = rgb8(255, 0, 255);
// #define deep_pink               = rgb8(255, 20, 147);
// #define orange_red              = rgb8(255, 69, 0);
// #define tomato                  = rgb8(255, 99, 71);
// #define hot_pink                = rgb8(255, 105, 180);
// #define coral                   = rgb8(255, 127, 80);
// #define dark_orange             = rgb8(255, 140, 0);
// #define light_salmon            = rgb8(255, 160, 122);
// #define orange                  = rgb8(255, 165, 0);
// #define light_pink              = rgb8(255, 182, 193);
// #define pink                    = rgb8(255, 192, 203);
// #define gold                    = rgb8(255, 215, 0);
// #define peach_puff              = rgb8(255, 218, 185);
// #define navajo_white            = rgb8(255, 222, 173);
// #define moccasin                = rgb8(255, 228, 181);
// #define bisque                  = rgb8(255, 228, 196);
// #define misty_rose              = rgb8(255, 228, 225);
// #define blanched_almond         = rgb8(255, 235, 205);
// #define papaya_whip             = rgb8(255, 239, 213);
// #define lavender_blush          = rgb8(255, 240, 245);
// #define sea_shell               = rgb8(255, 245, 238);
// #define cornsilk                = rgb8(255, 248, 220);
// #define lemon_chiffon           = rgb8(255, 250, 205);
// #define floral_white            = rgb8(255, 250, 240);
// #define snow                    = rgb8(255, 250, 250);
// #define yellow                  = rgb8(255, 255, 0);
// #define light_yellow            = rgb8(255, 255, 224);
// #define ivory                   = rgb8(255, 255, 240);
// #define white                   = rgb8(255, 255, 255);

// typedef union color_s {
//     struct { uint8_t r, g, b, a; };
//     uint8_t data[4];
// } color_t;

// #define rgb(r,g,b) ((color_t){{r,g,b,255}})
// #define rgba(r,g,b,a) ((color_t){{r,g,b,a}})
// #define frgb(r,g,b) rgb(r*255,g*255,b*255)
// #define frgba(r,g,b,a) rgba(r*255,g*255,b*255,a*255)

// #define const_rgb(r,g,b) {{r,g,b,255}}
// #define const_rgba(r,g,b,a) {{r,g,b,a}}
// #define const_frgb(r,g,b) const_rgb(r*255,g*255,b*255)
// #define const_frgba(r,g,b,a) const_rgba(r*255,g*255,b*255,a*255)

// #define unpack_rgb(c) c.r,c.g,c.b
// #define unpack_rgba(c) c.r,c.g,c.b,c.a
// #define unpack_frgb(c) c.r/255.0f,c.g/255.0f,c.b/255.0f
// #define unpack_frgba(c) c.r/255.0f,c.g/255.0f,c.b/255.0f,c.a/255.0f

//color_t color(const char *src);

//color_t color_from_hex(const char *src);

// const color_t black;
// const color_t navy;
// const color_t dark_blue;
// const color_t medium_blue;
// const color_t blue;
// const color_t dark_green;
// const color_t green;
// const color_t teal;
// const color_t dark_cyan;
// const color_t deep_sky_blue;
// const color_t dark_turquoise;
// const color_t medium_spring_green;
// const color_t lime;
// const color_t spring_green;
// const color_t aqua;
// const color_t cyan;
// const color_t midnight_blue;
// const color_t dodger_blue;
// const color_t light_sea_green;
// const color_t forest_green;
// const color_t sea_green;
// const color_t dark_slate_gray;
// const color_t dark_slate_grey;
// const color_t lime_green;
// const color_t medium_sea_green;
// const color_t turquoise;
// const color_t royal_blue;
// const color_t steel_blue;
// const color_t dark_slate_blue;
// const color_t medium_turquoise;
// const color_t indigo;
// const color_t dark_olive_green;
// const color_t cadet_blue;
// const color_t cornflower_blue;
// const color_t rebecca_purple;
// const color_t medium_aqua_marine;
// const color_t dim_gray;
// const color_t dim_grey;
// const color_t slate_blue;
// const color_t olive_drab;
// const color_t slate_gray;
// const color_t slate_grey;
// const color_t light_slate_gray;
// const color_t light_slate_grey;
// const color_t medium_slate_blue;
// const color_t lawn_green;
// const color_t chartreuse;
// const color_t aquamarine;
// const color_t maroon;
// const color_t purple;
// const color_t olive;
// const color_t gray;
// const color_t grey;
// const color_t sky_blue;
// const color_t light_sky_blue;
// const color_t blue_violet;
// const color_t dark_red;
// const color_t dark_magenta;
// const color_t saddle_brown;
// const color_t dark_sea_green;
// const color_t light_green;
// const color_t medium_purple;
// const color_t dark_violet;
// const color_t pale_green;
// const color_t dark_orchid;
// const color_t yellow_green;
// const color_t sienna;
// const color_t brown;
// const color_t dark_gray;
// const color_t dark_grey;
// const color_t light_blue;
// const color_t green_yellow;
// const color_t pale_turquoise;
// const color_t light_steel_blue;
// const color_t powder_blue;
// const color_t fire_brick;
// const color_t dark_golden_rod;
// const color_t medium_orchid;
// const color_t rosy_brown;
// const color_t dark_khaki;
// const color_t silver;
// const color_t medium_violet_red;
// const color_t indian_red;
// const color_t peru;
// const color_t chocolate;
// const color_t tan;
// const color_t light_gray;
// const color_t light_grey;
// const color_t thistle;
// const color_t orchid;
// const color_t golden_rod;
// const color_t pale_violet_red;
// const color_t crimson;
// const color_t gainsboro;
// const color_t plum;
// const color_t burly_wood;
// const color_t light_cyan;
// const color_t lavender;
// const color_t dark_salmon;
// const color_t violet;
// const color_t pale_golden_rod;
// const color_t light_coral;
// const color_t khaki;
// const color_t alice_blue;
// const color_t honey_dew;
// const color_t azure;
// const color_t sandy_brown;
// const color_t wheat;
// const color_t beige;
// const color_t white_smoke;
// const color_t mint_cream;
// const color_t ghost_white;
// const color_t salmon;
// const color_t antique_white;
// const color_t linen;
// const color_t light_golden_rod_yellow;
// const color_t old_lace;
// const color_t red;
// const color_t fuchsia;
// const color_t magenta;
// const color_t deep_pink;
// const color_t orange_red;
// const color_t tomato;
// const color_t hot_pink;
// const color_t coral;
// const color_t dark_orange;
// const color_t light_salmon;
// const color_t orange;
// const color_t light_pink;
// const color_t pink;
// const color_t gold;
// const color_t peach_puff;
// const color_t navajo_white;
// const color_t moccasin;
// const color_t bisque;
// const color_t misty_rose;
// const color_t blanched_almond;
// const color_t papaya_whip;
// const color_t lavender_blush;
// const color_t sea_shell;
// const color_t cornsilk;
// const color_t lemon_chiffon;
// const color_t floral_white;
// const color_t snow;
// const color_t yellow;
// const color_t light_yellow;
// const color_t ivory;
// const color_t white;

#endif