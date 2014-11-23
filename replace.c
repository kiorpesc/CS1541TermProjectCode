/**
 * File: replace.c
 * Author: Charles Kiorpes
 * Date: November 18th, 2014
 *
 * Takes in two files: a text file and a pattern file (CSV, one line)
 * For each pair of patterns in the CSV file, the program finds the 
 *   first pattern in the text and replaces it with the second pattern.
 **/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
 
#define PATTERN_BUFFER_SIZE   256


// Code found at DaniWeb.com - user Dave Sinkula
// https://www.daniweb.com/software-development/c/code/216517/strings-search-and-replace
/*
* Description:
* Find and replace text within a string.
*
* Parameters:
* src (in) - pointer to source string
* from (in) - pointer to search text
* to (in) - pointer to replacement text
*
* Returns:
* Returns a pointer to dynamically-allocated memory containing string
* with occurences of the text pointed to by 'from' replaced by with the
* text pointed to by 'to'.
*/
char *replace(const char *src, const char *from, const char *to)
{
  /*
  * Find out the lengths of the source string, text to replace, and
  * the replacement text.
  */
  size_t size = strlen(src) + 1;
  size_t fromlen = strlen(from);
  size_t tolen = strlen(to);

  //Allocate the first chunk with enough for the original string.
  char *value = malloc(size);

  //We need to return 'value', so let's make a copy to mess around with.
  char *dst = value;

  // Was malloc successful?
  if ( value != NULL )
  {
    // keep looping until no more matches
    for ( ;; )
    {
      const char *match = strstr(src, from);
      if ( match != NULL )
      {
        size_t count = match - src;
        char *temp;

        // adjust the realloc size based on
        // the lengths of the to and from strings
        size += tolen - fromlen;
        temp = realloc(value, size);

        // if realloc failed, return NULL
        if ( temp == NULL )
        {
          free(value);
          return NULL;
        }

        dst = temp + (dst - value);
        value = temp;

        memmove(dst, src, count);
        src += count;
        dst += count;

        memmove(dst, to, tolen);
        src += fromlen;
        dst += tolen;
      }
      else /* No match found. */
      {
        strcpy(dst, src);
        break;
      }
    }
  }
  return value;
}
// End DaniWeb code

// driver code
int main(void) 
{
  struct timespec start_time, end_time;
  long elapsed_sec, elapsed_nsec;
  int retVal;
  char text_filename[] = "text.txt";
  char pattern_filename[] = "pattern.txt";
  FILE *text_fp;
  FILE *pattern_fp;
  struct stat file_stats;
  off_t text_size;
  off_t pattern_size;  

 clock_gettime(CLOCK_REALTIME, &start_time);

  // get size of input file for buffer size
  stat(text_filename, &file_stats);
  text_size = file_stats.st_size;

  printf("Opening text file.\n");

  // open text file
  text_fp = fopen("text.txt", "r+");

  // load text into buffer
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

  // get the size of the pattern file to appropriately size the buffer
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

  // begin parsing pattern file for patterns, using strsep
  char ** next_token = &pattern_buffer;
  char * find_pattern = strsep(next_token, ",");
  char * replace_pattern;
  char * output_buffer;
  char * temp;

  // loop through the patterns and replace each
  while(find_pattern != NULL) {
    replace_pattern = strsep(next_token, ",");  // get second half of equation
    printf("%s,%s\n", find_pattern, replace_pattern); // a little debug now and then...
    output_buffer = replace(text_buffer, find_pattern, replace_pattern);  // new string will be in memory
    if(output_buffer == NULL){
      // were unable to allocate space
      fclose(text_fp);
      fclose(pattern_fp);
      return 1;
    }
    temp = text_buffer;
    text_buffer = output_buffer;
    free(temp); // remove old buffer from memory
    find_pattern = strsep(next_token, ",");
  }

  printf("checking length of output\n");
  int write_len = strlen(text_buffer);

  // write output to new file
  FILE * output_file;
  output_file = fopen("replace_output.txt", "w");
  fwrite(text_buffer, 1, write_len, output_file);

  // clean up
  fclose(text_fp);
  fclose(pattern_fp);
  fclose(output_file);
  free(text_buffer);
  free(pattern_buffer);
  
  clock_gettime(CLOCK_REALTIME, &end_time);

  elapsed_sec = end_time.tv_sec - start_time.tv_sec;
  elapsed_nsec = end_time.tv_nsec - start_time.tv_nsec;

  double elapsed_total = elapsed_sec + (elapsed_nsec/1000000000.0); 

  printf("Elapsed Time: %f sec\n", elapsed_total);
}
