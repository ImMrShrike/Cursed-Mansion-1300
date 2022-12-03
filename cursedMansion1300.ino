// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <Tinyfont.h>
#include <time.h>
#include "constants.h"



struct Mob {
  Rect hitBox;
  int floorNumber;
  int roomNumber;
  int roomCenterX;
  int roomCenterY;
  int direction;
  int previousDirection;
  int mobType; // 0 - spider / 1 - ghost / 2 - bat
};

struct Player {
  Rect hitBox;
  int floorNumber;
  int roomNumber;
  int viewportY;
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
    items[i].value = 'X';
  }
}

int getFirstEmptyItemSpot() {
  for (int i = 0; i < MAX_NUMBER_OF_ITEMS_IN_HOUSE; i++) {
    if (items[i].value == 'X') {
      return i;
    }
  }
  return -1;
}

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

int lives = 9;
int toggleMatch = 0;
int twentyFrames = 0;
char itemInUse = "X";
int touched = 0;
int gameOver = 0;
int matchesUsed = 0;
unsigned long now = 0;
int lights = 0;

void printItemInUse() {
  if (itemInUse == 'S') {
    Sprites::drawOverwrite(122, 30, scepterIcon, 0);
  }
  else if (itemInUse == 'C') {
    Sprites::drawOverwrite(122, 30, urnIcon, 0);
  }
  else if (itemInUse == 'R') {
    Sprites::drawOverwrite(122, 30, urnIcon, 1);
  }
  else if (itemInUse == 'L') {
    Sprites::drawOverwrite(122, 30, urnIcon, 2);
  }
  else if (itemInUse == 'M') {
    Sprites::drawOverwrite(122, 30, urnIcon, 3);
  }
  else if (itemInUse == 'J') {
    Sprites::drawOverwrite(122, 30, urnIcon, 4);
  }
  else if (itemInUse == 'I') {
    Sprites::drawOverwrite(122, 30, urnIcon, 5);
  }
  else if (itemInUse == 'H') {
    Sprites::drawOverwrite(122, 30, urnIcon, 6);
  }
  else if (itemInUse == 'K') {
    Sprites::drawOverwrite(122, 30, keyIcon, 0);
  }

}

Player player;

Mob spider;
Mob ghost;
Mob bat1;
Mob bat2;
Mob bat3;

//objects code:
// 0 - empty space
// w - wall
// e - exit
// s - stairs chance
// P/p - passage
// c - closed door
// u - stairs up
// d - stairs down
// o - open door
// l - locked door
// S - Scepter
// C - Complete Urn
// L - Left part of the Urn
// M - Middle part of the Urn
// R - Right part of the Urn
// K - Key
// I - Left and Middle part of the Urn
// H - Left and Right part of the Urn
// J - Middle and Right part of the Urn
// X - No Item


