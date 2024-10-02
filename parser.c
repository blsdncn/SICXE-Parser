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
	char HEADER[98];
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
            result = strtol(sizeBytes, NULL, 16);
            if(result <= 0){
                printf("Failed to convert size bytes to long.\n");
                free(line);
                fclose(objectFile);
                fclose(outputFile);
                return 1;
            }
            size_t size = result*2;
            char *buffer = malloc(size+1);
            size_t readresult = 0;
            readresult = fread(buffer,1,size,objectFile);
            if(readresult == size){
                buffer[size]='\0';
                char chunk[9];
                unsigned int SAMPLE = 0x0;
                int currentPosition = 0;
                while(currentPosition < size){
                    int originalPosition = currentPosition;
                    if(currentPosition+8 > size){
                        currentPosition -= (8-(size-currentPosition));
                        if(currentPosition < 0) currentPosition = 0;
                        strncpy(chunk,&buffer[currentPosition],8); 
                        chunk[8] = '\0';
                        SAMPLE = strtol(chunk,NULL,16);
                        SAMPLE = SAMPLE << (originalPosition-currentPosition)*4;
                        currentPosition = originalPosition;
                    } else {
                        strncpy(chunk,&buffer[currentPosition],8); 
                        chunk[8] = '\0';
                        SAMPLE = strtol(chunk,NULL,16);
                    }
                    int shiftback = ParseAndLogInstruction(SAMPLE,outputFile);
                    if(shiftback == -1) return 1;
                    currentPosition += (8-(4-shiftback)*2);
                    if(currentPosition >= size){
                        break;
                    }
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
