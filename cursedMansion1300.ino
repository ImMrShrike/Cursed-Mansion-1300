// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <Tinyfont.h>
#include <time.h>
#include <ArduboyTones.h>
#include "constants.h"



struct Mob {
  Rect hitBox;
  int floorNumber;
  int roomNumber;
  int roomCenterX;
  int roomCenterY;
  int direction;
  int mobType;  // 0 - spider / 1 - ghost / 2 - bat
};


struct Player {
  Rect hitBox;
  int floorNumber;
  int roomNumber;
  int viewportY;
  int lives;
  int faint;
  int cooldown;
  char itemInUse;
  int itemInUseW;
  int itemInUseH;
};


struct Item {
  int floorNumber;
  int x;
  int y;
  char value;
};

struct Room {
  int floorNumber;
  int roomNumber;
};



Item items[MAX_NUMBER_OF_ITEMS_IN_HOUSE];


void initializeItemArray() {
  for (int i = 0; i < MAX_NUMBER_OF_ITEMS_IN_HOUSE; i++) {
    items[i].value = NO_ITEM;
  }
}

int getFirstEmptyItemSpot() {
  for (int i = 0; i < MAX_NUMBER_OF_ITEMS_IN_HOUSE; i++) {
    if (items[i].value == NO_ITEM) {
      return i;
    }
  }
  return -1;
}

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

int toggleMatch = 0;
int twentyFrames = 0;
int touched = 0;
int matchesUsed = 0;
unsigned long now = 0;
unsigned long initTime = 0;
unsigned long endTime = 0;
int lights = 1;
int numberOfBats = 1;
int gamePhase = 0;
int color = 0;
int eyeRoll[8] = { 0, 1, 2, 2, 2, 1, 0, 0 };
int eyeRollX = 0;
int eyeRollY = 6;
int note = 190;
Mob *mobs;
Rect room[6];
Player player;

void printItemInUse(Player player) {
  if (player.itemInUse == SCEPTER) {
    Sprites::drawOverwrite(122, 30, scepterIcon, 0);
  } else if (player.itemInUse == COMPLETE_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 0);
  } else if (player.itemInUse == RIGHT_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 1);
  } else if (player.itemInUse == LEFT_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 2);
  } else if (player.itemInUse == MIDDLE_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 3);
  } else if (player.itemInUse == MIDDLE_AND_RIGHT_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 4);
  } else if (player.itemInUse == LEFT_AND_MIDDLE_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 5);
  } else if (player.itemInUse == LEFT_AND_RIGHT_PART_OF_URN) {
    Sprites::drawOverwrite(122, 30, urnIcon, 6);
  } else if (player.itemInUse == KEY) {
    Sprites::drawOverwrite(122, 30, keyIcon, 0);
  }
}



char getSpotValue(int i, int j, int floorNumber) {
  for (int k = 0; k < MAX_NUMBER_OF_ITEMS_IN_HOUSE; k++) {
    if (items[k].y == i && items[k].x == j && items[k].floorNumber == floorNumber) {
      return items[k].value;
    }
  }
  return NO_ITEM;
}

int areRoomsAdjacent(int room1, int room2) {
  if (room1 == 0 && (room2 == 1 || room2 == 2)) {
    return 1;
  }
  if (room1 == 1 && (room2 == 0 || room2 == 3)) {
    return 1;
  }
  if (room1 == 2 && (room2 == 0 || room2 == 3 || room2 == 4)) {
    return 1;
  }
  if (room1 == 3 && (room2 == 1 || room2 == 2 || room2 == 5)) {
    return 1;
  }
  if (room1 == 4 && (room2 == 2 || room2 == 5)) {
    return 1;
  }
  if (room1 == 5 && (room2 == 3 || room2 == 4)) {
    return 1;
  }
  return 0;
}

void setSpotValue(int i, int j, int floorNumber, char value) {
  int found = 0;
  for (int k = 0; k < MAX_NUMBER_OF_ITEMS_IN_HOUSE; k++) {
    if (items[k].y == i && items[k].x == j && items[k].floorNumber == floorNumber) {
      items[k].value = value;
      found = 1;
    }
  }
  if (!found) {
    int emptySpace = getFirstEmptyItemSpot();
    items[emptySpace].y = i;
    items[emptySpace].x = j;
    items[emptySpace].floorNumber = floorNumber;
    items[emptySpace].value = value;
  }
}

void generateExit() {
  Item exitDoor;
  exitDoor.floorNumber = 0;
  exitDoor.value = EXIT;

  int rnd = random(0, 4);
  if (rnd == 0) {
    exitDoor.x = 0;
  } else if (rnd == 1) {
    exitDoor.x = 2;
  } else if (rnd == 2) {
    exitDoor.x = 7;
  } else {
    exitDoor.x = 9;
  }

  if (rnd == 0 || rnd == 3) {
    exitDoor.y = 7;
  } else {
    if (random(0, 2) == 0) {
      exitDoor.y = 0;
    } else {
      exitDoor.y = 15;
    }
  }
  items[getFirstEmptyItemSpot()] = exitDoor;
}

