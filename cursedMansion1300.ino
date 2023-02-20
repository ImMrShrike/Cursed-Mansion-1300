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
  int previousDirection;
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

void printItemInUse(Player player) {
  if (player.itemInUse == 'S') {
    Sprites::drawOverwrite(122, 30, scepterIcon, 0);
  } else if (player.itemInUse == 'C') {
    Sprites::drawOverwrite(122, 30, urnIcon, 0);
  } else if (player.itemInUse == 'R') {
    Sprites::drawOverwrite(122, 30, urnIcon, 1);
  } else if (player.itemInUse == 'L') {
    Sprites::drawOverwrite(122, 30, urnIcon, 2);
  } else if (player.itemInUse == 'M') {
    Sprites::drawOverwrite(122, 30, urnIcon, 3);
  } else if (player.itemInUse == 'J') {
    Sprites::drawOverwrite(122, 30, urnIcon, 4);
  } else if (player.itemInUse == 'I') {
    Sprites::drawOverwrite(122, 30, urnIcon, 5);
  } else if (player.itemInUse == 'H') {
    Sprites::drawOverwrite(122, 30, urnIcon, 6);
  } else if (player.itemInUse == 'K') {
    Sprites::drawOverwrite(122, 30, keyIcon, 0);
  }
}

Player player;

Mob *mobs;

