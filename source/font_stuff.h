#include "all_assets.h"

// values taken from bigFont-hd.fnt
struct glyph {
    int id, x, y, width, height, xoffset, yoffset, xadvance;
};

struct charset {
    struct glyph *chars;
    int char_num;
};


#define NUM_LEVEL_FONTS 13

typedef struct {
    const unsigned char *data;
    size_t size;
} FontAsset;

extern const FontAsset font_assets[NUM_LEVEL_FONTS];
extern struct charset *font_charsets[NUM_LEVEL_FONTS];

extern struct glyph big_font_chars[96];
extern struct charset big_font;

extern struct glyph chat_font_chars[95];
extern struct charset chat_font;

extern struct glyph gold_font_chars[96];
extern struct charset gold_font;

extern struct glyph gjFont01_chars[67];
extern struct charset gjFont01;

extern struct glyph gjFont02_chars[83];
extern struct charset gjFont02;

extern struct glyph gjFont03_chars[88];
extern struct charset gjFont03;

extern struct glyph gjFont04_chars[89];
extern struct charset gjFont04;

extern struct glyph gjFont05_chars[96];
extern struct charset gjFont05;

extern struct glyph gjFont06_chars[93];
extern struct charset gjFont06;

extern struct glyph gjFont07_chars[93];
extern struct charset gjFont07;

extern struct glyph gjFont08_chars[93];
extern struct charset gjFont08;

extern struct glyph gjFont09_chars[87];
extern struct charset gjFont09;

extern struct glyph gjFont10_chars[69];
extern struct charset gjFont10;

extern struct glyph gjFont11_chars[80];
extern struct charset gjFont11;

extern struct glyph gjFont12_chars[96];
extern struct charset gjFont12;