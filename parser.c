#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	if(argc < 2){
		printf("Please specify an input file.\n");
		return 1;
	}
	const char *input_file = argv[1];
	const char *HEADER = 
    "INSTR           FORMAT          OAT             TAAM            OBJ\n";
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
        else{
            if(fseek(objectFile,7,SEEK_CUR)!=0){
                printf("Error skipping characters.\n");
                free(line);
                fclose(objectFile);
                fclose(outputFile);
                return 1;
            }
            char sizeBytes[3];
            long result;
            if(fgets(sizeBytes, sizeof(sizeBytes),objectFile) != NULL){
                result = strtol(sizeBytes, NULL, 16);
                if(result == 0){
                    printf("Failed to convert size bytes to long.\n");
                    free(line);
                    fclose(objectFile);
                    fclose(outputFile);
                    return 1;
                }
                size_t size = result*2;
                char *objectCode = malloc(size+1);
                
            } else {
                printf("Failed to read size bytes.\n");
                free(line);
                fclose(objectFile);
                fclose(outputFile);
                return 1;

            }
        }

    }
	
	
	
}
