#define _TESTING

#define _DEBUG

#include <FastLED.h>

#define PANEL_HEIGHT 8
#define PANEL_WIDTH 32
const int PANEL_SIZE = PANEL_HEIGHT * PANEL_WIDTH;
const int PANEL_OFFSET = PANEL_SIZE - 1;

#define HWSERIAL Serial2
#define ACTIVITY_LED_PIN 13

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
#define FRAME_BRIGHTNESS_LENGTH 5

unsigned long frame;
int frame_brightness;

int row;

// If the frame header brightness is below this value, dim the display brightness
#define BRIGHTNESS_MEDIUM_THRESHOLD 80
#define BRIGHTNESS_BRIGHT_THRESHOLD 120
#define BRIGHTNESS_DARK 16
#define BRIGHTNESS_MEDIUM 48
#define BRIGHTNESS_BRIGHT 64
#define LED_SETTING_BLINK_DURATION_MS 1000
#define LIGHT_TEMPERATURE_JUMPER_PIN 14
#define BRIGHTNESS_JUMPER_PIN 16
#define DISPLAY_LED_PIN  3
#define COLOR_ORDER RGB
#define CHIPSET     WS2811
boolean brightness_switch;
boolean temperature_switch;

//#define LIGHT_TEMPERATURE WarmFluorescent
//#define LIGHT_TEMPERATURE Halogen
//#define LIGHT_TEMPERATURE FullSpectrumFluorescent
#define LIGHT_TEMPERATURE_WARM Tungsten100W
#define LIGHT_TEMPERATURE_COOL CoolWhiteFluorescent

void testCoordinateMapping(int x, int y) {
  Serial.print("logical coordinates (x,y):");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(": pixel:");
  Serial.println(getPixelForCoord(x,y));
}

void showTestCoordinates() {
  testCoordinateMapping(0, 0);
  testCoordinateMapping(0, 7);
  testCoordinateMapping(0, 8);
  testCoordinateMapping(0, 15);
  
  testCoordinateMapping(30, 0);
  testCoordinateMapping(31, 0);
  testCoordinateMapping(31, 7);
  testCoordinateMapping(30, 8);
  testCoordinateMapping(30, 15);
  testCoordinateMapping(31, 8);
  testCoordinateMapping(31, 15);
}

void setup() {
  Serial.begin(9600);
  #ifdef _DEBUG
  Serial.println("setup");
  #endif
  
  HWSERIAL.begin(230400);
  HWSERIAL.setTimeout(1);
  pinMode(ACTIVITY_LED_PIN, OUTPUT);
  pinMode(BRIGHTNESS_JUMPER_PIN, INPUT_PULLUP);
  pinMode(LIGHT_TEMPERATURE_JUMPER_PIN, INPUT_PULLUP);

  digitalWrite(ACTIVITY_LED_PIN, HIGH);
  delay(500);
  digitalWrite(ACTIVITY_LED_PIN, LOW);
  pixels = 0;
  frame = 0;
  row = 0;
  recvd = false;
  frame_brightness = 0;

  FastLED.addLeds<CHIPSET, DISPLAY_LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalPixelString);
  setLEDLighting();
  FastLED.show();

  #ifdef _DEBUG
  Serial.println("/setup");
  Serial.println("/setup");
  #endif

  #ifdef _TESTING
  Serial.println("Testing coordinate mapping");
  showTestCoordinates();
  showTestLEDPattern();
  #endif

}

