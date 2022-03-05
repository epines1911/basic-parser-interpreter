#include "nlab.h"
// My code runs will :) But, I am trying to use #ifdef and #endif.
// If I write '#define INTERP' in my code, #ifdef INTERP doesn't work;
// If I comment '#define INTERP', #ifdef works. 
// I really don't know why so I just left it here.
// Hope #define INTERP and #define EXT still work on other machine. 

// #define INTERP
// #define EXT

int main(int argc, char* argv[]){
    test();
    Program* prog = ncalloc(1 , sizeof(Program));
 
    if(argc != 2){
        ERROR("Usage Example: ./parse file.nlb\n");
    }

    FILE* fp = nfopen(argv[1], "rt");

    for (int i = 0; i < MAXNUMTOKENS; i++)
    {
        // ignore everything before BEGIN
        // store words in prog->words until BEGIN shows
        if (fscanf(fp, "%s", prog->words[i]) == EOF)
        {
            i = MAXNUMTOKENS;
        }
        else
        {
            if (strsame(prog->words[i], "BEGIN"))
            {
                strcpy(prog->words[0], "BEGIN");
                i = 0;
            }
        }
    }
    fclose(fp);

    Prog(prog);
    free(prog);
    return 0;
}

void* ncalloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if(v==NULL){
      ERROR("Cannot calloc() space");
   }
   return v;
}

void* nfopen(char* fname, char* mode)
{
    FILE* fp;
    fp = fopen(fname, mode);
    if(!fp){
        ERROR("Cannot open file");
    }
    return fp;
}

void Prog(Program *p){
    if (!strsame(p->words[p->cur_word], "BEGIN"))
    {
        ERROR("No BEGIN statement?");
    }

    p->cur_word = p->cur_word + 1;
    if (!strsame(p->words[p->cur_word], "{"))
    {
        ERROR("No { statement?");
    }

    p->cur_word = p->cur_word + 1;
    Instrclist(p);
}

void Instrclist(Program *p){
    if (strsame(p->words[p->cur_word], "}"))
    {
        return;
    }
    else
    {
        Instrc(p);
        p->cur_word = p->cur_word + 1;
        Instrclist(p);
    }
    
}

void Instrc(Program *p){
    if (strsame(p->words[p->cur_word], "PRINT"))
    {
        p->cur_word = p->cur_word + 1; // will be varname or string
        Print(p);
    }
    else if (strsame(p->words[p->cur_word], "SET"))
    {
        p->cur_word = p->cur_word + 1; // will be varname
        Set(p);
    }
    else if (strsame(p->words[p->cur_word], "ONES"))
    {
        p->cur_word = p->cur_word + 1; // will be rows
        CreateOnes(p);
    }
    else if (strsame(p->words[p->cur_word], "READ"))
    {
        p->cur_word = p->cur_word + 1;  // will be filename
        CreateRead(p);
    }
    else if (strsame(p->words[p->cur_word], "LOOP"))
    {
        p->cur_word = p->cur_word + 1; // will be <VARNAME>
        Loop(p);
    }
#ifdef EXT
    else if (strsame(p->words[p->cur_word], "ZEROS"))
    {
        // Create an array full of zeros
        p->cur_word = p->cur_word + 1; // will be rows
        CreateZeros(p);
    }
    else if (strsame(p->words[p->cur_word], "CUT"))
    {
        // cut the other charactors after the index <INTEGER> 
        p->cur_word = p->cur_word + 1; // will be <STRING>
        Cut(p);
    }
#endif
    else
    {
        ERROR("No PRINT, SET, CREATE or LOOP statement?");
    }
    
}

