#include "./json.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <string>
#include <thread>
#include <unistd.h>

typedef std::string String;

using json = nlohmann::json;

#ifdef _WIN32
#define CMD_CLEAR "cls"
#else
#define CMD_CLEAR "clear"
#endif

namespace Color {
const std::string reset = "\033[0m";
const std::string black = "\033[30m";
const std::string red = "\033[31m";
const std::string green = "\033[32m";
const std::string yellow = "\033[33m";
const std::string blue = "\033[34m";
const std::string white = "\033[37m";
} // namespace Color

struct game_info {
  int tough;
  int highscore;
  bool start;
  int rows;
  int cols;
  int x_cord;
  int y_cord;
  int score;
  int wood;
  int stone;
  int iteration;
  int player_damage;
  int pickaxe_power;
  int axe_power;
};
game_info game;

struct item {
  char placeholder;
  String color;
  bool accessible;
  bool unlocked;
  int hp;
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

item player = {'X', Color::blue, 0, 0, 1, "", {}};
item air = {'-', Color::black, 1, 0, 0, "Ar", {}};
item tree = {
    'Y', Color::green, 0, 0, 0, "A basic tree, whispering in the wind.", {}};
item stone = {'O', Color::white, 0, 0, 0, "Just a boulder, blocking your path",
              {}};
item enemy = {
    'W',
    Color::red,
    0,
    0,
    0,
    "A wierd creature, you can't make out what it even is, better be careful",
    {}};

item left_wall{'[', Color::white, 0, 0, 0, "Just a wall", {}};
item right_wall{']', Color::white, 0, 0, 0, "Just a wall", {}};
item wall{'T', Color::white, 0, 0, 0, "Just a wall", {}};
item magic_amulet{'q', Color::yellow, 0, 0, 0, "ne", {}};

std::array<std::array<item, 7>, 7> game_grid;

std::array<std::array<std::string, 4>, 12> aliases = {
    {{"UP", "up", "W", "w"},
     {"DOWN", "down", "S", "s"},
     {"LEFT", "left", "A", "a"},
     {"RIGHT", "right", "D", "d"},
     {"ATTACK", "attack", "atk", "f"},
     {"CUT", "cut", "CT", "c"},
     {"MINE", "mine", "MN", "m"},
     {"EXIT", "exit", "EX", "ex"},
     {"INTERACT", "interact", "E", "e"},
     {"CRAFT", "craft", "craft", "r"},
     {"INFO", "info", "Q", "q"},
     {"HELP", "help", "HP", "hp"}}};

int enemy_count;
int default_enemy_count = 1;
int tree_count = 5;
int stone_count = 5;

int sleep_time_ms = 25;

template <size_t scene_rows, size_t scene_cols>
void setup_game(std::array<std::array<item, scene_cols>, scene_rows> &scene);

template <size_t scene_rows, size_t scene_cols>
void render_scene(std::array<std::array<item, scene_cols>, scene_rows> &scene,
                  int ms);

void initialize_variables();

void move(int dy, int dx);

void waitForEnter();
void debug();
void help();

void attack(String direction);
void cut(String direction);
void mine(String direction);
void interact();
void craft();
void talk();

void load_amulet();

void save_game();
void load_game();

void died();

void commands();

void alias_movement(String action, char x);

String alias(String input);

bool game_running = 1;
bool first_pass = 1;

int main() {
  game.iteration += 1;
  srand(time(NULL));
  String command;
  initialize_variables();
  setup_game(game_grid);
  game.start = 0;
  while (game_running) {
    if (first_pass) {
      render_scene(game_grid, sleep_time_ms);
      first_pass = 0;
    } else {
      render_scene(game_grid, 0);
    }
    std::cout << "\nType HELP for help\nCommand: ";
    std::cin >> command;

    if (alias(command) == "EXIT") {
      save_game();
      exit(0);
    } else if (command == "RESET") {
      died();
    } else if (alias(command) == "HELP") {
      help();
    } else if (command == "COMMANDS") {
      commands();
    } else if (alias(command) == "INFO") {
      debug();
    } else if (alias(command) == "UP") {
      move(-1, 0);
    } else if (alias(command) == "DOWN") {
      move(1, 0);
    } else if (alias(command) == "LEFT") {
      move(0, -1);
    } else if (alias(command) == "RIGHT") {
      move(0, +1);
    } else if (command[0] == 'f') {
      alias_movement("ATTACK", command[1]);
    } else if (command[0] == 'c' && alias(command) != "CRAFT") {
      alias_movement("CUT", command[1]);
    } else if (command[0] == 'm') {
      alias_movement("MINE", command[1]);
    } else if (alias(command) == "INTERACT") {
      interact();
    } else if (alias(command) == "CRAFT") {
      craft();
    } else {
    }
  }

  return 0;
}

template <size_t scene_rows, size_t scene_cols>
void render_scene(std::array<std::array<item, scene_cols>, scene_rows> &scene,
                  int ms) {
  system(CMD_CLEAR);
  std::cout << "Highscore: " << game.highscore << '\n';
  std::cout << "Score: " << game.score << '\n';
  std::cout << "W: " << game.wood << " - " << "S: " << game.stone << '\n';
  std::cout << "---------------\n";
  for (int i = 0; i < scene_rows; i++) {
    for (int j = 0; j < scene_cols; j++) {
      std::cout << scene[i][j].color << scene[i][j].placeholder << Color::reset
                << ' ';
      fflush(stdout);
      std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    std::cout << "|\n";
  }
  std::cout << "---------------\n";
}

void died() {
  game.tough = 1;
  game.player_damage = 1;
  game.axe_power = 1;
  game.pickaxe_power = 1;
  game.iteration = 0;
  game.score = 0;
  game.start = true;
  game.stone = 0;
  game.wood = 0;
  save_game();
  exit(0);
}

void initialize_variables() {
  load_game();
  default_enemy_count = game.tough;
  if (default_enemy_count > 10) {
    default_enemy_count = 10;
  }
  game.start = true;
  enemy_count = game.tough;
  first_pass = 1;
  game.rows = 7;
  game.cols = 7;
  game.x_cord = game.cols / 2;
  game.y_cord = game.rows / 2;
}

template <size_t scene_rows, size_t scene_cols>
void setup_game(std::array<std::array<item, scene_cols>, scene_rows> &scene) {
  if (game.start == 0 && rand() % 4 == 0) {
    load_amulet();
  } else {
    first_pass = 1;
    game.iteration++;
    if (game.iteration % 2 == 0) {
      game.tough += 1;
      enemy.hp = pow(game.tough, 1.5);
      tree.hp += game.tough;
      stone.hp += game.tough;
      if (default_enemy_count < 10) {
        default_enemy_count += 1;
      }
    }

    system(CMD_CLEAR);
    for (int i = 0; i < game.rows; i++) {
      for (int j = 0; j < game.cols; j++) {
        scene[i][j] = air;
      }
    }
    scene[game.y_cord][game.x_cord] = player;

    for (int i = 0; i < tree_count;) {
      int x = rand() % game.cols;
      int y = rand() % game.rows;
      if (scene[y][x].placeholder != 'X' && scene[y][x].placeholder != 'Y') {
        scene[y][x] = tree;
        i++;
      }
    }

    for (int i = 0; i < stone_count;) {
      int x = rand() % game.cols;
      int y = rand() % game.rows;
      if (scene[y][x].placeholder != 'X' && scene[y][x].placeholder != 'Y' &&
          scene[y][x].placeholder != 'O') {
        scene[y][x] = stone;
        i++;
      }
    }

    for (int i = 0; i < default_enemy_count;) {
      int x = rand() % game.cols;
      int y = rand() % game.rows;
      if (scene[y][x].placeholder != 'X' && scene[y][x].placeholder != 'Y' &&
          scene[y][x].placeholder != 'O' && scene[y][x].placeholder != 'W') {
        scene[y][x] = enemy;
        i++;
      }
    }
    enemy_count = default_enemy_count;
  }
}

void move(int dy, int dx) {
  int new_y = game.y_cord + dy;
  int new_x = game.x_cord + dx;
  if (enemy_count <= 0 && ((game.y_cord == 0 && dy == -1) ||
                           (game.y_cord == game.rows - 1 && dy == 1) ||
                           (game.x_cord == 0 && dx == -1) ||
                           (game.x_cord == game.cols - 1 && dx == 1))) {
    game_grid[game.y_cord][game.x_cord] = air;

    if (dy == -1)
      game.y_cord = game.rows - 1;
    else if (dy == 1)
      game.y_cord = 0;
    else if (dx == -1)
      game.x_cord = game.cols - 1;
    else if (dx == 1)
      game.x_cord = 0;
    game_grid[game.y_cord][game.x_cord] = player;
    setup_game(game_grid);
    return;
  }
  if (new_x < 0 || new_x >= game.cols || new_y < 0 || new_y >= game.rows)
    return;
  if (game_grid[new_y][new_x].placeholder == enemy.placeholder) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
    game.x_cord = new_x;
    render_scene(game_grid, 0);
    died();
    std::cout << "\nyou died\n";
    exit(0);
  }
  if (game_grid[new_y][new_x].accessible == 1) {
    game_grid[game.y_cord][game.x_cord] = air;
    game_grid[new_y][new_x] = player;
    game.y_cord = new_y;
    game.x_cord = new_x;
  }
}

void alias_movement(String action, char x) {
  if (action == "ATTACK") {
    if (x == 'w') {
      attack("UP");
    } else if (x == 's') {
      attack("DOWN");
    } else if (x == 'a') {
      attack("LEFT");
    } else if (x == 'd') {
      attack("RIGHT");
    }
  } else if (action == "CUT") {
    if (x == 'w') {
      cut("UP");
    } else if (x == 's') {
      cut("DOWN");
    } else if (x == 'a') {
      cut("LEFT");
    } else if (x == 'd') {
      cut("RIGHT");
    }
  } else if (action == "MINE") {
    if (x == 'w') {
      mine("UP");
    } else if (x == 's') {
      mine("DOWN");
    } else if (x == 'a') {
      mine("LEFT");
    } else if (x == 'd') {
      mine("RIGHT");
    }
  }
}

void attack(String direction) {
  if (alias(direction) == "UP") {
    if (game_grid[game.y_cord - 1][game.x_cord].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord - 1][game.x_cord].hp -= game.player_damage;
      if (game_grid[game.y_cord - 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord - 1][game.x_cord] = air;
        enemy_count -= 1;
        game.score++;
        if (game.score > game.highscore) {
          game.highscore = game.score;
          save_game();
        }
      }
    }
  } else if (alias(direction) == "DOWN") {
    if (game_grid[game.y_cord + 1][game.x_cord].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord + 1][game.x_cord].hp -= game.player_damage;
      if (game_grid[game.y_cord + 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord + 1][game.x_cord] = air;
        enemy_count -= 1;
        game.score++;
        if (game.score > game.highscore) {
          game.highscore = game.score;
          save_game();
        }
      }
    }
  } else if (alias(direction) == "LEFT") {
    if (game_grid[game.y_cord][game.x_cord - 1].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord][game.x_cord - 1].hp -= game.player_damage;
      if (game_grid[game.y_cord][game.x_cord - 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord - 1] = air;
        enemy_count -= 1;
        game.score++;
        if (game.score > game.highscore) {
          game.highscore = game.score;
          save_game();
        }
      }
    }
  } else if (alias(direction) == "RIGHT") {
    if (game_grid[game.y_cord][game.x_cord + 1].placeholder ==
        enemy.placeholder) {
      game_grid[game.y_cord][game.x_cord + 1].hp -= game.player_damage;
      if (game_grid[game.y_cord][game.x_cord + 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord + 1] = air;
        enemy_count -= 1;
        game.score++;
        if (game.score > game.highscore) {
          game.highscore = game.score;
          save_game();
        }
      }
    }
  } else {
    std::cout << "";
  };
}

