#include <FastLED.h>

#define HWSERIAL Serial2
#define LEDPIN 13
int pixels;
bool recvd;
#define ROWS 16
#define COLUMNS 32

#define NUM_LEDS (COLUMNS * ROWS)
CRGB leds_plus_safety_pixel[ NUM_LEDS];
CRGB* const leds( leds_plus_safety_pixel);

const byte FRAME_HEADER_1 = 0xF0;
const byte FRAME_HEADER_2 = 0x00;
const byte FRAME_HEADER_3 = 0x0F;
int frame;
int row;


#define LED_PIN  3
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
#define BRIGHTNESS 64

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
    byte b;
    byte colors[3];
    for (int colorIdx = 0; colorIdx < 3; colorIdx++)
      while (!getNextByte(&(colors[colorIdx]))) ;
    pixels += 1;
    leds[row*COLUMNS+pixels] = CRGB( colors[0], colors[1], colors[2]);
    if (!(pixels % COLUMNS)) {
      row += 1;
    }
  }
  Serial.print("END OF FRAME: ");
  Serial.println(frame++);
  row = 0;
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
  return true;
}

void loop() {
  digitalWrite(LEDPIN, LOW);
  while (!syncToFrame()) ;
  digitalWrite(LEDPIN, HIGH);
  getFrame();
  digitalWrite(LEDPIN, LOW);
  FastLED.show();
}
