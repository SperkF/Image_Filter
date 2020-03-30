#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h> //to work with round() ->link math librarly upon compiling: -lm
#include <unistd.h>  //to work with getopt()
#include "UIF_dataTypes.h"
#include "UIF_input.h"
#include "UIF_output.h"


//Defines 
#define MAX_ARG_LENGTH 20 
#define MAX_KERNEL_SIZE 25
#define SET 1


s_pixel *create_frame(s_pixel *ppm_as_array, const u_int img_height, const u_int img_width, const u_int frame_size);
double value_of(u_int array_element); //function takes value of array and retunrns it ->this way original array value is left unmanipulated when kernel is run over it
s_pixel *apply_kernel(const s_pixel *original_array, const int *kernel, const u_int height, const u_int width, const u_int color_depth, const u_int frame_size);
void print_help(void);


int main (int argc, char *argv[])
{

//varialbes to hold info from ppm-header
    u_int  color_depth = 0, height = 0, width=0; //these values are taken from header of input ppm file
//pointer to first positon of allocated array (hols all pixel values form ppm)
    s_pixel *ppm_without_frame = NULL; //original .ppm read from file
    s_pixel *ppm_with_frame = NULL; //original ppm with frame 
//for ppm read and write
    FILE *input_ppm = NULL; 
    FILE *output_ppm = NULL;
//to process kernel input
    int *kernel = NULL;
    int kernel_size = 0;
    int frame_size = 0;
    int i = 0, comma_cnt = 0;
    int arg_cnt = 0;
//variable to take getopt() return
    int getopt_return = 0;
//to work with strtod()
    char *strtod_error = NULL;
    char *strtod_input = NULL;


//-------getopt() to process input---------------------
//run getopt till getopt() return = -1, otherwise returns value of identifier [ASCII Table value]
    //each loop cycle getop() is called and returns the current identifier (beginns with i, next call->o, then k,..)
    //if there is ":" behind the identifier, getopt reads the string following the identifier and writes it to optarg (global char array ->null terminated sting)
    #if FS_DEBUG
        fprintf(stdout,"start of getopt()");
    #endif
    while ((getopt_return = getopt (argc, argv, "i:o:k:")) != -1)
    switch (getopt_return)
    {
//----------------------------------------------------------------------
//identifier -i... string that follows is name of ppm_file that the kernel is to applyed on
        case 'i':
            arg_cnt++; //erhöhe den arg_cnt um eins
        //check if user has entered correct syntax ->.ppm(\0) Endung sind die letzten 5 charactere
            //übergib an strncmp() also den string erst ab der viert-letzten Stelle &filename[Stelle]
            if(strncmp(&optarg[strlen(optarg)-4], ".ppm",5) != 0)
            {
                strcat(optarg, ".ppm"); //hänge .ppm Endung an falls noch keine da ist
            }
        //use fopen()
            input_ppm = fopen(optarg,"r"); //optarg -> holds string that followed the -i 
            if(input_ppm == NULL)
            {
                fprintf(stderr,"ERROR: fopen() for input_ppm failed\n");
                exit(EXIT_FAILURE);
            }
            else
            {
#if FS_DEBUG
        fprintf(stdout,"fopen() of input_ppm file worked\n");
#endif         
            }
            break;
//-----------------------------------------------------------
//identifier -o... string that follows is name of ppm_file that is to be created, and which holds the result of the claculations
        case 'o':
            arg_cnt++; //erhöhe den arg_cnt um eins
        //fopen() in w-mode ...we need to use fclose() later on
            if(strncmp(&optarg[strlen(optarg)-4], ".ppm",5) != 0)
            {
                strcat(optarg, ".ppm"); //hänge .ppm Endung an falls noch keine da ist
            }

            if( (output_ppm = fopen(optarg,"w")) == NULL)
            {
                fprintf(stderr,"ERROR: fopen() failed\n");
                exit(EXIT_FAILURE);
            }
            break;
//-----------------------------------------------------------
//identifier -o... string that follows is kernel which is expected to look somewhat like this: -1,-1,-1,-1,8,-1,-1,-1,-1 (9 int number, seperated by ',')
        case 'k':
            arg_cnt++; //erhöhe den arg_cnt um eins
        //check if no wrong character was entered + check if 9 numbers and 8 ',' were entered
            while(optarg[i] != '\0')  //parse throug optarg ->NULL-terminated string
            {
            // is not a number && is not '-' && is not ',' && is not ' '->ERROR
                if(isdigit((int)optarg[i]) == 0 && optarg[i] != '-' && optarg[i] != ',')
                {
                    fprintf(stderr,"ERROR: character %c of kernel can`t be processed\n",optarg[i]);
                    exit(EXIT_FAILURE);
                }           
                if(optarg[i] == ',')
                {
                    comma_cnt++;
                }
                i++; //move to next array position
            }
            if(comma_cnt != 8 && comma_cnt != 24) //dependat wehter user has entered a 3x3 kernel or a 5x5 kernel
            {
                fprintf(stderr,"ERROR: kernel contained the wrong amomount of commas to seperate the numbers, ammount of commas: %i\n",comma_cnt);
                fprintf(stderr,"Or kernel contained spaces, in this case only characters till space were read\n");
                exit(EXIT_FAILURE);
            }

            kernel_size = comma_cnt+1;

        //now that we know that the kernel has the right syntax, we process it
            i = 0; //reset array position
            if( (kernel = calloc(kernel_size, sizeof(int))) == NULL)
            {
                fprintf(stderr,"ERROR with kernel callocl()\n");
                exit(EXIT_FAILURE);
            }
            strtod_input = &optarg[0];
            while(i < kernel_size)
            {
                kernel[i] = (int)strtod(strtod_input, &strtod_error);
                #if FS_DEBUG
                    printf("inside while():%i || kernel: %i || strtod_error: %c\n",i, kernel[i], *strtod_error);
                #endif
    //if a comma is encountered ->move pointer and ikrement i; also incrment by one if string-termination('\0') is encountered
                if(*strtod_error == ',' || *strtod_error == '.' || *strtod_error == '\0') 
                {
                    strtod_error++; //move pointer by one position
                    strtod_input = strtod_error; //let input pointer point to new position
                    i++;
                }
            }
            break;
//-----------------------------------------------------------
        default:
            fprintf(stderr,"ERROR: wrong identifiers entered\n");
            print_help();
    }
    if(arg_cnt != 3)
    {
        fprintf(stderr,"ERROR: wrong ammount of argumetents entered\n");
        print_help();
    }

//========================================================================================
/*                                                                                      *
 * //--------end of getopt()-----------------------                                     *
 *                                                                                      */
//========================================================================================


//call function read_from_ppm()
    //retunrs pointer to first position of array that holds all pixels of ppm, adresses of color_depth; width; height ->quasi returns 
    //space for array is allocated inside the function ->remeber to free()  
    ppm_without_frame = read_from_ppm(input_ppm, &color_depth, &width, &height);
    if(ppm_without_frame == NULL)
    {
        fprintf(stderr,"ERROR: read_from_ppm() retunred NULL-pointer\n");
        exit(EXIT_FAILURE);
    }

    frame_size = (int)((sqrt(kernel_size)-1)/2);
    #if FS_DEBUG
        printf("frame size: %i\n\n",frame_size);
    #endif
//call function create_frame()
    if( (ppm_with_frame = create_frame(ppm_without_frame, height, width, frame_size)) == NULL)
    {
        fprintf(stderr,"ERROR: create_fram() returned NULL\n");
        exit(EXIT_FAILURE);
    }
    
//call function ->s_pixel *apply_kernel(const s_pixel *array, const int *kernel, const u_int height, const u_int width, const u_int color_depth)
    ppm_without_frame = apply_kernel(ppm_with_frame, &kernel[0], height, width, color_depth, frame_size);
    
    print_ppm(output_ppm, ppm_without_frame, color_depth, width, height, &kernel[0]);
//fclose() of all file-streams opened in write mode
    //check return ->​0​ on success, EOF otherwise.
    if(fclose(output_ppm) != 0)
    {
        fprintf(stderr,"ERROR: fclose() of output_ppm failed\n");
        exit(EXIT_FAILURE);
    }
//free() allocated memory space for our two arrays that hold ppm-picture + 1 array for kernel
    free(ppm_without_frame);
    free(ppm_with_frame);
    free(kernel);
    return 0; 
}


