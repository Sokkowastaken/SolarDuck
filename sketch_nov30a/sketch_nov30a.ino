#define BUFFER_SIZE 100 // Max size of buffer
byte buf[BUFFER_SIZE];  // Buffer to store incoming data

const unsigned long TIMEOUT = 3000; 

void setup() {
  Serial.begin(9600);   // Initialize Serial Monitor
  Serial1.begin(9600);  // Initialize Serial1 for device communication

  Serial.println("Arduino Mega 2560 Serial Communication Started");
  byte input1[] = {0x3F, 0x23, 0x0B, 0x34, 0x41, 0x0B, 0x32, 0x59, 0x31, 0x30, 0x30, 0x30, 0x23, 0x3F};
  Serial1.write(input1, sizeof(input1));
  delay(100); // Wait for a response

  // Read the response
  readResponse();

  //delay(1500); // Wait before sending the next command
  // Send the second command
  byte input2[] = {0x3F, 0x23, 0x0B, 0x34, 0x41, 0x0B, 0x23, 0x3F};
  Serial1.write(input2, sizeof(input2));
}

void loop() {
  // Send the first command
  //not sure if address is correct
  //could use 0 because that is broadcast address?
  //com address 0x11 = 0x0B
  //
  
  delay(100); // Wait for a response
  // Read the response
  readResponse();

  delay(3000); // Cycle delay
}

void readResponse() {
  unsigned long startTime = millis();  // Start a timer
  int index = 0;  // Buffer index

  // Clear the buffer
  memset(buf, 0, BUFFER_SIZE);

  //Serial.println("Reading response...");

  // Read data as long as it's available or until the timeout is reached
  while (millis() - startTime < TIMEOUT) {
    while (Serial1.available() > 0) {
      byte incomingByte = Serial1.read();

      // Prevent buffer overflow
      if (index < BUFFER_SIZE) {
        buf[index++] = incomingByte;
      }
    }
  }

  Print the received data
  Serial.print("Received Data: ");
  for (int i = 0; i < index; i++) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
