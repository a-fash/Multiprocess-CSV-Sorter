#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "mergeSort.c"

extern char *stringTitles[];
extern char *numericTitles[];

void fileHandler(char *, char *, char *);

int directoryHandler(char *directoryName, char *outputDirectory, char *columnName)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(directoryName);
	pid_t pid;
	int status;
	int numProcs = 0;

	
	if (d) {
		while ((dir = readdir(d)) != NULL) 
		{	
			if (dir->d_type == DT_REG) //found a file
			{
				pid = fork(); //fork
					if(pid == 0) //if child, output PID, run fileHandler, increament # of procs
					{
						fprintf(stdout, "%d, ", getpid());
						fileHandler(dir->d_name, directoryName, columnName); 
						numProcs++;
						exit(0);
					}
			} else if(dir->d_type == DT_DIR) //found a directory	
			{
				pid = fork(); //fork
				if(pid == 0) //if child, output PID, run directoryHandler, and increament # of procs by directoryHandler return
				{
					fprintf(stdout, "%d, ", getpid());
					numProcs += directoryHandler(strcat(directoryName, "/"), outputDirectory, columnName);
					exit(0);
				}
			}					
		}
	}
	closedir(d);
	while(wait(&status) > 0);
	return numProcs;
}

void fileHandler(char *path, char *outputDirectory, char *sortColumn)
{
	//gets file name from path
        char *fileName = (char *)malloc(sizeof(strrchr(path, '/')));
        strcpy(fileName, strrchr(path, '/')+1);
	
	//gets the file extension from fileName and stores it in fileExtension
        char fileExtension[5] = {fileName[sizeof(fileName)-4], fileName[sizeof(fileName)-3], fileName[sizeof(fileName)-2], fileName[sizeof(fileName)-1], '\0'};
	
	//checks if fileExtension is equal to ".csv"
	//if the file is not a .csv output error and return
        if(strcmp(fileExtension, ".csv") != 0){
                fprintf(stderr, "ERROR: File %s is not a .csv file\n", fileName);
		free(fileName);
                return;
        }

	//checks if file is already -sorted-
	//if file is already -sorted- output error and return
	if(strstr(fileName, "-sorted-") != NULL){
                fprintf(stderr, "ERROR: File %s is already sorted\n", fileName);
		free(fileName);
                return;
        }

	//create start of linkedList
        Node *head = (Node *)malloc(sizeof(Node*));

	//create and sort file
        //int result = csvToLinkedList(&head, path, column);

	//create newFileName for sorted file
        char *newFileName = (char *)malloc(sizeof(fileName) + sizeof(sortColumn) + sizeof("-sorted-"));
        strncpy(newFileName, fileName, sizeof(fileName)-4);
        strcat(newFileName, "-sorted-");
        strcat(newFileName, sortColumn);
        strcat(newFileName, ".csv");

	//concat newFileName to outputDirectory
        char *outputPath = (char *)malloc(sizeof(newFileName) + sizeof(outputDirectory));
        strcat(outputPath, outputDirectory);
        strcat(outputPath, newFileName);
	
	//creates new file at outputPath
	int fp = open(outputPath, O_CREAT | O_RDWR);
	
	//output head into fp	
	
	close(fp);
	
	//free malloced memory
	freeList(head);
	free(newFileName);
	free(outputPath);
	return;
}

//*****************DOES NOT WORK************************
//returns int for errors: incorrect headers, incorrect format i.e. commas, 
/*
int csvToLinkedList( Node **head, char *path, char *column)
{
	FILE *fp;
	int bufferSize = 255;
	char *buffer = (char *)malloc(sizeof(char)*bufferSize);

	fp = fopen(path, “r”);

	//read header of file
	
		
	//check header of file for correct columnName

	//check header of the file for correct column names



	//read lines of file
	//check correct number of commas
	//add lines and data to linked list using push(Node **headRef, void *data, size_t data_size, char *line, size_t line_size);
	if(sizeof(buffer) == bufferSize)
	{
		bufferSize *= 2;
		buffer = (char *)realloc(buffer, sizeof(char)*bufferSize);
	}

	switch(checkColumn(columnName){

		case 1:
			mergeSort(&head, comparator_STR);
			break;
		case 2:
			mergeSort(&head, comparator_INT);
			break;
		default:
			fprintf(STDERR, "ERROR: Column name not found in file: %s\n", fileName);
	}
		


	//add fileHeader as first node in list
	fclose(fp);
	free(buffer);
	return;
}
*/


//checks if the inputed column name is an allowed column name
int checkColumn(char *column)
{
	int i;

	for(i = 0; i < sizeof(stringTitles[0])/sizeof(stringTitles[0][0]); i++){
		if(strcmp(column, stringTitles[i]) == 0)
			return 1;
	} 
	for(i = 0; i < sizeof(numericTitles[0])/sizeof(numericTitles[0][0]); i++){
		if(strcmp(column, numericTitles[i]) == 0)
			return 2;
	}
	
	return -1;
}

//gets data at a given column number from a given string
void * getData(int columnNumber, char *line)
{
	int i;
	char *data = (char *)malloc(sizeof(line)*sizeof(char));
	data = strcpy(data, line);

	for(i = 0; i < columnNumber-1; i++){
		data = strchr(data, ',');
		data++;
	}
	
	int num = strcspn(data, ",");

	char *a = strncpy(a, data, num);

	free(data);	

	return (void*)a;
		
}	
