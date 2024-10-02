#include "interpret_instruction.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void logInstruction(char *buffer, size_t size, Instruction *instruction){
    char objCodeStr[9];
    // Pads with leading zeros to ensure proper width based on format (2*format, as 4-> 4 bytes-> 8 hex char, 3->3 bytes,2->2...).
    // Fixes case where 0x050000 would be printed as 50000
    snprintf(objCodeStr,sizeof(objCodeStr),"%0*X",instruction->format*2,instruction->objectCode); 
    snprintf(buffer,size,"%-19s%-19d%-19s%-19s%-19s\n"
    ,instruction->mnemonic,instruction->format,instruction->OAT,instruction->TAAM,objCodeStr);
}

int ParseAndLogInstruction(unsigned int SAMPLE, FILE *stream){
    //int mask = OP_MASK;
    int opcode = (SAMPLE & OP_MASK) >> OP_SHIFT; 
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
        free(instruction);
        return -1;
    }
    ///////////////////////////////////////////////////////
    //If format was found, then it's either 1 or 2, so we don't need to check for TAAM or OAAM
    if(instruction->format){ 
        strcpy(instruction->TAAM,"");
        strcpy(instruction->OAT,"");
    } else {
        instruction->format = 3 + ((SAMPLE & E_BIT_MASK) > 0); //If e bit is set, format is 4, otherwise it's 3
        int TAAM_index = (SAMPLE & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT;
        int OAT_index = (SAMPLE & ADDR_TYPE_MASK) >> ADDR_TYPE_SHIFT;
        if(TAAM_index >= sizeof(TAAM_LOOKUP_ARRAY)/sizeof(TAAM_LOOKUP_ARRAY[0])){
            printf("TAAM_index OOB: %d\n",TAAM_index);
            free(instruction);
            return -1;
        }
        if(OAT_index >= sizeof(ADDR_TYPE)/sizeof(ADDR_TYPE[0])){
            printf("OAT_index OOB: %d\n",OAT_index);
            free(instruction);
            return -1;
        }
        strcpy(instruction->TAAM,TAAM_LOOKUP_ARRAY[TAAM_index]); 
        strcpy(instruction->OAT,ADDR_TYPE[OAT_index]);
    }
    // How much to shift the sample by to get the object code depending on the format
    // Format 4 doesn't need to shift, 3 shifts by 1 byte, 2 shifts by 2 bytes,
    // 1 shifts by 3 bytes
    instruction->objectCode = SAMPLE >> ((4-instruction->format)*8); 
    char buffer[OUTPUT_BUFFER_SIZE];
    logInstruction(buffer,sizeof(buffer),instruction);
    fprintf(stream,"%s",buffer);
    //printf("%s",buffer);
    int format = instruction->format;
    free(instruction);
    return format;
}
