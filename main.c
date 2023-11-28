#include <SDL.h>
#include <stdio.h>

//define the window
#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 400

//define the wall's thickness
#define WALL_THICKNESS 20
//dimensions of snake
#define CELL_WIDTH 20
#define CELL_HEIGHT 20
#define CELL_COUNT ((SCREEN_WIDTH - WALL_THICKNESS * 2) * \
                    (SCREEN_HEIGHT - WALL_THICKNESS * 2)) / \
                    (CELL_WIDTH * CELL_HEIGHT)

#define SNAKE_START_X 200
#define SNAKE_START_Y 200

void initialize(void);
void terminate(int exit_code);
void handle_input(void);
//function to draw walls
void draw_walls(void);
//function to draw snake and function to spawn snake
void draw_snake(void);
void spawn_snake(void);
//function to move snake
void move_snake(void);
void change_direction(SDL_KeyCode new_direction);
//function to handle snake and wall collisions
void handle_collisions(void);
//function to spawn food and function to draw food
void spawn_food(void);
void draw_food(void);
//function to display score
void display_score(void);

typedef struct
{
    SDL_Renderer *renderer;
    SDL_Window *window;
    int running;
    SDL_Rect snake[CELL_COUNT]; //array to create snake segments
    int dx;
    int dy;
    int game_over;
    SDL_Rect food;
    int score;
} Game;

/* initialize global structure to store game state
 and SDL renderer for use in all functions
 */
Game game = {
    .running = 1,
    .dx = CELL_WIDTH,
    .food = {
        .w = CELL_WIDTH, .h = CELL_HEIGHT
    }
};

int main(int argc, char *argv[])
{
    // initialize SDL and the relevant structures
    initialize();

    //initialize snake
    //spawn snake()
    spawn_snake();
    //create food
    spawn_food();

    // enter game loop
    while(game.running)
    {
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderClear(game.renderer);

        handle_input();

        //add update and draw functions
        //move_snake()
        //draw_food()
        //draw_snake()
        //draw_wall()
        //moves the snake
        move_snake();

        //draws the food
        draw_food();
        //draws the snake
        draw_snake();
        //draws the walls
        draw_walls();

        SDL_RenderPresent(game.renderer);

        // wait 100 milliseconds before next iteration
        if(game.score >= 3 && game.score < 6)
        {
            SDL_Delay(140);
        }
        else if(game.score >= 6 && game.score < 10)
        {
            SDL_Delay(120);
        }
        else if(game.score >= 10 && game.score < 14)
        {
            SDL_Delay(110);
        }
        else if(game.score >= 14 && game.score < 18)
        {
            SDL_Delay(100);
        }
        else if(game.score >= 18 && game.score < 22)
        {
            SDL_Delay(80);
        }
        else if(game.score >= 22 && game.score < 32)
        {
            SDL_Delay(60);
        }
        else if(game.score >= 32)
        {
            SDL_Delay(50);
        }
        else
        {
            SDL_Delay(150);
        }
    }
    // make sure program cleans up on exit
    terminate(EXIT_SUCCESS);
}

