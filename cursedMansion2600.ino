// See: https://mlxxxp.github.io/documents/Arduino/libraries/Arduboy2/Doxygen/html/
#include <Arduboy2.h>
#include <Tinyfont.h>

constexpr uint8_t down[] PROGMEM
{
  // Width, Height
  12, 12,

  // Frame 0
  0x40, 0xC0, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0x40,
  0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00,
};

constexpr uint8_t up[] PROGMEM
{
  // Width, Height
  12, 12,

  // Frame 0
  0x20, 0x30, 0x38, 0xFC, 0xFE, 0xFF, 0xFF, 0xFE, 0xFC, 0x38, 0x30, 0x20,
  0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00,
};

constexpr uint8_t exitBlock[] PROGMEM
{
  // Width, Height
  12, 12,

  // Frame 0
  0x1E, 0x56, 0xD6, 0x52, 0x00, 0x52, 0xCC, 0x4C, 0x52, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x03, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr uint8_t blackBlock[] PROGMEM
{
  // Width, Height
  12, 12,

  // Frame 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

constexpr uint8_t completeCup[] PROGMEM
{
  // Width, Height
  6, 7,
  // Frame 0
  0x0F, 0x52, 0x7F, 0x7F, 0x52, 0x0F,
};

constexpr uint8_t ghost[] PROGMEM
{
  // Width, Height
  8, 7,
  // Frame 0
  0x03, 0x1E, 0x3D, 0x3F, 0x7D, 0x7E, 0x04, 0x08,

  // Frame 1
  0x08, 0x04, 0x7E, 0x7D, 0x3F, 0x3D, 0x1E, 0x03,

};

constexpr uint8_t bat[] PROGMEM
{
  // Width, Height
  8, 6,

  // Frame 0
  0x07, 0x1C, 0x38, 0x30, 0x30, 0x38, 0x1C, 0x07,

  // Frame 1
  0x38, 0x0E, 0x07, 0x03, 0x03, 0x07, 0x0E, 0x38,
};

constexpr uint8_t spiderSprite[] PROGMEM
{
  // Width, Height
  8, 8,

  // Frame 0
  0x88, 0x50, 0x57, 0x38, 0x38, 0x57, 0x50, 0x88,

  // Frame 1
  0x12, 0xCA, 0x2A, 0x1C, 0x1C, 0x2A, 0xCA, 0x12,
};

constexpr uint8_t key[] PROGMEM
{
  // Width, Height
  8, 3,

  // Frame 0
  0x06, 0x02, 0x06, 0x02, 0x02, 0x07, 0x05, 0x07,
};

constexpr uint8_t urn[] PROGMEM
{
  // Width, Height
  8, 8,

  // Frame 0
  0x38, 0x28, 0x7D, 0xFF, 0xFF, 0x7D, 0x28, 0x38,
};

constexpr uint8_t brokenUrnRight[] PROGMEM
{
  // Width, Height
  4, 4,

  // Frame 0
  0x08, 0x0F, 0x0E, 0x0A,
};

constexpr uint8_t brokenUrnLeft[] PROGMEM
{
  // Width, Height
  4, 4,

  // Frame 0
  0x0F, 0x0A, 0x0E, 0x02,
};

constexpr uint8_t brokenUrnCenter[] PROGMEM
{
  // Width, Height
  4, 8,

  // Frame 0
  0x41, 0xF7, 0xDF, 0x41,
};

constexpr uint8_t scepter[] PROGMEM
{
  // Width, Height
  8, 8,

  // Frame 0
  0x09, 0x06, 0x16, 0x09, 0x14, 0x20, 0x40, 0x80,
};

constexpr uint8_t cupIcon[] PROGMEM
{
  // Width, Height
  3, 7,

  // Frame 0
  0x0F, 0x52, 0x23,

  // Frame 1
  0x52, 0x7F, 0x52,

  // Frame 2
  0x23, 0x52, 0x0F,
};

struct Mob {
  Rect hitBox;
  int floorNumber;
};

struct Item {
  int floorNumber;
  int x;
  int y;
  char value;
};

//53 should be the number of all items placeable including doors, stairs, exit, scepter, key and urn
Item items[53];

void initializeItemArray(){
  for(int i=0;i<53;i++){
    items[i].value='X';
  }
}

int getFirstEmptyItemSpot(){
  for(int i=0;i<53;i++){
    if(items[i].value=='X'){
      return i;
    }
  }
  return -1;
}

Arduboy2 arduboy;
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, Arduboy2::width(), Arduboy2::height());

int lives = 9;
int viewportY = 0;
int toggleMatch = 0;
int twentyFrames = 0;
char itemInUse = "";

Mob spider;
Mob player;

//objects code:
//0 - empty space
//w - wall
//e - exit
//s - stairs chance
//p - passage
//u - stairs up
//d - stairs down
//o - open door
//l - locked door
//S - Scepter

constexpr char level[16][11] PROGMEM = { 
    {'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'p', 'p', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', 'w', 'p', 'w', 'w', 'w', 'w', 'p', 'w', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'s', '0', '0', '0', 'p', 'p', '0', '0', '0', 's', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', 'w', 'p', 'w', 'w', 'w', 'w', 'p', 'w', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'p', 'p', '0', '0', '0', 'w', 'w'},
    {'w', '0', '0', '0', 'w', 'w', '0', '0', '0', 'w', 'w'},
    {'w', 'w', 's', 'w', 'w', 'w', 'w', 's', 'w', 'w', 'w'}
};

Rect room[6];

char getSpotValue(int i, int j, int floorNumber){
  for(int k = 0;k<53;k++){
    if(items[k].y==i && items[k].x==j && items[k].floorNumber==0){
      return items[k].value;
    }
  }
  return 'X';
}

void generateExit() {
  int loop = 1;
  while (loop) {
    int i = random(0, 16);
    int j = random(0, 10);
    if (level[i][j] == 's') {
      Item exitDoor;
      exitDoor.y=i;
      exitDoor.x=j;
      exitDoor.floorNumber = 0;
      exitDoor.value = 'e';
      items[getFirstEmptyItemSpot()]=exitDoor;
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

void generateCenterUrn() {
  generateItem('C');
}

void generateKey() {
  generateItem('K');
}

void generateItem(char itemValue) {
  while (1) {
    int i = random(0, 16);
    int j = random(0, 10);
    int floorNumber = random(0, 4);
    if (level[i][j] == '0' && getSpotValue(i,j,floorNumber)=='X') {
      Item item;
      item.y=i;
      item.x=j;
      item.floorNumber = floorNumber;
      item.value = itemValue;
      items[getFirstEmptyItemSpot()]=item;
      return;
    }
  }
}

void generateStairs() {
  for (int flr = 0; flr < 3; flr++) {
    //generate stairs up (up to three)
    int loop = random(1, 3); //random(1,4);

    while (loop) {
      int i = random(0, 16);
      int j = random(0, 10);
      if (level[i][j] == 's') {
        Item doorUp;
        doorUp.y=i;
        doorUp.x=j;
        doorUp.floorNumber = flr;
        doorUp.value = 'u';
        items[getFirstEmptyItemSpot()]=doorUp;
        
        Item doorDown;
        doorDown.y=i;
        doorDown.x=j;
        doorDown.floorNumber = flr+1;
        doorDown.value = 'd';
        items[getFirstEmptyItemSpot()]=doorDown;
        loop--;
      }
    }
  }
}


void generateHouse(int lightsOn, int keyPresent) {

  generateExit();
  generateScepter();
  generateLeftUrn();
  generateRightUrn();
  generateCenterUrn();
  generateKey();
  generateStairs();
}


int playerAndEnemyInSameRoom() {
  int sameRoom = 0;
  Rect pl;
  pl.x = player.hitBox.x;
  pl.y = player.hitBox.y + viewportY;
  pl.width = player.hitBox.width;
  pl.height = player.hitBox.height;

  if (spider.floorNumber == player.floorNumber) {
    for (int i = 0; i < 6; i++) {
      if (arduboy.collide(room[i], spider.hitBox) && arduboy.collide(room[i], pl)) {
        return 1;
      }
    }
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

  spider.hitBox.x = random(0, 2) == 0 ? 26 : 86;
  spider.hitBox.y = random(12, 170);
  spider.hitBox.width = 8;
  spider.hitBox.height = 8;
  spider.floorNumber = random(0, 4);

  room[0].x = 0;
  room[0].y = 0;
  room[0].width = 60;
  room[0].height = 65;

  room[1].x = 60;
  room[1].y = 0;
  room[1].width = 60;
  room[1].height = 65;

  room[2].x = 0;
  room[2].y = 65;
  room[2].width = 60;
  room[2].height = 58;

  room[3].x = 60;
  room[3].y = 65;
  room[3].width = 60;
  room[3].height = 58;

  room[4].x = 0;
  room[4].y = 123;
  room[4].width = 60;
  room[4].height = 65;

  room[5].x = 60;
  room[5].y = 123;
  room[5].width = 60;
  room[5].height = 65;

}

int checkCollision(int direction) {
  Rect obstacle;
  Rect playerColl;

  // 1 - LEFT
  if (direction == 1) {
    playerColl.x = player.hitBox.x - 1;
    playerColl.y = player.hitBox.y + viewportY;
    playerColl.width = 1;
    playerColl.height = 3;
  }
  // 2 - UP
  else if (direction == 2) {
    playerColl.x = player.hitBox.x;
    playerColl.y = player.hitBox.y + viewportY - 1;
    playerColl.width = 8;
    playerColl.height = 1;
  }
  // 3 - RIGHT
  else if (direction == 3) {
    playerColl.x = player.hitBox.x + 8;
    playerColl.y = player.hitBox.y + viewportY;
    playerColl.width = 1;
    playerColl.height = 3;
  }
  // 4 - DOWN
  else if (direction == 4) {
    playerColl.x = player.hitBox.x;
    playerColl.y = player.hitBox.y + viewportY + 3;
    playerColl.width = 8;
    playerColl.height = 1;
  }

  //arduboy.fillRect(player.x,player.y-viewportY,player.width,player.height);

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
      else if (getSpotValue(i,j,player.floorNumber) == 'u') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (player.floorNumber < 3) {
            player.floorNumber++;
          }
        }
      }
      else if (getSpotValue(i,j,player.floorNumber) == 'd') {
        if (arduboy.collide(playerColl, obstacle)) {
          if (player.floorNumber > 0) {
            player.floorNumber--;
          }
        }
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

  //arduboy.print(player.hitBox.y+viewportY);

  //arduboy.drawRect(0,0,60,66-viewportY,WHITE);
  //arduboy.drawRect(0,67,60,66-viewportY,WHITE);
  //arduboy.setCursor(20,20);
  //arduboy.print(viewportY+player.hitBox.y);



  if (arduboy.everyXFrames(4) && toggleMatch) {
    arduboy.fillCircle(player.hitBox.x + 4, player.hitBox.y + 1, 10);
  }

  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 10; j++) {
      if ( player.hitBox.y + viewportY > (i * 12) - 64 && player.hitBox.y + viewportY < (i * 12) + 70) {
        if (level[i][j] == 'w') {
          //Sprites::drawOverwrite(12*j, 12*i-viewportY, blackBlock,0);
          arduboy.drawRect(12 * j, 12 * i - viewportY, 12, 12, WHITE);
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'e') {
          Sprites::drawOverwrite(12 * j, 12 * i - viewportY, exitBlock, 0);
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'u') {
          Sprites::drawOverwrite(12 * j, 12 * i - viewportY, up, 0);
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'd') {
          Sprites::drawOverwrite(12 * j, 12 * i - viewportY, down, 0);
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'S' && toggleMatch) {
          Rect scepterHitBox;
          scepterHitBox.x = 12 * j;
          scepterHitBox.y = 12 * i - viewportY;
          scepterHitBox.height = 8;
          scepterHitBox.width = 8;

          Rect matchHitBox;
          matchHitBox.x = player.hitBox.x - 3;
          matchHitBox.y = player.hitBox.y - 6;
          matchHitBox.height = 15;
          matchHitBox.width = 15;
          if (arduboy.collide(matchHitBox, scepterHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - viewportY, scepter, 0);
          }
          if (arduboy.collide(player.hitBox, scepterHitBox)) {
           // getSpotValue(i,j,player.floorNumber) = '0';
          }
          //arduboy.drawRect(player.hitBox.x-3,player.hitBox.y-6,15,15,WHITE);
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'R' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - viewportY;
          urnHitBox.height = 4;
          urnHitBox.width = 4;

          Rect matchHitBox;
          matchHitBox.x = player.hitBox.x - 3;
          matchHitBox.y = player.hitBox.y - 6;
          matchHitBox.height = 15;
          matchHitBox.width = 15;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - viewportY, brokenUrnRight, 0);
          }
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'L' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - viewportY;
          urnHitBox.height = 4;
          urnHitBox.width = 4;

          Rect matchHitBox;
          matchHitBox.x = player.hitBox.x - 3;
          matchHitBox.y = player.hitBox.y - 6;
          matchHitBox.height = 15;
          matchHitBox.width = 15;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - viewportY, brokenUrnLeft, 0);
          }
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'C' && toggleMatch) {
          Rect urnHitBox;
          urnHitBox.x = 12 * j;
          urnHitBox.y = 12 * i - viewportY;
          urnHitBox.height = 8;
          urnHitBox.width = 4;

          Rect matchHitBox;
          matchHitBox.x = player.hitBox.x - 3;
          matchHitBox.y = player.hitBox.y - 6;
          matchHitBox.height = 15;
          matchHitBox.width = 15;
          if (arduboy.collide(matchHitBox, urnHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - viewportY, brokenUrnCenter, 0);
          }
        }
        else if (getSpotValue(i,j,player.floorNumber) == 'K' && toggleMatch) {
          Rect keyHitBox;
          keyHitBox.x = 12 * j;
          keyHitBox.y = 12 * i - viewportY;
          keyHitBox.height = 3;
          keyHitBox.width = 8;

          Rect matchHitBox;
          matchHitBox.x = player.hitBox.x - 3;
          matchHitBox.y = player.hitBox.y - 6;
          matchHitBox.height = 15;
          matchHitBox.width = 15;
          if (arduboy.collide(matchHitBox, keyHitBox)) {
            Sprites::drawSelfMasked(12 * j, 12 * i - viewportY, key, 0);
          }
        }
      }
    }
  }

  arduboy.fillRect(player.hitBox.x, player.hitBox.y, 3, 3);
  arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y, 3, 3);


  //arduboy.drawRect(player.hitBox.x,player.hitBox.y,9,3);

  if (arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 2, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 7, player.hitBox.y + 1, 1, 1, BLACK);
    if (player.hitBox.x < 110 && !checkCollision(3)) {
      player.hitBox.x++;
    }

  }

  if (arduboy.pressed(LEFT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 5, player.hitBox.y + 1, 1, 1, BLACK);
    if (player.hitBox.x > 0 && !checkCollision(1)) {
      player.hitBox.x--;
    }
  }

  if (arduboy.pressed(DOWN_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 2, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 2, 1, 1, BLACK);

    if (!checkCollision(4)) {
      if (player.hitBox.y < 32 || (viewportY == 128 && player.hitBox.y < 60)) {
        player.hitBox.y++;
      }
      else if (viewportY < 128) {
        viewportY++;
      }
    }


  }

  if (arduboy.pressed(UP_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y, 1, 1, BLACK);

    if (!checkCollision(2)) {
      if (player.hitBox.y > 32 || (viewportY == 0 && player.hitBox.y > 0)) {
        player.hitBox.y--;
      }
      else if (viewportY > 0) {
        viewportY--;
      }
    }

  }


  //draw black eyes in middle
  if (!arduboy.pressed(UP_BUTTON) && !arduboy.pressed(DOWN_BUTTON) && !arduboy.pressed(LEFT_BUTTON) && !arduboy.pressed(RIGHT_BUTTON)) {
    arduboy.fillRect(player.hitBox.x + 1, player.hitBox.y + 1, 1, 1, BLACK);
    arduboy.fillRect(player.hitBox.x + 6, player.hitBox.y + 1, 1, 1, BLACK);
  }

  if (arduboy.justPressed(B_BUTTON)) {
    toggleMatch = (toggleMatch + 1) % 2;
  }

  // game goes here
  tinyfont.setCursor(122, 0);
  tinyfont.print(lives);
  tinyfont.setCursor(122, 10);
  tinyfont.print(player.floorNumber + 1);
  /*tinyfont.setCursor(122, 50);
  tinyfont.print(items.getCount());*/

  if (itemInUse == 'C') {
    Sprites::drawOverwrite(122, 20, completeCup, 0);
  } else if (itemInUse == 'S') {
    Sprites::drawOverwrite(122, 20, completeCup, 0);
  }
  else if (itemInUse == 'M') {
    Sprites::drawOverwrite(122, 20, cupIcon, 0);
  }




  if (arduboy.everyXFrames(10)) {
    twentyFrames = (twentyFrames + 1) % 2;

  }

  /*arduboy.print("player.hitBox.y");
    arduboy.print(player.hitBox.y);
    arduboy.print("spider.y");
    arduboy.print(spiderShape.y);*/



  if (playerAndEnemyInSameRoom()) {
    if (arduboy.everyXFrames(3)) {

      if (player.hitBox.x > spider.hitBox.x) {
        spider.hitBox.x++;
      }
      else if (player.hitBox.x < spider.hitBox.x) {
        spider.hitBox.x--;
      }

      if (player.hitBox.y + viewportY - 3 > spider.hitBox.y) {
        spider.hitBox.y = spider.hitBox.y + 1;
      }
      else if (player.hitBox.y + viewportY - 3 < spider.hitBox.y) {
        spider.hitBox.y = spider.hitBox.y - 1;

      }
    }
  }
  else {
    if (arduboy.everyXFrames(5)) {
      if (abs(spider.hitBox.x - 26) < abs(spider.hitBox.x - 86)) {
        if (spider.hitBox.x < 26) {
          spider.hitBox.x++;
        }
        else if (spider.hitBox.x > 26) {
          spider.hitBox.x--;
        }

      }
      else {
        if (spider.hitBox.x < 86) {
          spider.hitBox.x++;
        }
        else if (spider.hitBox.x > 86) {
          spider.hitBox.x--;
        }
      }
    }
  }

  if (player.floorNumber == spider.floorNumber) {
    Sprites::drawSelfMasked(spider.hitBox.x, spider.hitBox.y - viewportY, spiderSprite, twentyFrames);
  }

  arduboy.display();
}
