#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
struct timespec ts1 = {.tv_sec = 0, .tv_nsec = 0};

int highscore;
int score = 0;
int enemy_count;
int default_enemy_count = 1;
int default_tree_count;
int default_stone_count;

typedef char String[50];

char grid[9][9];
int rows;
int cols;
int x_cord;
int y_cord;

void getHigh();
void saveHigh();
void make_vars();
void go_up();
void go_down();
void go_left();
void go_right();
void attack();
void cut();
void mine();
void to_up(char *in);
char *alias(char *in);

void print_help();
void render_scene();
void setup_game();

int main() {
  getHigh();
  srand(time(NULL));
  system(CLEAR_CMD);
  make_vars();
  setup_game();
  render_scene();
  String command;

  while (1 == 1) {
    printf("\nType HELP for help\n");
    printf("Command: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
      clearerr(stdin);
      continue;
    }
    command[strcspn(command, "\n")] = '\0';
    to_up(command);

    system(CLEAR_CMD);
    if (strcmp(command, "DEBUG") == 0) {
      printf("Number of rows: %d\n", rows);
      printf("Number of columns: %d\n", cols);
      printf("Default number of Trees: %d\n", default_tree_count);
      printf("Default number of Stones: %d\n", default_stone_count);
      printf("Default enemy count for this level: %d\n", default_enemy_count);
      printf("Remaining enemies: %d\n", enemy_count);
    } else if (strcmp(alias(command), "HELP") == 0) {
      print_help();
      render_scene();
    } else if (strcmp(alias(command), "UP") == 0) {
      go_up();
      render_scene();
    } else if (strcmp(alias(command), "DOWN") == 0) {
      go_down();
      render_scene();
    } else if (strcmp(alias(command), "LEFT") == 0) {
      go_left();
      render_scene();
    } else if (strcmp(alias(command), "ATTACK") == 0) {
      attack();
      render_scene();
    } else if (strcmp(alias(command), "CUT") == 0) {
      cut();
      render_scene();
    } else if (strcmp(alias(command), "MINE") == 0) {
      mine();
      render_scene();
    } else if (strcmp(alias(command), "RIGHT") == 0) {
      go_right();
      render_scene();
    } else if (strcmp(alias(command), "EXIT") == 0) {
      system(CLEAR_CMD);
      printf("Your final score: %d", score);
      printf("Thanks for playing!\n");
      break;
    } else {
      render_scene();
      printf("INVALID COMMAND!\n");
      fflush(stdout);
    }
    saveHigh();
  }

  return 0;
}

void render_scene() {
  system(CLEAR_CMD);
  printf("Highscore: %d\n", highscore);
  printf("Score: %d\n", score);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      printf("%c ", grid[i][j]);
      fflush(stdout);
    }
    printf("\n");
  }
}

void go_up() {
  if (y_cord == 0 && enemy_count > 0) {
    printf("CANNOT MOVE YET\n");
  } else if (y_cord == 0 && enemy_count <= 0) {
    enemy_count = default_enemy_count;
    y_cord = rows - 1;
    setup_game();
    render_scene();
  } else if (grid[y_cord - 1][x_cord] == 'Y' ||
             grid[y_cord - 1][x_cord] == 'O') {
    printf("There is an obstacle\n");
  } else if (grid[y_cord - 1][x_cord] == 'W' ||
             grid[y_cord - 1][x_cord] == 'w') {
    grid[y_cord][x_cord] = '-';
    grid[y_cord - 1][x_cord] = 'x';
    y_cord = y_cord - 1;
    render_scene();
    printf("You died\n");
    exit(0);
  } else {
    grid[y_cord][x_cord] = '-';
    grid[y_cord - 1][x_cord] = 'X';
    y_cord = y_cord - 1;
  }
}