void cut(String direction) {
  if (alias(direction) == "UP") {
    if (game_grid[game.y_cord - 1][game.x_cord].placeholder ==
        tree.placeholder) {
      game_grid[game.y_cord - 1][game.x_cord].hp -= game.axe_power;
      if (game_grid[game.y_cord - 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord - 1][game.x_cord] = air;
        if (rand() % 3 == 0) {
          game.wood += 2;
        } else {
          game.wood += 1;
        }
      }
    }
  } else if (alias(direction) == "DOWN") {
    if (game_grid[game.y_cord + 1][game.x_cord].placeholder ==
        tree.placeholder) {
      game_grid[game.y_cord + 1][game.x_cord].hp -= game.axe_power;
      if (game_grid[game.y_cord + 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord + 1][game.x_cord] = air;
        if (rand() % 3 == 0) {
          game.wood += 2;
        } else {
          game.wood += 1;
        }
      }
    }
  } else if (alias(direction) == "LEFT") {
    if (game_grid[game.y_cord][game.x_cord - 1].placeholder ==
        tree.placeholder) {
      game_grid[game.y_cord][game.x_cord - 1].hp -= game.axe_power;
      if (game_grid[game.y_cord][game.x_cord - 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord - 1] = air;
        if (rand() % 3 == 0) {
          game.wood += 2;
        } else {
          game.wood += 1;
        }
      }
    }
  } else if (alias(direction) == "RIGHT") {
    if (game_grid[game.y_cord][game.x_cord + 1].placeholder ==
        tree.placeholder) {
      game_grid[game.y_cord][game.x_cord + 1].hp -= game.axe_power;
      if (game_grid[game.y_cord][game.x_cord + 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord + 1] = air;
        if (rand() % 3 == 0) {
          game.wood += 2;
        } else {
          game.wood += 1;
        }
      }
    }
  } else {
    std::cout << "";
  };
}

