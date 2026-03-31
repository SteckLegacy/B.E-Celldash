#include "psgl_graphics.h"

#include <stdlib.h>
#include <math.h>

#include "objects.h"
#include <stdio.h>

#include <unistd.h>
#include <cell/pad.h>

#include "main.h"
#include "game.h"
#include "custom_mp3player.h"
#include "oggplayer.h"

#include "level_loading.h"
#include "objects.h"

#include "cursor_png.h"

#include "player.h"
#include "math.h"

#include "game.h"
#include "menu.h"

#include "trail.h"

#include "animation.h"


// Declare Static Functions
static void ExitGame(void);

int screenWidth = 0;
int screenHeight = 0;
float screen_factor_x = 0;
float screen_factor_y = 0;
int widthAdjust = 0;

int gameRoutine = ROUTINE_MENU;

u64 startTime;
int frameCount = 0;
float fps = 0;

GameState state;

PSGL_texImg *font = NULL;
PSGL_texImg *cursor = NULL;
PSGL_texImg *big_font_text = NULL;

int frame_counter = 0;
int old_frame_counter = 0;

// Profiling
float obj_layer_time = 0;
float physics_time = 0;
float collision_time = 0;
float particles_time = 0;
float triggers_time = 0;
float player_time = 0;
float frame_time = 0;

float obj_particles_time = 0;
float draw_time = 0;
float layer_sorting = 0;
float player_draw_time = 0;

int number_of_collisions = 0;
int number_of_collisions_checks = 0;
int number_of_moving_objects = 0;

char launch_dir[256] = SDCARD_FOLDER;
float ir_x;
float ir_y;
float ir_angle;
float cursor_rotated_point_x;
float cursor_rotated_point_y;
int ir_is_valid;

AnimationLibrary robot_animations = {0};

