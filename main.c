#include <raylib.h> /* https://www.raylib.com/index.html */
#include <stdlib.h> /* malloc(), exit() */ 

#define TET_SIZE 4 /* Size of tetrimino array in x&y */ 

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

#define CELL_SIZE 16
#define TILE_SIZE 15

#define DEFAULT_SPEED 1.1

#define SCREEN_OFFSET 2

#define FIELD_WIDTH 12 /* Number of cols of the field including a wall */
#define FIELD_HEIGHT 21 /* Number of rows of the field including a floor */

char* tetrimino[6];
unsigned char* field; 
Color colors[9]; /* Array of Raylib colors, used for rectangle drawing */ 
int score;

/* Function Prototypes*/
int rotate(int px, int py, int r);
int does_collide(int tetrimino_id, int rotation, int pos_x, int pos_y);
void check_lines(int pos_y);
void draw_field(void);

int main(char argc, char** argv)
{
    int fx, fy; /* x & y in the field array */
    int px, py; /*x & y in the tetrimino array */
    int current_tet; /* Current tetrimino id*/
    int current_rotation = 0; /* Current Rotation */
    int pos_x = FIELD_WIDTH / 2; /* Current pos_x of tetrimino piece */
    int pos_y = 0; /* Current pos_y of tetrimino piece */
    float timer = 0.0; /* A timer to know when the tetrimino should fall down */
    float difficulty = 0.1;
    float current_speed;
    int is_gameover = 0;
    int piece_dropped = 0;
    
    /* Window Initialization */
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tetris");
    
    SetTargetFPS(60);
    
    /* Create tetrimonos 4*4 string */ 
    tetrimino[0] = "..x...x...x...x."; /* I-tetrimino */
    tetrimino[1] = ".....xx..xx....."; /* O-tetrimino */
    tetrimino[2] = ".x...xx...x....."; /* S_tetrimino */
    tetrimino[3] = "..x..xx..x......"; /* Z-tetrimino */
    tetrimino[4] = ".xx..x...x......"; /* J-tetrimino */
    tetrimino[5] = ".x...x...xx....."; /* L-tetrimino */
    tetrimino[6] = "..x..xxx........"; /* T-tetrimino */
    
    colors[0] = BLACK;
    colors[1] = SKYBLUE;
    colors[2] = YELLOW;
    colors[3] = GREEN;
    colors[4] = RED;
    colors[5] = BLUE;
    colors[6] = ORANGE;
    colors[7] = PURPLE;
    colors[8] = GOLD;
    colors[9] = GRAY;
    
    /* Create field */ 
    field = malloc(FIELD_WIDTH * FIELD_HEIGHT * sizeof(unsigned char));
    memset(field, 0, FIELD_WIDTH * FIELD_HEIGHT * sizeof(unsigned char));
    
    /* Write walls of the field*/
    for (fy = 0; fy < FIELD_HEIGHT; fy++){
        for (fx = 0; fx < FIELD_WIDTH; fx++){
            if(fy == FIELD_HEIGHT - 1 || fx == 0 || fx == FIELD_WIDTH -1)
                field[fy * FIELD_WIDTH + fx] = 9; /* This number will be an index of color array -> GRAY*/
        }
    }
    
    current_tet = GetRandomValue(0, 6);
    current_speed = DEFAULT_SPEED - difficulty;
    
    while(!WindowShouldClose()){
        if (is_gameover == 0){
            timer += GetFrameTime(); /* Get time in seconds for last frame drawn */
            
            /* increase difficulty every 5 piece dropped */
            if (piece_dropped  == 5){
                if (difficulty < 0.8) /* Max Difficulty -> MAX_DIFFICULTY */
                    difficulty += 0.05;
                piece_dropped = 0;
            }
            
            /* Tetrimino falls down every 1 sec */
            if (timer >= current_speed){
                timer = 0.0;
                if (does_collide(current_tet, current_rotation, pos_x, pos_y + 1) == 0){
                    pos_y += 1;
                }else{
                    /* Make the current tetrimino an object in the field */
                    for (py = 0; py < TET_SIZE; py++){
                        for (px = 0; px < TET_SIZE; px++){
                            if (tetrimino[current_tet][rotate(px, py, current_rotation)] == 'x'){
                                field[(pos_y + py) * FIELD_WIDTH + (pos_x + px)] = current_tet + 1;
                            }
                        }
                    }
                    
                    /* Check if there are completed line in the field */
                    check_lines(pos_y);
                    
                    /* Spawn a new tetrimino & reset values */
                    score += 25;
                    current_tet = GetRandomValue(0, 6);
                    current_rotation = 0;
                    pos_x = FIELD_WIDTH / 2;
                    pos_y = 0;
                    piece_dropped++;
                    
                    /* If the new piece does not fit, gameover */
                    if (does_collide(current_tet, current_rotation, pos_x, pos_y + 1) != 0){
                        is_gameover = 1;
                    }
                }
            }
            
            /* Input Handling */
            if (IsKeyPressed(KEY_RIGHT)){
                if (does_collide(current_tet, current_rotation, pos_x + 1, pos_y) == 0)
                    pos_x += 1;
            }else if (IsKeyPressed(KEY_LEFT)){
                if (does_collide(current_tet, current_rotation, pos_x - 1, pos_y) == 0)
                    pos_x -= 1;
            }else if (IsKeyDown(KEY_DOWN)){
                current_speed= 0.05;
            }else if (IsKeyReleased(KEY_DOWN)){
                current_speed = DEFAULT_SPEED - difficulty;
            }else if (IsKeyPressed(KEY_Z)){
                if (does_collide(current_tet, current_rotation + 1, pos_x, pos_y) == 0)
                    current_rotation += 1;
            }
        }
        
        BeginDrawing();
        ClearBackground(colors[0]); /* Clear screen with BLACK */
        
        /* Drawing Field*/
        draw_field();
        
        /* Draw Tetrimino*/
        for (py = 0; py < TET_SIZE; py++){
            for (px = 0; px < TET_SIZE; px++){
                if (tetrimino[current_tet][rotate(px, py, current_rotation)] == 'x')
                    DrawRectangle((pos_x + px + SCREEN_OFFSET) * CELL_SIZE, (pos_y + py + SCREEN_OFFSET) * CELL_SIZE, TILE_SIZE, TILE_SIZE, colors[current_tet + 1]);
            }
        }
        
        /* Text Drawing*/
        
        DrawText(TextFormat("SCORE: %d", score),(FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE, 20, RAYWHITE);
        
        DrawText(TextFormat("DIFFICULTY: %f", difficulty),(FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE + 20, 20, RAYWHITE);
        
        
        if (is_gameover != 0){
            DrawText("GameOver!!", (FIELD_WIDTH + SCREEN_OFFSET + 2) * CELL_SIZE, SCREEN_OFFSET * CELL_SIZE + 40, 20, RAYWHITE);
        }
        
        EndDrawing();
    }
    
    CloseWindow();
    free(field);
    return 0;
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
int does_collide(int tetrimino_id, int rotation, int pos_x, int pos_y){
    int px, py;
    int p_i; /* piece index*/
    
    for (py = 0; py < TET_SIZE; py++){
        for (px = 0; px < TET_SIZE; px++){
            p_i = rotate(px, py, rotation);
            
            if(tetrimino[tetrimino_id][p_i] != '.' && field[(pos_y + py) * FIELD_WIDTH + (pos_x + px)] != 0){
                return 1;
            }
        }
    }
    
    return 0;
}

void check_lines(int pos_y){
    int fy, fx;
    int c;
    int lines_completed[FIELD_HEIGHT] = {0};
    int target_fy;
    
    for (fy = pos_y; fy < pos_y + TET_SIZE; fy++){
        if (fy != FIELD_HEIGHT - 1){
            
            c = 0;
            for (fx = 0; fx < FIELD_WIDTH; fx++){
                if (field[fy * FIELD_WIDTH + fx] != 0)
                    c++;
            }
            
            if (c == FIELD_WIDTH){
                lines_completed[fy] = 1;
                score += 100;
            }
        }
    }
    
    /* Moving down the row by number of line completed */
    for (fy = pos_y; fy < pos_y + TET_SIZE; fy++){
        if (lines_completed[fy] == 1){
            
            for (target_fy = fy; target_fy >= 0; target_fy--){
                for (fx = 0; fx < FIELD_WIDTH; fx++){
                    if (target_fy == 0){
                        if (fx == 0 || fx == FIELD_WIDTH -1){
                            field[target_fy * FIELD_WIDTH + fx] = 9;
                        }else{
                            field[target_fy * FIELD_WIDTH + fx] = 0;
                        }
                    }else{
                        field[target_fy * FIELD_WIDTH + fx] = field[(target_fy - 1) * FIELD_WIDTH + fx];
                    }
                }
            }
        }
    }
}

void draw_field(){
    int fy, fx;
    
    for (fy = 0; fy < FIELD_HEIGHT; fy++){
        for(fx = 0; fx < FIELD_WIDTH; fx++){
            DrawRectangle((fx + SCREEN_OFFSET) * CELL_SIZE,  (fy + SCREEN_OFFSET) * CELL_SIZE, TILE_SIZE, TILE_SIZE, colors[field[fy * FIELD_WIDTH + fx]]);
        }
    }
}
