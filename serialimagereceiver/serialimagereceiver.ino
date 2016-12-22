#define HWSERIAL Serial2
#define LEDPIN 13
long bytes;
bool recvd;
#define ROWS 16
#define COLUMNS 32
const byte FRAME_HEADER_1 = 0xF0;
const byte FRAME_HEADER_2 = 0x00;
const byte FRAME_HEADER_3 = 0x0F;
int frame;
int row;

void setup() {
  Serial.begin(9600);
  Serial.println("/setup");

  HWSERIAL.begin(230400);
  HWSERIAL.setTimeout(1);
  pinMode(LEDPIN, OUTPUT); 

  
  digitalWrite(LEDPIN, HIGH);
  delay(500);
  digitalWrite(LEDPIN, LOW);
  bytes=0;
  frame = 0;
  row = 0;
  recvd = false;
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
  bytes = 0;
  while (row < ROWS) {
    byte b;
    while (!getNextByte(&b)) ;
    bytes += 1;
    if (!(bytes % COLUMNS)) {
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
}
