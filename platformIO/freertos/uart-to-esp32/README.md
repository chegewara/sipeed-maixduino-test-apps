
Simple demo is echoing data sent from esp32 to k210. Flash to esp32 this code:

```
void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
}
```