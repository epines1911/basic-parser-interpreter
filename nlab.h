#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define MAXNUMTOKENS 100
#define MAXTOKENSIZE 20
#define strsame(A, B) (strcmp(A, B)==0)

#define ERROR(PHRASE) {fprintf (stderr, \
"Fatal Error %s occurred in %s, line %d\n", PHRASE, __FILE__, __LINE__);  \
exit (EXIT_FAILURE); }

// There will be 26 variables. $A, $B, ..., $Z. 
// The No.27 Var will be a place to store <INTEGER> (a Var which contains a 1x1 array)
#define MAXVAR 27

#define MAXARRAY 100
struct var{
    int arr [MAXARRAY][MAXARRAY];
    int row;
    int col;
};
typedef struct var Var;

struct prog{
    char words [MAXNUMTOKENS][MAXTOKENSIZE];
    int cur_word; // Current Word
    Var data[MAXVAR];

    // to store the index of an variable which is to the left of ":=".
    int aimIndex; 

    // to store the index of an variable which is to the right of ":=".
    int rIndex1;

    // to store the index of another variable which is also to the right of ":=". 
    int rIndex2; 
};
typedef struct prog Program;

void* ncalloc(int n, size_t size);
void* nfopen(char* fname, char* mode);

void Prog(Program *p);
void Instrclist(Program *p);
void Instrc(Program *p);

void Print(Program *p);

void Set(Program *p);
bool nameIsValid(char c);

void Polishlist(Program *p);
void Polish(Program *p);
bool isInteger(char *c);

void UNot(Program *p);
void UEightcount(Program *p);

void BAnd(Program *p);
void BOr(Program *p);
void BGreater(Program *p);
void BLess(Program *p);
void BAdd(Program *p);
void BTimes(Program *p);
void BEquals(Program *p);

void CreateOnes(Program *p);
void CreateRead(Program *p);

void Loop(Program *p);

// extension
void CreateZeros(Program *p);
void Cut(Program *p);

void test();
