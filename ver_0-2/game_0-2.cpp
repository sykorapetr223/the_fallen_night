#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

typedef std::string String;

struct item {
  char placeholder;

  bool accessible;
  bool unlocked;
  String interaction_message;
  std::array<String, 5> inventory;
  String interact() { return interaction_message; }
  std::array<String, 5> get_inv() {
    if (unlocked) {
      return inventory;
    } else {
      return {};
    }
  }
};

item player = {'X', 0, 0, "", {}};
item air = {'-', 1, 0, "Dyk vitr", {}};
item tree = {'Y', 0, 0, "A basic tree, whispering in the wind.", {}};
item stone = {'O', 0, 0, "Just a boulder, blocking your path", {}};
item enemy = {
    'W',
    0,
    0,
    "A wierd creature, you can't make out what it even is, better be carefull",
    {}};

#ifdef _WIN32
#define CMD_CLEAR "cls"
#else
#define CMD_CLEAR "clear"
#endif

std::array<std::array<item, 7>, 7> game_grid;

struct game_info {
  int rows;
  int cols;
  int x_cord;
  int y_cord;
  int score;
};

int enemy_count;
int default_enemy_count = 2;
int tree_count = 5;
int stone_count = 4;

game_info game;

int sleep_time_ms = 25;

template <size_t scene_rows, size_t scene_cols>
void render_scene(std::array<std::array<item, scene_cols>, scene_rows> &scene,
                  int ms);

void initialize_variables();
void setup_game();

void move_up();
void move_down();
void move_left();
void move_right();

void debug();
void attack();
void cut();
void mine();
void interact();
void talk();

String alias(String input);

bool game_running = 1;
bool first_pass = 1;

int main() {

  srand(time(NULL));
  String command;
  initialize_variables();
  setup_game();

  while (game_running) {
    if (first_pass) {
      render_scene(game_grid, sleep_time_ms);
      first_pass = 0;
    } else {
      render_scene(game_grid, 0);
    }
    std::cout << "\nType HELP for help\nCommand: ";
    std::cin >> command;

    if (command == "EXIT") {
      exit(0);
    } else if (command == "DEBUG") {
      debug();
    } else if (alias(command) == "UP") {
      move_up();
    } else if (alias(command) == "DOWN") {
      move_down();
    } else if (alias(command) == "LEFT") {
      move_left();
    } else if (alias(command) == "RIGHT") {
      move_right();
    } else if (alias(command) == "ATTACK") {
      attack();
    } else {
    }
  }

  return 0;
}

template <size_t scene_rows, size_t scene_cols>
void render_scene(std::array<std::array<item, scene_cols>, scene_rows> &scene,
                  int ms) {
  system(CMD_CLEAR);
  std::cout << game.score << '\n';
  for (int i = 0; i < scene_rows; i++) {
    for (int j = 0; j < scene_cols; j++) {
      std::cout << scene[i][j].placeholder << ' ';
      fflush(stdout);
      std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    std::cout << '\n';
  }
}

void initialize_variables() {
  game.rows = 7;
  game.cols = 7;
  game.y_cord = game.cols / 2;
  game.x_cord = game.rows / 2;
  enemy_count = default_enemy_count;
}

void setup_game() {
  first_pass = 1;
  system(CMD_CLEAR);
  for (int i = 0; i < game.rows; i++) {
    for (int j = 0; j < game.cols; j++) {
      game_grid[i][j] = air;
    }
  }
  game_grid[game.y_cord][game.x_cord] = player;

  for (int i = 0; i < tree_count;) {
    int x = rand() % game.cols;
    int y = rand() % game.rows;
    if (game_grid[y][x].placeholder != 'X') {
      game_grid[y][x] = tree;
      i++;
    }
  }

  for (int i = 0; i < stone_count;) {
    int x = rand() % game.cols;
    int y = rand() % game.rows;
    if (game_grid[y][x].placeholder != 'X' &&
        game_grid[y][x].placeholder != 'Y') {
      game_grid[y][x] = stone;
      i++;
    }
  }
  enemy_count = default_enemy_count;
  for (int i = 0; i < default_enemy_count;) {
    int x = rand() % game.cols;
    int y = rand() % game.rows;
    if (game_grid[y][x].placeholder != 'X' &&
        game_grid[y][x].placeholder != 'Y' &&
        game_grid[y][x].placeholder != 'O') {
      game_grid[y][x] = enemy;
      i++;
    }
  }
}

void move_up() {
  int new_y = game.y_cord - 1;
  int new_x = game.x_cord;

  if (new_y < 0 && enemy_count == 0) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[game.rows - 1][game.x_cord] = player;
    game.y_cord = game.rows - 1;
    setup_game();
    return;
  } else if (game_grid[new_y][new_x].placeholder == enemy.placeholder) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
    render_scene(game_grid, 0);
    std::cout << "\nyou died\n";
    exit(0);
  } else if (game_grid[new_y][new_x].accessible == 1) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
  } else {
    return;
  }
}