void Print(Program *p){
    int endIndex = strlen(p->words[p->cur_word]) - 1;

    if (p->words[p->cur_word][0] == '$')
    {
        // It is <VARNAME>
        if (nameIsValid(p->words[p->cur_word][1]))
        {
#ifdef INTERP
            int index = p->words[p->cur_word][1] - 'A';
            Var temp = p->data[index];
            for (int i = 0; i < temp.row; i++)
            {
                for (int j = 0; j < temp.col; j++)
                {
                    printf("%d ", temp.arr[i][j]);
                }
                printf("\n");
            }
#endif
        }
        else
        {
            ERROR("Invalid <VARNAME>");
        }
    }
    else if (p->words[p->cur_word][0] == '"' && p->words[p->cur_word][endIndex] == '"')
    {
        // It is <STRING>
        // to delete the double " in this string
#ifdef INTERP
        char tempStr[MAXTOKENSIZE];
        strncpy(tempStr, p->words[p->cur_word] + 1, endIndex - 1);
        printf("%s\n", tempStr);
#endif
    }
    else
    {
        ERROR("No Valname or String statement?");
    }
}

void Set(Program *p){
    // check if the varname is valid
    if (!nameIsValid(p->words[p->cur_word][1]))
    {
        ERROR("Invalid name of variable");
    }
    else
    {
        p->aimIndex = p->words[p->cur_word][1] - 'A';
    }
    

    p->cur_word = p->cur_word + 1; // will be ':='
    if (!strsame(p->words[p->cur_word], ":="))
    {
        ERROR("No ':=' statement?");
    }

    p->cur_word = p->cur_word + 1; // will be polishlist
    Polishlist(p);  

}

bool nameIsValid(char c){
    int ascii = c;
    if (ascii > 64 && ascii < 91)
    {
        return true;
    }
    return false; 
}

void Polishlist(Program *p){
    if (strsame(p->words[p->cur_word], ";"))
    {
        return;
    }
    else
    {
        Polish(p); // p.cur_word is <PUSHDOWN>, <UNARYOP>, <BINARYOP>, or something wrong
        p->cur_word = p->cur_word + 1;
        Polishlist(p);
    }
    
}

void Polish(Program *p){
    if (p->words[p->cur_word][0] == '$')
    {
        if (!nameIsValid(p->words[p->cur_word][1]))
        {
            ERROR("Invalid name of variable");
        }
        else
        {
            #ifdef INTERP
            // to store the last index in p->rIndex2
            p->rIndex1 = p->rIndex2;
            p->rIndex2 = p->words[p->cur_word][1] - 'A';
            if (strsame(p->words[p->cur_word + 1], ";"))
            {
                p->data[p->aimIndex] = p->data[p->rIndex2];
            }
            #endif
        }

    }
    else if (isInteger(p->words[p->cur_word]))
    {
        Var temp;
        temp.row = 1;
        temp.col = 1;
        temp.arr[0][0] = atoi(p->words[p->cur_word]);
        p->data[26] = temp; // index 26 is a place for storing an <INTEGER>

        if (strsame(p->words[p->cur_word + 1], ";"))
        {
            p->data[p->aimIndex] = temp;
        }
        
        p->rIndex1 = p->rIndex2;
        p->rIndex2 = 26;
    }
    else if (strsame(p->words[p->cur_word], "U-NOT"))
    {
        UNot(p);
    }
    else if (strsame(p->words[p->cur_word], "U-EIGHTCOUNT"))
    {
        UEightcount(p);
    }
    else if (strsame(p->words[p->cur_word], "B-AND"))
    {
        BAnd(p);
    }
    else if (strsame(p->words[p->cur_word], "B-OR"))
    {
        BOr(p);
    }
    else if (strsame(p->words[p->cur_word], "B-GREATER"))
    {
        BGreater(p);
    }
    else if (strsame(p->words[p->cur_word], "B-LESS"))
    {
        BLess(p);
    }
    else if (strsame(p->words[p->cur_word], "B-ADD"))
    {
        BAdd(p);
    }
    else if (strsame(p->words[p->cur_word], "B-TIMES"))
    {
        BTimes(p);
    }
    else if (strsame(p->words[p->cur_word], "B-EQUALS"))
    {
        BEquals(p);
    }
    else
    {
        ERROR("No PUSHDOWN, UNARYOP or BINARYOP statement?");
    }
    
}

