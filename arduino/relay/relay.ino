const int relay1 = 2;
const int relay2 = 3;
const int relay3 = 4;

void setup() {
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  // Initialize all relays as OFF (HIGH = off for active-low relays)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
}

void loop() {
  if (Serial.available()) {
    char state[4];  // 3 characters + null terminator
    int len = Serial.readBytesUntil('\n', state, 4);
    if (len == 3) {
      digitalWrite(relay1, state[0] == '0' ? HIGH : LOW);
      digitalWrite(relay2, state[1] == '0' ? HIGH : LOW);
      digitalWrite(relay3, state[2] == '0' ? HIGH : LOW);
    }
  }
}