char level[16][11] = {
  {'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'P', 'p', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', 'w', 'P', 'w', 'w', 'w', 'w', 'P', 'w', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'s', '0', '0', '0', 'P', 'p', '0', '0', '0', 's', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', 'w', 'P', 'w', 'w', 'w', 'w', 'P', 'w', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'P', 'p', '0', '0', '0', 'w', 'w'},
  {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
  {'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w'}
};

Rect room[6];

char getSpotValue(int i, int j, int floorNumber) {
  for (int k = 0; k < MAX_NUMBER_OF_ITEMS_IN_HOUSE; k++) {
    if (items[k].y == i && items[k].x == j && items[k].floorNumber == floorNumber) {
      return items[k].value;
    }
  }
  return 'X';
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

void placeItemInRandomAccessibleRoom(char item, int startingRoom, int startingFloor) {

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
  int loop = 1;
  while (loop) {
    int i = random(0, 16);
    int j = random(0, 10);
    if (level[i][j] == 's') {
      Item exitDoor;
      exitDoor.y = i;
      exitDoor.x = j;
      exitDoor.floorNumber = 0;
      exitDoor.value = 'e';
      items[getFirstEmptyItemSpot()] = exitDoor;
      loop = 0;
    }
  }
}



void generateScepter() {
  generateItem('S');
}

void generateLeftUrn() {
  generateItem('L');
}

void generateRightUrn() {
  generateItem('R');
}

void generateMiddleUrn() {
  generateItem('M');
}

void generateKey() {
  generateItem('K');
}

void generateItem(char itemValue) {
  while (1) {
    int i = random(0, 16);
    int j = random(0, 10);
    int floorNumber = random(0, 4);
    if (level[i][j] == '0' && getSpotValue(i, j, floorNumber) == 'X') {
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
      if (level[i][j] == 's' && getSpotValue(i, j, flr) == 'X') {
        Item stairsUp;
        stairsUp.y = i;
        stairsUp.x = j;
        stairsUp.floorNumber = flr;
        stairsUp.value = 'u';
        items[getFirstEmptyItemSpot()] = stairsUp;

        Item stairsDown;
        stairsDown.y = i;
        stairsDown.x = j;
        stairsDown.floorNumber = flr + 1;
        stairsDown.value = 'd';
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
      if (level[i][j] == 'P' && getSpotValue(i, j, flr) == 'X') {
        Item door;
        door.y = i;
        door.x = j;
        door.floorNumber = flr;
        door.value = 'c';
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
        if (getSpotValue(i + y, j + x, floorNumber) == 'X' && level[i + y][j + x] == '0' && !arduboy.collide(player.hitBox, itemHitBox)) {
          setSpotValue(i, j, floorNumber, 'X');
          setSpotValue(i + y, j + x, floorNumber, item);
          flag = 1;
        }
      }
    }
  }
}





void generateHouse(int lightsOn, int keyPresent) {

  generateExit();
  generateScepter();
  generateLeftUrn();
  generateRightUrn();
  generateMiddleUrn();
  generateStairs();
  generateDoors();
  generateKey();
}


int playerAndEnemyInSameRoom(Mob mob) {
  int sameRoom = 0;
  Rect pl;
  pl.x = player.hitBox.x;
  pl.y = player.hitBox.y + player.viewportY;
  pl.width = player.hitBox.width;
  pl.height = player.hitBox.height;

  if (mob.floorNumber == player.floorNumber) {
    for (int i = 0; i < 6; i++) {
      if (arduboy.collide(room[i], mob.hitBox) && arduboy.collide(room[i], pl)) {
        return 1;
      }
    }
  }
  return 0;
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
  initializeItemArray();
  generateHouse(0, 0);
  // initialize things here

  player.hitBox.x = 26;
  player.hitBox.y = 20;
  player.hitBox.width = 8;
  player.hitBox.height = 3;
  player.floorNumber = 0;
  player.roomNumber = playerRoomNumber();
  player.viewportY = 0;

  spider.hitBox.x = random(0, 2) == 0 ? 26 : 86;
  spider.hitBox.y = random(12, 170);
  spider.hitBox.width = 8;
  spider.hitBox.height = 8;
  spider.roomCenterY = 0;
  spider.roomCenterX = 0;
  spider.floorNumber = random(0, 4);
  spider.roomNumber = mobRoomNumber(spider);
  spider.direction = random(0, 4);
  spider.mobType = 0;

  /*ghost.hitBox.x = random(0, 2) == 0 ? 26 : 86;
    ghost.hitBox.y = random(12, 170);
    ghost.hitBox.width = 8;
    ghost.hitBox.height = 7;
    ghost.roomCenterY=0;
    ghost.roomCenterX=0;
    ghost.floorNumber = random(0, 4);
    ghost.roomNumber = mobRoomNumber(ghost);
    ghost.direction = random(0, 4);
    ghost.mobType=1;

    bat1.hitBox.x = random(0, 2) == 0 ? 26 : 86;
    bat1.hitBox.y = random(12, 170);
    bat1.hitBox.width = 8;
    bat1.hitBox.height = 6;
    bat1.roomCenterY=0;
    bat1.roomCenterX=0;
    bat1.floorNumber = random(0, 4);
    bat1.roomNumber = mobRoomNumber(bat1);
    bat1.direction = random(0, 4);
    bat1.mobType=2;

    bat2.hitBox.x = random(0, 2) == 0 ? 26 : 86;
    bat2.hitBox.y = random(12, 170);
    bat2.hitBox.width = 8;
    bat2.hitBox.height = 6;
    bat2.roomCenterY=0;
    bat2.roomCenterX=0;
    bat2.floorNumber = random(0, 4);
    bat2.roomNumber = mobRoomNumber(bat2);
    bat2.direction = random(0, 4);
    bat2.mobType=2;

    bat3.hitBox.x = random(0, 2) == 0 ? 26 : 86;
    bat3.hitBox.y = random(12, 170);
    bat3.hitBox.width = 8;
    bat3.hitBox.height = 6;
    bat3.roomCenterY=0;
    bat3.roomCenterX=0;
    bat3.floorNumber = random(0, 4);
    bat3.roomNumber = mobRoomNumber(bat2);
    bat3.direction = random(0, 4);
    bat3.mobType=2;*/

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
      if (level[i][j] == 'w') {
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      }
      else if ((level[i][j] == 'P') && getSpotValue(i, j, player.floorNumber) == 'c' && !lights) {
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      }
      else if (getSpotValue(i, j, player.floorNumber) == 'u') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber < 3) {
                player.floorNumber++;
              }
              touched = 1;
            }
          }
          else {
            touched = 0;
          }
        }
      }
      else if (getSpotValue(i, j, player.floorNumber) == 'd') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber > 0) {
                player.floorNumber--;
              }
              touched = 1;
            }
          }
          else {
            touched = 0;
          }
        }
      }
      else if (getSpotValue(i, j, player.floorNumber) == 'e' && itemInUse == 'C' && arduboy.collide(playerColl, obstacle)
               && borderTouched(playerColl)) {
        gameOver = 1;
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

  if (millis() >= now + 20000) {
    toggleMatch = 0;
  }
  player.roomNumber = playerRoomNumber();

  //arduboy.print(player.hitBox.y+viewportY);

  //arduboy.drawRect(0,0,60,66-viewportY,WHITE);
  //arduboy.drawRect(0,67,60,66-viewportY,WHITE);
  //arduboy.setCursor(20,20);
  //arduboy.print(viewportY+player.hitBox.y);

  /*for(int i=0;i<MAX_NUMBER_OF_ITEMS_IN_HOUSE;i++){
    if(items[i].value=='S'){
      tinyfont.setCursor(20,10);
      tinyfont.print(items[i].x);
      tinyfont.setCursor(20,20);
      tinyfont.print(items[i].y);
      tinyfont.setCursor(20,30);
      tinyfont.print(items[i].floorNumber);
    }

     if(items[i].value=='K'){
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
  /* arduboy.setCursor(0,10);
    arduboy.print(itemY);
    arduboy.setCursor(0,20);
    arduboy.print(itemFloor);
  */

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
      if ( player.hitBox.y + player.viewportY > (i * 12) - 64 && player.hitBox.y + player.viewportY < (i * 12) + 70 && (lights || player.roomNumber == roomNumberOfCoordinates(i, j) || i ==  5 || i == 10 || j == 4 || j == 5)) {
        if (level[i][j] == 'w') {
          if (lights) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, whiteBlock, 0);
            //arduboy.drawRect(12 * j, 12 * i - player.viewportY, 12, 12, WHITE);
          }
          else {
            Sprites::drawPlusMask(12 * j, 12 * i - player.viewportY, blackBlock, 0);
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'e') {
          Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, exitBlock, 0);
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'u') {
          Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, up, 0);
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'd') {
          Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, down, 0);
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'S' && toggleMatch) {
          Rect scepterHitBox;
          scepterHitBox.x = 12 * j + 2;
          scepterHitBox.y = 12 * i - player.viewportY + 2;
          scepterHitBox.height = 8;
          scepterHitBox.width = 8;

          if (arduboy.collide(matchHitBox, scepterHitBox)) {
            Sprites::drawSelfMasked(12 * j + 2, 12 * i - player.viewportY + 2, scepter, 0);
          }
          if (arduboy.collide(player.hitBox, scepterHitBox)) {
            placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 8, 8);
            //setSpotValue(i,j, player.floorNumber, itemInUse);
            itemInUse = 'S';
          }

          //arduboy.drawRect(player.hitBox.x-3,player.hitBox.y-6,15,15,WHITE);
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'R' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 4;
          urnHitBox.width = 4;

          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnRight, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'L') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'H';
            }
            else if (itemInUse == 'M') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'J';
            }
            else if (itemInUse == 'I') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 4);
              itemInUse = 'R';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'L' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 4;
          urnHitBox.width = 4;

          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeft, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'R') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'H';
            }
            else if (itemInUse == 'M') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'I';
            }
            else if (itemInUse == 'J') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 4);
              itemInUse = 'L';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'M' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 4;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenter, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'R') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'J';
            }
            else if (itemInUse == 'L') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'I';
            }
            else if (itemInUse == 'H') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 8);
              itemInUse = 'M';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'H' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 4;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftRight, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'M') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 8);
              itemInUse = 'H';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'I' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 4;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftCenter, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'R') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 8);
              itemInUse = 'I';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'J' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 4;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenterRight, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            if (itemInUse == 'L') {
              placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
              itemInUse = 'C';
            }
            else {
              placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 4, 8);
              itemInUse = 'J';
            }
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'C' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - player.viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 8;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, urn, 0);
          }
          if (arduboy.collide(player.hitBox, urnHitBox)) {
            placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 8, 8);
            itemInUse = 'C';
          }
        }
        else if (getSpotValue(i, j, player.floorNumber) == 'K' && toggleMatch) {
          Rect keyHitBox;
          keyHitBox.x = 12 * j;
          keyHitBox.y = 12 * i - player.viewportY;
          keyHitBox.height = 3;
          keyHitBox.width = 8;

          if (arduboy.collide(matchHitBox, keyHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, key, 0);
          }
          if (arduboy.collide(player.hitBox, keyHitBox)) {
            placeItemInEmptySpot(i, j, player.floorNumber, itemInUse, 8, 3);
            itemInUse = 'K';
          }
        }
      }
    }
  }

  arduboy.fillRect(player.hitBox.x, player.hitBox.y, 3, 3);
  arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y, 3, 3);


  //arduboy.drawRect(player.hitBox.x,player.hitBox.y,9,3);

  if (arduboy.pressed(RIGHT_BUTTON) && !arduboy.pressed(LEFT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 2, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 7, player.hitBox.y + 1, 1, 1, BLACK);
    if (player.hitBox.x < 110 && !checkCollision(3)) {
      player.hitBox.x++;
    }

  }

  if (arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y + 1, 1, 1, BLACK);
    if (player.hitBox.x > 0 && !checkCollision(1)) {
      player.hitBox.x--;
    }
  }

  if (arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 2, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 2, 1, 1, BLACK);

    if (!checkCollision(4)) {
      if (player.hitBox.y < 32 || (player.viewportY == 128 && player.hitBox.y < 61)) {
        player.hitBox.y++;
      }
      else if (player.viewportY < 128) {
        player.viewportY++;
      }
    }


  }

  if (arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y, 1, 1, BLACK);

    if (!checkCollision(2)) {
      if (player.hitBox.y > 32 || (player.viewportY == 0 && player.hitBox.y > 0)) {
        player.hitBox.y--;
      }
      else if (player.viewportY > 0) {
        player.viewportY--;
      }
    }

  }


  //draw black eyes in middle
  if (!arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 1, 1, 1, BLACK);
  }

  if (arduboy.justPressed(B_BUTTON) && !toggleMatch) {
    toggleMatch = 1;
    now = millis();
    matchesUsed++;
  }

  // game goes here
  tinyfont.setCursor(122, 0);
  tinyfont.print(lives);
  tinyfont.setCursor(122, 10);
  tinyfont.print(player.floorNumber + 1);
  tinyfont.setCursor(122, 20);
  tinyfont.print(matchesUsed);
  printItemInUse();


  if (arduboy.everyXFrames(10)) {
    twentyFrames = (twentyFrames + 1) % 2;

  }

  moveMob(&spider);
  /* moveMob(&ghost);
    moveMob(&bat1);
    moveMob(&bat2);
    moveMob(&bat3);*/
  printMobs(spider, lights);
  /*printMobs(ghost);
    printMobs(bat1);
    printMobs(bat2);
    printMobs(bat3);*/

  char d = spider.direction;


  tinyfont.setCursor(80, 24);
  if (d == 0) {
    tinyfont.print('r');
  }
  else if (d == 1) {
    tinyfont.print('l');
  }
  else if (d == 2) {
    tinyfont.print('d');
  }
  else if (d == 3) {
    tinyfont.print('u');
  }

  tinyfont.setCursor(80, 32);
  tinyfont.print(spider.floorNumber + 1);
  tinyfont.setCursor(80, 40);
  tinyfont.print(player.roomNumber);
  /*tinyfont.print(spider.hitBox.x);
    tinyfont.setCursor(80, 56);
    tinyfont.print(spider.hitBox.y);*/


  arduboy.display();
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

