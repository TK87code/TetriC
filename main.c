#include <raylib.h>       /* https://www.raylib.com/index.html */
#include <stdlib.h>       /* calloc() */
#include <string.h>       /* strlen() */

#define TET_SIZE 4        /* Size of tetrimino array in x&y */ 

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 400
#define SCREEN_OFFSET 2

#define CELL_SIZE 16      /* Size of cell (which contains tiles) */
#define TILE_SIZE 15      /* Size of tile squares */

#define FIELD_WIDTH 12    /* Number of cols of the field including a wall */
#define FIELD_HEIGHT 21   /* Number of rows of the field including a floor */

/* Game settings */
#define DEFAULT_SPEED 1.1 /* Piece are forced to drop every this time */
#define DEFAULT_DIFF 0.1  /* Default difficulty */
#define MAX_DIFF 0.8
#define INCREASE_DIFF_EVERY 10
#define DIFF_STEP 0.05


/* Function Prototypes*/
unsigned char* create_field(void);
int rotate(int px, int py, int r);
int does_collide(unsigned char *field, char **tet, int tet_id, int rotation, int pos_x, int pos_y);
void check_lines(unsigned char *field, int pos_y, int *score);
void draw_field(unsigned char *field, Color *colors);
void draw_tetrimino(char **tet, int tet_id, int pos_x, int pos_y,int rotation, Color *colors);
void fix_tetrimino(unsigned char *field, char **tet, int tet_id, int pox_x, int pos_y, int rotation);

