#include <FastLED.h>

#define PANEL_HEIGHT 8
#define PANEL_WIDTH 32

#define HWSERIAL Serial2
#define LEDPIN 13
int pixels;
bool recvd;
const int ROWS = PANEL_HEIGHT * 2;
const int  COLUMNS = PANEL_WIDTH;

#define NUM_LEDS (COLUMNS * ROWS)
CRGB leds_plus_safety_pixel[ NUM_LEDS];
CRGB* const leds( leds_plus_safety_pixel);

const byte FRAME_HEADER_1 = 0x01;
const byte FRAME_HEADER_2 = 0x02;
const byte FRAME_HEADER_3 = 0x03;
unsigned long frame;
int row;


#define LED_PIN  3
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define BRIGHTNESS 32
//#define LIGHT_TEMPERATURE WarmFluorescent
//#define LIGHT_TEMPERATURE Halogen
//#define LIGHT_TEMPERATURE FullSpectrumFluorescent
#define LIGHT_TEMPERATURE CoolWhiteFluorescent

void setup() {
  Serial.begin(9600);
  Serial.println("/setup");

  HWSERIAL.begin(230400);
  HWSERIAL.setTimeout(1);
  pinMode(LEDPIN, OUTPUT); 

  
  digitalWrite(LEDPIN, HIGH);
  delay(500);
  digitalWrite(LEDPIN, LOW);
  pixels = 0;
  frame = 0;
  row = 0;
  recvd = false;

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalPixelString);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setTemperature(LIGHT_TEMPERATURE);
  FastLED.show();

  Serial.println("/setup");
}

bool getNextByte(byte *b) {
  bool recvd = false;
  if (HWSERIAL.available()) {
    recvd = true;
    *b = HWSERIAL.read();
  }
  return recvd;
}

void getFrame() {
  pixels = 0;
  while (row < ROWS) {
    byte colors[3];
    for (int colorIdx = 0; colorIdx < 3; colorIdx++) {
      while (!getNextByte(&(colors[colorIdx])));
    }
    // Pixel data from the imageprocessor is GRB so rearrange the colors here
    leds[getPixelForCoord(pixels, row)] = CRGB( colors[1], colors[0], colors[2]);
    pixels += 1;

    if (!(pixels % COLUMNS)) {
      row += 1;
      pixels = 0;
    }
  }
  #ifdef _DEBUG
  Serial.print("END OF FRAME: ");
  Serial.println(frame++);
  #endif
  if (frame > 1000) {
    #ifdef _DEBUG
    Serial.println("Frame counter reset");
    #endif
    frame = 0;
  }
  row = 0;
}

int getPixelForCoord(const int x, const int y) {
  if (y < PANEL_HEIGHT)
    return coordForPanel1(x, y);
  else
    return coordForPanel2(x, y);
}

int coordForPanel1(const int x,const int y) {
  if (x % 2)
    return countUpPanel1(x, y);
  else
    return countDownPanel1(x, y);
}
  
int coordForPanel2(const int x, const int y) {
  if (x % 2)
    return countUpPanel2(x, y);
  else
    return countDownPanel2(x, y);
}

int countDownPanel1(const int x, const int y) {
  return x * PANEL_HEIGHT + y;
}

int countUpPanel1(const int x, const int y) {
  return x * PANEL_HEIGHT + (PANEL_HEIGHT - 1 - y);
}

int countDownPanel2(const int x, const int y) {
  return (PANEL_WIDTH * PANEL_HEIGHT - 1)
    + (PANEL_HEIGHT * (PANEL_WIDTH - x))
    - (PANEL_HEIGHT - (y - PANEL_HEIGHT))
    + 1;
}

int countUpPanel2(const int x, const int y) {
  return (PANEL_WIDTH * PANEL_HEIGHT - 1)
    + (PANEL_HEIGHT * (PANEL_WIDTH - x))
    - (y - PANEL_HEIGHT);
}

void waitForByte(byte syncByte) {
  int f=0;
  byte b = ' ';
  while (b != syncByte) {
    while (!getNextByte(&b))
      ;
    f += 1;
  }
  #ifdef _DEBUG
  Serial.print("Footer: ");
  Serial.println(f-1);
  #endif
}

bool syncToFrame() {
  byte b;
  waitForByte(FRAME_HEADER_1);
  while (!getNextByte(&b))
    ;
  if (b != FRAME_HEADER_2) {
    Serial.println("!sync2");
    return false;
  }
  while (!getNextByte(&b))
   ;
  if (b != FRAME_HEADER_3) {
    Serial.println("!sync3");
    return false;
  }
  #ifdef _DEBUG
  Serial.println("sync");
  #endif
  return true;
}

void loop() {
  digitalWrite(LEDPIN, LOW);
  while (!syncToFrame()) Serial.println("Failed to Sync");
  digitalWrite(LEDPIN, HIGH);
  getFrame();
  digitalWrite(LEDPIN, LOW);
  FastLED.show();
}
