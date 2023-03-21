constexpr uint8_t heart[] PROGMEM
{
	// Width, Height
	8, 8,
	
	// Frame 0
	0x18, 0x3C, 0x7C, 0xF8, 0x7C, 0x3C, 0x18, 0x00,
};

constexpr uint8_t keyIcon[] PROGMEM
{
  // Width, Height
  3, 8,

  // Frame 0
  0xFF, 0xA5, 0x87,
};


constexpr uint8_t scepterIcon[] PROGMEM
{
  // Width, Height
  6, 6,

  // Frame 0
  0x02, 0x1B, 0x04, 0x0A, 0x12, 0x20,
};

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
  12, 16,

  // Frame 0
  0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F,
  0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0x00, 0x0F,
};

constexpr uint8_t whiteBlock[] PROGMEM
{
  // Width, Height
  12, 12,

  // Frame 0
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
};

constexpr uint8_t ghostSprite[] PROGMEM
{
  // Width, Height
  8, 7,
  // Frame 0
  0x03, 0x1E, 0x3D, 0x3F, 0x7D, 0x7E, 0x04, 0x08,

  // Frame 1
  0x08, 0x04, 0x7E, 0x7D, 0x3F, 0x3D, 0x1E, 0x03,

};

constexpr uint8_t batSprite[] PROGMEM
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

constexpr uint8_t brokenUrnLeftCenter[] PROGMEM
{
  // Width, Height
  6, 8,

  // Frame 0
  0x1C, 0x14, 0xBE, 0xFF, 0xFB, 0x82,
};

constexpr uint8_t brokenUrnCenterRight[] PROGMEM
{
  // Width, Height
  6, 8,

  // Frame 1
  0x82, 0xEF, 0xFF, 0xBE, 0x14, 0x1C,
};

constexpr uint8_t brokenUrnLeftRight[] PROGMEM
{
  // Width, Height
  8, 4,

  // Frame 0
  0x07, 0x05, 0x0F, 0x04, 0x01, 0x0F, 0x05, 0x07,
};

constexpr uint8_t scepter[] PROGMEM
{
  // Width, Height
  8, 8,

  // Frame 0
  0x09, 0x06, 0x16, 0x09, 0x14, 0x20, 0x40, 0x80,
};

constexpr uint8_t urnIcon[] PROGMEM
{
  // Width, Height
  6, 7,

  // Complete Icon
  0x0F, 0x52, 0x7F, 0x7F, 0x52, 0x0F,

  // Right Icon
  0x00, 0x00, 0x00, 0x08, 0x12, 0x0F,

  // Left Icon
  0x0F, 0x12, 0x02, 0x00, 0x00, 0x00,

  // Center Icon
  0x00, 0x40, 0x7D, 0x77, 0x40, 0x00,

  // Center Right Icon
  0x00, 0x40, 0x7D, 0x77, 0x52, 0x0F,

  // Left Center Icon
  0x0F, 0x52, 0x7F, 0x77, 0x40, 0x00,

  // Left Right Icon
  0x0F, 0x12, 0x02, 0x08, 0x12, 0x0F,
};

const int MAX_NUMBER_OF_ITEMS_IN_HOUSE = 44;
const int RIGHT = 0;
const int LEFT = 1;
const int UP = 2;
const int DOWN = 3;
const char EMPTY_SPACE = '0';
const char WALL ='w';
const char EXIT ='e';
const char STAIRS ='s';
const char PASSAGE ='P';
const char CLOSED_DOOR ='c';
const char OPEN_DOOR ='o';
const char LOCKED_DOOR ='l';
const char STAIRS_UP ='u';
const char STAIRS_DOWN ='d';
const char SCEPTER ='S';
const char KEY ='K';
const char COMPLETE_URN ='C';
const char LEFT_PART_OF_URN ='L';
const char RIGHT_PART_OF_URN ='R';
const char MIDDLE_PART_OF_URN ='M';
const char LEFT_AND_MIDDLE_PART_OF_URN ='I';
const char LEFT_AND_RIGHT_PART_OF_URN ='H';
const char MIDDLE_AND_RIGHT_PART_OF_URN ='J';
const char NO_ITEM ='X';

// 0 - white space
// w - wall
// p/P - passage
// s - stairs

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

const uint16_t FULL_URN_SOUND[] = {
  NOTE_B5,75,NOTE_REST,30,NOTE_B5,75,NOTE_REST,30,NOTE_B5,75,NOTE_REST,30,NOTE_B5,75,NOTE_REST,30,TONES_END
};

const uint16_t GAME_WIN[] = {
  NOTE_C4,280,NOTE_CS4,280,NOTE_C4,280,NOTE_A3,280,NOTE_C4,280,NOTE_CS4,280,NOTE_C4,280,NOTE_A3,280,NOTE_C4,280,NOTE_CS4,280,NOTE_C4,280,NOTE_A3,280,NOTE_C4,280,NOTE_CS4,280,NOTE_C4,280,NOTE_A3,280,TONES_END
};

const uint16_t GAME_OVER[] = {
  NOTE_B3,50,NOTE_REST,100,NOTE_B3,50,NOTE_REST,100,NOTE_B4,150,NOTE_REST,100,NOTE_B4,150,NOTE_REST,100,NOTE_D5,150,NOTE_FS5,150,NOTE_B3,50,NOTE_REST,100,NOTE_B3,50,NOTE_REST,100,NOTE_B4,150,NOTE_REST,100,NOTE_D5,150,NOTE_G5,150,NOTE_REST,100,NOTE_FS4,50,NOTE_REST,100,NOTE_G5,150,NOTE_FS5,150,NOTE_F5,150,NOTE_FS5,150,NOTE_G5,150,NOTE_REST,100,NOTE_G5,150,NOTE_AS4,150,NOTE_REST,100,NOTE_AS3,50,NOTE_REST,100,NOTE_B4,150,TONES_END
};


