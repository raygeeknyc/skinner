#define HWSERIAL Serial2
#define LEDPIN 13
long bytes;
bool recvd;
#define ROWS 16
#define COLUMNS 32
int frame;
int row;

void setup() {
  Serial.begin(9600);
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

void addByteToFrame(byte b) {
  if ((!(bytes % COLUMNS)) && bytes) {
    Serial.print("Byte: ");
    Serial.print(bytes);
    Serial.print(" END OF ROW: ");
    Serial.println(row++);
  }
  if ((!(row % ROWS)) && row) {
    Serial.print("END OF FRAME: ");
    Serial.println(frame++);
    row = 0;
  }
}

void loop() {
  digitalWrite(LEDPIN, LOW);
  byte b = ' ';
  if (getNextByte(&b)) {
    bytes += 1;
    addByteToFrame(b);
  }
  if (bytes > 0) {
    digitalWrite(LEDPIN, HIGH);
  }
}
