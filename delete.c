/**
 * File: delete.c
 * Author: Charles Kiorpes
 * Date: November 18th, 2014
 *
 * Takes in two files: a text file and a pattern file (CSV)
 * This program searches through the text for each pattern in the
 *   CSV file, removing any occurrance of it.
 **/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
 
#define PATTERN_BUFFER_SIZE   256


/**
 * removeSubstring:
 * 
 * args:  s         the string to search int
 *        toremove  the string to remove from s
 *
 * Code found on StackOverflow: 
 *    http://stackoverflow.com/questions/4833347/removing-substring-from-a-string
 **/
void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}
// End StackOverflow code

int main(void) 
{
  int retVal;
  char text_filename[] = "text.txt";
  char pattern_filename[] = "pattern.txt";
  FILE *text_fp;
  FILE *pattern_fp;
  struct stat file_stats;
  off_t text_size;
  off_t pattern_size;  

  // get size of text file for buffer size
  stat(text_filename, &file_stats);
  text_size = file_stats.st_size;

  printf("Opening text file.\n");

  // open text file
  text_fp = fopen("text.txt", "r+");

  char * text_buffer = malloc(text_size + 1); // leave extra byte for now?
  retVal = fread(text_buffer, (size_t)text_size, 1, text_fp);
  if(retVal < 1){
    printf("Error loading file into buffer.\n");
    fclose(text_fp);
    free(text_buffer);
    return 1;
  }

  text_buffer[text_size] = '\0';	// null terminate the file string  
  
  printf("Opening pattern file.\n");

  // get size of pattern file for buffer size
  stat(pattern_filename, &file_stats);
  pattern_size = file_stats.st_size;

  // open pattern file
  pattern_fp = fopen("pattern.txt", "r+");
  printf("Loading pattern file into memory.\n");
 
  char * pattern_buffer = malloc(pattern_size + 1);
  retVal = fread(pattern_buffer, (size_t)pattern_size, 1, pattern_fp);
  if(retVal < 1) {
    printf("Error loading patterns into buffer.\n");
    free(pattern_buffer);
    free(text_buffer);
    fclose(pattern_fp);
    fclose(text_fp);
    return 1;
  }

  pattern_buffer[pattern_size + 1] = '\0';

  // begin parsing pattern file for patterns using strsep
  char ** next_token = &pattern_buffer;
  char * current_pattern = strsep(next_token, ",");

  // loop through the patterns and remove each
  while(current_pattern != NULL) {
    printf("%s\n", current_pattern);
    removeSubstring(text_buffer, current_pattern);
    current_pattern = strsep(next_token, ",");
  }

  printf("checking length of output\n");
  int write_len = strlen(text_buffer);

  // write to new output file
  FILE * output_file;
  output_file = fopen("output.txt", "w");
  fwrite(text_buffer, 1, write_len, output_file);

  fclose(text_fp);
  fclose(pattern_fp);
  fclose(output_file);
  free(text_buffer);
  free(pattern_buffer);

}













