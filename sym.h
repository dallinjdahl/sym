//should include stdint before this file.
#define GROW 70
#define SRHINK 30
//initialize table 
void init();
//intern new string, returning symbol
uint16_t intern(char *s);
//remove symbol from table
void delete(uint16_t sym);
//print table (mostly for debugging)
void printtbl();
//return current number of items in table
uint16_t getload();
//get string associated with symbol
char *symstr(uint16_t sym);