void mine(String direction) {
  if (direction == "UP") {
    if (game_grid[game.y_cord - 1][game.x_cord].placeholder ==
        stone.placeholder) {
      game_grid[game.y_cord - 1][game.x_cord].hp -= game.pickaxe_power;
      if (game_grid[game.y_cord - 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord - 1][game.x_cord] = air;
        if (rand() % 3 == 0) {
          game.stone += 2;
        } else {
          game.stone += 1;
        }
      }
    }
  } else if (direction == "DOWN") {
    if (game_grid[game.y_cord + 1][game.x_cord].placeholder ==
        stone.placeholder) {
      game_grid[game.y_cord + 1][game.x_cord].hp -= game.pickaxe_power;
      if (game_grid[game.y_cord + 1][game.x_cord].hp <= 0) {
        game_grid[game.y_cord + 1][game.x_cord] = air;
        if (rand() % 3 == 0) {
          game.stone += 2;
        } else {
          game.stone += 1;
        }
      }
    }
  } else if (direction == "LEFT") {
    if (game_grid[game.y_cord][game.x_cord - 1].placeholder ==
        stone.placeholder) {
      game_grid[game.y_cord][game.x_cord - 1].hp -= game.pickaxe_power;
      if (game_grid[game.y_cord][game.x_cord - 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord - 1] = air;
        if (rand() % 3 == 0) {
          game.stone += 2;
        } else {
          game.stone += 1;
        }
      }
    }
  } else if (direction == "RIGHT") {
    if (game_grid[game.y_cord][game.x_cord + 1].placeholder ==
        stone.placeholder) {
      game_grid[game.y_cord][game.x_cord + 1].hp -= game.pickaxe_power;
      if (game_grid[game.y_cord][game.x_cord + 1].hp <= 0) {
        game_grid[game.y_cord][game.x_cord + 1] = air;
        if (rand() % 3 == 0) {
          game.stone += 2;
        } else {
          game.stone += 1;
        }
      }
    }
  } else {
    std::cout << "";
  };
}