void generateStartingPoint() {
  player.floorNumber = 0;
  for (int k = 0; k < MAX_NUMBER_OF_ITEMS_IN_HOUSE; k++) {
    if (items[k].value = EXIT) {
      //if exit is in room 0 or 2 or 4
      if (items[k].x == 0 || items[k].x == 2) {
        player.hitBox.x = 24;
      }
      //if exit is in room 1 or 3 or 5
      else {
        player.hitBox.x = 84;
      }

      player.hitBox.y = 32;
      //if exit is in room 0 or 1
      if (items[k].y == 0) {
        player.viewportY = 0;
      }
      //if exit is in room 2 or 3
      else if (items[k].y == 7) {
        player.viewportY = 64;
      }
      //if exit is in room 4 or 5
      else {
        player.viewportY = 128;
      }
      return;
    }
  }
  player.roomNumber = playerRoomNumber();
}

void generateScepter() {
  generateItem(SCEPTER);
}

void generateLeftUrn() {
  generateItem(LEFT_PART_OF_URN);
}

void generateRightUrn() {
  generateItem(RIGHT_PART_OF_URN);
}

void generateMiddleUrn() {
  generateItem(MIDDLE_PART_OF_URN);
}

void generateKey() {
  generateItem(KEY);
}

void generateItem(char itemValue) {
  while (1) {
    int i = random(0, 16);
    int j = random(0, 10);
    int floorNumber = random(0, 4);
    if (level[i][j] == EMPTY_SPACE && getSpotValue(i, j, floorNumber) == NO_ITEM) {
      Item item;
      item.y = i;
      item.x = j;
      item.floorNumber = floorNumber;
      item.value = itemValue;
      items[getFirstEmptyItemSpot()] = item;
      return;
    }
  }
}



void generateStairs() {
  for (int flr = 0; flr < 3; flr++) {
    //generate stairs up (up to three)
    int loop = random(1, 4);

    while (loop) {
      int i = random(0, 16);
      int j = random(0, 10);
      if (level[i][j] == STAIRS && getSpotValue(i, j, flr) == NO_ITEM) {
        Item stairsUp;
        stairsUp.y = i;
        stairsUp.x = j;
        stairsUp.floorNumber = flr;
        stairsUp.value = STAIRS_UP;
        items[getFirstEmptyItemSpot()] = stairsUp;

        Item stairsDown;
        stairsDown.y = i;
        stairsDown.x = j;
        stairsDown.floorNumber = flr + 1;
        stairsDown.value = STAIRS_DOWN;
        items[getFirstEmptyItemSpot()] = stairsDown;
        loop--;
      }
    }
  }
}

void generateDoors() {
  for (int flr = 0; flr < 4; flr++) {
    //generate doors up (up to five)
    int loop = random(1, 6);

    while (loop) {
      int i = random(0, 16);
      int j = random(0, 10);
      if (level[i][j] == PASSAGE && getSpotValue(i, j, flr) == NO_ITEM) {
        Item door;
        door.y = i;
        door.x = j;
        door.floorNumber = flr;
        door.value = CLOSED_DOOR;
        items[getFirstEmptyItemSpot()] = door;
        loop--;
      }
    }
  }
}

void placeItemInEmptySpot(int i, int j, int floorNumber, char item, int itemWidth, int itemHeight) {
  int flag = 0;
  Rect itemHitBox;
  itemHitBox.width = itemWidth;
  itemHitBox.height = itemHeight;


  for (int y = -1; y <= 1; y++) {
    for (int x = -1; x <= 1; x++) {
      if (!flag && !(y == 0 && x == 0)) {
        itemHitBox.x = j + x;
        itemHitBox.y = i + y;
        if (getSpotValue(i + y, j + x, floorNumber) == NO_ITEM && level[i + y][j + x] == EMPTY_SPACE && !arduboy.collide(player.hitBox, itemHitBox)) {
          setSpotValue(i, j, floorNumber, NO_ITEM);
          setSpotValue(i + y, j + x, floorNumber, item);
          flag = 1;
        }
      }
    }
  }
}

void generateHouse() {

  initializeItemArray();
  generateExit();
  generateStartingPoint();
  generateScepter();
  generateLeftUrn();
  generateRightUrn();
  generateMiddleUrn();
  generateStairs();
  generateMobs();
  if (!lights) {
    generateDoors();
    generateKey();
  }
}

int playerAndEnemyInSameRoom(Mob mob) {
  return mob.floorNumber == player.floorNumber && mob.roomNumber == player.roomNumber;
}

