#pragma once

#define FOUND 1 
#define NOT_FOUND 0

void initTable();
int crack(char* str, int len, char stop, uint8_t* search);
void parseHash(const char* str, const uint8_t *search);

