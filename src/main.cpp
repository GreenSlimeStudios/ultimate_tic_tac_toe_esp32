#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Keypad.h>
#include <vector>
#include <bits/stdc++.h>

const int TFT_CS = 5;
const int TFT_DC = 16;
const int TFT_RST = 2;
const int SC = 17;

const int SCREEN_HEIGHT = 240;
const int SCREEN_WIDTH = 240;

const int PART_STEP = 24;
int segment = SCREEN_WIDTH / PART_STEP;
int segment_h = SCREEN_HEIGHT / PART_STEP;
int space = 1;
int next_pos = 10;

Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

const byte ROWS = 6; // four rows
const byte COLS = 4; // three columns
char keys[ROWS][COLS] = {
    {'A', '0', '1', '2'},
    {'E', '3', '4', '5'},
    {'I', '6', '7', '8'},
    {'M', 'N', 'O', 'P'},
    {'R', 'S', 'T', 'U'},
    {'W', 'X', 'Y', 'Z'}};
byte colPins[COLS] = {14, 25, 26, 27};        // connect to the row pinouts of the keypad
byte rowPins[ROWS] = {33, 22, 21, 4, 13, 12}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void print_splash()
{
  display.fillScreen(ST77XX_BLACK);
  display.println("ULTIMATE TIC TAC TOE");
}

void print_board();
enum Square
{
  FREE,
  P1,
  P2,
  DRAW,
};
struct GameState
{
  std::vector<std::vector<Square>> board;
  Square turn = P1;
  bool lock_button = false;

  GameState()
  {
    board.clear();
    for (int i; i < 9; ++i)
    {
      board.push_back({
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
          Square::FREE,
      });
    }
  }
};
GameState gs = GameState();
void setup()
{
  Serial.begin(921600);
  display.init(240, 240);
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);
  display.drawRect(120, 120, 240, 240, ST77XX_BLACK);
  display.setTextColor(ST77XX_WHITE);
  display.setTextSize(2);
  print_splash();
  print_board();
}
// void stupid()
// {
//   display.fillRect(20, 100, 200, 35, ST77XX_RED);
//   display.setCursor(25, 110);
//   display.print("SYNTAX ERROR !!!");
//   delay(500);
// }

Square check_game(std::vector<Square> v)
{
  // if (next_pos)
  bool playable = false;
  if (v[0] == v[3] && v[3] == v[6] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[0] == v[4] && v[4] == v[8] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[0] == v[1] && v[1] == v[2] && v[0] != Square::FREE)
  {
    return v[0];
  }
  else if (v[3] == v[4] && v[4] == v[5] && v[3] != Square::FREE)
  {
    return v[3];
  }
  else if (v[1] == v[4] && v[4] == v[7] && v[1] != Square::FREE)
  {
    return v[1];
  }
  else if (v[6] == v[7] && v[7] == v[8] && v[6] != Square::FREE)
  {
    return v[6];
  }
  else if (v[2] == v[5] && v[5] == v[8] && v[2] != Square::FREE)
  {
    return v[2];
  }
  else if (v[6] == v[4] && v[4] == v[2] && v[6] != Square::FREE)
  {
    return v[6];
  }
  for (int i = 0; i < 9; ++i)
  {
    if (v[i] == FREE)
    {
      playable = true;
      return FREE;
    }
  }

  return DRAW;
}

void draw_symbol(Square whos, int x, int y,bool is_verdict)
{
  // display.drawCircle(x + SCREEN_WIDTH / 28 / 2, y + SCREEN_HEIGHT / 28 / 2, 3, ST77XX_WHITE);
  if (whos == P1)
  {
    display.drawCircle(x, y, 5,(!is_verdict)?ST77XX_RED:ST77XX_WHITE);
  }
  if (whos == P2)
  {
    display.drawLine(x - 4, y - 4, x + 4, y + 4, (!is_verdict)?ST77XX_BLUE:ST77XX_WHITE);
    display.drawLine(x - 4, y + 4, x + 4, y - 4, (!is_verdict)?ST77XX_BLUE:ST77XX_WHITE);
  }
  // display.drawPixel(x,y,ST77XX_CYAN);
}
void print_helper()
{
  Serial.println(next_pos);
  if (next_pos == 10)
  {
    for (int h = 0; h < 3; ++h)
    {
      for (int w = 0; w < 3; ++w)
      {
        // display.fillRoundRect(segment, segment, 6 * segment+1, 6 * segment+1,5, ST77XX_ORANGE);

        display.drawRoundRect(segment + 8 * segment * w, segment + 8 * segment * h, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_ORANGE);
      }
    }
    return;
  }
  int h = floor((next_pos) / 3);
  int w = (next_pos) % 3;

  display.drawRoundRect(segment + 8 * segment * w, segment + 8 * segment * h, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_ORANGE);
}