void interact() {
  String direction;
  std::cout << "What to interact with: ";
  std::cin >> direction;
  std::cout << '\n';
  if (alias(direction) == "UP") {
    if (game_grid[game.y_cord - 1][game.x_cord].placeholder == 'q') {
      std::cout << "You found a magic amulet, its power gave you + 3 to "
                   "every stat\n";
      game.axe_power += 3;
      game.pickaxe_power += 3;
      game.player_damage += 3;
      game_grid[game.y_cord - 1][game.x_cord] = air;
    } else {
      std::cout << game_grid[game.y_cord - 1][game.x_cord].interaction_message
                << '\n'
                << "HP: " << game_grid[game.y_cord - 1][game.x_cord].hp << '\n';
    }
  } else if (alias(direction) == "DOWN") {
    if (game_grid[game.y_cord + 1][game.x_cord].placeholder == 'q') {
      std::cout << "You found a magic amulet, its power gave you + 3 to "
                   "every stat\n";
      game.axe_power += 3;
      game.pickaxe_power += 3;
      game.player_damage += 3;
      game_grid[game.y_cord + 1][game.x_cord] = air;
    } else {
      std::cout << game_grid[game.y_cord + 1][game.x_cord].interaction_message
                << '\n'
                << "HP: " << game_grid[game.y_cord + 1][game.x_cord].hp << '\n';
    }
  } else if (alias(direction) == "LEFT") {
    if (game_grid[game.y_cord][game.x_cord - 1].placeholder == 'q') {
      std::cout << "You found a magic amulet, its power gave you + 3 to "
                   "every stat\n";
      game.axe_power += 3;
      game.pickaxe_power += 3;
      game.player_damage += 3;
      game_grid[game.y_cord][game.x_cord - 1] = air;
    } else {
      std::cout << game_grid[game.y_cord - 1][game.x_cord].interaction_message
                << '\n'
                << "HP: " << game_grid[game.y_cord][game.x_cord - 1].hp << '\n';
    }
  } else if (alias(direction) == "RIGHT") {
    if (game_grid[game.y_cord][game.x_cord + 1].placeholder == 'q') {
      std::cout << "You found a magic amulet, its power gave you + 3 to "
                   "every stat\n";
      game.axe_power += 3;
      game.pickaxe_power += 3;
      game.player_damage += 3;
      game_grid[game.y_cord][game.x_cord + 1] = air;
    } else {
      std::cout << game_grid[game.y_cord][game.x_cord + 1].interaction_message
                << '\n'
                << "HP: " << game_grid[game.y_cord][game.x_cord + 1].hp << '\n';
    }
  } else {
    std::cout << "";
  };
  waitForEnter();
}

