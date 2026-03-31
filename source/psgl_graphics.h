#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <cell/pngdec.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;

typedef enum PSGL_blendMode {
    BLEND_ALPHA = 0,
    BLEND_ADD = 1,
    BLEND_SCREEN = 2,
    BLEND_MULTI = 3,
    BLEND_INV = 4,
    BLEND_NONE = 5
} PSGL_blendMode;

typedef struct PSGL_texImg {
    u32 w;
    u32 h;
    GLuint textureID;
    int handlex;
    int handley;
    // Internal data for PNG management if needed
} PSGL_texImg;

typedef struct PSGL_drawSettings {
    bool antialias;
    PSGL_blendMode blend;
    int width;
    int height;
    int internalWidth;
    int internalHeight;
} PSGL_drawSettings;

extern PSGL_drawSettings PSGL_Settings;

// Core functions
int PSGL_Init(int width, int height);
void PSGL_Exit();
void Render();
void PSGL_FillScreen(u32 color);

// Texture functions
PSGL_texImg* PSGL_LoadTexturePNG(const u8* data, size_t size);
void PSGL_FreeTexture(PSGL_texImg* tex);
void PSGL_SetHandle(PSGL_texImg* tex, int x, int y);
void PSGL_SetBlend(PSGL_blendMode mode);

// Drawing functions
void PSGL_DrawImg(float x, float y, PSGL_texImg* tex, float rotation, float scaleX, float scaleY, u32 color);

// Shader support
void PSGL_EnableShader(bool enable);
void PSGL_LoadCgShader(const char* filename);

// Helper macros for colors (compatible with RGBA)
#define R(c)  (((c) >> 24) & 0xFF)
#define G(c)  (((c) >> 16) & 0xFF)
#define B(c)  (((c) >> 8) & 0xFF)
#define A(c)  ((c) & 0xFF)
#define RGBA(r, g, b, a) ((u32)((((u32)(r)) << 24) | (((u32)(g)) << 16) | (((u32)(b)) << 8) | ((u32)(a))))

#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_H