int borderTouched(Rect playerColl) {
  Rect upperBorder;
  upperBorder.x = 0;
  upperBorder.y = 0;
  upperBorder.width = 120;
  upperBorder.height = 1;

  Rect leftBorder;
  leftBorder.x = 0;
  leftBorder.y = 0;
  leftBorder.width = 1;
  leftBorder.height = 192;

  Rect rightBorder;
  rightBorder.x = 117;
  rightBorder.y = 0;
  rightBorder.width = 1;
  rightBorder.height = 192;

  Rect bottomBorder;
  bottomBorder.x = 0;
  bottomBorder.y = 191;
  bottomBorder.width = 120;
  bottomBorder.height = 1;

  if (arduboy.collide(playerColl, upperBorder) || arduboy.collide(playerColl, leftBorder) || arduboy.collide(playerColl, rightBorder) || arduboy.collide(playerColl, bottomBorder)) {
    return 1;
  }
  return 0;
}

void setup() {
  arduboy.begin();
  arduboy.initRandomSeed();

  room[0].x = 0;
  room[0].y = 0;
  room[0].width = 60;
  room[0].height = 66;

  room[1].x = 60;
  room[1].y = 0;
  room[1].width = 60;
  room[1].height = 66;

  room[2].x = 0;
  room[2].y = 65;
  room[2].width = 60;
  room[2].height = 60;

  room[3].x = 60;
  room[3].y = 65;
  room[3].width = 60;
  room[3].height = 60;

  room[4].x = 0;
  room[4].y = 124;
  room[4].width = 60;
  room[4].height = 66;

  room[5].x = 60;
  room[5].y = 124;
  room[5].width = 60;
  room[5].height = 66;
}

void generateMobs() {

  free(mobs);
  mobs = (Mob *)malloc((2 + numberOfBats) * sizeof(Mob));

  do {
    mobs[0].hitBox.x = random(0, 2) == 0 ? 26 : 86;
    mobs[0].hitBox.y = random(12, 170);
    mobs[0].hitBox.width = 8;
    mobs[0].hitBox.height = 8;
    mobs[0].roomCenterY = 0;
    mobs[0].roomCenterX = 0;
    mobs[0].floorNumber = random(0, 4);
    mobs[0].roomNumber = mobRoomNumber(mobs[0]);
    mobs[0].direction = random(0, 4);
    mobs[0].mobType = 0;
  } while (playerAndEnemyInSameRoom(mobs[0]));


  do {
    mobs[1].hitBox.x = random(0, 2) == 0 ? 26 : 86;
    mobs[1].hitBox.y = random(12, 170);
    mobs[1].hitBox.width = 8;
    mobs[1].hitBox.height = 7;
    mobs[1].roomCenterY = 0;
    mobs[1].roomCenterX = 0;
    mobs[1].floorNumber = random(0, 4);
    mobs[1].roomNumber = mobRoomNumber(mobs[1]);
    mobs[1].direction = random(0, 4);
    mobs[1].mobType = 1;
  } while (playerAndEnemyInSameRoom(mobs[1]));


  for (int i = 2; i < numberOfBats + 2; i++) {
    do {
      mobs[i].hitBox.x = random(0, 2) == 0 ? 26 : 86;
      mobs[i].hitBox.y = random(12, 170);
      mobs[i].hitBox.width = 8;
      mobs[i].hitBox.height = 6;
      mobs[i].roomCenterY = 0;
      mobs[i].roomCenterX = 0;
      mobs[i].floorNumber = random(0, 4);
      mobs[i].roomNumber = mobRoomNumber(mobs[i]);
      mobs[i].direction = random(0, 4);
      mobs[i].mobType = 2;
    } while (playerAndEnemyInSameRoom(mobs[i]));
  }
}

void newGame() {

  // initialize things here
  player.hitBox.width = 8;
  player.hitBox.height = 3;
  player.lives = 9;
  player.faint = 0;
  player.cooldown = 0;
  player.itemInUse = SCEPTER;  //NO_ITEM;

  generateHouse();

  initTime = millis();
}