void help() {
  system(CMD_CLEAR);
  std::cout << "This is a nice terminal-based game" << '\n';
  std::cout << "You move by w/a/s/d" << '\n';
  std::cout << "You can cut trees and mine stones (by cut or mine)" << '\n';
  std::cout << "You have to kill enemies and get to the edge of the map after, "
               "you attack by atk"
            << '\n';
  std::cout << "Stones, trees, and enemies get tougher the more iterations you "
               "go through"
            << '\n';
  std::cout << "Craft better gear in the crafting menu (craft)" << '\n';
  std::cout << "You can see all the commands by typing COMMANDS\n";
  std::cout << "Good luck and enjoy!" << '\n';
  waitForEnter();
}

void commands() {
  std::cout << "these are all the other avalible commands:\n\n";
  system(CMD_CLEAR);
  for (int i = 0; i < aliases.size(); i++) {
    for (int j = 0; j < aliases[i].size(); j++) {
      std::cout << aliases[i][j] << ' ';
    }
    std::cout << '\n';
  }
  waitForEnter();
}

void waitForEnter() {
  std::cout << "Press Enter to continue...";
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cin.get(); // wait for Enter
}

void debug() {
  std::cout << "Enemies: " << enemy_count << '\n';
  std::cout << "x,y :" << game.x_cord << ',' << game.y_cord << '\n';
  std::cout << "Iteration: " << game.iteration << '\n';
  std::cout << "Wood: " << game.wood << '\n';
  std::cout << "Stone: " << game.stone << '\n';
  std::cout << "Damage: " << game.player_damage << '\n';
  std::cout << "Axe Power: " << game.axe_power << '\n';
  std::cout << "Pickaxe Power: " << game.pickaxe_power << '\n';
  waitForEnter();
}

String alias(String input) {
  for (int i = 0; i < (sizeof(aliases) / sizeof(aliases[0])); i++) {
    for (int j = 0; j < (sizeof(aliases[0]) / sizeof(aliases[j][0])); j++) {
      if (input == aliases[i][j]) {
        return aliases[i][0];
      }
    }
  }
  return "unknown";
}

