/*
 * game.c - main game logic and state management
 *
 * init/shutdown, the main loop, collision detection, difficulty
 * ramping, and the state machine (start -> playing -> gameover)
 *
 * all the actual drawing of UI overlays is in hud.c
 */

#include "game.h"
#include "hud.h"
#include "texture.h"

static void game_reset(Game *game);
static void handle_events(Game *game);
static void update(Game *game);
static void render(Game *game);
static void check_collisions(Game *game);
static void update_difficulty(Game *game);

bool game_init(Game *game)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("ERROR: SDL_Init: %s\n", SDL_GetError());
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("ERROR: IMG_Init: %s\n", IMG_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        printf("ERROR: TTF_Init: %s\n", TTF_GetError());
        return false;
    }

    game->window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game->window) {
        printf("ERROR: window: %s\n", SDL_GetError());
        return false;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) {
        printf("ERROR: renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

    game->font = texture_load_font(FONT_PATH, FONT_SIZE);

    player_init(&game->player, game->renderer);
    enemy_init(&game->enemies, game->renderer);
    road_init(&game->road);
    audio_init(&game->audio);

    srand((unsigned int)time(NULL));

    game->high_score = 0;
    game->running = true;
    game_reset(game);
    game->state = STATE_START;   // override reset's STATE_PLAYING

    printf("ready to go!\n");
    return true;
}

// main loop. runs til the player closes the window or hits escape
void game_run(Game *game)
{
    while (game->running) {
        Uint32 t0 = SDL_GetTicks();

        handle_events(game);
        update(game);
        render(game);

        // simple frame limiter - sleep if we're ahead of schedule
        Uint32 elapsed = SDL_GetTicks() - t0;
        if (elapsed < FRAME_DELAY)
            SDL_Delay(FRAME_DELAY - elapsed);
    }
}

static void game_reset(Game *game)
{
    game->state = STATE_PLAYING;
    game->score = 0;
    game->lives = STARTING_LIVES;
    game->enemy_speed = ENEMY_BASE_SPEED;
    game->difficulty_level = 0;

    player_reset(&game->player);
    enemy_reset(&game->enemies);
    road_init(&game->road);
    audio_set_playing(&game->audio, true);
}

static void handle_events(Game *game)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            game->running = false;
            return;
        }
        if (ev.type != SDL_KEYDOWN) continue;

        switch (ev.key.keysym.sym) {
        case SDLK_ESCAPE:
            game->running = false;
            return;
        case SDLK_SPACE:
            if (game->state == STATE_START || game->state == STATE_GAMEOVER)
                game_reset(game);
            break;
        case SDLK_p:
            // toggle pause
            if (game->state == STATE_PLAYING) {
                game->state = STATE_PAUSED;
                audio_set_playing(&game->audio, false);
            } else if (game->state == STATE_PAUSED) {
                game->state = STATE_PLAYING;
                audio_set_playing(&game->audio, true);
            }
            break;
        case SDLK_m:
            // toggle music using the existing .playing flag
            audio_set_playing(&game->audio, !game->audio.playing);
            break;
        default:
            break;
        }
    }
}

static void update(Game *game)
{
    if (game->state != STATE_PLAYING) return;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    player_handle_input(&game->player, keys);
    enemy_update(&game->enemies, game->enemy_speed);
    road_update(&game->road, game->enemy_speed);
    check_collisions(game);
    game->score += SCORE_PER_FRAME;
    update_difficulty(game);
}

/*
 * collision detection - shrink both hitboxes a tiny bit
 * so near-misses feel fair instead of frustrating.
 * the shrink amount is kinda arbitrary, 5px felt right in testing
 */
static void check_collisions(Game *game)
{
    SDL_Rect prect = player_get_rect(&game->player);
    int shrink = 5;
    prect.x += shrink;
    prect.y += shrink;
    prect.w -= shrink * 2;
    prect.h -= shrink * 2;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &game->enemies.pool[i];
        if (!e->active) continue;

        SDL_Rect erect = enemy_get_rect(e);
        erect.x += shrink;
        erect.y += shrink;
        erect.w -= shrink * 2;
        erect.h -= shrink * 2;

        if (!SDL_HasIntersection(&prect, &erect)) continue;

        // hit!
        e->active = false;
        game->lives--;

        if (game->lives <= 0) {
            if (game->score > game->high_score)
                game->high_score = game->score;
            game->state = STATE_GAMEOVER;
            audio_set_playing(&game->audio, false);
            return;
        }

        // still alive, teleport back to center
        player_reset(&game->player);
    }
}

// every DIFFICULTY_STEP points things get a little faster
static void update_difficulty(Game *game)
{
    int level = game->score / DIFFICULTY_STEP;
    if (level <= game->difficulty_level) return;

    game->difficulty_level = level;
    game->enemy_speed = ENEMY_BASE_SPEED + level * SPEED_INCREMENT;

    game->enemies.spawn_interval -= 5;
    if (game->enemies.spawn_interval < 20)
        game->enemies.spawn_interval = 20;

    printf("lvl %d | speed %.0f | spawn %d\n",
           level, game->enemy_speed, game->enemies.spawn_interval);
}

static void render(Game *game)
{
    road_draw(game->renderer, &game->road);

    switch (game->state) {
    case STATE_START:
        hud_draw_start(game->renderer, game->font);
        break;

    case STATE_PLAYING:
        enemy_draw(game->renderer, &game->enemies);
        player_draw(game->renderer, &game->player);
        hud_draw_playing(game->renderer, game->font,
                         game->player.texture, game->score, game->lives);
        break;

    case STATE_PAUSED:
        enemy_draw(game->renderer, &game->enemies);
        player_draw(game->renderer, &game->player);
        hud_draw_playing(game->renderer, game->font,
                         game->player.texture, game->score, game->lives);
        hud_draw_paused(game->renderer, game->font);
        break;

    case STATE_GAMEOVER:
        enemy_draw(game->renderer, &game->enemies);
        player_draw(game->renderer, &game->player);
        hud_draw_gameover(game->renderer, game->font,
                          game->score, game->high_score);
        break;
    }

    SDL_RenderPresent(game->renderer);
}

void game_cleanup(Game *game)
{
    audio_cleanup(&game->audio);
    player_cleanup(&game->player);
    enemy_cleanup(&game->enemies);

    if (game->font) {
        TTF_CloseFont(game->font);
        game->font = NULL;
    }

    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window)   SDL_DestroyWindow(game->window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    printf("bye\n");
}
