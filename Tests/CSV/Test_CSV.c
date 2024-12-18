#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define DATA_SIZE 30
#define BUFFER_SIZE 1024

int write_header = 0;
const char *get_fault(unsigned char fault_code);
const char *get_inverter_status(unsigned char inverter_status);
void print_inverter_error(unsigned char inverter_status, unsigned short fault_code)
{
    const char *status_message = get_inverter_status(inverter_status);
    if (inverter_status == 3) // 3: Fault
    {
        const char *fault_message = get_fault(fault_code);
        printf("ERROR: Inverter Status: Fault | Fault Code: %d | Message: %s\n", fault_code, fault_message);
    }
    else if (inverter_status == 1)
    {
        printf("Inverter Status: Normal\n");
    }
    else if (inverter_status == 0)
    {
        printf("Inverter Status: Waiting\n");
    }
}

const char *get_fault(unsigned char fault_code)
{
    switch (fault_code)
    {
    case 24:
        return "Auto Test Failed";
    case 25:
        return "No AC Connection";
    case 26:
        return "PV Isolation Low";
    case 27:
        return "Residual I High";
    case 28:
        return "Output High DCI";
    case 29:
        return "PV Voltage High";
    case 30:
        return "AC V Outrange";
    case 31:
        return "AC F Outrange";
    case 32:
        return "Module Hot";
    default:
        if (fault_code >= 1 && fault_code <= 23)
        {
            static char error_message[20];
            sprintf(error_message, "Error: %d", 99 + fault_code);
            return error_message;
        }
        return "Unknown Fault";
    }
}

const char *get_inverter_status(unsigned char status)
{
    switch (status)
    {
    case 0:
        return "Waiting";
    case 1:
        return "Normal";
    case 3:
        return "Fault";
    default:
        return "Unknown Status";
    }
}

unsigned short combine_bytes(unsigned char high_byte, unsigned char low_byte)
{
    return (high_byte << 8) | low_byte;
}

bool file_has_header(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return false;
    }

    char buffer[BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        if (strstr(buffer, "PV1 Voltage (V)") != NULL)
        {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

// Function to write to csv file
int write_csv(const char *filename, const char *inputfile)
{
    write_header = !file_has_header(filename);

    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return -1;
    }

    if (write_header)
    {
        fprintf(file, "PV1 Voltage (V);PV2 Voltage (V);Grid Voltage (V);Grid Frequency (Hz);Output Power (W);Temperature (C);inverter_status;inverter_fault_code;Energy Today (kWh);Energy Total (kWh);total time (s)\n");
        write_header = 0;
    }

    FILE *input_file = fopen(inputfile, "r");
    if (input_file == NULL)
    {
        perror("Error opening input file for reading inverter input");
        return -1;
    }

    unsigned char data[DATA_SIZE];
    size_t bytes_read;

    while (1)
    {
        for (int i = 0; i < DATA_SIZE; i++)
        {
            if (fscanf(input_file, "%x", &data[i]) != 1)
            {
                if (i == 0)
                {
                    fclose(input_file);
                    fclose(file);
                    return 0;
                }

                fprintf(stderr, "Error: Data is not complete\n");
                fclose(input_file);
                fclose(file);
                break;
            }
        }

        // Combine data into 16-bit values
        double pv1_voltage = combine_bytes(data[0], data[1]) / 10.0;                                                  // D1D2
        double pv2_voltage = combine_bytes(data[4], data[5]) / 10.0;                                                  // D5D6
        double grid_voltage = combine_bytes(data[6], data[7]) / 10.0;                                                 // D7D8
        double grid_frequency = combine_bytes(data[8], data[9]) / 100.0;                                              // D9D10
        double output_power = combine_bytes(data[10], data[11]) / 10.0;                                               // D11D12
        double temperature = combine_bytes(data[12], data[13]) / 10.0;                                                // D13D14
        unsigned char inverter_status = data[14];                                                                     // D15
        unsigned char fault_code = data[15];                                                                          // D16D17
        double energy_today = combine_bytes(data[20], data[21]) / 10.0;                                               // D21D22
        double energy_total = (combine_bytes(data[22], data[23]) * 65536 + combine_bytes(data[24], data[25])) / 10.0; // D23D24D25D26
        unsigned int total_time = combine_bytes(data[26], data[27]) + combine_bytes(data[28], data[29]);              // D27D28D29D30

        fprintf(file, "%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%d;%d;%.1f;%.1f;%d;\n",
                pv1_voltage, pv2_voltage, grid_voltage, grid_frequency,
                output_power, temperature, inverter_status, fault_code, energy_today, energy_total, total_time);
    }
    fclose(input_file);
    fclose(file);
    return 0;
}
// Function to read data from csv file
int read_csv(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file for reading");
        return -1;
    }

    char buffer[BUFFER_SIZE];

    // Read line-by-line from the CSV file
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {

        printf("%s", buffer);
    }

    if (ferror(file))
    {
        perror("Error reading from file");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

void test_csv()
{
    // Test function for read and write actions.
    const char *input = "input.txt";
    const char *bad_input = "bad_input.txt";
    const char *valid_filename = "test.csv";
    const char *bad_input_filename = "bad_data.csv";
    const char *invalid_filename = "/fake_path/test.csv";

    printf("\n=== Testing Success Scenario ===\n");
    if (write_csv(valid_filename, input) == 0)
    {
        printf("Writing succeeded. Verifying contents:\n");
        if (read_csv(valid_filename) != 0)
        {
            printf("Failed to read valid CSV file.\n");
        }
        printf("Reading data was succesful\n");
    }
    else
    {
        printf("Failed to write valid CSV file.\n");
    }

    printf("=== Testing Foutmelding/storing Scenario ===\n");
    // triggering all different errors.
    print_inverter_error(0, 0);
    print_inverter_error(1, 0);
    print_inverter_error(3, 22);
    print_inverter_error(3, 34);

    printf("\n=== Testing Failure Scenarios ===\n");

    // Test writing to an invalid location
    if (write_csv(invalid_filename, input) == 0)
    {
        printf("This will never print because the code will fail inside the function write_csv\n");
    }

    // Test reading a non-existent file
    if (read_csv(invalid_filename) == 0)
    {
        printf("This will never print because the code will fail inside the function read_csv\n");
    }
    // Test trying to read incomplete data.
    if (write_csv(bad_input_filename, bad_input) == 0)
    {
        printf("Failed to write data to the bad_csv, file empty\n");
    }
    printf("Test completed");
}

int main()
{
    // Run test
    test_csv();
    return 0;
}
