#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DATA_SIZE 30
#define NUM_VALUES (DATA_SIZE / 2)

int Incomplete_data(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return -1;
    }

    // Simulating data reading from a source
    freopen("bad_input.txt", "r", stdin);
    unsigned short raw_data[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; i++)
    {
        unsigned short high_byte, low_byte;
        if (scanf("%hx %hx", &high_byte, &low_byte) != 2)
        {
            fprintf(stderr, "Error: Invalid input format at position %d\n", i);
            fclose(file);
            return -1;
        }
        raw_data[i] = (high_byte << 8) | low_byte;
    }
}

int write_json(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return -1;
    }

    // Simulating data reading from a source
    freopen("input.txt", "r", stdin);
    unsigned short raw_data[NUM_VALUES];
    for (int i = 0; i < NUM_VALUES; i++)
    {
        unsigned short high_byte, low_byte;
        if (scanf("%hx %hx", &high_byte, &low_byte) != 2)
        {
            fprintf(stderr, "Error: Invalid input format at position %d\n", i);
            fclose(file);
            return -1;
        }
        raw_data[i] = (high_byte << 8) | low_byte;
    }

    // Calculate values based on given formula
    double pv1_voltage = raw_data[0] / 10.0;                             // D1D2
    double pv2_voltage = raw_data[2] / 10.0;                             // D5D6
    double grid_voltage = raw_data[4] / 10.0;                            // D7D8
    double grid_frequency = raw_data[5] / 100.0;                         // D9D10
    double output_power = raw_data[6] / 10.0;                            // D11D12
    double temperature = raw_data[7] / 10.0;                             // D13D14
    double error = raw_data[9];                                          // D16
    double energy_today = raw_data[10] / 10.0;                           // D21D22
    double energy_total = ((raw_data[11] * 65536) + raw_data[12]) / 10.0; // D23D24D25D26
    double total_time = raw_data[13] + raw_data[14];                     // D27D28D29D30

    // Create a structured JSON-like string
    fprintf(file, "{\n");
    fprintf(file, "  \"PV1 voltage\": \"%.1fV\",\n", pv1_voltage);
    fprintf(file, "  \"Grid\": {\n");
    fprintf(file, "    \"voltage\": \"%.1fV\",\n", grid_voltage);
    fprintf(file, "    \"frequency\": \"%.1fHz\"\n", grid_frequency);
    fprintf(file, "  },\n");
    fprintf(file, "  \"Output power\": \"%.1fKw\",\n", output_power);
    fprintf(file, "  \"Inverter\": {\n");
    fprintf(file, "    \"temprature\": \"%.1f\",\n", temperature);
    fprintf(file, "    \"status\": \"Normal\",\n");
    fprintf(file, "    \"fault\": \"Error : %d\"\n",error);
    fprintf(file, "  },\n");
    fprintf(file, "  \"Produced\": {\n");
    fprintf(file, "    \"today\": \"%.1fW\",\n", energy_today);
    fprintf(file, "    \"lifetime\": \"%.1fKw\"\n", energy_total);
    fprintf(file, "  },\n");
    fprintf(file, "}\n");

    fclose(file);
    return 0;
}

int read_json(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file for reading");
        return -1;
    }

    char buffer[DATA_SIZE];

    // Read line-by-line from the JSON file using fgets
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        // Output the line from JSON
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


void test_json()
{
    // Test function for read and write actions.
    const char *valid_filename = "test.json";
    const char *bad_input_filename = "bad_data.json";
    const char *invalid_filename = "/fake_path/test.json";

    printf("\n=== Testing Success Scenario ===\n");
    if (write_json(valid_filename) == 0)
    {
        printf("Writing succeeded. Verifying contents:\n");
        if (read_json(valid_filename) != 0)
        {
            printf("Failed to read valid JSON file.\n");
        }
        printf("Reading data was succesful")
    }
    else
    {
        printf("Failed to write valid JSON file.\n");
    }

    printf("\n=== Testing Failure Scenarios ===\n");

    // Test writing to an invalid location
    if (write_json(invalid_filename) == 0)
    {
        printf("This will never print because the code will fail inside the function write_json\n");
    }

    // Test reading a non-existent file
    if (read_json(invalid_filename) == 0)
    {
        printf("This will never print because the code will fail inside the function read_json\n");
    }
    // Test trying to read incomplete data.
    if (Incomplete_data(bad_input_filename) == 0){
        printf("Error occurs inside of the Incomplete_data function, this should never print.\n");
    }
}

int main()
{
    // Run test
    test_json();
    return 0;
}