void printMobs(Mob mob, int lights) {
  if (player.floorNumber == mob.floorNumber && (lights || playerAndEnemyInSameRoom(mob))) {
    if (mob.mobType == 0) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, spiderSprite, twentyFrames);
    }
    else if (mob.mobType == 1) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, ghostSprite, twentyFrames);
    }
    else if (mob.mobType == 2) {
      Sprites::drawSelfMasked(mob.hitBox.x, mob.hitBox.y - player.viewportY, batSprite, twentyFrames);
    }
  }
}

void goToDirection(Mob *mob) {
  if (mob->direction == 0) {
    mob->hitBox.x++;
  }
  else if (mob->direction == 1) {
    mob->hitBox.x--;
  }
  else if (mob->direction == 2) {
    mob->hitBox.y++;
  }
  else if (mob->direction == 3) {
    mob->hitBox.y--;
  }
}

void getNextDirection(Mob *mob) {
  int flag = 1;
  int rnd = random(0, 4);
  while (flag) {
    if ((mob->roomNumber == 0 && rnd == 1) || (mob->roomNumber == 1 && rnd == 0)
        || (mob->roomNumber == 4 && rnd == 1) || (mob->roomNumber == 5 && rnd == 0)
        || (mob->roomNumber == 0 && rnd == 3 && getSpotValue(0, 2, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 1 && rnd == 3 && getSpotValue(0, 7, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 2 && rnd == 1 && getSpotValue(7, 0, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 3 && rnd == 0 && getSpotValue(7, 9, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 4 && rnd == 2 && getSpotValue(15, 2, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 5 && rnd == 2 && getSpotValue(15, 7, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 0 && rnd == 0 && getSpotValue(3, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 1 && rnd == 1 && getSpotValue(3, 5, mob->floorNumber) == 'l')
        || (mob->roomNumber == 0 && rnd == 2 && getSpotValue(5, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == 3 && getSpotValue(5, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 1 && rnd == 2 && getSpotValue(5, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == 3 && getSpotValue(5, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == 0 && getSpotValue(7, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == 1 && getSpotValue(7, 5, mob->floorNumber) == 'l')
        || (mob->roomNumber == 2 && rnd == 2 && getSpotValue(10, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 4 && rnd == 3 && getSpotValue(10, 2, mob->floorNumber) == 'l')
        || (mob->roomNumber == 3 && rnd == 2 && getSpotValue(10, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 5 && rnd == 3 && getSpotValue(10, 7, mob->floorNumber) == 'l')
        || (mob->roomNumber == 4 && rnd == 0 && getSpotValue(13, 4, mob->floorNumber) == 'l')
        || (mob->roomNumber == 5 && rnd == 1 && getSpotValue(13, 5, mob->floorNumber) == 'l')) {
      rnd = random(0, 4);
    }
    else {
      flag = 0;
    }
  }
  mob->direction = rnd;
}

void moveMob(Mob *mob) {
  int mobRoom = mobRoomNumber(*mob);

  if (playerAndEnemyInSameRoom(*mob)) {
    toggleMatch = 0;


    if (itemInUse != 'S') {
      mob->roomCenterX = 0;
      mob->roomCenterY = 0;

      if (arduboy.everyXFrames(3)) {
        if (player.hitBox.x > mob->hitBox.x) {
          mob->hitBox.x++;
        }
        else if (player.hitBox.x < mob->hitBox.x) {
          mob->hitBox.x--;
        }

        if (player.hitBox.y + player.viewportY - 3 > mob->hitBox.y) {
          mob->hitBox.y ++;
        }
        else if (player.hitBox.y + player.viewportY - 3 < mob->hitBox.y) {
          mob->hitBox.y--;
        }
      }
    }

  }
  else if (mob->roomCenterX == 1 && mob->roomCenterY == 1) {

    if (arduboy.everyXFrames(3)) {

      goToDirection(mob);

      if (mob->hitBox.x <= 0 || mob->hitBox.x >= 110 || mob->hitBox.y <= 0 || mob->hitBox.y >= 184) {
        if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == 'u') {
          mob->floorNumber++;
        }
        else if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == 'd') {
          mob->floorNumber--;
        }
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        mob->previousDirection = mob->direction;
        getNextDirection(mob);
      }
      if (mobRoomNumber(*mob) != mob->roomNumber) {
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        mob->roomNumber = mobRoomNumber(*mob);
        mob->previousDirection = mob->direction;
        getNextDirection(mob);

      }
    }
  }
  else {
    if (arduboy.everyXFrames(3)) {

      //Move on X axis
      if (mobRoom % 2 == 0) {
        if (mob->hitBox.x < 26) {
          mob->hitBox.x++;
        }
        else if (mob->hitBox.x > 26) {
          mob->hitBox.x--;
        }
        else {
          mob->roomCenterX = 1;
        }

      }
      else {
        if (mob->hitBox.x < 86) {
          mob->hitBox.x++;
        }
        else if (mob->hitBox.x > 86) {
          mob->hitBox.x--;
        }
        else {
          mob->roomCenterX = 1;
        }
      }

      //Move on Y axis
      if (mobRoom == 0 || mobRoom == 1) {
        if (mob->hitBox.y < 38) {
          mob->hitBox.y++;
        }
        else if (mob->hitBox.y > 38) {
          mob->hitBox.y--;
        }
        else {
          mob->roomCenterY = 1;
        }
      }
      if (mobRoom == 2 || mobRoom == 3) {
        if (mob->hitBox.y < 86) {
          mob->hitBox.y++;
        }
        else if (mob->hitBox.y > 86) {
          mob->hitBox.y--;
        }
        else {
          mob->roomCenterY = 1;
        }
      }
      if (mobRoom == 4 || mobRoom == 5) {
        if (mob->hitBox.y < 158) {
          mob->hitBox.y++;
        }
        else if (mob->hitBox.y > 158) {
          mob->hitBox.y--;
        }
        else {
          mob->roomCenterY = 1;
        }
      }
    }
  }

}
