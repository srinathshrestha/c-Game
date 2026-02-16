// game.c - everything in one place for now, might split later
#include "game.h"
#include "texture.h"

static void game_reset(Game *game);
static void handle_events(Game *game);
static void update(Game *game);
static void render(Game *game);
static void check_collisions(Game *game);

bool game_init(Game *game)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
    if (!game->window) return false;

    game->renderer = SDL_CreateRenderer(game->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) return false;

    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    game->font = texture_load_font(FONT_PATH, FONT_SIZE);

    player_init(&game->player, game->renderer);
    enemy_init(&game->enemies, game->renderer);
    road_init(&game->road);
    srand((unsigned int)time(NULL));

    game->high_score = 0;
    game->running = true;
    game_reset(game);
    game->state = STATE_START;
    return true;
}

void game_run(Game *game)
{
    while (game->running) {
        Uint32 t0 = SDL_GetTicks();
        handle_events(game);
        update(game);
        render(game);
        Uint32 elapsed = SDL_GetTicks() - t0;
        if (elapsed < FRAME_DELAY) SDL_Delay(FRAME_DELAY - elapsed);
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
}

static void handle_events(Game *game)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) { game->running = false; return; }
        if (ev.type != SDL_KEYDOWN) continue;
        switch (ev.key.keysym.sym) {
        case SDLK_ESCAPE: game->running = false; return;
        case SDLK_SPACE:
            if (game->state == STATE_START || game->state == STATE_GAMEOVER)
                game_reset(game);
            break;
        default: break;
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

    int level = game->score / DIFFICULTY_STEP;
    if (level > game->difficulty_level) {
        game->difficulty_level = level;
        game->enemy_speed = ENEMY_BASE_SPEED + level * SPEED_INCREMENT;
        game->enemies.spawn_interval -= 5;
        if (game->enemies.spawn_interval < 20)
            game->enemies.spawn_interval = 20;
    }
}

static void check_collisions(Game *game)
{
    SDL_Rect prect = { (int)game->player.x, (int)game->player.y,
                       game->player.w, game->player.h };
    int shrink = 5;
    prect.x += shrink; prect.y += shrink;
    prect.w -= shrink*2; prect.h -= shrink*2;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &game->enemies.pool[i];
        if (!e->active) continue;
        SDL_Rect erect = enemy_get_rect(e);
        erect.x += shrink; erect.y += shrink;
        erect.w -= shrink*2; erect.h -= shrink*2;
        if (!SDL_HasIntersection(&prect, &erect)) continue;

        e->active = false;
        game->lives--;
        if (game->lives <= 0) {
            if (game->score > game->high_score) game->high_score = game->score;
            game->state = STATE_GAMEOVER;
            return;
        }
        player_reset(&game->player);
    }
}

// all the drawing - kinda messy having it all here but whatever
static void render(Game *game)
{
    road_draw(game->renderer, &game->road);

    if (game->state == STATE_START) {
        SDL_Color w = {255,255,255,255};
        SDL_Color y = {255,255,0,255};
        texture_render_text(game->renderer, game->font, "CAR RACING", 300, 240, y);
        texture_render_text(game->renderer, game->font, "Press SPACE to Start", 250, 300, w);
    } else if (game->state == STATE_PLAYING) {
        enemy_draw(game->renderer, &game->enemies);
        player_draw(game->renderer, &game->player);
        // hud
        SDL_Color w = {255,255,255,255};
        char buf[64];
        snprintf(buf, sizeof(buf), "Score: %d", game->score);
        texture_render_text(game->renderer, game->font, buf, 10, 10, w);
        snprintf(buf, sizeof(buf), "Lives: %d", game->lives);
        texture_render_text(game->renderer, game->font, buf, 650, 10, w);
    } else {
        enemy_draw(game->renderer, &game->enemies);
        player_draw(game->renderer, &game->player);
        SDL_Color r = {255,50,50,255};
        SDL_Color w = {255,255,255,255};
        char buf[64];
        texture_render_text(game->renderer, game->font, "GAME OVER", 310, 220, r);
        snprintf(buf, sizeof(buf), "Score: %d", game->score);
        texture_render_text(game->renderer, game->font, buf, 320, 270, w);
        texture_render_text(game->renderer, game->font, "Press SPACE to Restart", 240, 340, w);
    }
    SDL_RenderPresent(game->renderer);
}

void game_cleanup(Game *game)
{
    player_cleanup(&game->player);
    enemy_cleanup(&game->enemies);
    if (game->font) { TTF_CloseFont(game->font); game->font = NULL; }
    if (game->renderer) SDL_DestroyRenderer(game->renderer);
    if (game->window) SDL_DestroyWindow(game->window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