int checkCollision(int direction) {
  Rect obstacle;
  Rect playerColl;

  // 1 - LEFT
  if (direction == 1) {
    playerColl.x = player.hitBox.x - 1;
    playerColl.y = player.hitBox.y + player.viewportY;
    playerColl.width = 1;
    playerColl.height = 3;
  }
  // 2 - UP
  else if (direction == 2) {
    playerColl.x = player.hitBox.x;
    playerColl.y = player.hitBox.y + player.viewportY - 1;
    playerColl.width = 8;
    playerColl.height = 1;
  }
  // 3 - RIGHT
  else if (direction == 3) {
    playerColl.x = player.hitBox.x + 8;
    playerColl.y = player.hitBox.y + player.viewportY;
    playerColl.width = 1;
    playerColl.height = 3;
  }
  // 4 - DOWN
  else if (direction == 4) {
    playerColl.x = player.hitBox.x;
    playerColl.y = player.hitBox.y + player.viewportY + 3;
    playerColl.width = 8;
    playerColl.height = 1;
  }

  //arduboy.fillRect(playerColl.x,playerColl.y-viewportY,playerColl.width,playerColl.height);

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 11; j++) {
      obstacle.x = j * 12;
      obstacle.y = i * 12;
      obstacle.width = 12;
      obstacle.height = 12;
      if (level[i][j] == WALL) {
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      } else if (level[i][j] == PASSAGE && getSpotValue(i, j, player.floorNumber) == CLOSED_DOOR && !lights && player.itemInUse != KEY) {
        if (i == 3 || i == 7 || i == 13) {
          obstacle.x = (j * 12) + 11;
          obstacle.y = i * 12;
          obstacle.width = 2;
          obstacle.height = 12;
        } else {
          obstacle.x = j * 12;
          obstacle.y = (i * 12) + 5;
          obstacle.width = 12;
          obstacle.height = 2;
        }
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      } else if (getSpotValue(i, j, player.floorNumber) == STAIRS_UP) {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber < 3) {
                player.floorNumber++;
                sound.tone(NOTE_A1, 250, NOTE_E2, 250, NOTE_G2, 500);
              }
              touched = 1;
            }
          } else {
            touched = 0;
          }
        }
      } else if (getSpotValue(i, j, player.floorNumber) == STAIRS_DOWN) {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber > 0) {
                player.floorNumber--;
                sound.tone(NOTE_G2, 500, NOTE_E2, 250, NOTE_A1, 250);
              }
              touched = 1;
            }
          } else {
            touched = 0;
          }
        }
      } else if (getSpotValue(i, j, player.floorNumber) == EXIT && player.itemInUse == COMPLETE_URN && arduboy.collide(playerColl, obstacle)
                 && borderTouched(playerColl)) {
        gamePhase = 2;
        sound.tones(GAME_WIN);
        endTime = millis();
      }
    }
  }
  return 0;
}

