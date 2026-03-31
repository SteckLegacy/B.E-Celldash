#pragma once

#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// Enable printing of mallocs and frees
//#define REPORT_LEAKS

#include "player.h"
#include "particles.h"
#include "psgl_graphics.h"
#include "ps3_audio.h"
#include "all_assets.h"


extern int screenWidth;
extern int widthAdjust;
extern float screen_factor_x;
extern float screen_factor_y;
extern int screenHeight;

#define HITBOX_TRAIL_SIZE 64

typedef struct {
    u8 pressedA:1;
    u8 holdA:1;

    u8 pressedB:1;
    u8 holdB:1;

    u8 pressedHome:1;

    u8 pressed1orX:1;
    u8 hold1orX:1;
    
    u8 pressed2orY:1;
    u8 hold2orY:1;
    
    u8 pressedPlusOrL:1;
    u8 holdPlusOrL:1;
    
    u8 pressedMinusOrR:1;
    u8 holdMinusOrR:1;

    u8 pressedJump:1;
    u8 holdJump:1;

    u8 pressedDir;
    u8 holdDir;
} KeyInput;

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float rotation;
    InternalHitbox internal_hitbox;
} PlayerHitboxTrail;

typedef struct {
    float camera_x;
    float camera_y;

    float camera_intended_y;

    float camera_x_lerp;
    float camera_y_lerp;

    float intermediate_camera_y;
    
    float camera_wall_timer;
    float camera_wall_initial_y;

    float ground_x;
    float ground_wall_initial_x;

    float background_x;
    float background_wall_initial_x;
    
    Player player;
    Player player2;

    unsigned char current_player;

    Player old_player;

    int mirror_mult;
    float intended_mirror_factor;
    float intended_mirror_speed_factor;
    float mirror_speed_factor;
    float mirror_factor;

    float ground_y;
    float ceiling_y;
    float ground_y_gfx;

    float dual_portal_y;
    unsigned char speed;

    bool noclip;
    int hitbox_display;
    bool dual;
    bool dual_setup;
    bool dead;

    bool paused;

    float level_progress;

    float timer;
    
    Particle particles[MAX_PARTICLES];

    int last_hitbox_trail;
    PlayerHitboxTrail hitbox_trail_players[2][HITBOX_TRAIL_SIZE];

    KeyInput input;
} GameState;

enum GameRoutine {
    ROUTINE_MENU,
    ROUTINE_GAME
};

#define INPUT_UP    0x01
#define INPUT_DOWN  0x02
#define INPUT_RIGHT 0x04
#define INPUT_LEFT  0x08

#define SDCARD_FOLDER "sd:/apps/wiidash"
#define RESOURCES_FOLDER "resources"
#define SONGS_FOLDER "songs"
#define USER_SONGS_FOLDER "user_songs"
#define USER_LEVELS_FOLDER "user_levels"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef CLAMP
#define CLAMP(x, lower, upper) (MIN((upper), MAX((x), (lower))))
#endif

extern int gameRoutine;

extern int frame_counter;

extern float physics_time;
extern float collision_time;
extern float particles_time;
extern float triggers_time;
extern float player_time;

extern float draw_time;
extern float layer_sorting;
extern float obj_particles_time;
extern float player_draw_time;

extern int number_of_collisions;
extern int number_of_collisions_checks;
extern int number_of_moving_objects;

extern char launch_dir[256];

extern int frameCount;

extern GameState state;

extern PSGL_texImg *big_font_text;
extern PSGL_texImg *font;
extern PSGL_texImg *cursor;

#include "animation.h"

extern AnimationLibrary robot_animations;

void update_ir_cursor();
void draw_ir_cursor();
extern float ir_x;
extern float ir_y;
extern float ir_angle;
extern float cursor_rotated_point_x;
extern float cursor_rotated_point_y;

extern float dt;

bool is_dolphin();
#ifdef REPORT_LEAKS
void* dbg_malloc(size_t size, const char* file, int line);
void* dbg_realloc(void* ptr, size_t size, const char* file, int line);
void dbg_free(void* ptr, const char* file, int line);
#define malloc(x) dbg_malloc(x, __FILE__, __LINE__)
#define realloc(p, s) dbg_realloc(p, s, __FILE__, __LINE__)
#define free(x)   dbg_free(x, __FILE__, __LINE__)
void report_leaks(void);
#endif

void draw_game();
void update_input();
void update_external_input(KeyInput *input);