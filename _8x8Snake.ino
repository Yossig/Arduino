



static const int MAX_COLS = 8;
static const int MAX_ROWS = 8;
static const int WALL = 1;
static const int SPACE = 0;
static const int FOOD = 2;

int DIGITS_ONE[MAX_COLS * MAX_ROWS] = {1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 0, 0, 0, 1, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 0, 0, 0, 0, 1, 1
                                      };
int DIGITS_TWO[MAX_COLS * MAX_ROWS] = {1, 1, 0, 0, 0, 0, 1, 1,
                                       1, 0, 0, 1, 1, 0, 0, 1,
                                       1, 1, 1, 1, 1, 0, 0, 1,
                                       1, 1, 1, 1, 0, 0, 1, 1,
                                       1, 1, 1, 0, 0, 1, 1, 1,
                                       1, 1, 0, 0, 1, 1, 1, 1,
                                       1, 0, 0, 1, 1, 1, 1, 1,
                                       1, 0, 0, 0, 0, 0, 0, 1
                                      };
int DIGITS_THREE[MAX_COLS * MAX_ROWS] = {1, 1, 1, 1, 1, 1, 1, 1,
                                         1, 1, 0, 0, 0, 0, 1, 1,
                                         1, 0, 0, 1, 1, 0, 0, 1,
                                         1, 1, 1, 1, 1, 0, 0, 1,
                                         1, 1, 1, 0, 0, 0, 1, 1,
                                         1, 1, 1, 1, 1, 0, 0, 1,
                                         1, 0, 0, 1, 1, 0, 0, 1,
                                         1, 1, 0, 0, 0, 0, 1, 1
                                        };


enum Direction {LEFT, RIGHT, UP, DOWN};

struct Vector {
  int vecX;
  int vecY;
};

struct Snake {
  struct Vector body[MAX_COLS * MAX_ROWS];
  int length;
} snake;

const int row[MAX_ROWS] = {
  2, 7, 19, 5, 13, 18, 12, 16
};
const int col[MAX_COLS] = {
  6, 11, 10, 3, 17, 4, 8, 9
};

struct Vector DIRECTIONS[4] = {{ -1, 0}, {1, 0}, {0, 1}, {0, -1}};
struct Vector vecFood;
int pixels[MAX_ROWS][MAX_COLS];
int gameMap[MAX_ROWS + 2][MAX_COLS + 2];
int x = 6;
int y = 6;
int blinkFlag = 0;
int thumbYPin = 0;
int thumbXPin = 1;
bool isGameOver;

struct Vector direction;

unsigned long lastUpdateTime;
unsigned long lastBlinkTime;

void setup() {
  Serial.begin(9600);
  setupMatrix();
  initMap();
  lastUpdateTime = millis();
  lastBlinkTime = millis();
  resetGame();
  isGameOver = true;
}

void loop() {
  // put your main code here, to run repeatedly:

  setDirection();
  updatePixels();
  refreshScreen();
}

void setDirection() {

  int currDirection = getDirection();
  if (currDirection == LEFT && vectorToDirection(direction) != RIGHT) {
    direction = DIRECTIONS[LEFT];
  }

  if (currDirection == RIGHT && vectorToDirection(direction) != LEFT) {
    direction = DIRECTIONS[RIGHT];
  }

  if (currDirection == UP && vectorToDirection(direction) != DOWN) {
    direction = DIRECTIONS[UP];
  }

  if (currDirection == DOWN && vectorToDirection(direction) != UP) {
    direction = DIRECTIONS[DOWN];
  }
}

void updatePixels() {

  for (int i = 0; i < snake.length; i++) {
    pixels[snake.body[i].vecX][snake.body[i].vecY] = HIGH;
  }

  pixels[vecFood.vecX][vecFood.vecY] = HIGH;

  if (millis() - lastBlinkTime  > 100) {
    blinkFlag = ~blinkFlag;
    lastBlinkTime = millis();
  }

  if (millis() - lastUpdateTime > 100) {
    lastUpdateTime = millis();
    if (gameMap[snake.body[0].vecX + WALL + direction.vecX][snake.body[0].vecY + WALL + direction.vecY] != WALL) {

      if (snake.body[0].vecX == vecFood.vecX && snake.body[0].vecY == vecFood.vecY) {
        snake.length++;
        setFoodLocation();
        gameMap[vecFood.vecX + WALL][vecFood.vecY + WALL] = FOOD;
      }

      gameMap[snake.body[snake.length - 1].vecX + WALL ][snake.body[snake.length - 1].vecY + WALL] = SPACE;

      for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
        gameMap[snake.body[i].vecX + WALL ][snake.body[i].vecY + WALL] = WALL;
      }

      snake.body[0].vecX += direction.vecX;
      snake.body[0].vecY += direction.vecY;
      gameMap[snake.body[0].vecX + WALL ][snake.body[0].vecY + WALL] = WALL;

    }
    else {
      resetGame();
    }
  }

  for (int i = 0; i < snake.length; i++) {
    pixels[snake.body[i].vecX][snake.body[i].vecY] = LOW;
  }

  if (blinkFlag) {
    pixels[vecFood.vecX][vecFood.vecY] = LOW;
  }
}

