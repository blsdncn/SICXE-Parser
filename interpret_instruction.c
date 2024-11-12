//interpret_instruction.c
#include "interpret_instruction.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void ObjCodeStr(char *buffer, size_t bufferSize, int objectCode, int format){
    // Pads with leading zeros to ensure proper width based on format (2*format, as 4-> 4 bytes-> 8 hex char, 3->3 bytes,2->2...).
    // Fixes case where 0x050000 would be printed as 50000
    snprintf(buffer, bufferSize, "%0*X",format*2,objectCode); 
}

void logInstruction(Instruction *instruction, FILE *stream){
    char buffer[OUTPUT_BUFFER_SIZE];
    char objCodeString[9];
    ObjCodeStr(objCodeString, sizeof(objCodeString), instruction->objectCode,instruction->format);
    if(instruction->OAT_index<0){ //Format 1 or 2
        snprintf(buffer,OUTPUT_BUFFER_SIZE,"%-19s%-19d%-19s%-19s%-19s\n",instruction->mnemonic,
        instruction->format,"","",objCodeString);
        fprintf(stream,"%s",buffer);
    } else {
        snprintf(buffer,OUTPUT_BUFFER_SIZE,"%-19s%-19d%-19s%-19s%-19s\n"
        ,instruction->mnemonic,instruction->format,ADDR_TYPE[instruction->OAT_index],TAAM_LOOKUP_ARRAY[instruction->TAAM_index]
        ,objCodeString);
        fprintf(stream,"%s",buffer);
    }
}

int ParseInstruction(unsigned int SAMPLE, Instruction *instruction){
    //int mask = OP_MASK;
    instruction->format = -1;
    instruction->mnemonic[0] = '\0';
    instruction->opcode = (SAMPLE & OP_MASK) >> OP_SHIFT; 
    ///////////////////////////////////////////////////////
   //Searching for opcode in the arrays 
   //Format 1
    for(int i = 0; i < sizeof(FORMAT1)/sizeof(int); i++){
        if(FORMAT1[i]==instruction->opcode){
            strcpy(instruction->mnemonic,FORMAT1_Mnemonic[i]);
            instruction->format = 1;
            break;
        }
    }
    //Format 2
    for(int i = 0; i < sizeof(FORMAT2)/sizeof(int); i++){
        if(FORMAT2[i]==instruction->opcode){
            strcpy(instruction->mnemonic,FORMAT2_Mnemonic[i]);
            instruction->format = 2;
            break;
        }
    }
    //Format 3
    for(int i = 0; i < sizeof(FORMAT3_4)/sizeof(int); i++){
        if(FORMAT3_4[i]==instruction->opcode){
            strcpy(instruction->mnemonic,FORMAT3_4_Mnemonic[i]);
            break;
        }
    }
    ///////////////////////////////////////////////////////
    //Checking if mnemonic was found.
    if(instruction->mnemonic[0] == '\0'){
        printf("Op: %x. Mnemonic not found.\n",instruction->opcode);
        return 1;
    }
    ///////////////////////////////////////////////////////
    //If format was found, then it's either 1 or 2, so we don't need to check for TAAM or OAAM
    if(instruction->format == 1 || instruction->format == 2){ 
        instruction->TAAM_index=-1;
        instruction->OAT_index=-1;
    } else {
        instruction->format = 3 + ((SAMPLE & E_BIT_MASK) > 0); //If e bit is set, format is 4, otherwise it's 3
        instruction->TAAM_index = (SAMPLE & ADDR_MODE_MASK) >> ADDR_MODE_SHIFT;
        instruction->OAT_index = (SAMPLE & ADDR_TYPE_MASK) >> ADDR_TYPE_SHIFT;
        if(instruction->TAAM_index >= sizeof(TAAM_LOOKUP_ARRAY)/sizeof(TAAM_LOOKUP_ARRAY[0])){
            printf("TAAM_index OOB: %d\n",instruction->TAAM_index);
            return 1;
        }
        if(instruction->OAT_index >= sizeof(ADDR_TYPE)/sizeof(ADDR_TYPE[0])){
            printf("OAT_index OOB: %d\n",instruction->OAT_index);
            return 1;
        }
        //strcpy(instruction->TAAM,TAAM_LOOKUP_ARRAY[instruction->TAAM_index]); 
        //strcpy(instruction->OAT,ADDR_TYPE[instruction->OAT_index]);
    }
    // How much to shift the sample by to get the object code depending on the format
    // Format 4 doesn't need to shift, 3 shifts by 1 byte, 2 shifts by 2 bytes,
    // 1 shifts by 3 bytes
    instruction->objectCode = SAMPLE >> ((4-instruction->format)*8); 
    //printf("%s",buffer);
    return 0;
}