void print_help(void)
{
    fprintf(stdout,"\nPURPOSE of this program::\n");
    fprintf(stdout,"\ttake a ppm_file as input, apply a given kernel to it and create a file that holds the result\n");
    fprintf(stdout,"\nSYNTAX:: ");
    fprintf(stdout,"UIF.exe -i input_ppm -o output_ppm -k kernel\n");
    fprintf(stdout,"\t>UIF.exe.....name of program\n");
    fprintf(stdout,"\t>input_ppm...name of file that kernel is to be applied on\n");
    fprintf(stdout,"\t>output_ppm..name of file that is going to be created, and will hold the result of kernel applied to input_ppm\n");
    fprintf(stdout,"\t>kernel......9 integer numbers seperated by 8 commas (=kernel)\n");
    exit(EXIT_FAILURE);
}

//depending on kernel size, this function creates a frame around the original ppm-pictur
    //kernel = 3x3 ->frame is one pixel wide; kernel = 5x5 ->frame is 2 pixel wide
    //frame is initalized to hold all -0- (done by allocations space with calloc() that automaically allocated 0-initilized memory)
s_pixel *create_frame(s_pixel *ppm_as_array, const u_int img_height, const u_int img_width, const u_int frame_size)
{
    s_pixel *ppm_with_frame = NULL;
    s_pixel *help = NULL;

    //allocate array space for new array that than hold original ppm (pp_as_array) + a frame that holds values zero
        //using calloc() ->all allocated arrayspace is automatically initilaized with zero
    if( (ppm_with_frame = calloc( (img_height+(2*frame_size))*(img_width+(2*frame_size)), sizeof(s_pixel))) == NULL) 
    {
        fprintf(stderr,"ERROR: calloc() inside create_frame() failed\n");
        exit(EXIT_FAILURE);
    } 

    help = ppm_with_frame; //help pointer points to first pos of ppm_with_frame ->needed because ppm_with_frame is moved in following for-loop

    for(u_int row = 1; row <= img_height+(2*frame_size); row++)
    {
        for(u_int col = 1; col <= img_width+(2*frame_size); col++)
        {
            //only manipulate pixels if you are not in: first row, last row, first collumn and last collum
            if(row != 1 && row != (img_height+(2*frame_size)) && col != 1 && col != (img_width+(2*frame_size)))
            {
                ppm_with_frame->red = ppm_as_array->red;
                ppm_with_frame->green = ppm_as_array->green;
                ppm_with_frame->blue = ppm_as_array->blue;
                ppm_as_array++; //move ppm_as_arry one pos
            }
            ppm_with_frame++; //move frame array one pos
        }
    }

    ppm_with_frame = help; //we could return -help- right away, but this approach aids in readability
    return ppm_with_frame;
}   