bool isInteger(char *c){
    int lenth = strlen(c);
    for (int i = 0; i < lenth; i++)
    {
        int ascii = c[i];
        if (ascii < 48 || ascii > 57)
        {
            return false;
        } 
    }
    return true;   
}

void UNot(Program *p){
    Var temp = p->data[p->rIndex2];
#ifdef INTERP
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            if (temp.arr[i][j] == 0)
            {
                temp.arr[i][j] = 1;
            }
            else
            {
                temp.arr[i][j] = 0;
            }
        }
    }
#endif
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
}

void UEightcount(Program *p){
    Var temp = p->data[p->rIndex2];
#ifdef INTERP
    int count = 0;
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            if (i-1 > 0 && j-1 > 0)
            {
                if (p->data[p->rIndex2].arr[i-1][j-1] > 0){ count += 1; } // NW
            }
            if (i-1 > 0)
            {
                if (p->data[p->rIndex2].arr[i-1][j] > 0){ count += 1; } // North
            }
            if (i-1 > 0 && j+1 < temp.col)
            {
                if (p->data[p->rIndex2].arr[i-1][j+1] > 0){ count += 1; } // NE
            }
            if (j-1 > 0)
            {
                if (p->data[p->rIndex2].arr[i][j-1] > 0){ count += 1; } // West
            }
            if (j+1 < temp.col)
            {
                if (p->data[p->rIndex2].arr[i][j+1] > 0){ count += 1; } // East
            }
            if (i+1 < temp.row && j-1 > 0)
            {
                if (p->data[p->rIndex2].arr[i+1][j-1] > 0){ count += 1; } // SW
            }
            if (i+1 < temp.row)
            {
                if (p->data[p->rIndex2].arr[i+1][j] > 0){ count += 1; } // South
            }
            if (i+1 < temp.row && j+1 < temp.col)
            {
                if (p->data[p->rIndex2].arr[i+1][j+1] > 0){ count += 1; } // SE
            }            
            temp.arr[i][j] = count;
            count = 0;
        }
    }
#endif
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
}

void BAnd(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
#ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] && p->data[p->rIndex1].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
#endif
        return;
    }

    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
#ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] && p->data[p->rIndex2].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
#endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col ||
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] && p->data[p->rIndex1].arr[i][j];
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BOr(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] || p->data[p->rIndex1].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }

    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] || p->data[p->rIndex2].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col ||
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] || p->data[p->rIndex1].arr[i][j];
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BGreater(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[0][0] > p->data[p->rIndex2].arr[i][j] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] > p->data[p->rIndex2].arr[0][0] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col || 
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] > p->data[p->rIndex2].arr[i][j] ?1:0;
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BLess(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[0][0] < p->data[p->rIndex2].arr[i][j] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] < p->data[p->rIndex2].arr[0][0] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col || 
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] < p->data[p->rIndex2].arr[i][j] ?1:0;
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BAdd(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] + p->data[p->rIndex1].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }

    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] + p->data[p->rIndex2].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col ||
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] + p->data[p->rIndex1].arr[i][j];
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BTimes(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] * p->data[p->rIndex1].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }

    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] * p->data[p->rIndex2].arr[0][0];
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col ||
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] * p->data[p->rIndex1].arr[i][j];
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void BEquals(Program *p){
    // if the first <VARNAME> is a 1x1 array
    if (p->data[p->rIndex1].col == 1 && p->data[p->rIndex1].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex2];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] == p->data[p->rIndex1].arr[0][0] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }

    // if the last <VARNAME> is a 1x1 array
    if (p->data[p->rIndex2].col == 1 && p->data[p->rIndex2].row == 1)
    {
        #ifdef INTERP
        Var temp = p->data[p->rIndex1];
        for (int i = 0; i < temp.row; i++)
        {
            for (int j = 0; j < temp.col; j++)
            {
                temp.arr[i][j] = p->data[p->rIndex1].arr[i][j] == p->data[p->rIndex2].arr[0][0] ?1:0;
            }
        }
        p->data[p->aimIndex] = temp;
        p->rIndex2 = p->aimIndex;
        #endif
        return;
    }
    
    // If both arrays are bigger than 1x1, they must be the same size
    if (p->data[p->rIndex2].col != p->data[p->rIndex1].col ||
        p->data[p->rIndex2].row != p->data[p->rIndex1].row)
    {
        ERROR("The two matrixs have different rows or cols");
    }

    #ifdef INTERP
    Var temp = p->data[p->rIndex2];
    for (int i = 0; i < temp.row; i++)
    {
        for (int j = 0; j < temp.col; j++)
        {
            temp.arr[i][j] = p->data[p->rIndex2].arr[i][j] == p->data[p->rIndex1].arr[i][j] ?1:0;
        }
    }
    p->data[p->aimIndex] = temp;
    p->rIndex2 = p->aimIndex;
    #endif
}

