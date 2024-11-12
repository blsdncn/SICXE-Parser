//parser.c
#include "interpret_instruction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TEXT_RECORD_MAX_LENGTH 70

int main(int argc, char **argv){
    char sizeBytes[3];
    sizeBytes[3]='\0';
    char startAddr[7];
    startAddr[7] = '\0';
	if(argc < 2){
		printf("Please specify an input file.\n");
		return 1;
	}
	const char *input_file_path = argv[1];
	char HEADER[OUTPUT_BUFFER_SIZE];
    snprintf(HEADER,97,"%-19s%-19s%-19s%-19s%-19s\n",
    "INSTR","FORMAT","OAT","TAAM","OBJ");
	FILE *objectFile = fopen(input_file_path,"r");
	if(objectFile == NULL){
		printf("Couldn't open file.\n");
		return 1;
	}
	FILE *outputFile = fopen("obj_struct.txt","w");
	if(outputFile == NULL){
		printf("Couldn't open output file.\n");
		fclose(objectFile);
		return 1;
	}

	fprintf(outputFile, "%s", HEADER);
	
	char line[TEXT_RECORD_MAX_LENGTH];
    while(fgets(line,TEXT_RECORD_MAX_LENGTH,objectFile)!= NULL){
        size_t len = strlen(line);
        int ptr = 0;
        if(len == TEXT_RECORD_MAX_LENGTH-1 && line[len-1] != '\n'){
            printf("Record is too long.\n");
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
        while(line[len-1]=='\r' || line[len-1]=='\n'){
            line[len-1] = '\0';
            len--;
        }
        len = strlen(line);
        if(line[ptr++] != 'T'){
            continue;
        } 
        if(len<9){
            printf("Text record is too short");
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
        strncpy(startAddr,&line[ptr],6);
        ptr += 6;
        unsigned int result;
        strncpy(sizeBytes,&line[ptr],2);
        ptr += 2;
        result = strtol(sizeBytes, NULL, 16); // Convert first two bytes to get size of obj code in bytes
        if(result <= 0){
            printf("Failed to convert size bytes to long.\n");
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
        //Each byte is 2 hex chars
        size_t size = result*2+ptr; // not quite the size of the object code, but instead length of object code + current ptr position
        if(len!=size){
            printf("Size mismatch between length of record obj code (%zu) and parsed string length (%zu).\n",size,len);
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
        char chunk[9];
        unsigned int SAMPLE = 0x0;
        while(ptr < len){
            int originalPosition = ptr;
            //Handling the case where the last chunk of obj code is less than 8 hex chars
            if(ptr+8 > size){
                ptr -= (8-(size-ptr)); //Move back such that the current position starts at the last 8 hex chars
                if(ptr < 0) ptr = 0; // Make sure we don't go out of bounds
                strncpy(chunk,&line[ptr],8); //Copy the last 8 hex chars
                chunk[8] = '\0';
                SAMPLE = strtol(chunk,NULL,16); //Convert to number
                SAMPLE = SAMPLE << (originalPosition-ptr)*4; //Shift the sample back such that the hex char at original position is at the rightmost position
                ptr = originalPosition; //Set the current position back to the original position for the break condition
            } else {
                //Copy 8 hex chars like normal
                strncpy(chunk,&line[ptr],8); 
                chunk[8] = '\0';
                SAMPLE = strtol(chunk,NULL,16);
            }
            //Shiftback is the format of the instruction, or -1 if the instruction is invalid
            Instruction *instruction = (Instruction*) malloc(sizeof(Instruction));
            if(ParseInstruction(SAMPLE, instruction)){
                printf("Instruction parsing falied.\n");
                free(instruction);
                return 1;
            }
            int shiftback = instruction->format;
            logInstruction(instruction,outputFile);
            free(instruction);
            ptr += (8-(4-shiftback)*2); //How much we advance the current position by based on format
            if(ptr >= size){
                break; }
        }
    }
    //printf("Completed.\n");
    fclose(objectFile);
    fclose(outputFile);
    return 0;
}

