#include "psgl_graphics.h"
#include <PSGL/psgl.h>
#include <PSGL/psglu.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

PSGL_drawSettings PSGL_Settings;
static PSGLcontext* psglContext = NULL;
static PSGLdevice* psglDevice = NULL;
static CGcontext cgContext;
static CGprogram vertexProgram;
static CGprogram fragmentProgram;
static bool shaderEnabled = false;

int PSGL_Init(int width, int height) {
    psglInit();

    // PS3 supports 480p, 720p, 1080i/p
    // For 1080i/p we want to upscale from 720p definition
    int renderWidth = width;
    int renderHeight = height;

    if (width > 1280) {
        renderWidth = 1280;
        renderHeight = 720;
    }

    psglDevice = psglCreateDeviceExtended(PSGL_DEVICE_FORMAT_X8R8G8B8, PSGL_DEVICE_FORMAT_DEPTH_24);
    psglContext = psglCreateContext();
    psglMakeCurrent(psglContext, psglDevice);

    PSGL_Settings.width = width;
    PSGL_Settings.height = height;
    PSGL_Settings.internalWidth = renderWidth;
    PSGL_Settings.internalHeight = renderHeight;
    PSGL_Settings.blend = BLEND_ALPHA;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, renderWidth, renderHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Init Cg
    cgContext = cgCreateContext();

    return 1;
}

void PSGL_Exit() {
    if (psglContext) psglDestroyContext(psglContext);
    if (psglDevice) psglDestroyDevice(psglDevice);
    psglExit();
}

void Render() {
    psglSwap();
}

void PSGL_FillScreen(u32 color) {
    glClearColor(R(color) / 255.0f, G(color) / 255.0f, B(color) / 255.0f, A(color) / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PSGL_SetBlend(PSGL_blendMode mode) {
    PSGL_Settings.blend = mode;
    switch (mode) {
        case BLEND_ALPHA:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BLEND_ADD:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BLEND_NONE:
            glDisable(GL_BLEND);
            return;
        default:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
    glEnable(GL_BLEND);
}

void PSGL_SetHandle(PSGL_texImg* tex, int x, int y) {
    if (!tex) return;
    tex->handlex = x;
    tex->handley = y;
}

// PNG loading via libpngdec
PSGL_texImg* PSGL_LoadTexturePNG(const u8* data, size_t size) {
    if (!data) return NULL;

    int32_t handle;
    CellPngDecThreadInParam thread_in;
    CellPngDecThreadOutParam thread_out;
    CellPngDecInParam in_param;
    CellPngDecOutParam out_param;
    CellPngDecInfo info;

    thread_in.enable = 0;
    thread_out.enable = 0;

    cellPngDecCreate(&handle, &thread_in, &thread_out);

    CellPngDecSrc src;
    src.srcSelect = CELL_PNGDEC_SRC_SELECT_MEMORY;
    src.u.memSrc.addr = (void*)data;
    src.u.memSrc.size = size;

    cellPngDecOpen(handle, &src, &info);

    in_param.command = CELL_PNGDEC_COMMAND_ALL;
    in_param.output_mode = CELL_PNGDEC_OUTPUT_MODE_RGBA;
    in_param.output_color_space = CELL_PNGDEC_COLOR_SPACE_RGBA;
    in_param.alpha_select = 0;

    // Allocate buffer for decoded data
    size_t pixelDataSize = info.width * info.height * 4;
    void* pixelData = malloc(pixelDataSize);
    if (!pixelData) {
        cellPngDecClose(handle);
        cellPngDecDestroy(handle);
        return NULL;
    }

    out_param.addr = pixelData;
    cellPngDecRead(handle, &in_param, &out_param);

    PSGL_texImg* tex = (PSGL_texImg*)malloc(sizeof(PSGL_texImg));
    tex->w = info.width;
    tex->h = info.height;
    tex->handlex = 0;
    tex->handley = 0;

    glGenTextures(1, &tex->textureID);
    glBindTexture(GL_TEXTURE_2D, tex->textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->w, tex->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    free(pixelData);
    cellPngDecClose(handle);
    cellPngDecDestroy(handle);

    return tex;
}

void PSGL_FreeTexture(PSGL_texImg* tex) {
    if (tex) {
        glDeleteTextures(1, &tex->textureID);
        free(tex);
    }
}

void PSGL_EnableShader(bool enable) {
    shaderEnabled = enable;
}

void PSGL_LoadCgShader(const char* filename) {
    char vp_path[256];
    char fp_path[256];
    snprintf(vp_path, sizeof(vp_path), "%s.vp.cg", filename);
    snprintf(fp_path, sizeof(fp_path), "%s.fp.cg", filename);

    vertexProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, vp_path, CG_PROFILE_SCE_VP_RSX, "main", NULL);
    fragmentProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, fp_path, CG_PROFILE_SCE_FP_RSX, "main", NULL);

    cgGLLoadProgram(vertexProgram);
    cgGLLoadProgram(fragmentProgram);
}

void PSGL_DrawImg(float x, float y, PSGL_texImg* tex, float rotation, float scaleX, float scaleY, u32 color) {
    if (!tex) return;

    if (shaderEnabled) {
        cgGLEnableProfile(CG_PROFILE_SCE_VP_RSX);
        cgGLEnableProfile(CG_PROFILE_SCE_FP_RSX);
        cgGLBindProgram(vertexProgram);
        cgGLBindProgram(fragmentProgram);
    }

    glPushMatrix();
    glTranslatef(x, y, 0);
    if (rotation != 0) glRotatef(rotation, 0, 0, 1);
    glScalef(scaleX, scaleY, 1);

    glBindTexture(GL_TEXTURE_2D, tex->textureID);

    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    float halfW = tex->w * 0.5f;
    float halfH = tex->h * 0.5f;

    float x1 = -halfW - tex->handlex + (tex->w / 2);
    float y1 = -halfH - tex->handley + (tex->h / 2);
    float x2 = halfW - tex->handlex + (tex->w / 2);
    float y2 = halfH - tex->handley + (tex->h / 2);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x1, y1);
    glTexCoord2f(1, 0); glVertex2f(x2, y1);
    glTexCoord2f(1, 1); glVertex2f(x2, y2);
    glTexCoord2f(0, 1); glVertex2f(x1, y2);
    glEnd();

    glPopMatrix();

    if (shaderEnabled) {
        cgGLDisableProfile(CG_PROFILE_SCE_VP_RSX);
        cgGLDisableProfile(CG_PROFILE_SCE_FP_RSX);
    }
}

#ifdef __cplusplus
}
#endif
