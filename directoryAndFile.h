#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#include "csvToSortedLL.h"

int fileHandler(char *, char *, char *, char *);
void printListToFile(Node *, char *);
int csvCheck(char *);

extern int NULLTERMINATOR;

void directoryHandler(char *directoryName, char *outputDirectory, char *columnName, FILE *temp)
{
	pid_t pid, wpid;
	int status;

	DIR *d;
	struct dirent *dir;
	d = opendir(directoryName);

	if(d == NULL)
	{
		fprintf(stderr, "Cannot open directory '%s'\n", directoryName);
		return;
	}

	//fflush(stdout);

	if (d) {
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG) //found a file
			{
				pid = fork(); //fork
				if(pid == 0) //if child, output PID, run fileHandler, increament # of procs
				{
					//fflush(stdout);
					//fprintf(stdout, "Hit File: %s\t In Directory: %s\t PID: %d\n", dir->d_name, directoryName, getpid());

					//fprintf(stdout, "f%d,", getpid());
					//printf("%d,", getpid());


					fprintf(temp, "%d,", getpid());
					fileHandler(directoryName, dir->d_name, outputDirectory, columnName);

					if( closedir(d) == -1)
						fprintf(stderr, "Close dir error: %d in proc %d\n", errno, getpid());

					free(directoryName);
					free(outputDirectory);
					free(columnName);

					exit(0);
				}

			} else if(dir->d_type == DT_DIR) //found a directory
			{

			//fprintf(stdout, "Hit Directory: %s\t In Directory: %s\tPID: %d\n", dir->d_name, directoryName, getpid());

				if(strcmp(dir->d_name, "..") != 0 && strcmp(dir->d_name, ".") != 0)
				{
					pid = fork(); //fork
					if(pid == 0) //if child, output PID, run directoryHandler, and increament # of procs by directoryHandler return
					{
						char *newSortDirectory = (char *)malloc(sizeof(char) * (strlen(directoryName) + strlen("/") + strlen(dir->d_name) + NULLTERMINATOR) );
						newSortDirectory = strcpy(newSortDirectory, directoryName);
						newSortDirectory = strcat(newSortDirectory, "/");
						newSortDirectory = strcat(newSortDirectory, dir->d_name);
						newSortDirectory = strcat(newSortDirectory, "\0");

						if( closedir(d) == -1)
							fprintf(stderr, "Close dir error: %d in proc %d\n", errno, getpid());

						free(directoryName);

						directoryHandler(newSortDirectory, outputDirectory, columnName, temp);
						fprintf(temp, "%d,", getpid());
						//fprintf(stdout, "dir%d,", getpid());

						free(outputDirectory);
						free(columnName);
						free(newSortDirectory);

						exit(0);
					}
				}
			}
		}
	}
	closedir(d);

	while( (wpid = wait(&status)) > 0);

	//free(directoryName);
	//free(outputDirectory);
	//free(columnName);

	return;
}

int fileHandler(char *path, char *fileName, char *outputDirectory, char *sortColumn)
{
	//checks if fileExtension is equal to ".csv"
	//if the file is not a .csv output error and return
	if(csvCheck(fileName) == -1)
	{
		fprintf(stderr, "ERROR: File %s is not a .csv file\n", path);
		return -1;
	}

	//printf("In file %s\n", fileName);
	//printf("File Extension: %s\n", );

	//checks if file is already -sorted-
	//if file is already -sorted- output error and return
	if(strstr(fileName, "-sorted-") != NULL){
		fprintf(stderr, "ERROR: File %s is already sorted\n", fileName);
		return -1;
  	}

	//printf("File %s not --sorted--\n", path);

	//create start of linkedList
	Node *head = NULL;

	char *filePath = (char *)malloc(sizeof(char *) * (strlen(path) + strlen("/") + strlen(fileName) + NULLTERMINATOR) );
	strcpy(filePath, path);
	strcat(filePath, "/");
	strcat(filePath, fileName);
	strcat(filePath, "\0");

	//printf("In file %s\n", filePath);

	//create and sort file
	int result = csvToSortedLinkedList(&head, filePath, sortColumn);

	//printf("result: %d\n", result);

	switch(result) {

		case 1: ;
			//create newFileName for sorted file
			char *newFileName = (char *)malloc(sizeof(char) * (strlen(fileName) + strlen("-sorted-") + strlen(sortColumn) + strlen(".csv") + NULLTERMINATOR) );
			strcpy(newFileName, fileName);
			newFileName[strlen(fileName)-4] = '\0';
			strcat(newFileName, "-sorted-");
			strcat(newFileName, sortColumn);
			strcat(newFileName, ".csv");

			//concat newFileName to outputDirectory
			char *outputPath = (char *)malloc(sizeof(char) * (strlen(outputDirectory) + strlen("/") + strlen(newFileName) + NULLTERMINATOR) );
			strcpy(outputPath, outputDirectory);
			strcat(outputPath, "/");
			strcat(outputPath, newFileName);

			//output head into fp
			printListToFile(head, outputPath);

			//free malloced memory
			freeList(head);
			free(filePath);
			free(newFileName);
			free(outputPath);
			return 1;
			break;
		case -1://file could not be opened
			freeList(head);
			free(filePath);
			return -1;
			break;
		case -2://sort column not found in file header
			freeList(head);
			free(filePath);
			return -1;
			break;
		case -3://header has incorrect column names
			freeList(head);
			free(filePath);
			return -1;
			break;
		case -4://a line in the file has an inccorect number of data values
			freeList(head);
			free(filePath);
			return -1;
			break;
		case -5://a line in the file has an open '"'
			freeList(head);
			free(filePath);
			return -1;
			break;

	}

	return -1;;
}

int csvCheck(char *fileName)
{
	char *csvExtension = (char *)malloc(sizeof(char) * 5);
	int fileLength = strlen(fileName);

	csvExtension[0] = fileName[fileLength-4];
	csvExtension[1] = fileName[fileLength-3];
	csvExtension[2] = fileName[fileLength-2];
	csvExtension[3] = fileName[fileLength-1];
	csvExtension[4] = '\0'; 

	if(strcmp(csvExtension, ".csv") == 0)
	{
		free(csvExtension);
		return 1;
	} else
	{
		free(csvExtension);
		return -1;
	}
}

void printListToFile(Node *head, char *outputPath)
{
	FILE *fp = fopen(outputPath, "w");

	if(fp == NULL)
	{
		fprintf(stderr, "ERROR: Could not open ouput path '%s'\n", outputPath);
		return;
	}

	Node *ptr = head;
	char *temp;

	while(ptr != NULL)
	{
		temp = (char *)malloc(sizeof(char) * (strlen(ptr->line) + strlen("\r\n") + NULLTERMINATOR) );
		strcpy(temp, ptr->line);
		strcat(temp, "\r\n");
		fwrite(temp, sizeof(char), strlen(ptr->line) + 2, fp);
		ptr = ptr->next;
		free(temp);
	}

	fclose(fp);
}

