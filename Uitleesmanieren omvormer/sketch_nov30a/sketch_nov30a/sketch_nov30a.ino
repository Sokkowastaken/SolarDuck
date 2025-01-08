#define BUFFER_SIZE 100 // Max size of buffer
byte buff[BUFFER_SIZE]; // Buffer to store incoming data

// Timeout in milliseconds for receiving data
const unsigned long TIMEOUT = 500;

void setup() {
  Serial.begin(9600);   // Initialize Serial Monitor
  Serial1.begin(9600);  // Initialize Serial1 for device communication

  Serial.println("Serial Communication Started");
}

void loop() {
  // 1st Command: Read Command for 1500ms periodic return interval
  byte input1[] = {0x3F, 0x23, 0x7E, 0x34, 0x41, 0x7E, 0x32, 0x59, 0x31, 0x35, 0x30, 0x30, 0x23, 0x3F};
  Serial.println("Sending Read Command...");
  Serial1.write(input1, sizeof(input1));
  if (readResponse()) {
    Serial.println("Read Command Response Received.");
  } else {
    Serial.println("Timeout: No response to Read Command.");
  }

  delay(1000); // Wait before next command

  // 2nd Command: Start Command
  byte input2[] = {0x3F, 0x23, 0x7E, 0x34, 0x42, 0x7E, 0x23, 0x3F};
  Serial.println("Sending Start Command...");
  Serial1.write(input2, sizeof(input2));
  if (readResponse()) {
    Serial.println("Start Command Response Received.");
  } else {
    Serial.println("Timeout: No response to Start Command.");
  }

  delay(2000); // Wait before the next cycle
}

bool readResponse() {
  unsigned long startTime = millis();
  int index = 0;
  memset(buff, 0, BUFFER_SIZE);

  Serial.println("Reading Growatt 750-S Response...");

  while (millis() - startTime < TIMEOUT) {
    if (Serial1.available() > 0) {
      byte incomingByte = Serial1.read();
      // Prevent buffer overflow
      if (index < BUFFER_SIZE) {
        buff[index++] = incomingByte;
      } else {
        Serial.println("Warning: Buffer Overflow!");
        break;
      }
      // Reset timeout after receiving data (optional)
      startTime = millis();
    }
  }

  // Check if we received data
  if (index == 0) {
    return false; // No data received within the timeout
  }

  // Print the received data
  Serial.print("Received Data: ");
  for (int i = 0; i < index; i++) {
    Serial.print(buff[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  return true;
}