void loop() {

  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();
  arduboy.pollButtons();

  if (gamePhase == 0) {
    arduboy.print("Cursed Mansion 1300");
    arduboy.setCursor(20, 20);
    arduboy.print("Press A to play!");
    arduboy.setCursor(0, 30);
    arduboy.print("Number of bats");
    arduboy.setCursor(100, 30);
    arduboy.print(numberOfBats);
    arduboy.setCursor(0, 40);
    arduboy.print("Lights");
    arduboy.setCursor(100, 40);
    if (lights) {
      arduboy.print("On");
    } else {
      arduboy.print("Off");
    }


    if (arduboy.justPressed(RIGHT_BUTTON)) {
      numberOfBats = (numberOfBats + 1) % 4;
      if (numberOfBats == 0) {
        numberOfBats++;
      }
    }

    if (arduboy.justPressed(LEFT_BUTTON)) {
      lights = (lights + 1) % 2;
    }

    if (arduboy.justPressed(A_BUTTON)) {
      newGame();
      gamePhase = 1;
    }
  } else if (gamePhase == 1) {
    if (toggleMatch && millis() >= now + 20000) {
      toggleMatch = 0;
    }
    if (player.cooldown && millis() >= now + 5000) {
      player.cooldown = 0;
      player.faint = 0;
      color = 0;
      note = 190;
      generateMobs();
    }
    player.roomNumber = playerRoomNumber();

    //arduboy.print(player.hitBox.y+viewportY);

    //arduboy.drawRect(0,0,60,66-viewportY,WHITE);
    //arduboy.drawRect(0,67,60,66-viewportY,WHITE);
    //arduboy.setCursor(20,20);
    //arduboy.print(viewportY+player.hitBox.y);

    /*for(int i=0;i<MAX_NUMBER_OF_ITEMS_IN_HOUSE;i++){
    if(items[i].value==SCEPTER){
      tinyfont.setCursor(20,10);
      tinyfont.print(items[i].x);
      tinyfont.setCursor(20,20);
      tinyfont.print(items[i].y);
      tinyfont.setCursor(20,30);
      tinyfont.print(items[i].floorNumber);
    }

     if(items[i].value==KEY){
      tinyfont.setCursor(60,10);
      tinyfont.print(items[i].x);
      tinyfont.setCursor(60,20);
      tinyfont.print(items[i].y);
      tinyfont.setCursor(60,30);
      tinyfont.print(items[i].floorNumber);
    }
    }*/

    //DEBUG
    /*arduboy.print(1);*/
    /*arduboy.setCursor(30,10);
    arduboy.print(items[exitIndex].x);
    arduboy.setCursor(30,20);
    arduboy.print(items[exitIndex].y);*/


    if (arduboy.everyXFrames(4) && toggleMatch) {
      arduboy.fillCircle(player.hitBox.x + 4, player.hitBox.y + 1, 10);
    }

    Rect matchHitBox;
    matchHitBox.x = player.hitBox.x - 4;
    matchHitBox.y = player.hitBox.y - 7;
    matchHitBox.height = 17;
    matchHitBox.width = 17;
    //arduboy.drawRect(player.hitBox.x - 4,player.hitBox.y - 7,17,17,WHITE);

    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 10; j++) {
        if (player.hitBox.y + player.viewportY > (i * 12) - 64 && player.hitBox.y + player.viewportY < (i * 12) + 70 && (lights || player.roomNumber == roomNumberOfCoordinates(i, j) || i == 5 || i == 10 || j == 4 || j == 5)) {
          if (level[i][j] == WALL) {
            if (lights) {
              if (!color) {
                Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, whiteBlock, 0);
                //Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, floorBrick, player.floorNumber);
              } else {
                Sprites::drawPlusMask(12 * j, 12 * i - player.viewportY, blackBlock, 0);
              }
            } else {
              if (color) {
                Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, whiteBlock, 0);
                //Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, floorBrick, player.floorNumber);
              } else {
                Sprites::drawPlusMask(12 * j, 12 * i - player.viewportY, blackBlock, 0);
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == EXIT) {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, exitBlock, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == STAIRS_UP) {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, up, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == STAIRS_DOWN) {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, down, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == SCEPTER && toggleMatch) {
            Rect scepterHitBox;
            scepterHitBox.x = 12 * j + 2;
            scepterHitBox.y = 12 * i - player.viewportY + 2;
            scepterHitBox.height = 8;
            scepterHitBox.width = 8;

            if (arduboy.collide(matchHitBox, scepterHitBox)) {
              Sprites::drawSelfMasked(12 * j + 2, 12 * i - player.viewportY + 2, scepter, 0);
            }
            if (arduboy.collide(player.hitBox, scepterHitBox)) {
              placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 8, 8);
              //setSpotValue(i,j, player.floorNumber, itemInUse);
              player.itemInUse = SCEPTER;
              player.itemInUseW = 8;
              player.itemInUseH = 8;
              sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
            }

            //arduboy.drawRect(player.hitBox.x-3,player.hitBox.y-6,15,15,WHITE);
          } else if (getSpotValue(i, j, player.floorNumber) == RIGHT_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 4;
            urnHitBox.width = 4;

            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == LEFT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = LEFT_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == MIDDLE_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = MIDDLE_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == LEFT_AND_MIDDLE_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 4);
                player.itemInUse = RIGHT_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 4;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == LEFT_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 4;
            urnHitBox.width = 4;

            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeft, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == RIGHT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = LEFT_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == MIDDLE_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = LEFT_AND_MIDDLE_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == MIDDLE_AND_RIGHT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 4);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 4);
                player.itemInUse = LEFT_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 4;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == MIDDLE_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenter, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == RIGHT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = MIDDLE_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == LEFT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = LEFT_AND_MIDDLE_PART_OF_URN;
                sound.tone(NOTE_A5, 30, NOTE_A6, 30, NOTE_A6, 30);
              } else if (player.itemInUse == LEFT_AND_RIGHT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = MIDDLE_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 8;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == LEFT_AND_RIGHT_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == MIDDLE_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = LEFT_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 8;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == LEFT_AND_MIDDLE_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftCenter, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == RIGHT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = LEFT_AND_MIDDLE_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 8;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == MIDDLE_AND_RIGHT_PART_OF_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenterRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == LEFT_PART_OF_URN) {
                placeItemInEmptySpot(i, j, player.floorNumber, NO_ITEM, 4, 8);
                player.itemInUse = COMPLETE_URN;
                sound.tones(FULL_URN_SOUND);
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = MIDDLE_AND_RIGHT_PART_OF_URN;
                sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              }
              player.itemInUseW = 4;
              player.itemInUseH = 8;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == COMPLETE_URN && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 8;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, urn, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 8, 8);
              player.itemInUse = COMPLETE_URN;
              sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              player.itemInUseW = 8;
              player.itemInUseH = 8;
            }
          } else if (getSpotValue(i, j, player.floorNumber) == KEY && toggleMatch) {
            Rect keyHitBox;
            keyHitBox.x = 12 * j;
            keyHitBox.y = 12 * i - player.viewportY;
            keyHitBox.height = 3;
            keyHitBox.width = 8;

            if (arduboy.collide(matchHitBox, keyHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, key, 0);
            }
            if (arduboy.collide(player.hitBox, keyHitBox)) {
              placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 8, 3);
              player.itemInUse = KEY;
              sound.tone(NOTE_A4, 30, NOTE_B4, 30, NOTE_B4, 30);
              player.itemInUseW = 8;
              player.itemInUseH = 3;
            }
          }
        }
      }
    }

    //print white of eyes
    arduboy.fillRect(player.hitBox.x, player.hitBox.y, 3, 3);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y, 3, 3);


    //arduboy.drawRect(player.hitBox.x,player.hitBox.y,9,3);
    if (player.faint) {

      if (note <= NOTE_D3) {
        note = random(400, 500);
      }
      sound.tone(note -= 20, 30);

      if (arduboy.everyXFrames(5)) {
        color = (color + 1) % 2;
        eyeRollX = (eyeRollX + 1) % 8;
        eyeRollY = (eyeRollY + 1) % 8;
      }
      arduboy.fillRect(player.hitBox.x + eyeRoll[eyeRollX], player.hitBox.y + eyeRoll[eyeRollY], 1, 1, BLACK);
      arduboy.fillRect(player.hitBox.x + 5 + eyeRoll[eyeRollX], player.hitBox.y + eyeRoll[eyeRollY], 1, 1, BLACK);
      if (!player.cooldown) {
        player.cooldown = 1;
        now = millis();
      }
    } else {
      printPlayerEyes();

      int moving = 0;


      if (arduboy.pressed(RIGHT_BUTTON) && !arduboy.pressed(LEFT_BUTTON)) {
        moving = 1;
        if (player.hitBox.x < 110 && !checkCollision(3)) {
          player.hitBox.x++;
        }
      }

      if (arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
        moving = 1;
        if (player.hitBox.x > 0 && !checkCollision(1)) {
          player.hitBox.x--;
        }
      }

      if (arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(UP_BUTTON)) {
        moving = 1;
        if (!checkCollision(4)) {
          if (player.hitBox.y < 32 || (player.viewportY == 128 && player.hitBox.y < 61)) {
            player.hitBox.y++;
          } else if (player.viewportY < 128) {
            player.viewportY++;
          }
        }
      }

      if (arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON)) {
        moving = 1;
        if (!checkCollision(2)) {
          if (player.hitBox.y > 32 || (player.viewportY == 0 && player.hitBox.y > 0)) {
            player.hitBox.y--;
          } else if (player.viewportY > 0) {
            player.viewportY--;
          }
        }
      }

      if (arduboy.everyXFrames(10) && !sound.playing() && moving) {
        sound.tone(NOTE_C1, 50);
      }

      if (arduboy.justPressed(B_BUTTON) && !toggleMatch) {
        toggleMatch = 1;
        now = millis();
        matchesUsed++;
      }

      if (arduboy.justPressed(A_BUTTON) && NO_ITEM != player.itemInUse) {
        placeItemInEmptySpot((player.hitBox.y + player.viewportY) / 12, player.hitBox.x / 12, player.floorNumber, player.itemInUse, player.itemInUseW, player.itemInUseH);
        player.itemInUse = NO_ITEM;
        sound.tone(NOTE_B4, 30, NOTE_A4, 30, NOTE_A4, 30);
      }
    }

    //LIVES
    Sprites::drawOverwrite(121, 0, heart, 0);
    tinyfont.setCursor(122, 12);
    tinyfont.print(player.lives);

    //ITEM IN USE
    printItemInUse(player);

    //FLOOR
    tinyfont.setCursor(122, 50);
    tinyfont.print("F");
    tinyfont.setCursor(122, 58);
    tinyfont.print(player.floorNumber + 1);

    if (arduboy.everyXFrames(10)) {
      twentyFrames = (twentyFrames + 1) % 2;
    }

    int changeColor = 0;
    for (int i = 0; i < 2 + numberOfBats; i++) {
      if (playerAndEnemyInSameRoom(mobs[i])) {
        toggleMatch = 0;
        changeColor = 1;
      }
      moveMob(&mobs[i]);
      printMob(mobs[i], lights);
    }

    if (!player.faint) {
      if (changeColor) {
        if (arduboy.everyXFrames(random(8, 10))) {
          color = (color + 1) % 2;
          if (note <= NOTE_C2 || note >= NOTE_G2) {
            note = random(NOTE_C2, NOTE_G2);
          }
          sound.tone(note -= 20, 200);
        }
      } else {
        color = 0;
      }
    }




    /*char d = spider.direction;


    tinyfont.setCursor(80, 24);
    if (d == 0) {
      tinyfont.print('r');
    } else if (d == 1) {
      tinyfont.print('l');
    } else if (d == 2) {
      tinyfont.print(STAIRS_DOWN);
    } else if (d == 3) {
      tinyfont.print(STAIRS_UP);
    }*/

    /* tinyfont.setCursor(80, 32);
    tinyfont.print(spider.floorNumber + 1);
    tinyfont.setCursor(80, 40);
    tinyfont.print(player.roomNumber);
    /*tinyfont.print(spider.hitBox.x);
    tinyfont.setCursor(80, 56);
    tinyfont.print(spider.hitBox.y);*/
  }
  //game phase 2
  else if (gamePhase == 2) {
    arduboy.setCursor(0, 10);
    arduboy.print("You successfully");
    arduboy.setCursor(10, 20);
    arduboy.print("escaped the house!");
    arduboy.setCursor(0, 35);
    arduboy.print("your time is: ");
    arduboy.print((endTime - initTime) / 1000);
    arduboy.setCursor(5, 50);
    arduboy.print("Press A to continue!");

    if (arduboy.justPressed(A_BUTTON)) {
      gamePhase = 0;
    }
  } else if (gamePhase == 3) {
    arduboy.setCursor(0, 20);
    arduboy.print("You perished in");
    arduboy.setCursor(15, 30);
    arduboy.print("search of the urn!");
    arduboy.setCursor(5, 50);
    arduboy.print("press A to continue!");
    if (arduboy.justPressed(A_BUTTON)) {
      gamePhase = 0;
    }
  }
  arduboy.display();
}

