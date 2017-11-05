  /* Test and timing harness program for developing a multichannel
     multikernel convolution (as used in deep learning networks)

     Note there are some simplifications around this implementation,
     in particular with respect to computing the convolution at edge
     pixels of the image.

     Author: David Gregg
     Date:   February 2017


     Version 1.4 : Modified the random generator to reduce the range
                   of generated values;
                   Changed the summation in the checking code from
                   float to double to try to bring the checked value
                   closer to the "true" value

     Version 1.3 : Fixed which loop variables were being incremented
                   in write_out();
                   Fixed dimensions of output and control_output 
                   matrices in main function

     Version 1.2 : Changed distribution of test data to (hopefully) 
                   eliminate random walk of floating point error;
                   Also introduced checks to restrict kernel-order to
                   a small set of values

     Version 1.1 : Fixed bug in code to create 4d matrix
  */

  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/time.h>
  #include <assert.h>
  #include <omp.h>
  #include <math.h>
  #include <x86intrin.h>

  /* the following two definitions of DEBUGGING control whether or not
     debugging information is written out. To put the program into
     debugging mode, uncomment the following line: */
  /*#define DEBUGGING(_x) _x */
  /* to stop the printing of debugging information, use the following line: */
  #define DEBUGGING(_x)
  #define ZERO_VALUE  0


  /* write 3d matrix to stdout */
     void write_out(float *** a, int dim0, int dim1, int dim2)
     {
      int i, j, k;

      for ( i = 0; i < dim0; i++ ) {
        printf("Outer dimension number %d\n", i);
        for ( j = 0; j < dim1; j++ ) {
          for ( k = 0; k < dim2 - 1; k++ ) {
            printf("%f, ", a[i][j][k]);
          }
        // print end of line
          printf("%f\n", a[i][j][dim2-1]);
        }
      }
    }


  /* create new empty 4d matrix */
    float **** new_empty_4d_matrix(int dim0, int dim1, int dim2, int dim3)
    {
      float **** result = malloc(dim0 * sizeof(float***));
      float *** mat1 = malloc(dim0 * dim1 * sizeof(float**));
      float ** mat2 = malloc(dim0 * dim1 * dim2 * sizeof(float*));
      float * mat3 = malloc(dim0 * dim1 * dim2 *dim3 * sizeof(float));
      int i, j, k;


      for ( i = 0; i < dim0; i++ ) {
        result[i] = &(mat1[i*dim1]);
        for ( j = 0; j < dim1; j++ ) {
          result[i][j] = &(mat2[i*dim1*dim2 + j*dim2]);
          for ( k = 0; k < dim2; k++ ) {
            result[i][j][k] = &(mat3[i*dim1*dim2*dim3+j*dim2*dim3+k*dim3]);
          }
        }
      }

      return result;
    }

  /* create new empty 3d matrix */
    float *** new_empty_3d_matrix(int dim0, int dim1, int dim2)
    {
      float **** mat4d;
      float *** mat3d;

    // create a 4d matrix with single first dimension
      mat4d = new_empty_4d_matrix(1, dim0, dim1, dim2);
    // now throw away out first dimension
      mat3d = mat4d[0];
      free(mat4d);
      return mat3d;
    }

  /* take a copy of the matrix asnd return in a newly allocated matrix */
    float **** copy_4d_matrix(float **** source_matrix, int dim0,
      int dim1, int dim2, int dim3)
    {
      int i, j, k, l;
      float **** result = new_empty_4d_matrix(dim0, dim1, dim2, dim3);

      for ( i = 0; i < dim0; i++ ) {
        for ( j = 0; j < dim1; j++ ) {
          for ( k = 0; k < dim2; k++ ) {
            for ( l = 0; l < dim3; l++ ) {
              result[i][j][k][l] = source_matrix[i][j][k][l];
            }
          }
        }
      }
      return result;
    }

  /* create a matrix and fill it with random numbers */
    float **** gen_random_4d_matrix(int dim0, int dim1, int dim2, int dim3)
    {
      float **** result;
      int i, j, k, l;
      struct timeval seedtime;
      int seed;

      result = new_empty_4d_matrix(dim0, dim1, dim2, dim3);

    /* use the microsecond part of the current time as a pseudorandom seed */
      gettimeofday(&seedtime, NULL);
      seed = seedtime.tv_usec;
      srandom(seed);

    /* fill the matrix with random numbers */
    const int range = 1 << 12; // 2^12
    const int bias = 1 << 16; // 2^16
    float offset = 0.0;
    for ( i = 0; i < dim0; i++ ) {
      for ( j = 0; j < dim1; j++ ) {
        for ( k = 0; k < dim2; k++ ) {
          for ( l = 0; l < dim3; l++ ) {
            // generate uniform random integer with mean of zero
            long long rand = random();
            // now cut down the range and bias the mean to reduce
            // the likelihood of large floating point round-off errors
            int reduced_range = (rand % range);
            float num = (((float) reduced_range) / ((float) bias))+offset;
            result[i][j][k][l] = num;
          }
        }
      }
    }

    return result;
  }

  /* create a matrix and fill it with random numbers */
  float *** gen_random_3d_matrix(int dim0, int dim1, int dim2)
  {
    float **** mat4d;
    float *** mat3d;

    // create a 4d matrix with single first dimension
    mat4d = gen_random_4d_matrix(1, dim0, dim1, dim2);
    // now throw away out first dimension
    mat3d = mat4d[0];
    free(mat4d);
    return mat3d;
  }

  /* check the sum of absolute differences is within reasonable epsilon */
  void check_result(float *** result, float *** control,
    int dim0, int dim1, int dim2)
  {
    int i, j, k;
    double sum_abs_diff = 0.0;
    const double EPSILON = 0.0625;

    //printf("SAD\n");
    
    for ( i = 0; i < dim0; i++ ) {
      for ( j = 0; j < dim1; j++ ) {
        for ( k = 0; k < dim2; k++ ) {
          double diff = fabs(control[i][j][k] - result[i][j][k]);
          assert( diff >= 0.0 );
          sum_abs_diff = sum_abs_diff + diff;
        }
      }
    }

    if ( sum_abs_diff > EPSILON ) {
      fprintf(stderr, "WARNING: sum of absolute differences (%f) > EPSILON (%f)\n",
        sum_abs_diff, EPSILON);
    }
    else {
      printf("COMMENT: sum of absolute differences (%f)  within acceptable range (%f)\n", sum_abs_diff, EPSILON);
    }
  }

  /* the slow but correct version of matmul written by David */
  void multichannel_conv(float *** image, float **** kernels, float *** output,
   int width, int height, int nchannels, int nkernels,
   int kernel_order)
  {
    int h, w, x, y, c, m;
    //#pragma omp parallel for private(m,w,h,c,x,y) collapse(3)
    for ( m = 0; m < nkernels; m++ ) { //cycle through each kernel
      for ( w = 0; w < width; w++ ) { //cycle through each x cooridinate of the image
        for ( h = 0; h < height; h++ ) { //cycle through each y coordinate of the image
          double sum = 0.0;
          for ( c = 0; c < nchannels; c++ ) { //cycle through each channel of the image and the kernel
            for ( x = 0; x < kernel_order; x++) { //cycle through each x coordinate of the kernel
              for ( y = 0; y < kernel_order; y++ ) { //cycle through each y coordinate of the kernel
                sum += image[w+x][h+y][c] * kernels[m][c][x][y];
              }
            }
          }
          output[m][w][h] = sum;
        }
      }
    }
  }


  /* the fast version of matmul written by the team */
  void team_conv(float *** image, float **** kernels, float *** output,
   int width, int height, int nchannels, int nkernels,
   int kernel_order)
  {
    int h, w, x, y, c, m;
    __m128 imageVector = _mm_setzero_ps(); //initialise imageVector
    __m128 kernelVector = _mm_setzero_ps(); //initialise kernelVector
    __m128 productVector = _mm_setzero_ps(); //initialise productVector
    __m128 sumVector = _mm_setzero_ps(); //initialise sumVector

        float **** myReversedKernel = new_empty_4d_matrix(nkernels,kernel_order,kernel_order,nchannels);
        for ( m = 0; m < nkernels; m++ ) { //cycle through each kernel
          for(x = 0; x<kernel_order; x++){
            for(y=0; y<kernel_order; y++){
              for(c=0; c<nchannels; c++){
                //creates reordered kernel so that channels can be accessed in innermost loop
                myReversedKernel[m][x][y][c] = kernels[m][c][x][y];
              }
            }
          }
        }

    if(kernel_order == 1){

    //parallelizes three outermost for loops
    #pragma omp parallel for private(m,w,h,c,imageVector,kernelVector,productVector,sumVector) collapse(3)
    for ( m = 0; m < nkernels; m++ ) { //cycle through each kernel
      for ( w = 0; w < width; w++ ) { //cycle through each x cooridinate of the image
        for ( h = 0; h < height; h++ ) { //cycle through each y coordinate of the image
          double sum = 0.0;
          for ( c = 0; c < nchannels-3; c = c + 4 ) { //cycle through each channel of the image and the kernel
            imageVector = _mm_loadu_ps(&image[w][h][c]); //loads 4 different channels of an image pixel into vector
            kernelVector = _mm_loadu_ps(&myReversedKernel[m][0][0][c]); //loads 4 different channels of a kernel pixel into vector
            productVector = _mm_mul_ps(imageVector,kernelVector); //multiplies two vectors together
            sumVector = _mm_add_ps(sumVector, productVector); //adds two vectors together
          }

          //if number of channels remaining is not divisible by 4
          for (; c < nchannels; c++){
            //horizontally sum channel value of remaining pixel as this cannot be loaded into vector
            sum += image[w][h][c] * myReversedKernel[m][0][0][c];
          }
          //horizontally adds sumVector (adjacent elements in the same operand are added together)
          sumVector = _mm_hadd_ps(sumVector,sumVector);
          sumVector = _mm_hadd_ps(sumVector,sumVector);
          
          sum += _mm_cvtss_f32(sumVector); //extracts the lower order floating point value from sumVector
          output[m][w][h] = sum; //stores sum in output array
          sumVector = _mm_setzero_ps(); //set sumVector equal to zero
        }
      }
    }
  }else{ //when kernel_order is 3, 5 or 7
        imageVector = _mm_setzero_ps(); //set imageVector equal to zero
        kernelVector = _mm_setzero_ps(); //set kernelVector equal to zero
        productVector = _mm_setzero_ps(); //set productVector equal to zero
        sumVector = _mm_setzero_ps(); //set sumVector equal to zero

      //parallelizes three outermost for loops
      #pragma omp parallel for private(m,w,h,c,x,y,imageVector,kernelVector,productVector,sumVector) collapse(3)
        for ( m = 0; m < nkernels; m++ ) { //cycle through each kernel
          for ( w = 0; w < width; w++ ) { //cycle through each x cooridinate of the image
            for ( h = 0; h < height; h++ ) { //cycle through each y coordinate of the image
              double sum = 0.0;
              //cycle through each channel of the image and the kernel
                for ( x = 0; x < kernel_order; x++) { //cycle through each x coordinate of the kernel
                  for ( y = 0; y < kernel_order; y++ ) { //cycle through each y coordinate of the kernel
                    for ( c = 0; c < nchannels-3; c = c+4 ) { 
                    imageVector = _mm_load_ps(&image[w+x][h+y][c]); //loads 4 different channels of an image pixel into vector
                    kernelVector = _mm_load_ps(&myReversedKernel[m][x][y][c]); //loads 4 different channels of a kernel pixel into vector
                    productVector = _mm_mul_ps(imageVector,kernelVector); //multiplies two vectors together
                    sumVector = _mm_add_ps(sumVector, productVector);  //adds two vectors together
                    //sum += image[w+x][h+y][c] * kernels[m][c][x][y];
                  }
                }
        //if number of channels remaining is not divisible by 4
          for (; c < nchannels; c++){
            for(x=0; x<kernel_order; x++){
              for(y=0; y<kernel_order; y++){
            //horizontally sum channel value of remaining pixel as this cannot be loaded into vector
            sum += image[w+x][h+y][c] * myReversedKernel[m][x][y][c]; 
          }
        }
      }   

          //horizontally adds sumVector (adjacent elements in the same operand are added together)
          sumVector = _mm_hadd_ps(sumVector,sumVector);
          sumVector = _mm_hadd_ps(sumVector,sumVector);
          
          sum += _mm_cvtss_f32(sumVector); //extracts the lower order floating point value from sumVector
          output[m][w][h] = sum; //stores sum in output array
          sumVector = _mm_setzero_ps(); //set sumVector to zero
              }
            }
          }
        }
      }
    }


    int main(int argc, char ** argv)
    {
    //float image[W][H][C];
    //float kernels[M][C][K][K];
    //float output[M][W][H];

      float *** image, **** kernels, *** output;
      float *** control_output;
      long long mul_time_my_team, mul_time_david_gregg;
      int width, height, kernel_order, nchannels, nkernels;
      struct timeval start_time, start_time2;
      struct timeval stop_time, stop_time2;

      if ( argc != 6 ) {
        fprintf(stderr, "Usage: conv-harness <image_width> <image_height> <kernel_order> <number of channels> <number of kernels>\n");
        exit(1);
      }
      else {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        kernel_order = atoi(argv[3]);
        nchannels = atoi(argv[4]);
        nkernels = atoi(argv[5]);
      }
      switch ( kernel_order ) {
        case 1:
        case 3:
        case 5:
        case 7: break;
        default:
        fprintf(stderr, "FATAL: kernel_order must be 1, 3, 5 or 7, not %d\n",
          kernel_order);
        exit(1);
      }

    /* allocate the matrices */
      image = gen_random_3d_matrix(width+kernel_order, height + kernel_order,
       nchannels);
      kernels = gen_random_4d_matrix(nkernels, nchannels, kernel_order, kernel_order);
      output = new_empty_3d_matrix(nkernels, width, height);
      control_output = new_empty_3d_matrix(nkernels, width, height);

    //DEBUGGING(write_out(A, a_dim1, a_dim2));

    /* use a simple multichannel convolution routine to produce control result */
      gettimeofday(&start_time2, NULL);

      multichannel_conv(image, kernels, control_output, width,
        height, nchannels, nkernels, kernel_order);

      gettimeofday(&stop_time2, NULL);

      mul_time_david_gregg = (stop_time2.tv_sec - start_time2.tv_sec) * 1000000L +
      (stop_time2.tv_usec - start_time2.tv_usec);


    /* record starting time of team's code*/
      gettimeofday(&start_time, NULL);

    /* perform student team's multichannel convolution */
      team_conv(image, kernels, output, width,
        height, nchannels, nkernels, kernel_order);

    /* record finishing time */
      gettimeofday(&stop_time, NULL);
      mul_time_my_team = (stop_time.tv_sec - start_time.tv_sec) * 1000000L +
      (stop_time.tv_usec - start_time.tv_usec);


      printf("David Gregg conv time: %lld microseconds\n", mul_time_david_gregg);
      printf("Our Team conv time: %lld microseconds\n", mul_time_my_team);

      long long speed = mul_time_david_gregg /mul_time_my_team;
      printf("Speed Factor : %lld \n ", speed);

      DEBUGGING(write_out(output, nkernels, width, height));

    /* now check that the team's multichannel convolution routine
       gives the same answer as the known working version */
      check_result(output, control_output, nkernels, width, height);

      return 0;
    }