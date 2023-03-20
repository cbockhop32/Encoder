#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <err.h>




int main(int argc, char* argv[]){
  static char const alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/=";

  const char* fname = argv[1];

  FILE* input;
 
  if(argc > 2) {
    fprintf(stderr,"Error: Invalid number of arguements\n");
    err(errno=EINVAL, "Usage %s [FILE]",argv[0]);
  }

  if(fname == NULL && argc == 1) {
    input = stdin;
  } else if(strcmp(fname, "-") == 0) {
    input = stdin;

  } else {
      input = fopen(fname, "r");

      if(input == NULL) {
        perror("File opening failed");
        return EXIT_FAILURE;
      }
  }

  uint32_t output_count = 0;
  uint8_t out_idx[4];
  int outbuff_size = sizeof out_idx / sizeof out_idx[0];

    //reads 3 blocks of data of 1 byte each(size of char)
   while(1){
      uint8_t file_buffer[3] = {0};
      
      size_t read_amt = fread(file_buffer, sizeof *file_buffer , (sizeof file_buffer)/(sizeof *file_buffer), input);
      
    
      if (read_amt >= 1) {
        out_idx[0] = file_buffer[0] >> 2;
        out_idx[1] = ((file_buffer[0] & 0x03u) << 4) | (file_buffer[1] >> 4);
        out_idx[2] = ((file_buffer[1] & 0x0Fu) << 2) | (file_buffer[2] >> 6);
        out_idx[3] = (file_buffer[2] & 0x3Fu);
      
        //Adding padding "="
        //Only 1 byte was entered
        if(read_amt == 1) {
          out_idx[2] = (sizeof alphabet)-2;
          out_idx[3] = (sizeof alphabet)-2; 
        }
         
        //Only 2 bytes were entered
        if(read_amt == 2) {
          out_idx[3] = sizeof(alphabet)-2;
        } 

        for(int i = 0; i < outbuff_size; ++i){
          char conv = alphabet[out_idx[i]];
          printf("%c", conv);

          output_count++;

          if((output_count % 76) == 0) { 
            putchar('\n');
          }


          if(fflush(stdout) != 0){
            break;
          }; 
        }

      } else {
          if(ferror(input)) {
            fprintf(stderr,"%s","Error Reading File");
            return EXIT_FAILURE;
          }
          else if(feof(input)) {           
            break;
          }
        }        
   }
    
    
    if(output_count != 0) {
      putchar('\n');
    }
    
    fclose(input);

  return EXIT_SUCCESS;
}

