#include <EEPROM.h>

// Function to read a byte from EEPROM at a given address
// @param address: The address from which to read the byte
// @return: The byte read from the EEPROM
uint8_t readEEPROM(int address) {
    return EEPROM.read(address);
}

// Function to write a byte to EEPROM at a given address
// @param address: The address to which the byte will be written
// @param value: The byte value to write to the EEPROM
void writeEEPROM(int address, uint8_t value) {
    EEPROM.write(address, value);
}

// Function to read a block of data from EEPROM
// @param address: The starting address from which to read the data
// @param data: Pointer to the buffer where the read data will be stored
// @param length: The number of bytes to read from the EEPROM
void readEEPROMBlock(int address, uint8_t* data, int length) {
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + i);
    }
}

// Function to write a block of data to EEPROM
// @param address: The starting address to which the data will be written
// @param data: Pointer to the buffer containing the data to write
// @param length: The number of bytes to write to the EEPROM
void writeEEPROMBlock(int address, const uint8_t* data, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(address + i, data[i]);
    }
}