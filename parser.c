//parser.c
#include "interpret_instruction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
	if(argc < 2){
		printf("Please specify an input file.\n");
		return 1;
	}
	const char *input_file = argv[1];
	char HEADER[OUTPUT_BUFFER_SIZE];
    snprintf(HEADER,97,"%-19s%-19s%-19s%-19s%-19s\n",
    "INSTR","FORMAT","OAT","TAAM","OBJ");
	FILE *objectFile = fopen(input_file,"r");
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
	
	char *line = NULL;
	size_t len = 0;
	// Discard first line of input file
    while(feof(objectFile) == 0){
        if(fgetc(objectFile) != 'T'){
            getline(&line, &len, objectFile); // Discard non-T lines
            continue;
        } 
        if(fseek(objectFile,6,SEEK_CUR)!=0){
            printf("Error skipping characters.\n");
            free(line);
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
        char sizeBytes[3];
        unsigned int result;
        if(fgets(sizeBytes, sizeof(sizeBytes),objectFile) != NULL){
            result = strtol(sizeBytes, NULL, 16); // Convert first two bytes to get size of obj code in bytes
            if(result <= 0){
                printf("Failed to convert size bytes to long.\n");
                free(line);
                fclose(objectFile);
                fclose(outputFile);
                return 1;
            }
            //Each byte is 2 hex chars
            size_t size = result*2;
            char *buffer = malloc(size+1);
            size_t readresult = 0;
            readresult = fread(buffer,1,size,objectFile);
            if(readresult == size){ //checking that all chars were read successfully
                buffer[size]='\0';
                char chunk[9];
                unsigned int SAMPLE = 0x0;
                int currentPosition = 0;
                while(currentPosition < size){
                    int originalPosition = currentPosition;
                    //Handling the case where the last chunk of obj code is less than 8 hex chars
                    if(currentPosition+8 > size){
                        currentPosition -= (8-(size-currentPosition)); //Move back such that the current position starts at the last 8 hex chars
                        if(currentPosition < 0) currentPosition = 0; // Make sure we don't go out of bounds
                        strncpy(chunk,&buffer[currentPosition],8); //Copy the last 8 hex chars
                        chunk[8] = '\0';
                        SAMPLE = strtol(chunk,NULL,16); //Convert to number
                        SAMPLE = SAMPLE << (originalPosition-currentPosition)*4; //Shift the sample back such that the hex char at original position is at the rightmost position
                        currentPosition = originalPosition; //Set the current position back to the original position for the break condition
                    } else {
                        //Copy 8 hex chars like normal
                        strncpy(chunk,&buffer[currentPosition],8); 
                        chunk[8] = '\0';
                        SAMPLE = strtol(chunk,NULL,16);
                    }
                    //Shiftback is the format of the instruction, or -1 if the instruction is invalid
                    int shiftback = ParseAndLogInstruction(SAMPLE,outputFile);
                    if(shiftback == -1) return 1;
                    currentPosition += (8-(4-shiftback)*2); //How much we advance the current position by based on format
                    if(currentPosition >= size){
                        break;
                    }
                }
                //flushing out the line after end of record is reached.
                if (getline(&line, &len, objectFile) == -1) {
                    break; // End of file or error
                }
                free(buffer);
            } else {
                printf("Failed to grab obj code chunk. Expected %zu hex chars, got %zu.",
                size,readresult);
                free(buffer);
                free(line);
                fclose(objectFile);
                fclose(outputFile);
                return 1;
            }
        } else {
            printf("Failed to read size bytes.\n");
            free(line);
            fclose(objectFile);
            fclose(outputFile);
            return 1;
        }
    }
    //printf("Completed.\n");
    free(line);
    fclose(objectFile);
    fclose(outputFile);

	
	
	
}
