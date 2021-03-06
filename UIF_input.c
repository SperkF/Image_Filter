#include "UIF_input.h"


//parameters: input_ppm..pointer to FILE to read from; color_depth, witdh, and height ->quasi returns (take poiter, and manipulate value at position)
//returns pointer to first position of array whose elements represent all the pixels of the ppm (array-space is allocated inside function)
s_pixel *read_from_ppm(FILE *input_ppm, u_int *color_depth, u_int *width, u_int *height)
{
#if FS_DEBUG
    fprintf(stderr,"\t-read_from_ppm- entered\n");
#endif
//variables for the various loops and ifs()
    int c = 0;
    unsigned int array_element = 0;
    int string_hit = 0, string_cnt = 0;
    char pixel_cnt = 0; //counts from 0 to 2 ->0= red information, 1 = green information, 2 = blue inforamtion of one pixel
    char *str_end = NULL; //to work with strtoul
    s_pixel *ppm_as_array = NULL;
    char arg[MAX_STRING_LEN]; //holds each word of the ppm-file during reading
 
//read throught the ppm-file skip comments, process header and pixels
    while ((c = fgetc(input_ppm)) != EOF)
    {
        if(isspace(c) == 0)
        {
            int i = 0;
            ungetc(c, input_ppm); //push character we pulled from file to id EOF condition back to stream   
            while (i < (MAX_STRING_LEN-1))
            {
                arg[i] = fgetc(input_ppm);
                if(isspace((int)arg[i]) != 0)
                {
                    arg[i] = '\0';
                    break;
                }
                if(arg[i] == '#')
                {
                    arg[i] = '\0';
                    for( ; fgetc(input_ppm) != '\n'; );
                    break;
                }
                i++;
            }
            if(arg[i] != '\0') //don´t set an other null-termination character if one has already been set
            {
                arg[i+1] = '\0';
            }
            i = 0; //reset i

            if(strlen(arg) >= 1) //filter out empty strings (empty string <= 1, everything elese is hight)
            {
            #if INPUT_FS_DEBUG
                printf("current string: |%s|\n",arg);
            #endif
                //strcpy(arg, "P3");
        //identify magic number P3..if P6 was entered ->ERROR message this format is not supported by program, otherwise(if P3 is magic-no, no action needs to be taken)
                if(string_cnt == 0 && strncmp(&arg[0], "P6", 3) == 0) //magic number is P6 ->not supported
                {
                    fprintf(stderr,"ERROR: P6 ppm-image format is not supported by program\n");
                    exit(EXIT_FAILURE); //termiate program clean
                }
                if(string_cnt == 0 && strncmp(&arg[0], "P3", 3) == 0) //magic number is P3
                {
                #if INPUT_FS_DEBUG
                    printf("magic no found\n");
                #endif
                    string_hit = 1;
                }
//the first two number-string inside the ppm file are the width and height of the picture
                if(string_cnt == 1 && arg[0] != '\0') //read in height
                {
                //*width ->because we manipulate value that is pointed to by a pointer
                    *width = (unsigned int)strtoul(&arg[0],&str_end,10);
                /*check if strol() run proper*/
                //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                    if(*str_end != '\0')
                    {
                        fprintf(stderr,"ERROR: problem with strtoul() of width and character: %c can`t be converted\n",*width);
                        exit(EXIT_FAILURE);
                    }
                //empty string was passed on to stroul
                    if(arg[0] == *str_end)
                    {
                            fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in width\n");
                            exit(EXIT_FAILURE);
                    }
                //converted value falls out of range 
                    if (errno == ERANGE)
                    {
                        fprintf(stderr,"range error when reading in width");
                        exit(EXIT_FAILURE);
                        errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                    }


                    string_hit = 1;
                }

                if(string_cnt == 2 && arg[0] != '\0') //read in height
                {
                    *height = (unsigned int)strtoul(&arg[0],&str_end,10);
                /*check if strol() run proper*/
                //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                    if(*str_end != '\0')
                    {
                        fprintf(stderr,"ERROR: problem with strtoul() of height and character: %c can`t be converted\n",*width);
                        exit(EXIT_FAILURE);
                    }
                //empty string was passed on to stroul
                    if(arg[0] == *str_end)
                    {
                            fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in height\n");
                            exit(EXIT_FAILURE);
                    }
                //converted value falls out of range 
                    if (errno == ERANGE)
                    {
                        fprintf(stderr,"range error when reading in height");
                        exit(EXIT_FAILURE);
                        errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                    }

                    string_hit = 1;
                #if INPUT_FS_DEBUG
                    printf("format-height: %d\n",*height);
                    printf("format-width: %d\n",*width);
                #endif
                        //allocate array with fitting space to later save the pixels
                #if INPUT_FS_DEBUG
                    printf("\tarray space is allocated\n");
                #endif
                    ppm_as_array = (s_pixel*)calloc( (*height) * (*width), sizeof(s_pixel));
                    if(ppm_as_array == NULL)
                    {
                        fprintf(stderr,"ERROR: problem with array allocation iside input.c\n");
                        exit(EXIT_FAILURE);
                    }
                }


                if(string_cnt == 3)
                {
            //read in color depth
                    *color_depth = (unsigned int)strtoul(&arg[0],&str_end,10);
                /*check if strol() run proper*/
                //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                    if(*str_end != '\0')
                    {
                        fprintf(stderr,"ERROR: problem with strtoul() of color_depth and characrter: %c can`t be converted\n",*width);
                        exit(EXIT_FAILURE);
                    }
                //empty string was passed on to stroul
                    if(arg[0] == *str_end)
                    {
                            fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in color_depth\n");
                            exit(EXIT_FAILURE);
                    }
                //converted value falls out of range 
                    if (errno == ERANGE)
                    {
                        fprintf(stderr,"range error when reading in color_depth\n");
                        exit(EXIT_FAILURE);
                        errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                    }

                //check if color depth is in valid range
                    //see ppm-specifications: http://netpbm.sourceforge.net/doc/ppm.html
                //The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536 and more than 0 (this is guaranteed as color_depth is unsigned so no negativ value possible anyway)
                    if(*color_depth > 65536)
                    {
                        fprintf(stderr,"ERROR: color_depth of ppm is not in valid range 0..65536\n");
                        exit(EXIT_FAILURE);            
                    }
                    string_hit = 1;
                #if INPUT_FS_DEBUG
                    printf("color_depth: %i\n",*color_depth);
                #endif
                }

//now read pixels to allocated array
                if(string_cnt >= 4)
                {
                    //static variable: holds value from function call/sub-routine to function call/sub-routine and is initalized with 0 upon first call
                    static char pixel_cnt_reset; //numeric value, but char saves memory space compared to int
    //read in red pixel value
                    if(pixel_cnt == 0)
                    {

/* stroul()  ->string to unsigned long (long...long integer)
general syntax:
    unsigned long strtoul( const char *str, char **str_end, int base );
parameters:
    # str...pointer to the null-terminated byte string to be interpreted 
    # str_end...pointer to a pointer to character. 
    # base...base of the interpreted integer value 
return:
    # on success::  Integer value corresponding to the contents of str
    # on failure::  see error flags
error-flags:
    # If the converted value falls out of range of corresponding return type, range error occurs (errno is set to ERANGE) 
      and ULONG_MAX or ULLONG_MAX is returned. 
    # If no conversion can be performed, ​0​ is returned.
    # if a character in the string is not a vlid number of the given base (dec, hex, bin, oc,..) stroul stops the conversion 
      and returns the value of the number represented by the string that it has read up to the false characrter + str_end points 
      points to the false character (under a perfect read str_end should point to '\0')
*/
                        ppm_as_array[array_element].red = (unsigned int)strtoul(&arg[0],&str_end,10);              
                    /*check if strol() run proper*/
                    //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                        if(*str_end != '\0')
                        {
                            fprintf(stderr,"ERROR: problem with strtoul() of red pixel value, characrter: %c can`t be converted\n",\
                                ppm_as_array[array_element].red);
                            exit(EXIT_FAILURE);
                        }
                    //empty string was passed on to stroul
                        if(arg[0] == *str_end)
                        {
                                fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in red pixel value\n");
                                exit(EXIT_FAILURE);
                        }
                    //converted value falls out of range 
                        if (errno == ERANGE)
                        {
                            fprintf(stderr,"range error when reading in red pixel value\n");
                            exit(EXIT_FAILURE);
                            errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                        }

                    //check if pixel value is in valid range
                        if(ppm_as_array[array_element].red > *color_depth)
                        {
                            fprintf(stderr,"ERROR: red color value in pixel No: %i is out of range(0..%i)\n", (array_element+1), *color_depth);
                            exit(EXIT_FAILURE);            
                        }
                    }
    //read in green pixel value
                    if(pixel_cnt == 1)
                    {
                        ppm_as_array[array_element].green = (unsigned int)strtoul(&arg[0],&str_end,10);
                    /*check if strol() run proper*/
                    //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                        if(*str_end != '\0')
                        {
                            fprintf(stderr,"ERROR: problem with strtoul() of green pixel value, characrter: %c can`t be converted\n",\
                                ppm_as_array[array_element].green);
                            exit(EXIT_FAILURE);
                        }
                    //empty string was passed on to stroul
                        if(arg[0] == *str_end)
                        {
                                fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in green pixel value\n");
                                exit(EXIT_FAILURE);
                        }
                    //converted value falls out of range 
                        if (errno == ERANGE)
                        {
                            fprintf(stderr,"range error when reading in green pixel value\n");
                            exit(EXIT_FAILURE);
                            errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                        }

                    //check if pixel value is in valid range
                        if(ppm_as_array[array_element].green > *color_depth)
                        {
                            fprintf(stderr,"ERROR: green color value in pixel No: %i is out of range(0..%i)\n", (array_element+1), *color_depth);
                            exit(EXIT_FAILURE);            
                        }
                    } 
    //read in blue pixel value
                    if(pixel_cnt == 2)
                    {
                        ppm_as_array[array_element].blue = (unsigned int)strtoul(&arg[0],&str_end,10);
                    /*check if strol() run proper*/
                    //str_end == '\0', if not \0.. conversion failed ->problem character is pointet to by str_end
                        if(*str_end != '\0' && *str_end != EOF) //*str_end != EOF ...to avoid error when last element of ppm-file is read in
                        {
                            fprintf(stderr,"ERROR: problem with strtoul() of blue pixel value, characrter: %c can`t be converted\n",\
                                ppm_as_array[array_element].blue);
                            exit(EXIT_FAILURE);
                        }
                    //empty string was passed on to stroul
                        if(arg[0] == *str_end)
                        {
                                fprintf(stderr,"ERROR: empty string was passed on to strtoul() when reading in blue pixel value\n");
                                exit(EXIT_FAILURE);
                        }
                    //converted value falls out of range 
                        if (errno == ERANGE)
                        {
                            fprintf(stderr,"range error when reading in blue pixel value\n");
                            exit(EXIT_FAILURE);
                            errno = 0; //reset error identifer ->not needed in this case as be terminate the program one line above
                        }

                    //check if pixel value is in valid range
                        if(ppm_as_array[array_element].blue > *color_depth)
                        {
                            fprintf(stderr,"ERROR: blue color value in pixel No: %i is out of range(0..%i)\n", (array_element+1), *color_depth);
                            exit(EXIT_FAILURE);            
                        }
                           
                        #if INPUT_FS_DEBUG
                        //print current pixel ->every 3rd read a new pixel is printed and also print current array-element position
                            printf("Array element: %i\t",array_element);
                            printf("PIXEL: %u, %u, %u\n\n",ppm_as_array[array_element].red, ppm_as_array[array_element].green, ppm_as_array[array_element].blue);
                        #endif  
                    //reset pixel count and ikrement array position by one (next element -consisting of 3 variables(struct) can be read in)
                        pixel_cnt = 0;
                        pixel_cnt_reset = 1;
                        array_element++;
                    }
                //only increment pixel_cnt if it was`t just resetet
                    if(pixel_cnt_reset == 1) 
                    {
                        pixel_cnt_reset = 0;
                    }
                    else
                    {
                        pixel_cnt++;
                    }
                    
                }
                if(string_hit == 1)
                {
                    string_cnt++;
                }
            }

        }
    }
#if 0
    if (feof(input_ppm))         // hit end of file
    {
        fclose(input_ppm); //not realy necessary to close strem of file that was opened in read-mode
#if FS_DEBUG
    fprintf(stdout,"EOF succesfull encountered\n");
#endif
    }
    else         // some other error interrupted the read
    {
        fprintf(stderr,"ERROR: read from file failed, read was interupted by something other than EOF encounter\n");
        exit(EXIT_FAILURE);
    }
#endif
//check if size matches with elements inside the ppm ->to avoid writing over array end
    //to many pixels
#if INPUT_FS_DEBUG
    printf("array element: %i, format: %i (%i x %i)\n", array_element, (*width) * (*height), *width, *height);
#endif
    if(array_element > (*width) * (*height))
    {
        fprintf(stderr,"ERROR: There are more pixel informations than specified by the format in the given file\n");
        exit(EXIT_FAILURE);
    }
    //to few pixels
    if(array_element < (*width) * (*height))
    {
        fprintf(stderr,"ERROR: There are less pixel informations than specified by the format in the given file\n");
        exit(EXIT_FAILURE);
    }
#if FS_DEBUG
    fprintf(stderr,"\t-read_from_ppm- exited , color_depth:%u, width: %u, height:%u\n", *color_depth, *width, *height);
#endif
    return ppm_as_array; //return pointer to first pos. of array that holds all pixel infos of the input ppm-file
}