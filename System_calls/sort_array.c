/**********************************************************************
 * 			INCLUDES
 * ******************************************************************/

#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

/**********************************************************************
 *                      Main Function
 * ******************************************************************/

int main()
{
	//get pid number
	printf("PID %d\n", getpid());

	//get user id
	printf("Get User Id %d\n",getuid());
	
	//get time stamp
	struct timeval t_obj;

	time_t time_current;

	gettimeofday(&t_obj, NULL);

	time_current = t_obj.tv_sec;

	printf(" Time Stamp :%s\n",ctime(&time_current));


	//Initiating random generator
	srandom(time(NULL));

	int32_t array_length = 0;
	int32_t index = 0;

	//Getting the array length from random generator
	//ensuring that the random number generated is between 0 and 256

	array_length = (random() % 256) ;
	

	printf("Array length %d\n",array_length);
	//creating a dynamic array for source buffer
	int32_t *array = (int32_t *) malloc(array_length * sizeof(int32_t));
	if(array != NULL)
	{
		//filling the source buffer with random values through random generator
		for(index =0; index < array_length; index++)
		{
			array[index] = (random() % 10000 );
		}
	}
	else
	{
                printf("Malloc failed for array\n");
                return ENOMEM;
        }

	
	//creating a dynamic array for destination array where the sorted elements are
	int32_t *dest = (int32_t *) malloc(array_length * sizeof(int32_t));
	if(dest == NULL)
	{
		printf("Malloc failed for dest\n");
		return ENOMEM;
	}

	//For calculating the time elapsed for system call
	clock_t begin = clock();

	//System call
	long int return_status = syscall(398,array,array_length,dest);
	
	//Calculates the time taken for system call
	printf("Time taken for system call execution is %f\n",((double)( (clock() - (double)begin)/ CLOCKS_PER_SEC)));

	//Checks the return status and possible errors of system call
	if(return_status == 0)
	{
		printf("success on system call\n");
	}
	else
	{
		printf("Failure on system call\n");
		
		//Tracks error due to invalid arguments
		if(return_status == EINVAL)
		{
			printf("Invalid arguments\n");
			return EINVAL;
		}

		//Tracks error due to shortage of memory
		if(return_status == ENOMEM)
		{
			printf("No memory\n");
			return ENOMEM;
		}

		//Any other faults
		else
		{
			printf("Error\n");
			return EFAULT;
		}
	}

	printf("user: pre- Sorted List from user\n");
        
	//Prints the source array
	for(index = 0; index < array_length; index++)
        {
                printf("%d\n",array[index]);
        }

	//Sorted array
	printf("user: Sorted List from kernel\n");
	for(index = 0; index < array_length; index++)
	{
		printf("%d\n",dest[index]);
	}

	free(array);
	free(dest);

	return 0;
}