void refreshScreen() {
  for (int thisRow = 0; thisRow < 8; thisRow++) {
    digitalWrite(row[thisRow], HIGH);

    for (int thisCol = 0; thisCol < 8; thisCol++) {
      int thisPixel = pixels[thisRow][thisCol];
      digitalWrite(col[thisCol], thisPixel);

      if (thisPixel == LOW) {
        digitalWrite(col[thisCol], HIGH);
      }
    }

    digitalWrite(row[thisRow], LOW);
  }
}

void setupMatrix() {
  for (int thisPin = 0; thisPin < 8; thisPin++) {
    pinMode(col[thisPin], OUTPUT);
    pinMode(row[thisPin], OUTPUT);

    digitalWrite(col[thisPin], HIGH);
  }

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      pixels[x][y] = HIGH;
    }
  }
}

int treatValue(int data) {
  return (data * 3 / 1024);
}

Direction analogToDirection(int x, int y) {
  Direction curDirection;

  if ((x == 0 && y == 1) || (x == 0 && y == 2) || (x == 0 && y == 0)) {
    curDirection = LEFT;
  }
  if ((x == 2 && y == 1) || ( x == 2 && y == 0) || ( x == 2 && y == 2)) {
    curDirection = RIGHT;
  }
  if (x == 1 && y == 0) {
    curDirection = DOWN;
  }
  if (x == 1 && y == 2) {
    curDirection = UP;
  }
  if (x == 1 && y == 1) {
    curDirection = vectorToDirection(direction);
  }
  return  curDirection;
}

Direction vectorToDirection(Vector vec) {
  if (vec.vecX == -1 && vec.vecY == 0)
    return LEFT;
  if (vec.vecX == 1 && vec.vecY == 0)
    return RIGHT;
  if (vec.vecX == 0 && vec.vecY == -1)
    return DOWN;
  if (vec.vecX == 0 && vec.vecY == 1)
    return UP;

}

Direction getDirection() {
  int x = treatValue(analogRead(thumbXPin));
  int y = treatValue(analogRead(thumbYPin));

  return  analogToDirection(x, y);
}

void initMap() {
  for (int i = 0; i < MAX_ROWS + 2; i++) {
    for (int j = 0; j < MAX_COLS + 2; j++) {
      gameMap[i][j] = WALL;
    }
  }

  for (int i = 1; i < MAX_ROWS + 1; i++) {
    for (int j = 1; j < MAX_COLS + 1; j++) {
      gameMap[i][j] = SPACE;
    }
  }

  gameMap[vecFood.vecX + WALL][vecFood.vecY + WALL] = FOOD;
}

void resetGame() {
  lastUpdateTime = millis();
  while (millis() - lastUpdateTime < 2000) {
    if (millis() - lastUpdateTime > 0 && millis() - lastUpdateTime < 666) {
      printMatrix(DIGITS_THREE, MAX_ROWS, MAX_COLS);
      refreshScreen();
      clearScreen();
    }
    if (millis() - lastUpdateTime > 666 && millis() - lastUpdateTime < 1332) {
      printMatrix(DIGITS_TWO, MAX_ROWS, MAX_COLS);
      refreshScreen();
      clearScreen();
    }
    if (millis() - lastUpdateTime > 1332 && millis() - lastUpdateTime < 1998) {
      printMatrix(DIGITS_ONE, MAX_ROWS, MAX_COLS);
      refreshScreen();
      clearScreen();
    }
  }
  initMap();
  resetSnake();
  setFoodLocation();
  direction = DIRECTIONS[LEFT];
}

void resetSnake() {
  snake.body[0] = {4, 4};
  snake.length = 1;
}

void setFoodLocation() {
  Vector result;

  result.vecX = getRandom(0, MAX_COLS);
  result.vecY = getRandom(0, MAX_ROWS);

  while (gameMap[result.vecX + WALL][result.vecY + WALL] != SPACE) {
    result.vecX = getRandom(0, MAX_COLS);
    result.vecY = getRandom(0, MAX_ROWS);
  }

  vecFood = result;
}

int getRandom(int min, int max) {
  return rand() % max + min;
}

void printMatrix(int digits[], int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      pixels[i][cols - 1 - j] = digits[j * cols + i];
    }
  }
}

void clearScreen() {
  for (int i = 0; i < MAX_ROWS; i++) {
    for (int j = 0; j < MAX_COLS; j++) {
      pixels[i][j] = HIGH;
    }
  }
}