void showTestLEDPattern() {
  Serial.println("Displaying test pattern");
  setLEDLighting();
  for (int x=0; x<COLUMNS; x++) {
    for (int y=0; y<ROWS; y++) {
      if ((x % 2) && (y % 2) )
       leds[getPixelForCoord(x, y)] = CRGB( 255, 0, 0);
       else
       leds[getPixelForCoord(x, y)] = CRGB( 255, 0, 0);
    }
  }  
  FastLED.show();
  Serial.println("Displayed test pattern");
  delay(5000);
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
    // Pixel data from the imageprocessor is RGB but even tho our FASTLED color order is RGB we need to rearrange them
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

int coordForPanel1(const int x, const int y) {
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
  return PANEL_OFFSET + (x * PANEL_HEIGHT) + (y - PANEL_HEIGHT + 1) ;
}

int countUpPanel2(const int x, const int y) {
  return PANEL_OFFSET + (x + 1) * PANEL_HEIGHT - (y - PANEL_HEIGHT);
}

void waitForByte(byte syncByte) {
  int f = 0;
  byte b = ' ';
  while (b != syncByte) {
    while (!getNextByte(&b))
      ;
    f += 1;
  }
#ifdef _DEBUG
  Serial.print("Footer: ");
  Serial.println(f - 1);
#endif
}

// This returns true if a frame header was found
// and sets the brightness global
bool syncToFrame() {
  byte b;
  byte header_brightness[FRAME_BRIGHTNESS_LENGTH + 1];
  header_brightness[FRAME_BRIGHTNESS_LENGTH] = '\0';
  waitForByte(FRAME_HEADER_1);
  while (!getNextByte(&b))
    ;
  if (b != FRAME_HEADER_2) {
    Serial.println("!sync2");
    return false;
  }

  for (int i = 0; i < FRAME_BRIGHTNESS_LENGTH; i++) {
    while (!getNextByte(&b))
      ;
    header_brightness[i] = b;
  }
#ifdef _DEBUG
  Serial.print("brightness");
  Serial.println((char*)header_brightness);
#endif

  while (!getNextByte(&b))
    ;
  if (b != FRAME_HEADER_3) {
    Serial.println("!sync3");
    return false;
  }
  String brightnessString = (char*)header_brightness;
  frame_brightness = brightnessString.toInt();
#ifdef _DEBUG
  Serial.println("sync");
#endif
  return true;
}

boolean isLightingSettingChanged() {
  boolean changed = false;

  if (brightness_switch != digitalRead(BRIGHTNESS_JUMPER_PIN)) {
    brightness_switch = digitalRead(BRIGHTNESS_JUMPER_PIN);
    changed = true;
  }
  if (temperature_switch != digitalRead(LIGHT_TEMPERATURE_JUMPER_PIN)) {
    temperature_switch = digitalRead(LIGHT_TEMPERATURE_JUMPER_PIN);
    changed = true;
  }

  return changed;
}

void setLEDLighting() {
  digitalWrite(ACTIVITY_LED_PIN, HIGH);
  delay(LED_SETTING_BLINK_DURATION_MS);
  digitalWrite(ACTIVITY_LED_PIN, LOW);
  if (digitalRead(BRIGHTNESS_JUMPER_PIN) == LOW) {
    FastLED.setBrightness(BRIGHTNESS_MEDIUM);
  } else if (frame_brightness < BRIGHTNESS_MEDIUM_THRESHOLD) {
    FastLED.setBrightness(BRIGHTNESS_DARK);
  } else if (frame_brightness < BRIGHTNESS_BRIGHT_THRESHOLD) {
    FastLED.setBrightness(BRIGHTNESS_MEDIUM);
  } else {
    FastLED.setBrightness(BRIGHTNESS_BRIGHT);
  }
  FastLED.setTemperature((digitalRead(LIGHT_TEMPERATURE_JUMPER_PIN) == HIGH) ? LIGHT_TEMPERATURE_WARM : LIGHT_TEMPERATURE_COOL);
  FastLED.clear();
}

void loop() {
  digitalWrite(ACTIVITY_LED_PIN, LOW);
   while (!syncToFrame()) Serial.println("Failed to Sync");
  digitalWrite(ACTIVITY_LED_PIN, HIGH);
  if (isLightingSettingChanged()) {
    setLEDLighting();
  }
  getFrame();
  digitalWrite(ACTIVITY_LED_PIN, LOW);
  FastLED.show();
}