int main(char argc, char **argv)
{
    char* tetrimino[7];
    unsigned char* field;
    Color colors[10] = {BLACK, SKYBLUE, YELLOW, GREEN, RED, BLUE, ORANGE, PURPLE, GOLD, GRAY}; /* Array of Raylib colors, used for rectangle drawing */ 
    int pos_x, pos_y;         /* Current position of tetrimino piece */
    int current_tet;          /* Current tetrimino id*/
    int next_tet;             /* Next tetrimino id */
    int current_rotation = 0; /* Current Rotation */
    float timer = 0.0;        /* A timer to know when the tetrimino should fall down */
    float current_speed;      /* When timer == speed, tetrimino falls down */
    float difficulty;         /* Amount of seconds to subtract from speed */
    int is_gameover = 0;      /* A flag to know if it's game over. */
    int piece_dropped = 0;    /* Number of piace dropped. */
    int score = 0;            /* A strage of score value */
    
    /* Window Initialization */
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TetriC");
    SetTargetFPS(60);
    SetTraceLogLevel(LOG_ERROR);
    
    /* Create tetrimonos 4*4 string */ 
    tetrimino[0] = "..x...x...x...x."; /* I-tetrimino */
    tetrimino[1] = ".....xx..xx....."; /* O-tetrimino */
    tetrimino[2] = ".x...xx...x....."; /* S_tetrimino */
    tetrimino[3] = "..x..xx..x......"; /* Z-tetrimino */
    tetrimino[4] = ".xx..x...x......"; /* J-tetrimino */
    tetrimino[5] = ".x...x...xx....."; /* L-tetrimino */
    tetrimino[6] = "..x..xxx........"; /* T-tetrimino */
    
    field = create_field();
    
    current_tet = GetRandomValue(0, 6);
    next_tet = GetRandomValue(0, 6);
    difficulty = DEFAULT_DIFF;
    current_speed = DEFAULT_SPEED - difficulty;
    pos_x = (FIELD_WIDTH / 2) - (TET_SIZE / 2);
    pos_y = 0;
    
    while(!WindowShouldClose()){
        if (is_gameover == 0){
            timer += GetFrameTime(); /* Get time in seconds for last frame drawn */
            
            /* Tetrimino falls down every 1 sec */
            if (timer >= current_speed){
                timer = 0.0;
                if (does_collide(field, tetrimino, current_tet, current_rotation, pos_x, pos_y + 1) == 0)
                    pos_y += 1;
                else{
                    fix_tetrimino(field, tetrimino, current_tet, pos_x, pos_y, current_rotation);
                    
                    /* Check if there are completed line in the field */
                    check_lines(field, pos_y, &score);
                    
                    /* Spawn a new tetrimino & reset values */
                    score += 25;
                    current_tet = next_tet;
                    next_tet = GetRandomValue(0, 6);
                    current_rotation = 0;
                    pos_x = (FIELD_WIDTH / 2) - (TET_SIZE / 2);
                    pos_y = 0;
                    piece_dropped++;
                    
                    /* Change difficulty */
                    if ((piece_dropped % INCREASE_DIFF_EVERY == 0) && difficulty < MAX_DIFF)
                        difficulty += DIFF_STEP;
                    
                    
                    /* If the new piece does not fit, gameover */
                    if (does_collide(field, tetrimino, current_tet, current_rotation, pos_x, pos_y + 1) != 0)
                        is_gameover = 1;
                }
            }
            
            /* Input Handling */
            if (IsKeyPressed(KEY_RIGHT) && does_collide(field, tetrimino, current_tet, current_rotation, pos_x + 1, pos_y) == 0)
                pos_x += 1;
            
            if (IsKeyPressed(KEY_LEFT) && does_collide(field, tetrimino, current_tet, current_rotation, pos_x - 1, pos_y) == 0)
                pos_x -= 1;
            
            if (IsKeyDown(KEY_DOWN)){
                current_speed= 0.05;
            }
            else if (IsKeyReleased(KEY_DOWN))
                current_speed = DEFAULT_SPEED - difficulty;
            
            if (IsKeyPressed(KEY_Z) && does_collide(field, tetrimino, current_tet, current_rotation + 1, pos_x, pos_y) == 0)
                current_rotation += 1;
        }
        
        BeginDrawing();
        ClearBackground(colors[0]); /* Clear screen with BLACK */
        
        draw_field(field, colors);
        draw_tetrimino(tetrimino, current_tet, pos_x, pos_y, current_rotation, colors);
        
        /* Text Drawing*/
        DrawText(TextFormat("LEVEL: %d",((int)((difficulty - DEFAULT_DIFF) / DIFF_STEP) + 1)), (FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE, 20, RAYWHITE);
        
        DrawText("--SCORE--",(FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE * 3, 20, RAYWHITE);
        
        DrawText(TextFormat("%d",score),(FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE * 4, 20, RAYWHITE);
        
        
        DrawText("--NEXT--",(FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE * 6, 20, RAYWHITE);
        
        /* Draw next tetrimino */
        draw_tetrimino(tetrimino, next_tet, (FIELD_WIDTH + 3), 12, 0, colors);
        
        if (is_gameover != 0)
            DrawText("GameOver!!", (SCREEN_WIDTH / 2) - ((strlen("GameOver!!") - 1) * 20), (SCREEN_HEIGHT / 2) - 20, 40, RED);
        
        EndDrawing();
    }
    
    CloseWindow();
    free(field);
    return 0;
}

/* create_field: This function allocate memory for field array, places walls on it. */
unsigned char *create_field(void)
{
    int fx, fy;
    unsigned char *f;
    
    f = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(unsigned char));
    
    /* Write walls of the field*/
    for (fy = 0; fy < FIELD_HEIGHT; fy++){
        for (fx = 0; fx < FIELD_WIDTH; fx++){
            f[fy * FIELD_WIDTH + fx] = (fy == FIELD_HEIGHT - 1 || fx == 0 || fx == FIELD_WIDTH -1) ? 9 : 0;
        }
    }
    
    return f;
}

/* rotate: this function returns calculated index of tetrimino string when rotate given degrees. */
int rotate(int px, int py, int r)
{
    int p_i = 0; /* piece index */
    
    switch (r % 4){
        case 0:{
            p_i = py * TET_SIZE + px; /* 0 dgree */ 
        }break; 
        
        case 1:{
            p_i = 12 + py - (px * TET_SIZE); /* 90 degree */
        }break; 
        
        case 2:{
            p_i = 15 - (py * TET_SIZE) - px; /* 180 degree */
        }break;
        
        case 3:{
            p_i = 3 - py + (TET_SIZE * px); /* 270 degree */
        }break;
    }
    
    return p_i;
}

/* does_collode: Check if the piece collide with a field objects or boundary. Return 0 on false, 1 on true.*/
int does_collide(unsigned char *field, char** tet, int tet_id, int rotation, int pos_x, int pos_y)
{
    int px, py;
    int p_i; /* piece index*/
    
    for (py = 0; py < TET_SIZE; py++){
        for (px = 0; px < TET_SIZE; px++){
            p_i = rotate(px, py, rotation);
            
            if(tet[tet_id][p_i] != '.' && field[(pos_y + py) * FIELD_WIDTH + (pos_x + px)] != 0)
                return 1;
        }
    }
    
    return 0;
}

/* check_lines: Check if there are complete lines, if so, delete the row and fall pieces above.*/
void check_lines(unsigned char *field, int pos_y, int *score)
{
    int fy, fx, fy2;
    int c;
    int lc = 0;
    
    for (fy = pos_y; fy < pos_y + TET_SIZE; fy++){
        if (fy != FIELD_HEIGHT - 1){
            
            c = 0;
            for (fx = 0; fx < FIELD_WIDTH; fx++){
                if (field[fy * FIELD_WIDTH + fx] != 0)
                    c++;
            }
            
            if (c == FIELD_WIDTH){
                lc++;
                for (fy2 = fy; fy2 > 0; fy2--){
                    for (fx = 1; fx < FIELD_WIDTH - 1; fx++){
                        field[fy2 * FIELD_WIDTH + fx] = field[(fy2 - 1) * FIELD_WIDTH + fx];
                        field[fx] = (fx == 0 || fx == FIELD_WIDTH - 1) ? 9 : 0; 
                    }
                }
            }
        }
    }
    if (lc != 0)
        *score += (lc == 1) ? 100 : (100 * lc) * (1 + (float)(lc - 1) / 10.0);
}

/* draw_field: Draw field */
void draw_field(unsigned char *field, Color *colors)
{
    int fy, fx;
    
    for (fy = 0; fy < FIELD_HEIGHT; fy++){
        for(fx = 0; fx < FIELD_WIDTH; fx++){
            DrawRectangle((fx + SCREEN_OFFSET) * CELL_SIZE,  (fy + SCREEN_OFFSET) * CELL_SIZE, TILE_SIZE, TILE_SIZE, colors[field[fy * FIELD_WIDTH + fx]]);
        }
    }
}

/* draw_tetrimino: Draw tetrimino */
void draw_tetrimino(char** tet, int tet_id, int pos_x, int pos_y, int rotation, Color *colors)
{
    int py, px;
    
    for (py = 0; py < TET_SIZE; py++){
        for (px = 0; px < TET_SIZE; px++){
            if (tet[tet_id][rotate(px, py, rotation)] == 'x')
                DrawRectangle((pos_x + px + SCREEN_OFFSET) * CELL_SIZE, (pos_y + py + SCREEN_OFFSET) * CELL_SIZE, TILE_SIZE, TILE_SIZE, colors[tet_id + 1]);
        }
    }
}

/* fix_tetrimino: Make a tetrimino object on the field(Write it onto the field memory). */
void fix_tetrimino(unsigned char *field, char **tet, int tet_id, int pos_x, int pos_y, int rotation)
{
    int py, px;
    
    for (py = 0; py < TET_SIZE; py++){
        for (px = 0; px < TET_SIZE; px++){
            if (tet[tet_id][rotate(px, py, rotation)] == 'x'){
                field[(pos_y + py) * FIELD_WIDTH + (pos_x + px)] = tet_id + 1;
            }
        }
    }
}