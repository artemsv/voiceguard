//  10.10.2006

void delay_loop_2(uint16_t __count);
void epmrbl(void *pointer_ram,
                   const void *pointer_eeprom,
                   size_t size);
void epmwbl(const void *pointer_ram,
                    void *pointer_eeprom,
                    size_t size);

unsigned short epmrw(const uint16_t * adr);
uint8_t eeprom_read_byte_1 (unsigned char* addr);
void eeprom_write_byte_1 (unsigned char *addr, unsigned char value);
void delayms(unsigned short ms);
void SetSpeakerVolume(unsigned char value);
unsigned char HexByte(unsigned char byte);
unsigned short HexToInt(char* sz);
