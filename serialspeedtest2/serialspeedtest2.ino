#define HWSERIAL Serial2
#define LEDPIN 13
long bytes;
bool recvd;

void setup() {
  Serial.begin(9600);
  HWSERIAL.begin(230400);
  HWSERIAL.setTimeout(1);
  pinMode(LEDPIN, OUTPUT); 
  digitalWrite(LEDPIN, HIGH);
  delay(500);
  digitalWrite(LEDPIN, LOW);
  bytes=0;
  recvd = false;
}

void loop() {
  int incomingByte;
  digitalWrite(LEDPIN, LOW);
  byte type = ' ';
  if (HWSERIAL.available()) {
    recvd = true;
    digitalWrite(LEDPIN, HIGH);
    type = HWSERIAL.read();
    HWSERIAL.write(type);
    bytes+=1;
  }
  if (recvd) {
    Serial.println(type);
    Serial.print("bytes: ");
    Serial.println(bytes);
    recvd = false;
  }
}