void printPlayerEyes() {
  if (arduboy.pressed(RIGHT_BUTTON) && arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 2, player.hitBox.y, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 7, player.hitBox.y, 1, 1, BLACK);
  } else if (arduboy.pressed(RIGHT_BUTTON) && arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 2, player.hitBox.y + 2, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 7, player.hitBox.y + 2, 1, 1, BLACK);
  } else if (arduboy.pressed(LEFT_BUTTON) && arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(player.hitBox.x, player.hitBox.y, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y, 1, 1, BLACK);
  } else if (arduboy.pressed(LEFT_BUTTON) && arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(player.hitBox.x, player.hitBox.y + 2, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y + 2, 1, 1, BLACK);
  } else if (arduboy.pressed(RIGHT_BUTTON) && !arduboy.pressed(LEFT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 2, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 7, player.hitBox.y + 1, 1, 1, BLACK);
  } else if (arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y + 1, 1, 1, BLACK);
  } else if (arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 2, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 2, 1, 1, BLACK);
  } else if (arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y, 1, 1, BLACK);
  } else {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 1, 1, 1, BLACK);
  }
}

int mobRoomNumber(Mob mob) {
  for (int i = 0; i < 6; i++) {
    if (arduboy.collide(room[i], mob.hitBox)) {
      return i;
    }
  }
}