void CreateOnes(Program *p){
    // current word is ROWS
    int tempRows;
    int tempCols;
    if (isInteger(p->words[p->cur_word]))
    {
        tempRows = atoi(p->words[p->cur_word]);
    }
    else
    {
        ERROR("ROWS is not a valid integer");
    }

    p->cur_word = p->cur_word + 1; // will be cols
    if (isInteger(p->words[p->cur_word]))
    {
        tempCols = atoi(p->words[p->cur_word]);
    }
    else
    {
        ERROR("COLS is not a valid integer");
    }
    p->cur_word = p->cur_word + 1; // will be varname
    int index = p->words[p->cur_word][1] - 'A';
    Var tempVar;
    tempVar.row = tempRows;
    tempVar.col = tempCols;
#ifdef INTERP
    for (int i = 0; i < tempRows; i++)
    {
        for (int j = 0; j < tempCols; j++)
        {
            tempVar.arr[i][j] = 1;
        }
    }
#endif
    p->data[index] = tempVar;
}

void CreateRead(Program *p){
    // current word is <FILENAME>
    Var tempVar;
    int endIndex = strlen(p->words[p->cur_word]) - 1;
    if (p->words[p->cur_word][0] == '"' && p->words[p->cur_word][endIndex] == '"')
    {
        char filename[MAXTOKENSIZE];
        strncpy(filename, p->words[p->cur_word] + 1, endIndex - 1);
        filename[endIndex - 1] = '\0';
        int nameLen = strlen(filename);

        // make sure it is a .arr file       
        if (filename[nameLen - 4] == '.' && filename[nameLen - 3] == 'a' &&
            filename[nameLen - 2] == 'r' && filename[nameLen - 1] == 'r')
        {
            FILE* fp = nfopen(filename, "rt");
            if(fscanf(fp, "%d", &tempVar.row) != 1)
            {
                ERROR("The value of row is invalid");

            }
            if(fscanf(fp, "%d", &tempVar.col) != 1)
            {
                ERROR("The value of col is invalid");

            }

            for (int i = 0; i < tempVar.row; i++)
            {
                for (int j = 0; j < tempVar.col; j++)
                {
                    if(fscanf(fp, "%d", &tempVar.arr[i][j]) != 1)
                    {
                        ERROR("The value in this array is invalid");
                    }
                }
            }
            fclose(fp);

        }
        else
        {
            ERROR("It's not a '.arr' file");
        }
    }
    else
    {
        ERROR("FILENAME is not a <STRING>");
    }
    p->cur_word = p->cur_word + 1; // will be <VARNAME>
    int index = p->words[p->cur_word][1] - 'A';
    p->data[index] = tempVar;
}