void draw_game() {
    draw_background(state.background_x / 8, -(state.camera_y / 8) + 416);

    u64 t0 = gettime();
    draw_all_object_layers();
    u64 t1 = gettime();
    
    obj_layer_time = ticks_to_microsecs(t1 - t0) / 1000.f;

    draw_end_wall();

    draw_ground(0, FALSE);
    
    if (state.ground_y_gfx > 0) {
        if (state.camera_y + state.ground_y_gfx > 0) draw_ground(state.camera_y + state.ground_y_gfx, FALSE);
        draw_ground(state.camera_y + SCREEN_HEIGHT_AREA - state.ground_y_gfx, TRUE);
    }
    
    // FPS logic
    frameCount++;
    u64 currentTime = gettime();
    float elapsed = ticks_to_millisecs(currentTime - startTime) / 1000.0f;

    if (elapsed >= 1.0f) {
        fps = frameCount / elapsed;
        frameCount = 0;
        startTime = currentTime;
    }
    
    
    char percentage[64];
    snprintf(percentage, sizeof(percentage), "%d%%%%", (int) state.level_progress);
    int textOffset = (get_text_length(big_font, 0.5, percentage)) / 2;
    draw_text(big_font, big_font_text, screenWidth/2 - textOffset, 10, 0.5, percentage);
    

    if (state.noclip) {
        draw_text(big_font, big_font_text, screenWidth - 200, 20, 0.25, "Noclip activated");
    }

    if (enable_info) {    
        t0 = gettime();

        // Render FPS
        // char fpsText[64];
        // snprintf(fpsText, sizeof(fpsText), "FPS: %.2f Steps: %d Objs: %d Layers: %d", fps, frame_counter - old_frame_counter, level_info.object_count, layersArrayList->count);
        // draw_text(big_font, big_font_text, 20, 20, 0.25, fpsText);  // White tex
        
        // char layerText[64];
        // snprintf(layerText, sizeof(layerText), "Drawn layers: %d", layersDrawn);
        // draw_text(big_font, big_font_text, 20, 50, 0.25, layerText);
        // old_frame_counter = frame_counter;
        
        // char player_x[64];
        // snprintf(player_x, sizeof(player_x), "X: %.2f VX: %.2f", state.player.x, state.player.vel_x);
        // draw_text(big_font, big_font_text, 20, 80, 0.25, player_x);

        // char player_y[64];
        // snprintf(player_y, sizeof(player_y), "Y: %.2f VY: %.2f", state.player.y, state.player.vel_y);
        // draw_text(big_font, big_font_text, 20, 110, 0.25, player_y);
        
        // char obj_layer[64];
        // snprintf(obj_layer, sizeof(obj_layer), "GFX: %.2f ms (Pl: %.2f Pt: %.2f, St: %.2f, D: %.2f)", obj_layer_time, player_draw_time, obj_particles_time, layer_sorting, draw_time);
        // draw_text(big_font, big_font_text, 20, 140, 0.25, obj_layer);
        
        // char physics[128];
        // snprintf(physics, sizeof(physics), "Physics: %.2f ms (P: %.2f Obj: %.2f E: %.2f)", physics_time, player_time, triggers_time, particles_time);
        // draw_text(big_font, big_font_text, 20, 170, 0.25, physics);
        
        // char objects[128];
        // snprintf(objects, sizeof(objects), "Obj: Move %d", number_of_moving_objects);
        // draw_text(big_font, big_font_text, 20, 200, 0.25, objects);

        // char collision[128];
        // snprintf(collision, sizeof(collision), "Collision: %.2f ms (Checks: %d Succeded: %d)", collision_time, number_of_collisions_checks, number_of_collisions);
        // draw_text(big_font, big_font_text, 20, 230, 0.25, collision);

        // t1 = gettime();
        // float text = ticks_to_microsecs(t1 - t0) / 1000.f;
        
        // char text_ms[64];
        // snprintf(text_ms, sizeof(text_ms), "Text: %.2f ms", text);
        // draw_text(big_font, big_font_text, 20, 260, 0.25, text_ms);

        // u64 last_frame = gettime();
        // float cpu_time = ticks_to_microsecs(last_frame - start_frame) / 1000.f;
        
        // char cpu_usage[64];
        // snprintf(cpu_usage, sizeof(cpu_usage), "CPU: %.2f%%%% Free MEM1: %d Free MEM2: %d", (cpu_time / 16.666666) * 100, SYS_GetArena1Hi() - SYS_GetArena1Lo(), SYS_GetArena2Hi() - SYS_GetArena2Lo());
        // draw_text(big_font, big_font_text, 20, 400, 0.25, cpu_usage);
    }
    
    draw_time = 0;
    obj_particles_time = 0;

    if (state.paused) {
        PSGL_FillScreen(RGBA(0, 0, 0, 127));
        textOffset = (get_text_length(big_font, 0.5, "PAUSED")) / 2;
        draw_text(big_font, big_font_text, screenWidth / 2 - textOffset, screenHeight / 2 - 15, 0.5, "PAUSED");
        update_ir_cursor();
        draw_ir_cursor();
    }
    layersDrawn = 0;

    state.timer += dt;
}

#include <mad.h>

void update_input() {
    update_external_input(&state.input);
}