int playerRoomNumber() {
  Rect pl;
  pl.x = player.hitBox.x;
  pl.y = player.hitBox.y + player.viewportY;
  pl.width = player.hitBox.width;
  pl.height = player.hitBox.height;
  for (int i = 0; i < 6; i++) {
    if (arduboy.collide(room[i], pl)) {
      return i;
    }
  }
}

int roomNumberOfCoordinates(int y, int x) {
  Point p;
  p.x = x * 12;
  p.y = y * 12;
  for (int i = 0; i < 6; i++) {
    if (arduboy.collide(p, room[i])) {
      return i;
    }
  }
}

void printMob(Mob mob, int lights) {
  if (player.floorNumber == mob.floorNumber && (lights || playerAndEnemyInSameRoom(mob))) {
    if (mob.mobType == 0) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, spiderSprite, twentyFrames);
    } else if (mob.mobType == 1) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, ghostSprite, twentyFrames);
    } else if (mob.mobType == 2) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, batSprite, twentyFrames);
    }
  }
}

void goToDirection(Mob *mob) {
  if (mob->direction == RIGHT) {
    mob->hitBox.x++;
  } else if (mob->direction == LEFT) {
    mob->hitBox.x--;
  } else if (mob->direction == UP) {
    mob->hitBox.y++;
  } else if (mob->direction == DOWN) {
    mob->hitBox.y--;
  }
}

void getNextDirection(Mob *mob) {
  int flag = 1;
  int rnd = random(0, 4);
  while (flag==1) {
    if ((mob->roomNumber == 0 && rnd == LEFT) || (mob->roomNumber == 1 && rnd == RIGHT)
        || (mob->roomNumber == 4 && rnd == LEFT) || (mob->roomNumber == 5 && rnd == RIGHT)
        || (mob->roomNumber == 0 && rnd == UP && getSpotValue(0, 2, mob->floorNumber) == (NO_ITEM || EXIT))
        || (mob->roomNumber == 1 && rnd == UP && getSpotValue(0, 7, mob->floorNumber) == (NO_ITEM || EXIT))
        || (mob->roomNumber == 2 && rnd == LEFT && getSpotValue(7, 0, mob->floorNumber) == (NO_ITEM || EXIT))
        || (mob->roomNumber == 3 && rnd == RIGHT && getSpotValue(7, 9, mob->floorNumber) == (NO_ITEM || EXIT))
        || (mob->roomNumber == 4 && rnd == DOWN && getSpotValue(15, 2, mob->floorNumber) == (NO_ITEM || EXIT))
        || (mob->roomNumber == 5 && rnd == DOWN && getSpotValue(15, 7, mob->floorNumber) == (NO_ITEM || EXIT))
        /*|| (mob->roomNumber == 0 && rnd == RIGHT && getSpotValue(3, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 1 && rnd == LEFT && getSpotValue(3, 5, mob->floorNumber) == 'l')
        || (mob->roomNumber == 0 && rnd == UP && getSpotValue(5, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == DOWN && getSpotValue(5, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 1 && rnd == UP && getSpotValue(5, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == DOWN && getSpotValue(5, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == RIGHT && getSpotValue(7, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == LEFT && getSpotValue(7, 5, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == UP && getSpotValue(10, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 4 && rnd == DOWN && getSpotValue(10, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == UP && getSpotValue(10, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 5 && rnd == DOWN && getSpotValue(10, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 4 && rnd == RIGHT && getSpotValue(13, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 5 && rnd == LEFT && getSpotValue(13, 5, mob->floorNumber) == 'l')*/
    ) {
      rnd = random(0, 4);
    } else {
      flag = 0;
    }
  }
  mob->direction = rnd;
}

