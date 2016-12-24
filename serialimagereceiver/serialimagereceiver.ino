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

const byte FRAME_HEADER_1 = 0xF0;
const byte FRAME_HEADER_2 = 0x00;
const byte FRAME_HEADER_3 = 0x0F;
unsigned long frame;
int row;


#define LED_PIN  3
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define BRIGHTNESS 32

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

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
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
    /*
    Serial.print("getting pixel r=");
    Serial.print(row);
    Serial.print(",c=");
    Serial.print(pixels);
    */
    for (int colorIdx = 0; colorIdx < 3; colorIdx++) {
      while (!getNextByte(&(colors[colorIdx])));
      //Serial.print(".");
    }
    //Serial.println();
    pixels += 1;
    leds[getPixelForCoord(pixels,row)] = CRGB( colors[0], colors[1], colors[2]);
    if (!(pixels % COLUMNS)) {
      row += 1;
      pixels = 0;
    }
  }
  Serial.print("END OF FRAME: ");
  Serial.println(frame++);
  if (frame > 1000) {
    Serial.println("Frame counter reset");
    frame = 0;
  }
  row = 0;
}

int getPixelForCoord(int x, int y) {
  if (y < PANEL_HEIGHT)
    return coordForPanel1(x, y);
  else
    return coordForPanel2(x, y);
}

int coordForPanel1(int x, int y) {
  if (x % 2)
    return countUpPanel1(x, y);
  else
    return countDownPanel1(x, y);
}
  
int coordForPanel2(int x, int y) {
  if (x % 2)
    return countUpPanel2(x, y);
  else
    return countDownPanel2(x, y);
}

int countDownPanel1(int x, int y) {
  return x * PANEL_HEIGHT + y;
}

int countUpPanel1(int x, int y) {
  return x * PANEL_HEIGHT + (PANEL_HEIGHT - 1 - y);
}

int countDownPanel2(int x, int y) {
  return (PANEL_WIDTH * PANEL_HEIGHT - 1)
    - (PANEL_HEIGHT * (x+1))
    + (y - PANEL_HEIGHT);
}

int countUpPanel2(int x, int y) {
  return ((PANEL_HEIGHT * PANEL_WIDTH) - 1)
    + (x * PANEL_HEIGHT)
    - (y - PANEL_HEIGHT);
}

bool syncToFrame() {
  byte b = ' ';
  while ((!getNextByte(&b)) || b != FRAME_HEADER_1) ;

  while (!getNextByte(&b)) ;
  if (b != FRAME_HEADER_2) {
    return false;
  }
  while (!getNextByte(&b)) ;
  if (b != FRAME_HEADER_3) {
    return false;
  }
  Serial.println("sync");
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