void update_external_input(KeyInput *input) {
    CellPadData padData;
    if (cellPadGetData(0, &padData) == CELL_PAD_OK && padData.len > 0) {
        uint16_t buttons = (padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] << 8) | padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2];

        static uint16_t prevButtons = 0;
        uint16_t pressed = buttons & ~prevButtons;

        input->pressedA = (pressed & CELL_PAD_CTRL_CROSS) > 0;
        input->holdA    = (buttons & CELL_PAD_CTRL_CROSS) > 0;

        input->pressedB = (pressed & CELL_PAD_CTRL_CIRCLE) > 0;
        input->holdB    = (buttons & CELL_PAD_CTRL_CIRCLE) > 0;

        input->pressedHome = (pressed & CELL_PAD_CTRL_START) > 0;

        input->pressed1orX = (pressed & CELL_PAD_CTRL_SQUARE) > 0;
        input->hold1orX    = (buttons & CELL_PAD_CTRL_SQUARE) > 0;

        input->pressed2orY = (pressed & CELL_PAD_CTRL_TRIANGLE) > 0;
        input->hold2orY    = (buttons & CELL_PAD_CTRL_TRIANGLE) > 0;

        input->pressedPlusOrL = (pressed & CELL_PAD_CTRL_L1) > 0;
        input->holdPlusOrL    = (buttons & CELL_PAD_CTRL_L1) > 0;

        input->pressedMinusOrR = (pressed & CELL_PAD_CTRL_R1) > 0;
        input->holdMinusOrR    = (buttons & CELL_PAD_CTRL_R1) > 0;

        input->pressedDir = 0;
        if (pressed & CELL_PAD_CTRL_UP)    input->pressedDir |= INPUT_UP;
        if (pressed & CELL_PAD_CTRL_DOWN)  input->pressedDir |= INPUT_DOWN;
        if (pressed & CELL_PAD_CTRL_LEFT)  input->pressedDir |= INPUT_LEFT;
        if (pressed & CELL_PAD_CTRL_RIGHT) input->pressedDir |= INPUT_RIGHT;

        input->holdDir = 0;
        if (buttons & CELL_PAD_CTRL_UP)    input->holdDir |= INPUT_UP;
        if (buttons & CELL_PAD_CTRL_DOWN)  input->holdDir |= INPUT_DOWN;
        if (buttons & CELL_PAD_CTRL_LEFT)  input->holdDir |= INPUT_LEFT;
        if (buttons & CELL_PAD_CTRL_RIGHT) input->holdDir |= INPUT_RIGHT;

        input->pressedJump = input->pressedA || input->pressed2orY || input->pressedB;
        input->holdJump    = input->holdA || input->hold2orY || input->holdB;

        prevButtons = buttons;
    }
}

void set_launch_dir(const char* path) {
    if (!path) return;

    const char *last_slash = strrchr(path, '/');
    if (!last_slash) return;

    size_t dir_len = last_slash - path;
    if (dir_len >= 256) dir_len = 255;

    strncpy(launch_dir, path, dir_len);
    launch_dir[dir_len] = '\0';
}

void update_ir_cursor() {
    // Stub for PS3 cursor
}

void draw_ir_cursor() {
    if (ir_is_valid) {
        PSGL_DrawImg(ir_x, ir_y, cursor, ir_angle,1,1,RGBA(255,255,255,255)); // draw cursor
    }
}

int main(int argc, char **argv) {
    if (argc > 0) {
        set_launch_dir(argv[0]);
    }

    // Fat filesystem is not needed on PS3 like this
    // if (!fatInitDefault()) {
	//	output_log("fatInitDefault failure\n");
	// }

    // SYS_STDIO_Report(true);
    
    // PS3 Initialization
    cellPadInit(1);

    int width = 1280; // Defaulting to 720p for now
    int height = 720;

    // PS3 SDK would normally provide the resolution here
    PSGL_Init(width, height);

    srand(time(NULL));

    // Initialise the audio subsystem
    PS3Audio_Init();
    
    // SetVolumeOgg(255);
    load_spritesheet();

    // Set resolution based on mode
    screenWidth  = PSGL_Settings.internalWidth;
    screenHeight = PSGL_Settings.internalHeight;

    // For 720p (1280x720), a centered 4:3 area would be 960x720.
    // widthAdjust = (1280 - 960) / 2 = 160.
    // The game was originally 640x480. 720 / 480 = 1.5. 640 * 1.5 = 960.
    widthAdjust = (screenWidth - (screenHeight * 4.0f / 3.0f)) / 2.0f;

    screen_factor_x = (float) screenWidth / 640;
    screen_factor_y = (float) screenHeight / 480;

    // startTime = gettime();

    cursor = PSGL_LoadTexturePNG(cursor_png, cursor_png_size);
    PSGL_SetHandle(cursor, 14, 22);
    big_font_text = PSGL_LoadTexturePNG(bigFont_png, bigFont_png_size);

    // hopefully this fixes the ir position
    // WPAD_SetVRes(WPAD_CHAN_0,screenWidth,screenHeight);
    
    char robot_plist_path[278];
    
    snprintf(robot_plist_path, sizeof(robot_plist_path), "%s/%s/%s", launch_dir, RESOURCES_FOLDER, "robot.plist");
    parsePlist(robot_plist_path, &robot_animations);

    output_log("Loaded %d animations\n", robot_animations.animCount);
    for (int i = 0; i < robot_animations.animCount; i++) {
        output_log("Animation %s: %d frames\n",
               robot_animations.animations[i].name,
               robot_animations.animations[i].frameCount);
    }
    // WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);

    full_init_variables();
    while(1) {
        switch (gameRoutine) {
            case ROUTINE_MENU:
                if (menu_loop()) goto Exit;
                break;
            case ROUTINE_GAME:
                if(game_loop()) goto Exit;
                break;
        }
    }
Exit:
    PS3Audio_StopMusic();
    ExitGame();
    return 0;
}

