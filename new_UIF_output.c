
#include "UIF_output.h"
//function returns: pointer to array-space that was freed at the end of function ->pointer should point to NULL now
//inside function do:
    //print ppm-head
    //print pixels with help_pointer
    //fclose(output_ppm)
    //free() allocated array space at the end and set pointer array to point to NULL (this pointer is also returned then)
/*print_ppm()
general syntax:
    - s_pixel *print_ppm(FILE *output_ppm, const s_pixel *array, const u_int color_depth, const u_int width, const u_int height)
paramters::
    - output_ppm...FILE pointer to fiel that the ppm-image should be writen to
    - array...holds the pixel values of the array that is going to be printed to output_ppm
    - color_depth...color_depth of ppm-image
    - width/height...widht and height of ppm-image
note::
    - function reads the elements(each element of type -pixel-) of an 1D array(pointed to by -array_pos-) and prints
      them into an ppm-file (to generate a picture)
    - not only the values of the array, are printed, but also a header(needed for every ppm-file)
return::
    - on success: returns NULL
    - on failure: undefined
*/
void print_ppm(FILE *output_ppm, s_pixel *array, const u_int color_depth, const u_int width, const u_int height, const int *kernel)
{

//================================print ppm head =========================================
/*      P3                                                                              *
 *      #ppm_created by Fabian Sperk //simple comment                                   *
 *      format (width height)                                                           *
 *      color_depth                                                                     */
//========================================================================================

    if( fprintf(output_ppm,"P3\n") < 0 ) //magic number
    {
        fprintf(stderr,"ERROR: print of magic-No to output-pmm failed\n");
        exit(EXIT_FAILURE);
    }
//----------
    time_t result = time(NULL);
    if(result == -1)
    {
        fprintf(stderr,"ERROR: time() inside UIF_output.c failed\n");
        exit(EXIT_FAILURE);
    }
    if( fprintf(output_ppm,"#ppm created by Fabian Sperk time upon creation: %s\n",asctime(gmtime(&result))) < 0 )
    {
        fprintf(stderr,"ERROR: create by fprintf() failed\n");
        exit(EXIT_FAILURE);
    }
    if( fprintf(output_ppm,"#kernel applied was: ") < 0 )
    {
        fprintf(stderr,"ERROR: print of kernel comment failed\n");
        exit(EXIT_FAILURE);
    }
    //print kernel values in loop ->kernel holds values as int
    for(int i = 0; i < 9; i++)
        fprintf(output_ppm,"%i ",kernel[i]);
    if( fprintf(output_ppm,"\n%i   %i #img_width * img_height\n",width, height) < 0 )
    {
        fprintf(stderr,"ERROR: print of img width and height comment failed\n");
        exit(EXIT_FAILURE);
    }
    if( fprintf(output_ppm,"%i #color_depth\n",color_depth) < 0 )
    {
        fprintf(stderr,"ERROR: print of color depth comment failed\n");
        exit(EXIT_FAILURE);
    }

//========================================================================================
/*                                                                                      *
 * //──── //print each pixel (containing 3 values(r,g,b)) ──────────────────────────────*
 *        234 200 35                                                                    *
 *        23 76 89                                                                      */
//========================================================================================

//fill in ppm-body (=all pixels)
    for(u_int row = 1; row <= height; row++)
    {
        #if OUTPUT_FS_DEBUG
            printf("first loop\n");
        #endif
        for(u_int col = 1; col <= width; col++)
        {
            #if OUTPUT_FS_DEBUG
                printf("second loop\n");
            #endif
            //only print pixels if we are not in: first row, last row, first collumn and last collum  ->effectifly means: dont print frame
           // if(row != 1 && row != (height+2) && col != 1 && col != (width+2))
           // {
            //print red-value of the pixel
                fprintf(output_ppm,"%i ",array->red);
            //print green-value of the pixel
                fprintf(output_ppm,"%i ",array->green);
            //print blue-value of the pixel
                fprintf(output_ppm,"%i",array->blue);
            //enter next row (we print red,gree,blue of one pixel in one line)
                fprintf(output_ppm,"\n");
                #if OUTPUT_FS_DEBUG
                    printf("\toutput_pixels: %i %i %i\n",array->red, array->green, array->blue);
                #endif
            //}
        //move pointer by one position
            array++;
        }
    }
#if FS_DEBUG
    printf("\tPRINT_PPM exited\n");
#endif
}   