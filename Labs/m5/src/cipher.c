#include "m5.h"
#include "ptb.h"
#include "user_app.h"
#include <stdbool.h>



//Function dedicated to ASCII to integer conversion
uint32_t ascii_to_integer(uint8_t txt[4])
{
    int sum, digit, i;
    sum = 0;
    for (i = 0; i < 4; i++) {
		if(txt[i] != '0'){
        	digit = txt[i] - 0x30;
		}
		else{
			digit = 0;
		}
        sum = (sum * 10) + digit;
    }
    return sum;
}

/*
Rotates the key left by by 1 character, this is in order to make sure
the cipher is enciphering by the correct key every time it gets an 
input from the user.
*/
void rotate_key(void){
    uint8_t u8_key_length = 22;
    uint8_t u8_j;
    uint8_t temp = key[0];
    for(u8_j = 0; u8_j < u8_key_length-1; u8_j++){
        key[u8_j] = key[u8_j+1];
    }
    key[u8_j] = temp;
}

void rotate_key2(void){
    uint8_t u8_key_length = 21;
    uint8_t u8_j;
    uint8_t temp = key2[0];
    for(u8_j = 0; u8_j < u8_key_length-1; u8_j++){
        key2[u8_j] = key2[u8_j+1];
    }
    key2[u8_j] = temp;
}

//Rotates the key by a certain amount siginifed by the uint16_t num variable
void rotate_key_left_by(uint16_t num){
    for(int i = 0;i < num; i++){
        rotate_key();
    }
}

//Encyphers/Decyphers the input from the user using the Vigenere Cipher
uint8_t encypher(uint8_t u8_text_to_encrypt){
    
    uint8_t u8_output = 0;
    //Converts the input to upper case to make the ascii math simpler
    u8_text_to_encrypt = toupper(u8_text_to_encrypt); 

    u8_output = ((u8_text_to_encrypt + key[0]) % 26) + 'A';
    rotate_key();
    return u8_output;

}

uint8_t decypher(uint8_t u8_text_to_encrypt)
{
    uint8_t u8_output = 0;
    //Converts the input to upper case to make the ascii math simpler
    u8_text_to_encrypt = toupper(u8_text_to_encrypt); 
    u8_output = ( ((u8_text_to_encrypt - key2[0]) + 26) % 26) + 'A';
    rotate_key2();
    return u8_output;
}

//Resets the key array to the original key
void reset_key(void){
    uint8_t au8_temp_key[] = "BOLDFEARLESSCONFIDENT";
    
    for(int i = 0; i < 22; i++){
        key[i] = au8_temp_key[i];
    }

    for(int i = 0; i < 22; i++){
        key2[i] = au8_temp_key[i];
    }
}


