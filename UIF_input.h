#ifndef _input_
#define _input_

//includes
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h> //to work with stroul() error checking->ERANGE is declared here
#include "UIF_dataTypes.h" 

//function prototypes
s_pixel *read_from_ppm(FILE *input_ppm, u_int *color_depth, u_int *width, u_int *height);

//defines
#define MAX_STRING_LEN 20 

#endif