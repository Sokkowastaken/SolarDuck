#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DATA_SIZE 30
#define BUFFER_SIZE 1024
#define NUM_VALUES (DATA_SIZE / 2)

int write_header = 0;
int Incomplete_data(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return -1;
    }
    else {
        write_header = 1;
    }

    if (write_header) {
        fprintf(file, "PV1 Voltage (V);PV2 Voltage (V);PV2 Voltage (V);Grid Voltage (V);Grid Frequency (Hz);Output Power (W);Temperature (C);Energy Today (kWh);Energy Total (kWh);total time (s)\n");
    }

    // SIMULATING DATA FOR WRITING PURPOSES
    // DATA IS STORED IN SAME FOLDER AS C FILE FOR TESTING.
    freopen("bad_input.txt", "r", stdin);
    unsigned short raw_data[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; i++)
    {
        unsigned short high_byte, low_byte;
        if (scanf("%hx %hx", &high_byte, &low_byte) != 2)
        {
            fprintf(stderr, "Error: Invalid input format at position %d\n", i);
            exit(EXIT_FAILURE);
        }

        raw_data[i] = (high_byte << 8) | low_byte;
        printf("raw_data[%d] = %04x\n", i, raw_data[i]);
    }

}

// Function to write to csv file
int write_csv(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return -1;
    }
    else {
        write_header = 1;
    }

    if (write_header) {
        fprintf(file, "PV1 Voltage (V);PV2 Voltage (V);PV2 Voltage (V);Grid Voltage (V);Grid Frequency (Hz);Output Power (W);Temperature (C);Energy Today (kWh);Energy Total (kWh);total time (s)\n");
    }

    // SIMULATING DATA FOR WRITING PURPOSES
    // DATA IS STORED IN SAME FOLDER AS C FILE FOR TESTING.
    freopen("input.txt", "r", stdin);
    unsigned short raw_data[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; i++)
    {
        unsigned short high_byte, low_byte;
        if (scanf("%hx %hx", &high_byte, &low_byte) != 2)
        {
            fprintf(stderr, "Error: Invalid input format at position %d\n", i);
            exit(EXIT_FAILURE);
        }

        raw_data[i] = (high_byte << 8) | low_byte;
        printf("raw_data[%d] = %04x\n", i, raw_data[i]);
    }

    // Calculate values based on given formula.
    double pv1_voltage = (raw_data[0]) / 10.0;                            // D1D2
    double pv2_voltage = (raw_data[2]) / 10.0;                            // D5D6
    double grid_voltage = (raw_data[4]) / 10.0;                           // D7D8
    double grid_frequency = (raw_data[5]) / 100.0;                        // D9D10
    double output_power = (raw_data[6]) / 10.0;                           // D11D12
    double temperature = (raw_data[7]) / 10.0;                            // D13D14 
    double energy_today = (raw_data[10]) / 10.0;                          // D21D22
    double energy_total = ((raw_data[11] * 65536) + raw_data[12]) / 10.0; // D23D24D25D26
    double total_time = (raw_data[13] + raw_data[14]);                    // D27D28D29D30

    // Write results to the CSV file
    fprintf(file, "%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f,%d\n", pv1_voltage, pv2_voltage, grid_voltage, grid_frequency, output_power, temperature, energy_today, energy_total, total_time);
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

    // Read line-by-line from the CSV file using fgets
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        // Output the line from CSV
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
    const char *valid_filename = "test.csv";
    const char *bad_input_filename = "bad_data.csv";
    const char *invalid_filename = "/fake_path/test.csv";

    printf("\n=== Testing Success Scenario ===\n");
    if (write_csv(valid_filename) == 0)
    {
        printf("Writing succeeded. Verifying contents:\n");
        if (read_csv(valid_filename) != 0)
        {
            printf("Failed to read valid CSV file.\n");
        }
    }
    else
    {
        printf("Failed to write valid CSV file.\n");
    }

    printf("\n=== Testing Failure Scenarios ===\n");

    // Test writing to an invalid location
    if (write_csv(invalid_filename) == 0)
    {
        printf("This will never print because the code will fail inside the function write_csv\n");
    }

    // Test reading a non-existent file
    if (read_csv(invalid_filename) == 0)
    {
        printf("This will never print because the code will fail inside the function read_csv\n");
    }
    // Test trying to read incomplete data.
    if (Incomplete_data(bad_input_filename) == 0){
        printf("Error occurs inside of the Incomplete_data function, this should never print.\n");
    }
}

int main()
{
    // Run test
    test_csv();
    return 0;
}
