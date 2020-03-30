#ifndef _datatypes_
#define _datatypes_

//defines used in multiple files
    /*Fabian Sperk debug comments 1->aktiv; 0->inaktiv ; name is almost certain not to be present in 
      any libraries therefore no collission to be expected*/
#define FS_DEBUG 0 //0..debug comments deactivated; 1..debug comments activated
#define INPUT_FS_DEBUG 0 //in combination with UIF_input.c ->to avoid printing out every pixel when reading in ppm in "debug mode"
#define KERNEL_FS_DEBUG 0

//user-defined datatypes used in multiple files
/*declaring struct datattype*/
struct pixel{
    unsigned int red;
    unsigned int green;
    unsigned int blue;
}; 
/*use typedef to make declaring variables of type struct pixel easier*/
typedef struct pixel s_pixel;

typedef unsigned int u_int;

#endif