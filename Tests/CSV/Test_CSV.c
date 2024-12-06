#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DATA_SIZE 32
#define BUFFER_SIZE 1024
#define NUM_VALUES (DATA_SIZE / 2)

// Functie om data naar een CSV-bestand te schrijven.
int write_csv(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        return -1;
    }

    // CSV-inhoud
    const char *D[DATA_SIZE] = {
        "08", "FC", "00", "00", "C3", "50", "09", "60", "13", "88", "75", "30", "00", "FA", "01", "00",
        "00", "00", "00", "00", "00", "9C", "00", "01", "E2", "40", "00", "00", "27", "10"
    };

    unsigned short values[DATA_SIZE];
    for (int i = 0; i < NUM_VALUES; i++) {
    unsigned short high_byte = (unsigned short)strtol(D[2 * i], NULL, 16);
    unsigned short low_byte = (unsigned short)strtol(D[2 * i + 1], NULL, 16);
    values[i] = (high_byte << 8) | low_byte; // Combine high and low bytes
    }

    // Print the decimal values
    printf("Decimal values:\n");
    for (int i = 0; i < NUM_VALUES; i++) {
        printf("Value %d: %u\n", i, values[i]);


    }
     const char *data = "";


    ssize_t bytes_written = write(fd, values,  NUM_VALUES * sizeof(values[0]));
        if (bytes_written == -1) {
        perror("Error writing to file");
        close(fd);
        return -1;
        }


    close(fd);
    return 0;
}

// Functie om data van een CSV-bestand te lezen
int read_csv(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Lees data in stukken van BUFFER_SIZE
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // Zorg ervoor dat de buffer nul-geëindigd is
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("Error reading from file");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// Testfunctie om de schrijf- en leesfunctionaliteit te controleren
void test_csv() {
    const char *filename = "test.csv";

    if (write_csv(filename) != 0) {
        printf("Test failed writing to csv file.\n");
        return;
    }

  
    printf("Inhoud van het CSV-bestand:\n");
    if (read_csv(filename) != 0) {
        printf("Test failed reading the csv file\n");
        return;
    }

    printf("\nTest succesvol voltooid.\n");
}

int main() {
    // Voer de CSV-test uit
    test_csv();
    return 0;
}