void move_down() {
  int new_y = game.y_cord + 1;
  int new_x = game.x_cord;

  if (new_y > game.rows - 1 && enemy_count == 0) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[0][game.x_cord] = player;
    game.y_cord = 0;
    setup_game();
    return;
  } else if (game_grid[new_y][new_x].placeholder == enemy.placeholder) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
    render_scene(game_grid, 0);
    std::cout << "\nyou died\n";
    exit(0);
  } else if (game_grid[new_y][new_x].accessible == 1) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
  } else {
    return;
  }
}

void move_left() {
  int new_y = game.y_cord;
  int new_x = game.x_cord - 1;

  if (new_x < 0 && enemy_count == 0) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[game.y_cord][game.cols - 1] = player;
    game.x_cord = game.cols - 1;
    setup_game();
    return;
  } else if (game_grid[new_y][new_x].placeholder == enemy.placeholder) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.x_cord = new_x;
    render_scene(game_grid, 0);
    std::cout << "\nyou died\n";
    exit(0);
  } else if (game_grid[new_y][new_x].accessible == 1) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.x_cord = new_x;
  } else {
    return;
  }
}

void move_right() {
  int new_y = game.y_cord;
  int new_x = game.x_cord + 1;
  if (new_x > game.cols - 1 && enemy_count == 0) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[game.y_cord][0] = player;
    game.x_cord = game.cols - 1;
    setup_game();
    return;
  } else if (game_grid[new_y][new_x].placeholder == enemy.placeholder) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.x_cord = new_x;
    render_scene(game_grid, 0);
    std::cout << "\nyou died\n";
    exit(0);
  } else if (game_grid[new_y][new_x].accessible == 1) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.x_cord = new_x;
  } else {
    return;
  }
}

void attack() {
  String direction;
  std::cout << "What direction to attack in: ";
  std::cin >> direction;
  if (alias(direction) == "UP") {
    if (game_grid[game.y_cord - 1][game.x_cord].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord - 1][game.x_cord] = air;
      enemy_count -= 1;
      game.score++;
    }
  } else if (alias(direction) == "DOWN") {
    if (game_grid[game.y_cord + 1][game.x_cord].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord + 1][game.x_cord] = air;
      enemy_count -= 1;
      game.score++;
    }
  } else if (alias(direction) == "LEFT") {
    if (game_grid[game.y_cord][game.x_cord - 1].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord][game.x_cord - 1] = air;
      enemy_count -= 1;
      game.score++;
    }
  } else if (alias(direction) == "RIGHT") {
    if (game_grid[game.y_cord][game.x_cord + 1].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord][game.x_cord + 1] = air;
      enemy_count -= 1;
      game.score++;
    }
  } else {
    std::cout << "";
  };
}

void waitForEnter() {
  std::cout << "Press Enter to continue...";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get(); // wait for Enter
}

void debug() {
  std::cout << "Enemies: " << enemy_count << '\n';
  std::cout << "x,y" << game.x_cord << ',' << game.y_cord << '\n';
  waitForEnter();
}

String aliases[10][4] = {{"UP", "up", "W", "w"},
                         {"DOWN", "down", "S", "s"},
                         {"LEFT", "left", "A", "a"},
                         {"RIGHT", "right", "D", "d"},
                         {"ATTACK", "attack", "ATK", "atk"},
                         {"CUT", "cut", "CT", "ct"},
                         {"MINE", "mine", "MN", "mn"},
                         {"EXIT", "exit", "EX", "ex"},
                         {"HELP", "help", "HP", "hp"}};

String alias(String input) {
  for (int i = 0; i < (sizeof(aliases) / sizeof(aliases[0])); i++) {
    for (int j = 0; j < (sizeof(aliases[0]) / sizeof(aliases[j][0])); j++) {
      if (input == aliases[i][j]) {
        return aliases[i][0];
      }
    }
  }
  return "ne";
}