void go_down() {
  if (y_cord == rows - 1 && enemy_count > 0) {
    printf("CANNOT MOVE YET\n");
  } else if (y_cord == rows - 1 && enemy_count <= 0) {
    enemy_count = default_enemy_count;
    y_cord = 0;
    setup_game();
    render_scene();
  } else if (grid[y_cord + 1][x_cord] == 'Y' ||
             grid[y_cord + 1][x_cord] == 'O') {
    printf("There is an obstacle\n");
  } else if (grid[y_cord + 1][x_cord] == 'W' ||
             grid[y_cord + 1][x_cord] == 'w') {
    grid[y_cord][x_cord] = '-';
    grid[y_cord + 1][x_cord] = 'x';
    y_cord = y_cord + 1;
    render_scene();
    printf("You died\n");
    exit(0);
  } else {
    grid[y_cord][x_cord] = '-';
    grid[y_cord + 1][x_cord] = 'X';
    y_cord = y_cord + 1;
  }
}

void go_left() {
  if (x_cord == 0 && enemy_count > 0) {
    printf("CANNOT MOVE YET\n");
  } else if (x_cord == 0 && enemy_count <= 0) {
    enemy_count = default_enemy_count;
    x_cord = cols - 1; /* FIXED: wrap to last column, not rows - 1 */
    setup_game();
    render_scene();
  } else if (grid[y_cord][x_cord - 1] == 'Y' ||
             grid[y_cord][x_cord - 1] == 'O') {
    printf("There is an obstacle\n");
  } else if (grid[y_cord][x_cord - 1] == 'W' ||
             grid[y_cord][x_cord - 1] == 'w') {
    grid[y_cord][x_cord] = '-';
    grid[y_cord][x_cord - 1] = 'x';
    x_cord = x_cord - 1;
    render_scene();
    printf("You died\n");
    exit(0);
  } else {
    grid[y_cord][x_cord] = '-';
    grid[y_cord][x_cord - 1] = 'X';
    x_cord = x_cord - 1;
  }
}

void go_right() {
  if (x_cord == cols - 1 && enemy_count > 0) {
    printf("CANNOT MOVE YET\n");
  } else if (x_cord == cols - 1 && enemy_count <= 0) {
    enemy_count = default_enemy_count;
    x_cord = 0;
    setup_game();
    render_scene();
  } else if (grid[y_cord][x_cord + 1] == 'Y' ||
             grid[y_cord][x_cord + 1] == 'O') {
    printf("There is an obstacle\n");
  } else if (grid[y_cord][x_cord + 1] == 'W' ||
             grid[y_cord][x_cord + 1] == 'w') {
    grid[y_cord][x_cord] = '-';
    grid[y_cord][x_cord + 1] = 'x';
    x_cord = x_cord + 1;
    render_scene();
    printf("You died\n");
    exit(0);
  } else {
    grid[y_cord][x_cord] = '-';
    grid[y_cord][x_cord + 1] = 'X';
    x_cord = x_cord + 1;
  }
}

void attack() {
  char direction[10];

  while (1) {
    render_scene();
    printf("\nAttack in what direction: ");
    if (fgets(direction, sizeof(direction), stdin) == NULL) {
      clearerr(stdin);
      continue;
    }
    direction[strcspn(direction, "\n")] = '\0';

    if (strcmp(alias(direction), "UP") == 0) {
      if (y_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord - 1][x_cord] == 'W' || grid[y_cord - 1][x_cord] == 'w') {
        grid[y_cord - 1][x_cord] = '-';
        score++;
        enemy_count--;
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no enemy above\n");
      }
    } else if (strcmp(alias(direction), "DOWN") == 0) {
      if (y_cord == rows - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord + 1][x_cord] == 'W' || grid[y_cord + 1][x_cord] == 'w') {
        grid[y_cord + 1][x_cord] = '-';
        score++;
        enemy_count--;
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no enemy below\n");
      }
    } else if (strcmp(alias(direction), "LEFT") == 0) {
      if (x_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord - 1] == 'W' || grid[y_cord][x_cord - 1] == 'w') {
        grid[y_cord][x_cord - 1] = '-';
        score++;
        enemy_count--;
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no enemy to the left\n");
      }
    } else if (strcmp(alias(direction), "RIGHT") == 0) {
      if (x_cord == cols - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord + 1] == 'W' || grid[y_cord][x_cord + 1] == 'w') {
        grid[y_cord][x_cord + 1] = '-';
        score++;
        enemy_count--;
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no enemy to the right\n");
      }
    } else {
      system(CLEAR_CMD);
      printf("Invalid direction\n");
    }
  }
}