void Loop(Program *p){
    int loop = 1;
    int loopIndex = 0;
#ifdef INTERP
    Var loopValue;
    loopValue.row = 1;
    loopValue.col = 1;
#endif

    if (p->words[p->cur_word][0] == '$' && nameIsValid(p->words[p->cur_word][1]))
    {
        #ifdef INTERP
        loopIndex = p->words[p->cur_word][1] - 'A';
        #endif
    }
    else
    {
        ERROR("Invalid <VARNAME> or No <VARNAME> statement");
    }

    p->cur_word = p->cur_word + 1; // will be <INTEGER>
    if (isInteger(p->words[p->cur_word]))
    {
        #ifdef INTERP
        loopValue.arr[0][0] = 0;
        loop = atoi(p->words[p->cur_word]);
        p->data[loopIndex] = loopValue;
        #endif
    }
    else
    {
        ERROR("Invalid <INTEGER>");
    }

    p->cur_word = p->cur_word + 1; // will be '{'
    if (!strsame(p->words[p->cur_word], "{"))
    {
        ERROR("No { statement?");
    }
    
    p->cur_word = p->cur_word + 1; // will be <INSTRCLIST>

    // record the index of current word when the loop starts
    int curWord = p->cur_word;
    for (p->data[loopIndex].arr[0][0] = 1; 
         p->data[loopIndex].arr[0][0] < loop + 1;
         p->data[loopIndex].arr[0][0]++)
    {
        // jump back to the start of loop
        p->cur_word = curWord;
        Instrclist(p);
    }
}

void CreateZeros(Program *p){
    // current word is ROWS
    int tempRows;
    int tempCols;
    if (isInteger(p->words[p->cur_word]))
    {
        tempRows = atoi(p->words[p->cur_word]);
    }
    else
    {
        ERROR("ROWS is not a valid integer");
    }

    p->cur_word = p->cur_word + 1; // will be cols
    if (isInteger(p->words[p->cur_word]))
    {
        tempCols = atoi(p->words[p->cur_word]);
    }
    else
    {
        ERROR("COLS is not a valid integer");
    }
    p->cur_word = p->cur_word + 1; // will be varname
    int index = p->words[p->cur_word][1] - 'A';
    Var tempVar;
    tempVar.row = tempRows;
    tempVar.col = tempCols;
#ifdef EXT
    for (int i = 0; i < tempRows; i++)
    {
        for (int j = 0; j < tempCols; j++)
        {
            tempVar.arr[i][j] = 0;
        }
    }
#endif
    p->data[index] = tempVar;
}

void Cut(Program *p){
    // current word is STRING
    int endIndex = strlen(p->words[p->cur_word]) - 1;

    if (p->words[p->cur_word][0] == '"' && p->words[p->cur_word][endIndex] == '"')
    {
        // It is <STRING>
        // to delete the double " in this string
#ifdef EXT
        char tempStr[MAXTOKENSIZE];
        strncpy(tempStr, p->words[p->cur_word] + 1, endIndex - 1);

        p->cur_word = p->cur_word + 1; // will be <INTEGER>
        int endI = 0;
        if (isInteger(p->words[p->cur_word]))
        {
            endI = atoi(p->words[p->cur_word]);
        }
        else
        {
            ERROR("Invalid INTEGER");
        }
        strncpy(p->words[p->cur_word], tempStr, endI + 1);
#endif
    }
    else
    {
        ERROR("No Valname or String statement?");
    }
}

void test()
{
    Program *p = ncalloc(1, sizeof(Program));
    p->cur_word = 0;

    assert(!nameIsValid('e'));
    assert(nameIsValid('Z'));

    strcpy(p->words[0], "0");
    assert(isInteger(p->words[0]));
    strcpy(p->words[0], "~");
    assert(!isInteger(p->words[0]));

    // use test.nlb to test other functions
    FILE* fp = nfopen("test.nlb", "rt");
    for (int i = 0; i < MAXNUMTOKENS; i++)
    {
        if (fscanf(fp, "%s", p->words[i]) == EOF)
        {
            i = MAXNUMTOKENS;
        }
        else
        {
            if (strsame(p->words[i], "BEGIN"))
            {
                strcpy(p->words[0], "BEGIN");
                i = 0;
            }
        }
    }
    fclose(fp);
    Prog(p);
    free(p);
    return;
}