static FILE *log_file = NULL;

static void ensure_log_file(void) {
    if (log_file) return;  // already open

    // Initialize SD/USB filesystem
    // if (!fatInitDefault()) return;

    char log_filename[278];
    snprintf(log_filename, sizeof(log_filename), "%s/%s", launch_dir, "output.txt");
    
    log_file = fopen(log_filename, "a");
}

static void close_log_file(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

int output_log(const char *fmt, ...) {
    ensure_log_file();

    if (!log_file) return 0; // failed to open log

    va_list args1, args2;
    int ret;

    va_start(args1, fmt);
    va_copy(args2, args1);  // duplicate args for two outputs

    // Output to log file
    ensure_log_file();
    if (log_file) {
        ret = vfprintf(log_file, fmt, args1);
        fflush(log_file);
    }

    // Output to console (stdout)
    ret = vprintf(fmt, args2);
    
    va_end(args2);
    va_end(args1);

    return ret;
}

static void ExitGame(void) {
    unload_spritesheet();

    close_log_file();

    // Deinitialize Audio
    PS3Audio_Exit();

    // Deinitialize PSGL
    PSGL_Exit();

    // Exit application
    exit(0);
}

#define SPR_ECID_U        924
bool is_dolphin() {        
    return false;
}

unsigned long allocation = 0;

#ifdef REPORT_LEAKS

#ifdef malloc
    #undef malloc
#endif
#ifdef realloc
    #undef realloc
#endif
#ifdef free
    #undef free
#endif
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct AllocInfo {
    void* ptr;
    size_t size;
    const char* file;
    int line;
    struct AllocInfo* next;
} AllocInfo;

static AllocInfo* allocList = NULL;

void add_alloc(void* ptr, size_t size, const char* file, int line) {
    AllocInfo* info = (AllocInfo*)malloc(sizeof(AllocInfo));
    info->ptr = ptr;
    info->size = size;
    info->file = file;
    info->line = line;
    info->next = allocList;
    allocList = info;
}

void remove_alloc(void* ptr) {
    AllocInfo** current = &allocList;
    while (*current) {
        if ((*current)->ptr == ptr) {
            AllocInfo* toDelete = *current;
            *current = toDelete->next;
            free(toDelete);
            return;
        }
        current = &(*current)->next;
    }
}

void* dbg_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (ptr) add_alloc(ptr, size, file, line);
    //printf("[malloc] %zu bytes at %p (%s:%d)\n", size, ptr, file, line);
    return ptr;
}

void dbg_free(void* ptr, const char* file, int line) {
    remove_alloc(ptr);
    //printf("[free] %p (%s:%d)\n", ptr, file, line);
    free(ptr);
}

void* dbg_realloc(void* ptr, size_t size, const char* file, int line) {
    void *old_ptr = ptr;
    void* new_ptr = realloc(ptr, size);
    if (new_ptr) {
        remove_alloc(old_ptr);  // old one
        add_alloc(new_ptr, size, file, line);  // new one
    }
    //printf("[realloc] %p -> %p (%zu bytes, %s:%d)\n",
    //       old_ptr, new_ptr, size, file, line);
    return new_ptr;
}

void report_leaks(void) {
    AllocInfo* current = allocList;
    while (current) {
        printf("[LEAK] %zu bytes at %p allocated in %s:%d\n",
               current->size, current->ptr,
               current->file, current->line);
        current = current->next;
    }
}
#ifndef malloc
    #define malloc(x) dbg_malloc(x, __FILE__, __LINE__)
#endif
#ifndef realloc
    #define realloc(p, s)   dbg_realloc(p, s, __FILE__, __LINE__)
#endif
#ifndef free
    #define free(x)   dbg_free(x, __FILE__, __LINE__)
#endif

#endif