void isEveryRoomReachable() {
}

void moveMob(Mob *mob) {
  mob->roomNumber = mobRoomNumber(*mob);

  if (playerAndEnemyInSameRoom(*mob) && (player.itemInUse != SCEPTER || mob->mobType == 1) && !player.faint) {
    mob->roomCenterX = 0;
    mob->roomCenterY = 0;

    if (arduboy.everyXFrames(mob->mobType == 1 ? 2 : 3)) {
      if (player.hitBox.x > mob->hitBox.x) {
        mob->hitBox.x++;
      } else if (player.hitBox.x < mob->hitBox.x) {
        mob->hitBox.x--;
      }

      if (player.hitBox.y + player.viewportY - 3 > mob->hitBox.y) {
        mob->hitBox.y++;
      } else if (player.hitBox.y + player.viewportY - 3 < mob->hitBox.y) {
        mob->hitBox.y--;
      }

      Rect pl;
      pl.x = player.hitBox.x;
      pl.y = player.hitBox.y + player.viewportY;
      pl.width = player.hitBox.width;
      pl.height = player.hitBox.height;

      //if enemy collides player
      if (arduboy.collide(pl, mob->hitBox) && !player.faint) {
        player.faint = 1;
        player.lives--;
        if (player.lives == 0) {
          gamePhase = 3;
          sound.tones(GAME_OVER);
        }
        //if bat take item in use away
        if (mob->mobType == 2) {
          generateItem(player.itemInUse);
          player.itemInUse = NO_ITEM;
        }
      }
    }


  } else if (mob->roomCenterX == 1 && mob->roomCenterY == 1) {

    if (arduboy.everyXFrames(4)) {

      goToDirection(mob);
      //if mob hits the borderk aka change floor
      if (mob->hitBox.x <= 0 || mob->hitBox.x >= 110 || mob->hitBox.y <= 0 || mob->hitBox.y >= 184) {
        if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == STAIRS_UP) {
          mob->floorNumber++;
        } else if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == STAIRS_DOWN) {
          mob->floorNumber--;
        }
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        getNextDirection(mob);
      }
      //if mob changes room
      if (mobRoomNumber(*mob) != mob->roomNumber) {
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        mob->roomNumber = mobRoomNumber(*mob);
        getNextDirection(mob);
      }
    }
  } else {
    if (arduboy.everyXFrames(4)) {

      //Move on X axis
      if (mob->roomNumber % 2 == 0) {
        if (mob->hitBox.x < 26) {
          mob->hitBox.x++;
        } else if (mob->hitBox.x > 26) {
          mob->hitBox.x--;
        } else {
          mob->roomCenterX = 1;
        }

      } else {
        if (mob->hitBox.x < 86) {
          mob->hitBox.x++;
        } else if (mob->hitBox.x > 86) {
          mob->hitBox.x--;
        } else {
          mob->roomCenterX = 1;
        }
      }

      //Move on Y axis
      if (mob->roomNumber == 0 || mob->roomNumber == 1) {
        if (mob->hitBox.y < 38) {
          mob->hitBox.y++;
        } else if (mob->hitBox.y > 38) {
          mob->hitBox.y--;
        } else {
          mob->roomCenterY = 1;
        }
      }
      if (mob->roomNumber == 2 || mob->roomNumber == 3) {
        if (mob->hitBox.y < 86) {
          mob->hitBox.y++;
        } else if (mob->hitBox.y > 86) {
          mob->hitBox.y--;
        } else {
          mob->roomCenterY = 1;
        }
      }
      if (mob->roomNumber == 4 || mob->roomNumber == 5) {
        if (mob->hitBox.y < 158) {
          mob->hitBox.y++;
        } else if (mob->hitBox.y > 158) {
          mob->hitBox.y--;
        } else {
          mob->roomCenterY = 1;
        }
      }
    }
  }
}