void initialize(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("error: failed to initialize SDL: %s\n", SDL_GetError());
        terminate(EXIT_FAILURE);
    }

    // create the game window
    game.window = SDL_CreateWindow("Score: 0",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if(!game.window)
    {
        printf("error: failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        terminate(EXIT_FAILURE);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);

    if(!game.renderer)
    {
        printf("error: failed to create renderer: %s\n", SDL_GetError());
        terminate(EXIT_FAILURE);
    }
}

void terminate(int exit_code)
{
    if(game.renderer)
    {
        SDL_DestroyRenderer(game.renderer);
    }
    if(game.window)
    {
        SDL_DestroyWindow(game.window);
    }
    SDL_Quit();
    exit(exit_code);
}

void handle_input()
{
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        /* change the state to not running when close
        or the esc key is pressed so that the game
        loop is exited in main
        */
        if(e.type == SDL_Quit || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
        {
            game.running = 0;
        }
        if(e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_UP
                                     || e.key.keysym.sym == SDLK_DOWN
                                     || e.key.keysym.sym == SDLK_LEFT
                                     || e.key.keysym.sym == SDLK_RIGHT))
        {
            change_direction(e.key.keysym.sym);
        }
    }
}

void draw_walls(void)
{
    //color of the walls are set
    SDL_SetRenderDrawColor(game.renderer, 10, 209, 205, 255);
    //change color of the walls if game over
    if(game.game_over)
    {
            SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    }

    SDL_Rect block = {
        .x = 0,
        .y = 0,
        .w = WALL_THICKNESS,
        .h = SCREEN_HEIGHT,
    };

    //create left wall
    SDL_RenderFillRect(game.renderer, &block);

    //create right wall
    block.x = SCREEN_WIDTH - WALL_THICKNESS;
    SDL_RenderFillRect(game.renderer, &block);

    //create top wall
    block.x = 0;
    block.w = SCREEN_WIDTH;
    block.h = WALL_THICKNESS;
    SDL_RenderFillRect(game.renderer, &block);

    //create bottom wall
    block.y = SCREEN_HEIGHT - WALL_THICKNESS;
    SDL_RenderFillRect(game.renderer, &block);
}

void draw_snake(void)
{
    //create the snake
    int i;
    for(i = 0; i < sizeof(game.snake) / sizeof(game.snake[0]); i++)
    {
        //NULL check
        if(game.snake[i].w == 0)
        {
            break;
        }
        //change color of snake if game over
        if(game.game_over)
        {
            SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(game.renderer, 0, 128, 0, 255);
        }
        // change snake to red upon losing
        //create green part of snake
        SDL_RenderFillRect(game.renderer, &game.snake[i]);

        //create snake body outline
        SDL_SetRenderDrawColor(game.renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(game.renderer, &game.snake[i]);
    }
}

void spawn_snake(void)
{
    //initialize parts of snake to 0
    int i;
    for(i = 0; i < sizeof(game.snake) / sizeof(game.snake[0]); i++)
    {
        game.snake[i].x = 0;
        game.snake[i].y = 0;
        game.snake[i].w = 0;
        game.snake[i].h = 0;
    }
    //set element 0 to head
    game.snake[0].x = SNAKE_START_X;
    game.snake[0].y = SNAKE_START_Y;
    game.snake[0].w = CELL_WIDTH;
    game.snake[0].h = CELL_HEIGHT;

    //create the snake body
    for(i = 1; i < 5; i++)
    {
        game.snake[i] = game.snake[0];
        game.snake[i].x = game.snake[0].x - (CELL_WIDTH * i);
    }
}

void move_snake(void)
{
    //check if game over
    if(game.game_over)
        return;
    //shift elements to the right to make room for the snake head
    int i;
    for(i = sizeof(game.snake) / sizeof(game.snake[0]) - 1; i >= 0; i--)
    {
        game.snake[i] = game.snake[i - 1];
    }
    //insert the head's new position at the beginning of the snake array
    game.snake[0].x = game.snake[1].x + game.dx;
    game.snake[0].y = game.snake[1].y + game.dy;
    game.snake[0].w = CELL_WIDTH;
    game.snake[0].h = CELL_HEIGHT;

    /*if the food touches the snake, make it grow by not removing the tail
    and spawn the food in a new random location
    */
    if(game.food.x == game.snake[0].x && game.food.y == game.snake[0].y)
    {
        spawn_food();
        game.score++;
        display_score();
    }
    else
    {
        //remove tail of snake and initialize element in front of it
        for(i = 5; i < sizeof(game.snake) / sizeof(game.snake[0]); i++)
        {
            if(game.snake[i].w == 0)
            {
                game.snake[i-1].x = 0;
                game.snake[i-1].y = 0;
                game.snake[i-1].w = 0;
                game.snake[i-1].h = 0;
                break;
            }
        }
    }
    //if snake eats, grow snake and score
    //check if snake lost
    handle_collisions();
}

void change_direction(SDL_KeyCode new_direction)
{
    //initialize snake directions
    int going_up = game.dy == -CELL_HEIGHT;
    int going_down = game.dy == CELL_HEIGHT;
    int going_left = game.dx == -CELL_WIDTH;
    int going_right = game.dx == CELL_WIDTH;
    //change the direction of snake using if statements
    if(new_direction == SDLK_UP && !going_down)
    {
        game.dx = 0;
        game.dy = -CELL_HEIGHT;
    }
    if(new_direction == SDLK_DOWN && !going_up)
    {
        game.dx = 0;
        game.dy = CELL_HEIGHT;
    }
    if(new_direction == SDLK_LEFT && !going_right)
    {
        game.dx = -CELL_WIDTH;
        game.dy = 0;
    }
    if(new_direction == SDLK_RIGHT && !going_left)
    {
        game.dx = CELL_WIDTH;
        game.dy = 0;
    }
}

void handle_collisions(void)
{
    //check if snake has collision with itself
    int i;
    for(i = 1; i < sizeof(game.snake) / sizeof(game.snake[0]); i++)
    {
        //exit loop after traversing snake body
        if(game.snake[i].w == 0)
            break;
        if(game.snake[0].x == game.snake[i].x && game.snake[0].y == game.snake[i].y)
        {
            game.game_over = 1;
            return;
        }
    }
    //check if snake has collision with the left wall
    if(game.snake[0].x < WALL_THICKNESS)
    {
        game.game_over = 1;
        return;
    }
    //check if snake has collision with the right wall
    if(game.snake[0].x > SCREEN_WIDTH - WALL_THICKNESS - CELL_WIDTH)
    {
        game.game_over = 1;
        return;
    }
    //check if snake has collision with the top wall
    if(game.snake[0].y < WALL_THICKNESS)
    {
        game.game_over = 1;
        return;
    }
    //check if snake has collision with the bottom wall
    if(game.snake[0].y > SCREEN_HEIGHT - WALL_THICKNESS - CELL_HEIGHT)
    {
        game.game_over = 1;
        return;
    }
}

void spawn_food()
{
    /*generate a random number in multiples of 10
    along the x-axis that fits between the left and right walls
    */
    game.food.x = (rand() % (((SCREEN_WIDTH - CELL_WIDTH - WALL_THICKNESS) / CELL_WIDTH) + 1) * CELL_WIDTH);
    /*generate a random number in multiples of 10
    along the y-axis that fits between the top and bottom walls
    */
    game.food.y = (rand() % (((SCREEN_HEIGHT - CELL_HEIGHT - WALL_THICKNESS) / CELL_HEIGHT) + 1) * CELL_HEIGHT);

    /*if the random number generated is less than the thickness of the left wall,
    have the food spawn next to the left wall
    */
    if(game.food.x < WALL_THICKNESS)
        game.food.x = WALL_THICKNESS;
    /*if the random number generated is less than the thickness of the top wall,
    have the food spawn next to the top wall
    */
    if(game.food.y < WALL_THICKNESS)
        game.food.y = WALL_THICKNESS;

    //spawn food if not in snake
    int i;
    for(i = 0; i < sizeof(game.snake) / sizeof(game.snake[0]); i++)
    {
        //exit loop at the end of the snake
        if(game.snake[i].w == 0)
            break;
        if(game.snake[i].x == game.food.x && game.snake[i].y == game.food.y)
        {
            spawn_food();
            break;
        }
    }
}

void draw_food()
{
    //make red food
    SDL_SetRenderDrawColor(game.renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(game.renderer, &game.food);
}

void display_score(void)
{
    char buffer[20];
    snprintf(buffer, 20, "Score: %d", game.score);
    SDL_SetWindowTitle(game.window, buffer);
}
