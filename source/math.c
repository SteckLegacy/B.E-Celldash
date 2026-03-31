#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "psgl_graphics.h"
#include "math.h"
#include "objects.h"
#include "game.h"
#include "player.h"
#include "main.h"
#include "easing.h"
#include "font_stuff.h"
#include <stdio.h>

#include "../libraries/color.h"

Vec2D normalize(Vec2D v) {
    float len = sqrtf(v.x*v.x + v.y*v.y);
    return (Vec2D){ v.x / len, v.y / len };
}

float dot_vec(Vec2D a, Vec2D b) {
    return a.x * b.x + a.y * b.y;
}

Vec2D perpendicular(Vec2D v) {
    return (Vec2D){ -v.y, v.x };
}

float positive_fmod(float n, float divisor) {
    float value = fmod(n, divisor);
    return value + (value < 0 ? divisor : 0);
}

Color color_lerp(Color color1, Color color2, float fraction) {
    u8 r1 = color1.r;
    u8 r2 = color2.r;
    u8 g1 = color1.g;
    u8 g2 = color2.g;
    u8 b1 = color1.b;
    u8 b2 = color2.b;

    Color returned;
    returned.r = (r2 - r1) * fraction + r1;
    returned.g = (g2 - g1) * fraction + g1;
    returned.b = (b2 - b1) * fraction + b1;

    return returned;
}

Vec2D rotate(Vec2D point, float angle, Vec2D origin) {
    float s = sinf(angle);
    float c = cosf(angle);
    point.x -= origin.x;
    point.y -= origin.y;
    Vec2D rotated = {
        point.x * c - point.y * s + origin.x,
        point.x * s + point.y * c + origin.y
    };
    return rotated;
}

float maxf(float a, float b) {
    return a > b ? a : b;
}

float approachf(float current, float target, float speed, float smoothing) {
    float diff = target - current;
    float step = diff * smoothing; // smoothing in [0,1], e.g. 0.1 for gentle, 0.5 for fast
    if (fabsf(diff) < speed)
        return target;
    return current + step + (diff > 0 ? speed : -speed);
}

// This one only works for drawing
void rotate_point_around_center_gfx(
    float x, float y,               // anchor position on screen
    float offset_x, float offset_y, // object position (pre-rotation)
    float center_x, float center_y, // pivot center in the object
    float width, float height,      // size of the object
    float rotation_deg,             // rotation angle in degrees
    float *output_x, float *output_y // output screen position
) {
    float rot = DegToRad(rotation_deg);
    float sin_theta = sinf(rot);
    float cos_theta = cosf(rot);

    // Relative position to rotation center
    float relative_x = offset_x - center_x + (width / 2.0f);
    float relative_y = offset_y - center_y + (height / 2.0f);

    // Apply rotation
    float rotated_x = (relative_x * cos_theta) - (relative_y * sin_theta);
    float rotated_y = (relative_y * cos_theta) + (relative_x * sin_theta);

    // Final screen position
    *output_x = x + (center_x - (width / 2.0f)) + rotated_x;
    *output_y = y + (center_y - (height / 2.0f)) + rotated_y;
}

void rotate_point_around_center(float cx, float cy, float angle, float x, float y, float *out_x, float *out_y) {
    float s = sinf(DegToRad(-angle));
    float c = cosf(DegToRad(-angle));

    x -= cx;
    y -= cy;

    float xnew = x * c - y * s;
    float ynew = x * s + y * c;

    *out_x = xnew + cx;
    *out_y = ynew + cy;
}

float randomf() {
    return (float)rand() / RAND_MAX;
}

