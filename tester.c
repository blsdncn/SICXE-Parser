#include "interpret_instruction.h"
#include <stdio.h>



int main(){
    int opcode = (SAMPLE & OP_MASK) >> OP_SHIFT; 
    char *mnemonic = NULL;
    while(!mnemonic){
        for(int i = 0; i < sizeof(FORMAT1)/sizeof(int); i++){
            if(FORMAT1[i]==opcode){
                mnemonic = FORMAT1_Mnemonic[i];
                continue;
            }
        }
        for(int i = 0; i < sizeof(FORMAT2)/sizeof(int); i++){
            if(FORMAT2[i]==opcode){
                mnemonic = FORMAT2_Mnemonic[i];
                continue;
            }
        }
        for(int i = 0; i < sizeof(FORMAT3_4)/sizeof(int); i++){
            if(FORMAT3_4[i]==opcode){
                mnemonic = FORMAT3_4_Mnemonic[i];
                continue;
            }
        }
        printf("Op: %x. Mnemonic not found.\n",opcode);
        return 1;
    }
    printf("%x|%s",opcode,mnemonic);
    return 0;
}