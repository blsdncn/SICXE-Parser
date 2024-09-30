#include "interpret_instruction.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void logInstruction(Instruction *instruction){
    printf("%-19s%-19d%-19s%-19s%-19x\n"
    ,instruction->mnemonic,instruction->format,instruction->OAT,instruction->TAAM,instruction->objectCode);
}

int ParseAndLogInstruction(unsigned int SAMPLE){
//    int mask = OP_MASK;
    int opcode = (SAMPLE & OP_MASK) >> OP_SHIFT*HEX_CHAR_SHIFT_AMOUNT; 
    Instruction *instruction = malloc(sizeof(Instruction));
    instruction->format = 0;
    ///////////////////////////////////////////////////////
   //Searching for opcode in the arrays 
   //Format 1
    for(int i = 0; i < sizeof(FORMAT1)/sizeof(int); i++){
        if(FORMAT1[i]==opcode){
            strcpy(instruction->mnemonic,FORMAT1_Mnemonic[i]);
            instruction->format = 1;
        }
    }
    //Format 2
    for(int i = 0; i < sizeof(FORMAT2)/sizeof(int); i++){
        if(FORMAT2[i]==opcode){
            strcpy(instruction->mnemonic,FORMAT2_Mnemonic[i]);
            instruction->format = 2;
        }
    }
    //Format 3
    for(int i = 0; i < sizeof(FORMAT3_4)/sizeof(int); i++){
        if(FORMAT3_4[i]==opcode){
            strcpy(instruction->mnemonic,FORMAT3_4_Mnemonic[i]);
        }
    }
    ///////////////////////////////////////////////////////
    //Checking if mnemonic was found.
    if(instruction->mnemonic == NULL){
        printf("Op: %x. Mnemonic not found.\n",opcode);
        return -1;
    }
    ///////////////////////////////////////////////////////
    //If format was found, then it's either 1 or 2, so we don't need to check for TAAM or OAAM
    if(instruction->format){ 
        strcpy(instruction->TAAM,"");
        strcpy(instruction->OAT,"");
    } else {
        instruction->format = 3 + ((SAMPLE & E_BIT_MASK) > 0); //If e bit is set, format is 4, otherwise it's 3
        strcpy(instruction->TAAM,TAAM_LOOKUP_ARRAY[(SAMPLE & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT*HEX_CHAR_SHIFT_AMOUNT]); 
        strcpy(instruction->OAT,ADDR_TYPE[(SAMPLE & ADDR_TYPE_MASK) >> ADDR_TYPE_SHIFT*HEX_CHAR_SHIFT_AMOUNT]);
    }
    instruction->objectCode = SAMPLE >> ((4-instruction->format)*8); //How much to shift the sample by to get the object code depending on the format
    logInstruction(instruction);
    int format = instruction->format;
    free(instruction);
    return format;
}