float random_float(float min, float max) {
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

int random_int(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

float map_range(float val, float min1, float max1, float min2, float max2) {
    return min2 + ((max2 - min2) / (max1 - min1)) * (val - min1);
}

float adjust_angle(float angle, int flipX, int flipY) {
    // Normalize to [0, 360)
    angle = positive_fmod(angle, 360);

    if (flipX && flipY) {
        angle = fmodf(angle + 180.0f, 360.0f);
    } else if (flipX) {
        angle = 180.0f - angle;
    } else if (flipY) {
        angle = -angle;
    }

    // Normalize again
    angle = positive_fmod(angle, 360);

    return angle;
}

float adjust_angle_x(float angle, int flipX) {
    angle = positive_fmod(angle, 360);

    if (flipX) {
        angle = 180.0f - angle;
    } 

    angle = positive_fmod(angle, 360);

    return angle;
}

float adjust_angle_y(float angle, int flipY) {
    angle = positive_fmod(angle, 360);

    if (flipY) {
        angle = angle + 180;
    }

    angle = positive_fmod(angle, 360);

    return angle;
}

float square_distance(float xa, float ya, float xb, float yb) {
	return ((xb - xa) * (xb - xa)) + ((yb - ya) * (yb - ya));
}

float clampf(float d, float min, float max) {
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

float repeat(float a, float length) {
	return clampf(a - floorf(a / length) * length, 0.0f, length);
}

float slerp(float a, float b, float ratio) {
	float delta = repeat((b - a), 360.f);
	if (delta > 180.f)
		delta -= 360.f;
	return a + delta * clampf(ratio, 0.f, 1.f);
}

float lerp(float from, float to, float alpha) {
    return from * (1.0f - alpha) + to * alpha;
}

float iLerp(float a, float b, float ratio, float dt) {
	const float rDelta = dt * STEPS_HZ;
	const float s	  = 1.f - ratio;

	float iRatio = 1.f - powf(s, rDelta);

	return lerp(a, b, iRatio);
}

float iSlerp(float a, float b, float ratio, float dt) {
	const float rDelta = dt * STEPS_HZ;
	const float s	  = 1.f - ratio;

	float iRatio = 1.f - powf(s, rDelta);

	return slerp(a, b, iRatio);
}

// Generic easing out function, not from GD
float ease_out(float current, float target, float smoothing) {
    return current + (target - current) * smoothing;
}

// Custom PSGL functions for maximum performance (so it can be batched)

void set_texture(const PSGL_texImg *tex) {
    if (tex == NULL)
        return;

    glBindTexture(GL_TEXTURE_2D, tex->textureID);
    prev_tex = (PSGL_texImg *)tex;
}

void  custom_drawImg (const f32 xpos, const f32 ypos, const PSGL_texImg *tex, const f32 degrees, const f32 scaleX, const f32 scaleY, const u32 color) {
    PSGL_DrawImg(xpos, ypos, (PSGL_texImg*)tex, degrees, scaleX, scaleY, color);
}

void custom_ellipse(const float x, const float y, const float radiusX,
                    const float radiusY, const u32 color, const u8 filled,
                    const float lineWidth) {
    int segments = (int)(MAX(radiusX, radiusY) * 0.75f); // tweak factor for smoothness
    segments = CLAMP(segments, 12, 256);                 // minimum and maximum

    glDisable(GL_TEXTURE_2D);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
    } else {
        glLineWidth(lineWidth);
        glBegin(GL_LINE_STRIP);
    }

    for (int i = 0; i <= segments; i++) {
        float angle = i * 2 * M_PI / segments;
        float px = cosf(angle) * radiusX + x;
        float py = sinf(angle) * radiusY + y;
        glVertex2f(px, py);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void  custom_circle (const f32 x, const f32 y, const f32 radius,
                     const u32 color) {
    custom_ellipse(x, y, radius, radius, color, TRUE, 1);
}

void  custom_circunference (const f32 x, const f32 y, const f32 radius,
                     const u32 color, const f32 lineWidth) {
    custom_ellipse(x, y, radius, radius, color, FALSE, lineWidth);
}

void  custom_drawPart (const f32 xpos, const f32 ypos, const f32 partx, const f32 party, const f32 partw, const f32 parth, const PSGL_texImg *tex, const f32 degrees, const f32 scaleX, const f32 scaleY, const u32 color) {
    if (!tex) return;

    glPushMatrix();
    glTranslatef(xpos, ypos, 0);
    if (degrees != 0) glRotatef(degrees, 0, 0, 1);
    glScalef(scaleX, scaleY, 1);

    glBindTexture(GL_TEXTURE_2D, tex->textureID);

    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    const f32 texWidth  = tex->w;
    const f32 texHeight = tex->h;
    const f32 s1 = partx / texWidth;
    const f32 s2 = (partx + partw) / texWidth;
    const f32 t1 = party / texHeight;
    const f32 t2 = (party + parth) / texHeight;

    glBegin(GL_QUADS);
    glTexCoord2f(s1, t1); glVertex2f(0, 0);
    glTexCoord2f(s2, t1); glVertex2f(partw, 0);
    glTexCoord2f(s2, t2); glVertex2f(partw, parth);
    glTexCoord2f(s1, t2); glVertex2f(0, parth);
    glEnd();

    glPopMatrix();
}

void  custom_rectangle (const f32 x,      const f32 y,
                        const f32 width,  const f32 height,
                        const u32 color, const bool filled) {
    glDisable(GL_TEXTURE_2D);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    if (filled) {
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    } else {
        glBegin(GL_LINE_STRIP);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glVertex2f(x, y);
        glEnd();
    }
    glEnable(GL_TEXTURE_2D);
}

void custom_rounded_rectangle(float x, float y,
                              float width, float height,
                              float radius,
                              u32 color) {
    // Drawn as 3 rectangles and 4 circles for corners
    // Center rectangle
    custom_rectangle(x + radius, y, width - 2 * radius, height, color, true);
    // Left rectangle
    custom_rectangle(x, y + radius, radius, height - 2 * radius, color, true);
    // Right rectangle
    custom_rectangle(x + width - radius, y + radius, radius, height - 2 * radius, color, true);

    // Corners
    custom_circle(x + radius, y + radius, radius, color);
    custom_circle(x + width - radius, y + radius, radius, color);
    custom_circle(x + radius, y + height - radius, radius, color);
    custom_circle(x + width - radius, y + height - radius, radius, color);
}

float normalize_angle(float angle) {
    angle = fmodf(angle, 360.0f);
    if (angle < 0) angle += 360.0f;
    return angle;
}

bool is_effectively_integer(float x) {
    return fabsf(x - roundf(x)) < FLT_EPSILON;
}

float ip1_ceilf(float x) {
    float c = ceilf(x);
    if (is_effectively_integer(x)) {
        return c + 1.0f;
    }
    return c;
}

void custom_line (const f32 x1, const f32 y1,
                   const f32 x2, const f32 y2, const u32 color) {
    glDisable(GL_TEXTURE_2D);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}
void draw_thick_line(const float x1, const float y1, const float x2, const float y2, const float thickness, const u32 color) {
    glDisable(GL_TEXTURE_2D);
    glLineWidth(thickness);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

// Returns true if vertices are counter-clockwise
bool is_ccw(Vec2D *poly, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        sum += (poly[j].x - poly[i].x) * (poly[j].y + poly[i].y);
    }
    return sum < 0.0f; // CCW if sum < 0
}


void draw_hitbox_line_inward(Vec2D rect[4], 
                             const float x1, const float y1,
                             const float x2, const float y2,
                             const float thickness,
                             const float cx, const float cy,
                             const u32 color) {

    float ex = x2 - x1;
    float ey = y2 - y1;
    float length = sqrtf(ex * ex + ey * ey);

    // Inward normal depends on winding
    float nx, ny;
    if (is_ccw(rect, 4)) {
        nx = -ey / length;
        ny =  ex / length;
    } else {
        nx =  ey / length;
        ny = -ex / length;
    }

    // Flip it to point inward if not mirror
    if (state.mirror_factor < 0.5f) {
        nx = -nx;
        ny = -ny;
    }

    // Offset inward
    float ox = nx * thickness;
    float oy = ny * thickness;

    // Build quad
    float x1a = x1;
    float y1a = y1;
    float x2a = x2;
    float y2a = y2;

    float x1b = x1 + ox;
    float y1b = y1 + oy;
    float x2b = x2 + ox;
    float y2b = y2 + oy;

    glDisable(GL_TEXTURE_2D);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    glBegin(GL_QUADS);
    glVertex2f(x1a, y1a);
    glVertex2f(x2a, y2a);
    glVertex2f(x2b, y2b);
    glVertex2f(x1b, y1b);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void compute_mitered_offsets(Vec2D *poly, Vec2D *offsets, int n, float thickness, bool ccw) {
    for (int i = 0; i < n; i++) {
        int prev = (i + n - 1) % n;
        int next = (i + 1) % n;

        // Edge vectors
        Vec2D e_prev = { poly[i].x - poly[prev].x, poly[i].y - poly[prev].y };
        Vec2D e_next = { poly[next].x - poly[i].x, poly[next].y - poly[i].y };

        // Normals
        Vec2D n_prev = normalize(perpendicular(e_prev));
        Vec2D n_next = normalize(perpendicular(e_next));

        // Flip to point inward if mirror
        if (state.mirror_factor < 0.5f) { n_prev.x = -n_prev.x; n_prev.y = -n_prev.y; n_next.x = -n_next.x; n_next.y = -n_next.y; }

        // Miter = normalize(n_prev + n_next)
        Vec2D miter = normalize((Vec2D){ n_prev.x + n_next.x, n_prev.y + n_next.y });

        // Compute miter length to preserve thickness
        float cos_half_angle = dot_vec(miter, n_next); // cos(theta/2)
        float miter_length = thickness / cos_half_angle;

        offsets[i].x = poly[i].x + miter.x * miter_length;
        offsets[i].y = poly[i].y + miter.y * miter_length;
    }
}

void draw_polygon_inward_mitered(Vec2D *poly, int n, float thickness, u32 color) {
    bool ccw = is_ccw(poly, n);
    Vec2D offsets[n];
    compute_mitered_offsets(poly, offsets, n, thickness, ccw);

    glDisable(GL_TEXTURE_2D);
    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;

        // Original vertices
        glVertex2f(poly[i].x, poly[i].y);
        glVertex2f(poly[j].x, poly[j].y);
        glVertex2f(offsets[j].x, offsets[j].y);

        glVertex2f(offsets[j].x, offsets[j].y);
        glVertex2f(offsets[i].x, offsets[i].y);
        glVertex2f(poly[i].x, poly[i].y);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

float opacity = 0;

void draw_fade() {
    PSGL_FillScreen(RGBA(0,0,0, (int) opacity));
    Render();
}


void fade_out() {
    // In PSGL we could use glReadPixels to implement framebuffer fade,
    // but for now we'll just fade to black.

    u64 prevTicks = 0; // gettime();
    while (opacity < 255) {
        // start_frame = gettime();
        float frameTime = 1.0f/60.0f; // ticks_to_secs_float(start_frame - prevTicks);
        dt = frameTime;
        // prevTicks = start_frame;

        opacity += FADE_SPEED * dt;
        if (opacity > 255) opacity = 255;
        draw_fade();
    }
}

void fade_in() {
    u64 prevTicks = 0; // gettime();
    while (opacity > 0) {
        // start_frame = gettime();
        float frameTime = 1.0f/60.0f; // ticks_to_secs_float(start_frame - prevTicks);
        dt = frameTime;
        // prevTicks = start_frame;
        opacity -= FADE_SPEED * dt;
        if (opacity < 0) opacity = 0;
        draw_fade();
    }
}

void fade_in_level() {
    u64 prevTicks = gettime();
    double accumulator = 0.0;
    while (opacity > 0) {
        start_frame = gettime();
        float frameTime = ticks_to_secs_float(start_frame - prevTicks);
        dt = frameTime;
        
        accumulator += frameTime;

        prevTicks = start_frame;
        opacity -= FADE_SPEED * dt;
        if (opacity < 0) opacity = 0;
        
        while (accumulator >= STEPS_DT) {
            update_percentage();
            handle_objects();
            accumulator -= STEPS_DT;
        }
        
        update_particles();
        draw_game();
        draw_fade();
    }
}

void wait_initial_time() {
    u64 prevTicks = gettime();
    double accumulator = 0.0;
    float time_elapsed = 0.f;
    while (time_elapsed < 0.5f) {
        start_frame = gettime();
        float frameTime = ticks_to_secs_float(start_frame - prevTicks);
        dt = frameTime;
        
        accumulator += frameTime;
        time_elapsed += frameTime;

        prevTicks = start_frame;
        opacity -= FADE_SPEED * dt;
        if (opacity < 0) opacity = 0;

        while (accumulator >= STEPS_DT) {
            update_beat();
            accumulator -= STEPS_DT;
        }
        update_particles();
    
        draw_game();
        draw_fade();
    }
}

void  draw_glyph (const f32 xpos, const f32 ypos, const f32 partx, const f32 party, const f32 partw, const f32 parth, const PSGL_texImg *tex, const f32 degrees, const f32 scaleX, const f32 scaleY, const u32 color) {
    if (!tex) return;

    glPushMatrix();
    glTranslatef(xpos, ypos, 0);
    if (degrees != 0) glRotatef(degrees, 0, 0, 1);
    glScalef(scaleX, scaleY, 1);

    glBindTexture(GL_TEXTURE_2D, tex->textureID);

    float r = R(color) / 255.0f;
    float g = G(color) / 255.0f;
    float b = B(color) / 255.0f;
    float a = A(color) / 255.0f;
    glColor4f(r, g, b, a);

    const f32 texWidth  = tex->w;
    const f32 texHeight = tex->h;
    const f32 s1 = partx / texWidth;
    const f32 s2 = (partx + partw) / texWidth;
    const f32 t1 = party / texHeight;
    const f32 t2 = (party + parth) / texHeight;

    glBegin(GL_QUADS);
    glTexCoord2f(s1, t1); glVertex2f(0, 0);
    glTexCoord2f(s2, t1); glVertex2f(partw, 0);
    glTexCoord2f(s2, t2); glVertex2f(partw, parth);
    glTexCoord2f(s1, t2); glVertex2f(0, parth);
    glEnd();

    glPopMatrix();
}

struct glyph *get_glyph(struct charset font, char character) {
    for (int i = 0; i < font.char_num; i++) {
        if (character == font.chars[i].id) {
            return &font.chars[i];
        }
    }
    // If not found and a lowercase letter, convert to uppercase
    if (character >= 'a' && character <= 'z') {
        character -= 32;

        // Search again
        for (int i = 0; i < font.char_num; i++) {
            if (character == font.chars[i].id) {
                return &font.chars[i];
            }
        }
    }
    
    return NULL;
}

float get_text_length(struct charset font, const float zoom, const char *text, ...) {
    
    char tmp[1024];

    va_list argp;
    va_start(argp, text);
    const int size = vsnprintf(tmp, sizeof(tmp), text, argp);
    va_end(argp);
    
    float text_length = 0;
    for (int i = 0; i < size; i++) {
        struct glyph *character = get_glyph(font, tmp[i]);

        if (character != NULL) {
            float xadvance = character->xadvance * zoom;

            text_length += xadvance;
        }
    }
    return text_length;
}

void draw_text(struct charset font, PSGL_texImg *tex, const float x, const float y, const float zoom, const char *text, ...) {
    if (!text || !tex) {
        return;
    }
    PSGL_SetHandle(tex, tex->w / 2, tex->h / 2);

    char tmp[1024];

    va_list argp;
    va_start(argp, text);
    const int size = vsnprintf(tmp, sizeof(tmp), text, argp);
    va_end(argp);

    float offset = 0;
    for (int i = 0; i < size; i++) {
        struct glyph *character = get_glyph(font, tmp[i]);

        if (character != NULL) {
            int tile_x = character->x;
            int tile_y = character->y;
            int width = character->width;
            int height = character->height;
            float xoffset = character->xoffset * zoom;
            float yoffset = character->yoffset * zoom;
            float xadvance = character->xadvance * zoom;

            draw_glyph(x + offset + xoffset, y + yoffset, tile_x, tile_y, width, height, tex, 0, zoom, zoom, 0xffffffff);
            
            offset += xadvance;
        }
    }
}

void draw_rotated_text(struct charset font, PSGL_texImg *tex, const float x, const float y, const float rotation, const float zoom_x, const float zoom_y, const u32 color, const char *text, ...) {
    if (!text || !tex) {
        return;
    }
    
    PSGL_SetHandle(tex, tex->w / 2, tex->h / 2);

    char tmp[1024];

    va_list argp;
    va_start(argp, text);
    const int size = vsnprintf(tmp, sizeof(tmp), text, argp);
    va_end(argp);

    float length = get_text_length(font, fabsf(zoom_x), tmp);

    float rad = DegToRad(rotation); // convert to radians
    float offset = 0;

    int flip_x = (zoom_x < 0 ? -1 : 1);
    int flip_y = (zoom_y < 0 ? -1 : 1);

    for (int i = 0; i < size; i++) {
        struct glyph *character = get_glyph(font, tmp[i]);
        
        if (character != NULL) {
            int tile_x = character->x;
            int tile_y = character->y;
            int width = character->width;
            int height = character->height;
            float xoffset = character->xoffset * zoom_x;
            float yoffset = character->yoffset * zoom_y;
            float xadvance = character->xadvance * zoom_x;

            // Compute glyph center position before rotation
            float gx = (x - length * flip_x / 2.f) + offset + xoffset + width * zoom_x / 2.0f;
            float gy = (y - 55 * fabsf(zoom_y) * flip_y / 2.f) + yoffset + height * zoom_y / 2.0f;

            // Rotate glyph center around text center (x, y)
            float dx = gx - x;
            float dy = gy - y;
            float final_x = x + dx * cosf(rad) - dy * sinf(rad);
            float final_y = y + dx * sinf(rad) + dy * cosf(rad);

            // Draw glyph so its center is at (final_x, final_y)
            draw_glyph(final_x - width * zoom_x / 2.0f, final_y - height * zoom_y / 2.0f,
                       tile_x, tile_y, width, height,
                       tex, rotation, zoom_x, zoom_y, color);

            offset += xadvance;
        }
    }
    
    // GX_SetVtxDesc(GX_VA_TEX0,   GX_DIRECT);
    // GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
}

Color HSV_combine(Color color, HSV hsv) {
    if (hsv.h == 0 && hsv.s == 0 && hsv.v == 0) {
        return color;
    }

    HSV color_hsv;
    convertRGBtoHSV(color.r, color.g, color.b, &color_hsv.h, &color_hsv.s, &color_hsv.v);

    hsv.h += color_hsv.h;

    if (hsv.sChecked) {
        hsv.s += color_hsv.s;
    } else {
        hsv.s *= color_hsv.s;
    }
    
    if (hsv.vChecked) {
        hsv.v += color_hsv.v;
    } else {
        hsv.v *= color_hsv.v;
    }

    if (hsv.h < 0) hsv.h += 360;
    if (hsv.h >= 360) hsv.h -= 360;
    if (hsv.s > 1) hsv.s = 1;
    if (hsv.v > 1) hsv.v = 1;

    Color returned_color;
    convertHSVtoRGB(hsv.h, hsv.s, hsv.v, &returned_color.r, &returned_color.g, &returned_color.b);

    return returned_color;
}

bool colors_equal(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}
