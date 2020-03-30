#ifndef _uifOutput_
#define _uifOutput_
//includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>  //to work with time()
#include "UIF_dataTypes.h" 

//function prototypes
void print_ppm(FILE *output_ppm, s_pixel *array, const u_int color_depth, const u_int width, const u_int height, const int *kernel);
//inside function do:
    //print ppm-head
    //print pixels with help_pointer
    //fclose(output_ppm)
    //free() allocated array space at the end with original pointer that was given
        //return the freed pointer ->should point to NULL if everything was succesfull;



#endif