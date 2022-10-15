#include <stdio.h>
#include <stdlib.h>

int main(int argv, char *argc[])
{

	FILE *fd = fopen(argc[1], "r");

	if(fd == NULL)
	{
		printf("Error openeing file %s\n", argc[1]);
		return -1;
	}

	int i = 0;
	double val;
	double total = 0;

	char line[50];

	while( ( fgets(line, 50, fd) ) != NULL)
	{
		val = atof(line);
		total += val;
		i++;
	}

	fclose(fd);

	printf("%f\n", (double)total/i);

	return 0;
}