void mine() {
  char direction[10];

  while (1) {
    render_scene();
    printf("\nMine in what direction: ");
    if (fgets(direction, sizeof(direction), stdin) == NULL) {
      clearerr(stdin);
      continue;
    }
    direction[strcspn(direction, "\n")] = '\0';

    if (strcmp(alias(direction), "UP") == 0) {
      if (y_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord - 1][x_cord] == 'O') {
        grid[y_cord - 1][x_cord] = 'o';
        break;
      } else if (grid[y_cord - 1][x_cord] == 'o') {
        grid[y_cord - 1][x_cord] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no stone above\n");
      }
    } else if (strcmp(alias(direction), "DOWN") == 0) {
      if (y_cord == rows - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord + 1][x_cord] == 'O') {
        grid[y_cord + 1][x_cord] = 'o';
        break;
      } else if (grid[y_cord + 1][x_cord] == 'o') {
        grid[y_cord + 1][x_cord] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no stone below\n");
      }
    } else if (strcmp(alias(direction), "LEFT") == 0) {
      if (x_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord - 1] == 'O') {
        grid[y_cord][x_cord - 1] = 'o';
        break;
      } else if (grid[y_cord][x_cord - 1] == 'o') {
        grid[y_cord][x_cord - 1] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no stone to the left\n");
      }
    } else if (strcmp(alias(direction), "RIGHT") == 0) {
      if (x_cord == cols - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord + 1] == 'O') {
        grid[y_cord][x_cord + 1] = 'o';
        break;
      } else if (grid[y_cord][x_cord + 1] == 'o') {
        grid[y_cord][x_cord + 1] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no stone to the right\n");
      }
    } else {
      system(CLEAR_CMD);
      printf("Invalid direction\n");
    }
  }
}

void cut() {
  char direction[10];

  while (1) {
    render_scene();
    printf("\nCut in what direction: ");
    if (fgets(direction, sizeof(direction), stdin) == NULL) {
      clearerr(stdin);
      continue;
    }
    direction[strcspn(direction, "\n")] = '\0';

    if (strcmp(alias(direction), "UP") == 0) {
      if (y_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord - 1][x_cord] == 'Y') {
        grid[y_cord - 1][x_cord] = 'y';
        break;
      } else if (grid[y_cord - 1][x_cord] == 'y') {
        grid[y_cord - 1][x_cord] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no tree above\n");
      }
    } else if (strcmp(alias(direction), "DOWN") == 0) {
      if (y_cord == rows - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord + 1][x_cord] == 'Y') {
        grid[y_cord + 1][x_cord] = 'y';
        break;
      } else if (grid[y_cord + 1][x_cord] == 'y') {
        grid[y_cord + 1][x_cord] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no tree below\n");
      }
    } else if (strcmp(alias(direction), "LEFT") == 0) {
      if (x_cord == 0) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord - 1] == 'Y') {
        grid[y_cord][x_cord - 1] = 'y';
        break;
      } else if (grid[y_cord][x_cord - 1] == 'y') {
        grid[y_cord][x_cord - 1] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no tree to the left\n");
      }
    } else if (strcmp(alias(direction), "RIGHT") == 0) {
      if (x_cord == cols - 1) {
        system(CLEAR_CMD);
        printf("Out of bounds\n");
      } else if (grid[y_cord][x_cord + 1] == 'Y') {
        grid[y_cord][x_cord + 1] = 'y';
        break;
      } else if (grid[y_cord][x_cord + 1] == 'y') {
        grid[y_cord][x_cord + 1] = '-';
        break;
      } else {
        system(CLEAR_CMD);
        printf("There is no tree to the right\n");
      }
    } else {
      system(CLEAR_CMD);
      printf("Invalid direction\n");
    }
  }
}