void print_small_board(int i, int j,bool is_verdict)
{
  display.drawLine(segment * 1 + segment * i * 8, segment_h * 3 + segment_h * j * 8, segment * 7 + segment * i * 8, segment_h * 3 + segment_h * j * 8, ST77XX_WHITE);
  display.drawLine(segment * 1 + segment * i * 8, segment_h * 5 + segment_h * j * 8, segment * 7 + segment * i * 8, segment_h * 5 + segment_h * j * 8, ST77XX_WHITE);
  display.drawLine(segment * 3 + segment * j * 8, segment_h * 1 + segment_h * i * 8, segment * 3 + segment * j * 8, segment_h * 7 + segment_h * i * 8, ST77XX_WHITE);
  display.drawLine(segment * 5 + segment * j * 8, segment_h * 1 + segment_h * i * 8, segment * 5 + segment * j * 8, segment_h * 7 + segment_h * i * 8, ST77XX_WHITE);
  for (int h = 0; h < 3; ++h)
  {
    for (int w = 0; w < 3; ++w)
    {
      draw_symbol(gs.board[i * 3 + j][h * 3 + w], SCREEN_WIDTH / PART_STEP + SCREEN_WIDTH / PART_STEP * j * 8 + SCREEN_WIDTH / PART_STEP * w * 2 + SCREEN_WIDTH / PART_STEP, segment_h + segment_h * i * 8 + segment_h * h * 2 + segment_h,is_verdict);
    }
  }
}
Square check_game_state()
{
  std::vector<Square> b_game;
  for (int i = 0; i < 9; ++i)
  {
    b_game.push_back(check_game(gs.board[i]));
  }
  return check_game(b_game);
}
void print_board()
{
  display.fillScreen(ST77XX_BLACK);
  if (gs.turn == P1)
  {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST77XX_RED);
  }
  else
  {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST77XX_BLUE);
  }
  print_helper();

  display.drawLine(SCREEN_WIDTH / 3, 0, SCREEN_WIDTH / 3, SCREEN_HEIGHT, ST77XX_WHITE);
  display.drawLine(SCREEN_WIDTH / 3 * 2, 0, SCREEN_WIDTH / 3 * 2, SCREEN_HEIGHT, ST77XX_WHITE);
  display.drawLine(0, SCREEN_WIDTH / 3, SCREEN_WIDTH, SCREEN_WIDTH / 3, ST77XX_WHITE);
  display.drawLine(0, SCREEN_WIDTH / 3 * 2, SCREEN_WIDTH, SCREEN_WIDTH / 3 * 2, ST77XX_WHITE);

  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      switch (check_game(gs.board[j * 3 + i]))
      {
      case P1:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_RED);
        print_small_board(j, i,true);
        break;
      case P2:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_BLUE);
        print_small_board(j, i,true);
        break;
      case DRAW:
        display.fillRoundRect(segment + 8 * segment * i, segment + 8 * segment * j, 6 * segment + 1, 6 * segment + 1, 5, ST77XX_CASET);
        print_small_board(j, i,true);

      break;

      default:
        print_small_board(j, i,false);
        break;
      }
    }
  }
}

void end_game(Square state)
{
  if (state == P1)
  {
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_RED);
    display.setTextColor(ST77XX_WHITE);
    display.println("circle wins :3");
  }
  else if (state == P2)
  {
    // display.fillScreen(ST77XX_BLUE);
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_BLUE);
    display.setTextColor(ST77XX_WHITE);
    display.println("cross wins >:3");
  }
  else
  {
    // display.fillScreen(ST77XX_BLACK);
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, ST77XX_CASET);
    display.setTextColor(ST77XX_WHITE);
    display.println("draw >:[");
  }
  while (true)
  {
  }
}
void loop()
{
  char key = keypad.getKey();
  int big_pos;

  Square game_state = check_game_state();
  if (game_state != FREE)
  {
    end_game(game_state);
  }

  if (next_pos == 10 || check_game(gs.board[next_pos]) != FREE)
  {
    big_pos = 10;
    next_pos = 10;
    print_board();
    while (true)
    {
      key = keypad.getKey();
      if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '0' || key == '6' || key == '7' || key == '8')
      {
        int num = key - '0';
        if (check_game(gs.board[num]) == FREE)
        {
          big_pos = num;
          next_pos = num;
          gs.lock_button = true;
          print_board();
          break;
        }
      }
    }
  }
  else
  {
    big_pos = next_pos;
  }
  int cur = 0;
  if (gs.lock_button)
  {
    if (!key)
    {
      gs.lock_button = false;
    }
  }
  if (key && gs.lock_button == false)
  {
    switch (key)
    {
    case '0':
      cur = 0;
      break;
    case '1':
      cur = 1;
      break;
    case '2':
      cur = 2;
      break;
    case '3':
      cur = 3;
      break;
    case '4':
      cur = 4;
      break;
    case '5':
      cur = 5;
      break;
    case '6':
      cur = 6;
      break;
    case '7':
      cur = 7;
      break;
    case '8':
      cur = 8;
      break;

    default:
      break;
    }
    if (gs.board[big_pos][cur] == Square::FREE)
    {
      next_pos = cur;
      gs.board[big_pos][cur] = gs.turn;
      if (gs.turn == P1)
      {
        gs.turn = P2;
      }
      else
      {
        gs.turn = P1;
      }
    }
    print_board();
  }
}
