#include "game.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    Game game = {0};
    if (!game_init(&game)) {
        printf("init failed\n");
        game_cleanup(&game);
        return 1;
    }
    game_run(&game);
    game_cleanup(&game);
    return 0;
}
