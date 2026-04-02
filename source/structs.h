
#include "psgl_graphics.h"

typedef struct {
    float x, y;
} Vec2D;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

typedef struct {
    float h;
    float s;
    float v;
    bool sChecked;
    bool vChecked;
} HSV;

typedef struct {
    float r;
    float g;
    float b;
    float a;
} ColorAlpha;