#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main()
{
	printf("Hello Steve!\n");

	FILE *fp = fopen("File1.txt","w");

	char data[2] = "a";
	int32_t length = strlen(data);

	if(fp !=NULL)
	{
		fwrite(data,1,length,fp);
		fclose(fp);
	}

	char *input_string = (char *) malloc (100);

	fp = fopen("File1.txt","a");
	if(fp != NULL)
	{
		if(input_string == NULL)
		{
			printf("Error on allocating malloc\n");
			return 1;
		}

		printf("Enter the string\n");
		scanf("%s",input_string);
		

		fwrite(input_string,1,strlen(input_string),fp);


		fflush(fp);

		fclose(fp);

	}

	fp = fopen("File1.txt","r");
	if(fp != NULL)
	{
		char file_character = getc(fp);
		printf("Reading a character from file %c\n",file_character);

		memset(input_string,0,strlen(input_string));

		while(!feof(fp))
		{
			fgets(input_string,100,fp);
			printf("Reading string from file %s\n",input_string);
		}

		fclose(fp);

		free(input_string);

	}

	return 0;
}
		