void craft() {
  String command;
  system(CMD_CLEAR);
  while (true) {
    system(CMD_CLEAR);
    std::cout << "Hey! What do you want to craft?\n";
    std::cout << "W: " << game.wood << " - " << "S: " << game.stone << '\n';
    std::cout << "damage_thing (+1 damage, stackable, 2W 2S)" << '\n';
    std::cout << "stronger_axe (+1 cutting power, stackable, 2W 1S)" << '\n';
    std::cout << "stronger_pickaxe (+1 mining power, stackable, 2W 3S)" << '\n';
    std::cout << "?: ";
    std::cin >> command;
    if (command == "axe" || command == "stronger_axe") {
      if (game.wood >= 2 && game.stone >= 1) {
        game.wood -= 2;
        game.stone -= 1;
        game.axe_power += 1;
        std::cout << "\nSuccess\n";
        waitForEnter();
      } else {
        std::cout << "\nNot enough resources\n";
        waitForEnter();
        break;
      }
    } else if (command == "pickaxe" || command == "stronger_pickaxe") {
      if (game.wood >= 2 && game.stone >= 3) {
        game.wood -= 2;
        game.stone -= 3;
        game.pickaxe_power += 1;
        std::cout << "\nSuccess\n";
        waitForEnter();
      } else {
        std::cout << "\nNot enough resources\n";
        waitForEnter();
        break;
      }
    } else if (command == "damage" || command == "damage_thing") {
      if (game.wood >= 2 && game.stone >= 2) {
        game.wood -= 2;
        game.stone -= 2;
        game.player_damage += 1;
        std::cout << "\nSuccess\n";
        waitForEnter();
      } else {
        std::cout << "\nNot enough resources\n";
        waitForEnter();
        break;
      }
    } else if (command == "EXIT" || command == "exit") {
      break;
    } else {
      std::cout << "\nNot a thing\n";
      waitForEnter();
      break;
    }
  }
}

void save_game() {
  json j;
  j["game"]["highscore"] = game.highscore;
  j["game"]["score"] = game.score;
  j["game"]["wood"] = game.wood;
  j["game"]["stone"] = game.stone;
  j["game"]["axe"] = game.axe_power;
  j["game"]["pickaxe"] = game.pickaxe_power;
  j["game"]["damage"] = game.player_damage;
  j["game"]["tough"] = game.tough;
  j["game"]["iteration"] = game.iteration;
  j["game"]["start"] = game.start;

  std::ofstream file("game_data.json");
  if (!file.is_open()) {
    std::cerr << "Failed to open file for writing\n";
    return;
  }
  file << j.dump(4);
  file.close();
}

void load_game() {
  std::ifstream file("game_data.json");
  if (!file.is_open()) {
    std::cerr << "No save file found, starting fresh.\n";
    return;
  }

  json j;
  file >> j;
  file.close();

  game.axe_power = j["game"].value("axe", 1);
  game.pickaxe_power = j["game"].value("pickaxe", 1);
  game.player_damage = j["game"].value("damage", 1);
  game.wood = j["game"].value("wood", 0);
  game.stone = j["game"].value("stone", 0);
  game.score = j["game"].value("score", 0);
  game.highscore = j["game"].value("highscore", 0);
  game.iteration = j["game"].value("iteration", 0);
  game.tough = j["game"].value("tough", 1);
  game.start = j["game"].value("start", true);
}

void load_amulet() {
  for (int i = 0; i < game_grid.size(); i++) {
    for (int j = 0; j < game_grid[i].size(); j++) {
      game_grid[i][j] = air;
    }
  }
  game_grid[1][1] = left_wall;
  game_grid[2][1] = left_wall;
  game_grid[3][1] = left_wall;
  game_grid[4][1] = left_wall;
  game_grid[5][1] = left_wall;

  game_grid[1][2] = wall;
  game_grid[1][3] = wall;
  game_grid[1][4] = wall;

  game_grid[1][5] = right_wall;
  game_grid[2][5] = right_wall;
  game_grid[3][5] = right_wall;
  game_grid[4][5] = right_wall;
  game_grid[5][5] = right_wall;

  game_grid[5][2] = wall;
  game_grid[5][4] = wall;

  game_grid[3][3] = magic_amulet;

  game_grid[game.y_cord][game.x_cord] = player;
}