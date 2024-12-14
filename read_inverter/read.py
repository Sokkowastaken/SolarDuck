import serial
import time

#windows = "COMx" linux = /dev/usbttyx
PORT = 'COM6'
BAUDRATE = 9600  

# Open the serial port
ser = serial.Serial(
    port=PORT,
    baudrate=BAUDRATE,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1 
)
#CHECK INVERTER ADDRESS x7E
inverter_command = b'\x3F\x23\x0B\x34\x41\x0B\x32\x59\x31\x35\x30\x30\x23\x3F'
inverter_command_2 = b'\x3F\x23\x0B\x34\x42\x0B\x23\x3F'

if ser.is_open:
    print(f"Serial port {PORT} opened successfully")

def send_command(command):
    ser.write(command)
    print(f"Sent: {command}")

def read_response():
    response = ser.read(30)
    print(f"Received: {response}")
    return response


def main():
    print("listening for data")
    while True:
        read_response()
        time.sleep(1.5) 
    
if __name__ == "__main__":
    try:
        send_command(inverter_command)
        time.sleep(1)
        read_response()
        send_command(inverter_command_2)
        time.sleep(1.5)
        main()
    
    except Exception as e:
        print(f"Error: {e}")


# import serial
# import time

# ser = serial.Serial('COM3', 9600, timeout=1)
# time.sleep(2)  

# # File to save the data
# output_file = "arduino_output.txt"

# with open(output_file, "w") as file:
#     print("Logging data... Press Ctrl+C to stop.")
#     try:
#         while True:
#             if ser.in_waiting > 0:
#                 line = ser.readline().decode('utf-8').strip()
#                 print(line)  # Print to console
#                 file.write(line + "\n")  # Write to file
#     except KeyboardInterrupt:
#         print("\nLogging stopped.")
#     finally:
#         ser.close()