void print_help() {
  system(CLEAR_CMD);
  printf("Hello, welcome!\n");
  printf("This is a basic terminal game made in C\n");
  printf("It is a project to learn 2D arrays and array manipulation\n");
  printf("enjoy\n");
  printf("\n");
  printf("Signs: \n");
  printf("YOU are the X\n");
  printf("Ys are trees, obstacles you can't pass through\n");
  printf("Os are stones, obstacles you can't pass through\n");
  printf("Ws are enemies, you can kill them, if you touch them, you die\n");
  printf("\n");
  printf("Available commands:\n");
  printf("HELP to show this\n");
  printf("EXIT to exit the game\n");
  printf("UP to go up\n");
  printf("DOWN to go down\n");
  printf("LEFT to go left\n");
  printf("RIGHT to go right\n");
  printf("ATTACK to attack enemies\n");
  printf("MINE to mine stone\n");
  printf("CUT to cut trees\n");
  printf("\nPress Enter to continue...");
  fflush(stdout);
  while (getchar() != '\n')
    ;
}

void to_up(char *in) {
  for (int i = 0; i < (int)strlen(in); i++) {
    in[i] = (char)toupper((unsigned char)in[i]);
  }
}

int rand_y;
int rand_x;

void setup_game() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      grid[i][j] = '-';
    }
  }
  grid[y_cord][x_cord] = 'X';

  for (int i = 0; i < default_tree_count; i = i) {
    rand_y = rand() % rows;
    rand_x = rand() % cols;
    char sign = grid[rand_y][rand_x];
    if (sign != 'X' && sign != 'Y') {
      grid[rand_y][rand_x] = 'Y';
      i++;
    }
  }
  for (int i = 0; i < default_stone_count; i = i) {
    rand_y = rand() % rows;
    rand_x = rand() % cols;
    char sign = grid[rand_y][rand_x];
    if (sign != 'X' && sign != 'Y' && sign != 'O') {
      grid[rand_y][rand_x] = 'O';
      i++;
    }
  }

  if (default_enemy_count <= 10) {
    default_enemy_count = default_enemy_count + 1;
  }
  
  enemy_count = default_enemy_count;
  for (int i = 0; i <= enemy_count; i++) {
    rand_y = rand() % rows;
    rand_x = rand() % cols;
    char sign = grid[rand_y][rand_x];
    if (sign != 'X' && sign != 'Y' && sign != 'O' && sign != 'W') {
      grid[rand_y][rand_x] = 'W';
      i++;
    }
  }
}

void make_vars() {
  rows = sizeof(grid) / sizeof(grid[0]);
  cols = sizeof(grid[0]) / sizeof(grid[0][0]);
  if (rows < 4 || cols < 4) {
    printf("To small grid size to setup\n");
    exit(0);
  }
  y_cord = (rows / 2);
  x_cord = (cols / 2);
  default_tree_count = (rows / 2 + cols / 2) + rows / 2;
  default_stone_count = default_tree_count * 2 / 3;
  default_enemy_count = 1;
  enemy_count = default_enemy_count;
}

char aliases[10][4][10] = {{"UP", "up", "W", "w"},
                           {"DOWN", "down", "S", "s"},
                           {"LEFT", "left", "A", "a"},
                           {"RIGHT", "right", "D", "d"},
                           {"ATTACK", "attack", "ATK", "atk"},
                           {"CUT", "cut", "CT", "ct"},
                           {"MINE", "mine", "MN", "mn"},
                           {"EXIT", "exit", "EX", "ex"},
                           {"HELP", "help", "HP", "hp"}};

char *alias(char *in) {
  for (int i = 0; i < (int)(sizeof(aliases) / sizeof(aliases[0])); i++) {
    for (int j = 0; j < (int)(sizeof(aliases[0]) / sizeof(aliases[0][0]));
         j++) {
      if (strcmp(in, aliases[i][j]) == 0) {
        return aliases[i][0];
      }
    }
  }
  return "unknown";
}

void getHigh() {
  FILE *highFile = fopen("./highscore.txt", "r");
  if (highFile != NULL) {
    fscanf(highFile, "%d", &highscore);
    fclose(highFile);
  } else {
    printf("Failed to open file.\n");
  }
}

void saveHigh() {
  if (score > highscore) {
    highscore = score;
    FILE *highFile = fopen("./highscore.txt", "w");
    if (highFile == NULL) {
      perror("Failed to open file for writing");
      return;
    }
    if (fprintf(highFile, "%d", highscore) < 0)
      perror("Failed to write highscore");

    fclose(highFile);
  }
}
