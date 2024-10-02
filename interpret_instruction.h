#include<stdlib.h>
#include <stdio.h>

// Format 3/4 instructions
#define ADD 0x18
#define ADDF 0x58
#define AND 0x40
#define COMP 0x28
#define COMPF 0x88
#define DIV 0x24
#define DIVF 0x64
#define J 0x3C
#define JEQ 0x30
#define JGT 0x34
#define JLT 0x38
#define JSUB 0x48
#define LDA 0x00
#define LDB 0x68
#define LDCH 0x50
#define LDF 0x70
#define LDL 0x08
#define LDS 0x6C
#define LDT 0x74
#define LDX 0x04
#define LPS 0xD0
#define MUL 0x20
#define MULF 0x60
#define OR 0x44
#define RD 0xD8
#define RSUB 0x4C
#define SSK 0xEC
#define STA 0x0C
#define STB 0x78
#define STCH 0x54
#define STF 0x80
#define STI 0xD4
#define STL 0x14
#define STS 0x7C 
#define STSW 0xE8 
#define STT 0x84 
#define STX 0x10 
#define SUB 0x1C 
#define SUBF 0x5C
#define TD 0xE0
#define TIX 0x2C
#define WD 0xDC

// Format 2 instructions
#define ADDR 0x90
#define CLEAR 0xB4
#define COMPR 0xA0
#define DIVR 0x9C
#define MULR 0x98
#define RMO 0xAC
#define SHIFTL 0xA4
#define SHIFTR 0xA8
#define SUBR 0x94
#define SVC 0xB0
#define TIXR 0xB8

// Format 1 instructions
#define FIX 0xC4
#define FLOAT 0xC0
#define HIO 0xF4
#define NORM 0xC8
#define SIO 0xF0
#define TIO 0xF8

// Masks
#define OP_MASK 0xFC000000
#define ADDR_TYPE_MASK 0x03000000
#define E_BIT_MASK 0x00100000
#define ADDR_MODE_MASK 0x00E00000

// Shifts
#define OP_SHIFT 24
#define ADDR_TYPE_SHIFT 24
//For e bit just check > 0
#define ADDR_MODE_SHIFT 21

#define OUTPUT_BUFFER_SIZE 98 //Room for 5 strings of 19 characters, a newline and a null terminator.

typedef struct{
    char mnemonic[19];
    int format;
    char TAAM[19];
    char OAT[19];
    int objectCode;
} Instruction;

int ParseAndLogInstruction(unsigned int SAMPLE, FILE *stream);
void logInstruction(char *buffer, size_t size, Instruction *instruction);

const static int FORMAT2[] = {ADDR, CLEAR, COMPR, DIVR, MULR, RMO, SHIFTL, SHIFTR, SUBR, SVC, TIXR};
const static char *FORMAT2_Mnemonic[] = {"ADDR", "CLEAR", "COMPR", "DIVR", "MULR", "RMO", "SHIFTL", "SHIFTR", "SUBR", "SVC", "TIXR"};

const static int FORMAT3_4[] = {ADD, ADDF, AND, COMP, COMPF, DIV, DIVF, J, JEQ, JGT,
                             JLT, JSUB, LDA, LDB, LDCH, LDF, LDL, LDS, LDT, LDX,
                             LPS, MUL, MULF, OR, RD, RSUB, SSK, STA, STB, STCH,
                             STF, STI, STL, STS, STSW, STT, STX, SUB, SUBF, TD, TIX, WD};

const static char *FORMAT3_4_Mnemonic[] = {"ADD", "ADDF", "AND", "COMP", "COMPF", "DIV", "DIVF", "J", "JEQ", "JGT",
                                        "JLT", "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX",
                                        "LPS", "MUL", "MULF", "OR", "RD", "RSUB", "SSK", "STA", "STB", "STCH",
                                        "STF", "STI", "STL", "STS", "STSW", "STT", "STX", "SUB", "SUBF", "TD", "TIX", "WD"};

const static int FORMAT1[] = {FIX, FLOAT, HIO, NORM, SIO, TIO};
const static char *FORMAT1_Mnemonic[] = {"FIX", "FLOAT", "HIO", "NORM", "SIO", "TIO"};

const static char *TAAM_LOOKUP_ARRAY[] = {"absolute","pc","base",NULL,"absolute_indexed","pc_indexed","base_indexed"};

const static char *ADDR_TYPE[] = {"simple","immediate","indirect","simple"};





