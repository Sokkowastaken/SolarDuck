// File: SerialCommunicationHandler.ino

#define BUFFER_SIZE 100 // Max size of buffer
byte buf[BUFFER_SIZE];  // Buffer to store incoming data

// Timeout in milliseconds for receiving data
const unsigned long TIMEOUT = 500; 

void setup() {
  Serial.begin(9600);   // Initialize Serial Monitor
  Serial1.begin(9600);  // Initialize Serial1 for device communication

  Serial.println("Arduino Mega 2560 Serial Communication Started");
}

void loop() {
  // Send the first command
  byte input1[] = {0x3F, 0x23, 0x7E, 0x34, 0x41, 0x7E, 0x32, 0x59, 0x31, 0x35, 0x30, 0x30, 0x23, 0x3F};
  Serial1.write(input1, sizeof(input1));
  delay(100); // Wait for a response

  // Read the response
  readResponse();

  delay(1000); // Wait before sending the next command
  
  // Send the second command
  byte input2[] = {0x3F, 0x23, 0x7E, 0x34, 0x42, 0x7E, 0x23, 0x3F};
  Serial1.write(input2, sizeof(input2));
  delay(100); // Wait for a response

  // Read the response
  readResponse();

  delay(2000); // Cycle delay
}

void readResponse() {
  unsigned long startTime = millis(); // Start a timer
  int index = 0; // Buffer index

  // Clear the buffer
  memset(buf, 0, BUFFER_SIZE);

  Serial.println("Reading response...");
  
  while (millis() - startTime < TIMEOUT) {
    if (Serial1.available() > 0) {
      byte incomingByte = Serial1.read();

      // Prevent buffer overflow
      if (index < BUFFER_SIZE) {
        buf[index++] = incomingByte;
      }
    }
  }

  // Print the received data
  Serial.print("Received Data: ");
  for (int i = 0; i < index; i++) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
