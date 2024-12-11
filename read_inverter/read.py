import serial
import time

#windows = "COM3" linux = /dev/usbttyx
PORT = 'COM3'
BAUDRATE = 9600  

# Open the serial port
ser = serial.Serial(
    port=PORT,
    baudrate=BAUDRATE,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

inverter_command = b'\x3F\x23\x7E\x34\x41\x7E\x32\x59\x31\x30\x30\x23\x3F'
inverter_command_2 = b'\x3F\x23\x7E\x34\x42\x7E\x23\x3F'
if ser.is_open:
    print(f"Serial port {PORT} opened successfully")

def send_command(command):
    ser.write(command)
    print(f"Sent: {command}")

def read_response():
    response = ser.read(100)
    print(f"Received: {response}\n")
    return response


def main():
    print("listening for data")
    while True:
        read_response()
        #sleeping for 1.5 seconds sleeptime based on the value filled in by inverter command.
        time.sleep(1.5) 
    
if __name__ == "__main__":
    send_command(inverter_command)
    time.sleep(1.5)
    read_response()
    time.sleep(0.5)
    send_command(inverter_command_2)
    time.sleep(1.5)
    main()