/*Mob spider;
Mob ghost;
Mob bat1;
Mob bat2;
Mob bat3;*/

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
  { 'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'P', 'p', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', 'w', 'P', 'w', 'w', 'w', 'w', 'P', 'w', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 's', '0', '0', '0', 'P', 'p', '0', '0', '0', 's', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', 'w', 'P', 'w', 'w', 'w', 'w', 'P', 'w', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'P', 'p', '0', '0', '0', 'w', 'w' },
  { 'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w' },
  { 'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w' }
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





void generateHouse(int lights, int numberOfBats) {

  initializeItemArray();
  generateExit();
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


  for (int i = 2; i < numberOfBats + 2; i++) {
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
  }
}

void newGame() {
  generateHouse(lights, numberOfBats);
  // initialize things here

  player.hitBox.x = 26;
  player.hitBox.y = 20;
  player.hitBox.width = 8;
  player.hitBox.height = 3;
  player.floorNumber = 0;
  player.roomNumber = playerRoomNumber();
  player.viewportY = 0;
  player.lives = 9;
  player.faint = 0;
  player.cooldown = 0;
  player.itemInUse = 'S';

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
      if (level[i][j] == 'w') {
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      } else if (level[i][j] == 'P' && getSpotValue(i, j, player.floorNumber) == 'c' && !lights && player.itemInUse != 'K') {
        if(i==3 || i==7 || i==13){
            obstacle.x = (j * 12) + 11;
            obstacle.y = i * 12;
            obstacle.width = 2;
            obstacle.height = 12;       
        }   
        else{
          obstacle.x = j * 12;
          obstacle.y = (i * 12) + 5;
          obstacle.width = 12;
          obstacle.height = 2;  
        }             
        if (arduboy.collide(playerColl, obstacle)) {
          return 1;
        }
      } else if (getSpotValue(i, j, player.floorNumber) == 'u') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber < 3) {
                player.floorNumber++;
              }
              touched = 1;
            }
          } else {
            touched = 0;
          }
        }
      } else if (getSpotValue(i, j, player.floorNumber) == 'd') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (borderTouched(playerColl)) {
            if (!touched) {
              if (player.floorNumber > 0) {
                player.floorNumber--;
              }
              touched = 1;
            }
          } else {
            touched = 0;
          }
        }
      } else if (getSpotValue(i, j, player.floorNumber) == 'e' && player.itemInUse == 'C' && arduboy.collide(playerColl, obstacle)
                 && borderTouched(playerColl)) {
        gamePhase = 2;
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
      generateMobs();
    }
    //player.roomNumber = playerRoomNumber();

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
        if (player.hitBox.y + player.viewportY > (i * 12) - 64 && player.hitBox.y + player.viewportY < (i * 12) + 70 && (lights || playerRoomNumber() == roomNumberOfCoordinates(i, j) || i == 5 || i == 10 || j == 4 || j == 5)) {
          if (level[i][j] == 'w') {
            if (lights) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, whiteBlock, 0);
              //arduboy.drawRect(12 * j, 12 * i - player.viewportY, 12, 12, WHITE);
            } else {
              Sprites::drawPlusMask(12 * j, 12 * i - player.viewportY, blackBlock, 0);
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'e') {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, exitBlock, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == 'u') {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, up, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == 'd') {
            Sprites::drawOverwrite(12 * j, 12 * i - player.viewportY, down, 0);
          } else if (getSpotValue(i, j, player.floorNumber) == 'S' && toggleMatch) {
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
              player.itemInUse = 'S';
            }

            //arduboy.drawRect(player.hitBox.x-3,player.hitBox.y-6,15,15,WHITE);
          } else if (getSpotValue(i, j, player.floorNumber) == 'R' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 4;
            urnHitBox.width = 4;

            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'L') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'H';
              } else if (player.itemInUse == 'M') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'J';
              } else if (player.itemInUse == 'I') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 4);
                player.itemInUse = 'R';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'L' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 4;
            urnHitBox.width = 4;

            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeft, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'R') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'H';
              } else if (player.itemInUse == 'M') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'I';
              } else if (player.itemInUse == 'J') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 4);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 4);
                player.itemInUse = 'L';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'M' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenter, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'R') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'J';
              } else if (player.itemInUse == 'L') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'I';
              } else if (player.itemInUse == 'H') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = 'M';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'H' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'M') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = 'H';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'I' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnLeftCenter, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'R') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = 'I';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'J' && toggleMatch) {
            Rect urnHitBox;
            urnHitBox.x = 12 * j;
            urnHitBox.y = 12 * i - player.viewportY;
            urnHitBox.height = 8;
            urnHitBox.width = 4;
            if (arduboy.collide(matchHitBox, urnHitBox)) {
              Sprites::drawSelfMasked(12 * j, 12 * i - player.viewportY, brokenUrnCenterRight, 0);
            }
            if (arduboy.collide(player.hitBox, urnHitBox)) {
              if (player.itemInUse == 'L') {
                placeItemInEmptySpot(i, j, player.floorNumber, 'X', 4, 8);
                player.itemInUse = 'C';
              } else {
                placeItemInEmptySpot(i, j, player.floorNumber, player.itemInUse, 4, 8);
                player.itemInUse = 'J';
              }
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'C' && toggleMatch) {
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
              player.itemInUse = 'C';
            }
          } else if (getSpotValue(i, j, player.floorNumber) == 'K' && toggleMatch) {
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
              player.itemInUse = 'K';
            }
          }
        }
      }
    }

    arduboy.fillRect(player.hitBox.x, player.hitBox.y, 3, 3);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y, 3, 3);


    //arduboy.drawRect(player.hitBox.x,player.hitBox.y,9,3);
    if (player.faint) {
      if (!player.cooldown) {
        player.cooldown = 1;
        now = millis();
      }
    } else {
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
          } else if (player.viewportY < 128) {
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
          } else if (player.viewportY > 0) {
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

      /*if (arduboy.justPressed(A_BUTTON)) {
        arduboy.invert(++color%2);   
      }*/
    }



    // game goes here
    tinyfont.setCursor(122, 0);
    tinyfont.print(player.lives);
    tinyfont.setCursor(122, 10);
    tinyfont.print(player.floorNumber + 1);
    tinyfont.setCursor(122, 20);
    tinyfont.print(matchesUsed);
    printItemInUse(player);


    if (arduboy.everyXFrames(10)) {
      twentyFrames = (twentyFrames + 1) % 2;
    }

    for (int i = 0; i < 2 + numberOfBats; i++) {
      moveMob(&mobs[i]);
      printMob(mobs[i], lights);
    }


    /*char d = spider.direction;


    tinyfont.setCursor(80, 24);
    if (d == 0) {
      tinyfont.print('r');
    } else if (d == 1) {
      tinyfont.print('l');
    } else if (d == 2) {
      tinyfont.print('d');
    } else if (d == 3) {
      tinyfont.print('u');
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
  while (flag) {
    if ((mob->roomNumber == 0 && rnd == LEFT) || (mob->roomNumber == 1 && rnd == RIGHT)
        || (mob->roomNumber == 4 && rnd == LEFT) || (mob->roomNumber == 5 && rnd == RIGHT)
        || (mob->roomNumber == 0 && rnd == UP && getSpotValue(0, 2, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 1 && rnd == UP && getSpotValue(0, 7, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 2 && rnd == LEFT && getSpotValue(7, 0, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 3 && rnd == RIGHT && getSpotValue(7, 9, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 4 && rnd == DOWN && getSpotValue(15, 2, mob->floorNumber) == ('X' || 'e'))
        || (mob->roomNumber == 5 && rnd == DOWN && getSpotValue(15, 7, mob->floorNumber) == ('X' || 'e'))
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

void removeDeadlocks() {
}

void isKeyReachable() {
}

void isEveryRoomReachable() {
}

void moveMob(Mob *mob) {
  int mobRoom = mobRoomNumber(*mob);

  if (playerAndEnemyInSameRoom(*mob) && (player.itemInUse != 'S' || mob->mobType == 1) && !player.faint) {
    toggleMatch = 0;

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
        }
        //if bat take item in use away
        if (mob->mobType == 2) {
          generateItem(player.itemInUse);
          player.itemInUse = 'X';
        }
      }
    }


  } else if (mob->roomCenterX == 1 && mob->roomCenterY == 1) {

    if (arduboy.everyXFrames(4)) {

      goToDirection(mob);
      //if mob hits the borderk aka change floor
      if (mob->hitBox.x <= 0 || mob->hitBox.x >= 110 || mob->hitBox.y <= 0 || mob->hitBox.y >= 184) {
        if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == 'u') {
          mob->floorNumber++;
        } else if (getSpotValue(mob->hitBox.y / 12, mob->hitBox.x / 12, mob->floorNumber) == 'd') {
          mob->floorNumber--;
        }
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        mob->previousDirection = mob->direction;
        getNextDirection(mob);
      }
      //if mob changes room
      if (mobRoomNumber(*mob) != mob->roomNumber) {
        mob->roomCenterX = 0;
        mob->roomCenterY = 0;
        mob->roomNumber = mobRoomNumber(*mob);
        mob->previousDirection = mob->direction;
        getNextDirection(mob);
      }
    }
  } else {
    if (arduboy.everyXFrames(4)) {

      //Move on X axis
      if (mobRoom % 2 == 0) {
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
      if (mobRoom == 0 || mobRoom == 1) {
        if (mob->hitBox.y < 38) {
          mob->hitBox.y++;
        } else if (mob->hitBox.y > 38) {
          mob->hitBox.y--;
        } else {
          mob->roomCenterY = 1;
        }
      }
      if (mobRoom == 2 || mobRoom == 3) {
        if (mob->hitBox.y < 86) {
          mob->hitBox.y++;
        } else if (mob->hitBox.y > 86) {
          mob->hitBox.y--;
        } else {
          mob->roomCenterY = 1;
        }
      }
      if (mobRoom == 4 || mobRoom == 5) {
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