/*
|1 2 3|     |a d g|
|4 5 6| *   |b e h| 
|7 8 9|     |c f i|
new_pixel_val = roud( ( (1/255)*a + (2/255)*b + (3/255)*c + (4/255)*d + (5/255)*e +(6/255)*f + (7/255)*g + (8/255)*h + (9/255)*i ) *255 )
*/
s_pixel *apply_kernel(const s_pixel *original_array, const int *kernel, const u_int height, const u_int width, const u_int color_depth, const u_int frame_size)
{
    #if FS_DEBUG
        printf("apply_kernel() entered\n");
    #endif
    double new_col_val = 0;
    s_pixel *manipulated_array = NULL;
    s_pixel *help_manipulated_array = NULL; //to later be able to retunr first pos of manipulated array
    manipulated_array = calloc( ((width)*(height)) ,sizeof(s_pixel));
    if(manipulated_array == NULL)
    {
        fprintf(stderr,"ERROR: inside apply_kernel() calloc() for manipulated_array failed\n");
        exit(EXIT_FAILURE);
    }   

    help_manipulated_array = manipulated_array; //let help_manipulated_array point to first pos of manipulated_array 
    if(help_manipulated_array == NULL)
    {
        fprintf(stderr,"ERROR: inside apply_kernel() pointer refference of help_manipulated_array to manipulated_array failed\n");
        exit(EXIT_FAILURE);
    }   

    for(u_int row = 1; row <= height+(frame_size*2); row++)
    {
        for(u_int col = 1; col <= width+(frame_size*2); col++)
        {
            if(row != 1 && row != (height+(frame_size*2)) && col != 1 && col != (width+(frame_size*2)))
            {
        //calcualte for red value of pixel
                new_col_val = round(
                            ((value_of((original_array-width-1)->red)/255) * (*(kernel)) + (value_of((original_array-width)->red)/255) * (*(kernel+3)) + (value_of((original_array-width+1)->red)/255) * (*(kernel+6)) \
                            +(value_of((original_array-1)->red)/255) * (*(kernel+1)) + (value_of((original_array)->red)/255) * (*(kernel+4)) + (value_of((original_array+1)->red)/255) * (*(kernel+7)) \
                            +(value_of((original_array+width-1)->red)/255) * (*(kernel+2)) +(value_of((original_array+width)->red)/255) * (*(kernel+5)) + (value_of((original_array+width+1)->red)/255) * (*(kernel+8)) \
                            )*255 \
                                );
            //if color value is neagtiv
                if( new_col_val < 0)
                    manipulated_array->red = 0;
            //else if color value is bigger than max color_depth
                if( new_col_val > color_depth)
                    manipulated_array->red = color_depth;
            //neither smaller than 0, nor bigger than color_depth ->save value direct
                if( 0 < new_col_val && new_col_val < color_depth)
                    manipulated_array->red = (u_int)new_col_val;
        //calcualte for green value of pixel
                new_col_val = round(
                            ((value_of((original_array-width-1)->green)/255) * (*(kernel)) + (value_of((original_array-width)->green)/255) * (*(kernel+3)) + (value_of((original_array-width+1)->green)/255) * (*(kernel+6)) \
                            +(value_of((original_array-1)->green)/255) * (*(kernel+1)) + (value_of((original_array)->green)/255) * (*(kernel+4)) + (value_of((original_array+1)->green)/255) * (*(kernel+7)) \
                            +(value_of((original_array+width-1)->green)/255) * (*(kernel+2)) +(value_of((original_array+width)->green)/255) * (*(kernel+5)) + (value_of((original_array+width+1)->green)/255) * (*(kernel+8)) \
                            )*255 \
                                );
            //if color value is neagtiv
                if( new_col_val < 0)
                    manipulated_array->green = 0;
            //else if color value is bigger than max color_depth
                if( new_col_val > color_depth)
                    manipulated_array->green = color_depth;
            //neither smaller than 0, nor bigger than color_depth ->save value direct
                if( 0 < new_col_val && new_col_val < color_depth)
                    manipulated_array->green = (u_int)new_col_val;

        //calcualte for blue value of pixel
                new_col_val = round(
                            ((value_of((original_array-width-1)->blue)/255) * (*(kernel)) + (value_of((original_array-width)->blue)/255) * (*(kernel+3)) + (value_of((original_array-width+1)->blue)/255) * (*(kernel+6)) \
                            +(value_of((original_array-1)->blue)/255) * (*(kernel+1)) + (value_of((original_array)->blue)/255) * (*(kernel+4)) + (value_of((original_array+1)->blue)/255) * (*(kernel+7)) \
                            +(value_of((original_array+width-1)->blue)/255) * (*(kernel+2)) +(value_of((original_array+width)->blue)/255) * (*(kernel+5)) + (value_of((original_array+width+1)->blue)/255) * (*(kernel+8)) \
                            )*255 \
                                );
            //if color value is neagtiv
                if( new_col_val < 0)
                    manipulated_array->blue = 0;
            //else if color value is bigger than max color_depth
                if( new_col_val > color_depth)
                    manipulated_array->blue = color_depth;
            //neither smaller than 0, nor bigger than color_depth ->save value direct
                if( 0 < new_col_val && new_col_val < color_depth)
                    manipulated_array->blue = (u_int)new_col_val;
                #if KERNEL_FS_DEBUG
                    printf("\t\t\tred: %i || green: %i || blue: %i\n",manipulated_array->red, manipulated_array->green, manipulated_array->blue);
                #endif
                 manipulated_array++; //jump one position further 
            }
            
            original_array++; 
        }
    }
    return help_manipulated_array; //first pos of manipulated_array
}

//function takes value of array and retunrns it ->this way original array value is left unmanipulated when kernel is run over it
double value_of(u_int array_element)
{
    return array_element; 
}