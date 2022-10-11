#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mergeSort.h"

char *stringTitles[] = {"color", "director_name", "actor_2_name", "genres", "actor_1_name", "movie_title", "actor_3_name", "plot_keywords", "movie_imdb_link", "language", "country", "content_rating"};

char *numericTitles[] = {"num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes", "actor_1_facebook_likes", "gross", "num_voted_users", "cast_total_facebook_likes", "facenumber_in_poster", "num_user_for_reviews", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"};

int headerSyntax(char *);
int sortColumnType(char *);
void *getData(char *, int);
int getColumnNumber(char *, char *);
char *readLine(FILE *);
int commaCheck(char *);
char *trimString(char *);

void mergeSort(Node **, int (*comparatorFnPtr)(void*, void*));
void printList(Node *);

extern int NULLTERMINATOR;

int csvToSortedLinkedList( Node **head, char *path, char *column)
{
	FILE *fp;
	char *buffer;
	char *header;
	int columnNumber;

	fp = fopen(path, "r");

	if(fp == NULL)
	{
		return -1;
	}

	//read header of file
	header = readLine(fp);

	//get column number to sort on from the given column and header
	columnNumber = getColumnNumber(header, column);

	if(columnNumber == -1)
	{
		free(header);
		fclose(fp);
		return -2;
	}

	//checks if header has the exact column names
	if(headerSyntax(header) == -1){
		free(header);
		fclose(fp);
		return -3;
	}
	//check correct number of commas

	int sct = sortColumnType(column);
	int lineCheck;
	int lineNumber = 0;

	//read lines of file
	//add line to linked list
	while(!feof(fp))
	{
		buffer = readLine(fp);
		lineNumber++;
		if(buffer == NULL) {break;}
		if( (lineCheck = commaCheck(buffer)) != 1) {break;}

		switch(sct)
		{

			case 1:
				push(head, getData(buffer, columnNumber), sizeof(char), buffer, (strlen(buffer)*sizeof(char)));
				break;
			case 2: ;
				char *temp = getData(buffer, columnNumber);
				if(strcmp(temp, "NULL") == 0) {
					push(head, temp, sizeof(char), buffer, (strlen(buffer)*sizeof(char)));
					break;
				} else 
				{
					int *num = (int *)malloc(sizeof(int));
					*num = atoi(temp);
					free(temp);
					push(head, num, sizeof(int), buffer, (strlen(buffer)*sizeof(char)));
				break;
				}
		}
	}

	switch(lineCheck){

		case -1:
			free(header);
			fprintf(stderr, "ERROR: File '%s' has incorrect number of values on line %d\n", path, lineNumber+1);
			return -4;

		case -2:
			free(header);
			fprintf(stderr, "ERROR: File '%s' is missing closing '\"' on line %d\n", path, lineNumber+1);
			return -5;
	}

	//sorts list based on the type of column: string or numeric
	switch(sct){

		case 1:
			mergeSort(head, comparator_STR);
			break;
		case 2:
			mergeSort(head, comparator_INT);
			break;
		default:
			fprintf(stderr, "ERROR: Column name not found in file: %s\n", path);
	}

	//add fileHeader as first node in list
	push(head, getData(header, columnNumber), sizeof(int), header, (strlen(header)*sizeof(char)));

	fclose(fp);

	return 1;
}

//checks if the header has the exact numeric and string column names
int headerSyntax(char *header)
{
	int numOfStringTitles = sizeof(stringTitles)/sizeof(stringTitles[0]);
	int numOfNumericTitles = sizeof(numericTitles)/sizeof(numericTitles[0]);

	//check header of file for correct column names
	for(int i = 0; i < numOfStringTitles+numOfNumericTitles; i++){

		void *col = (char *)getData(header,  i);

		//checks if the column title  col is a correct column title
		if(sortColumnType(col) == -1)
		{
			free(col);
			return -1;
		}
		free(col);
	}

	return 1;
}


// checks if sort column is string(1), numeric(2), or missing(-1)
int sortColumnType(char *column)
{
	int i;

	for(i = 0; i < sizeof(stringTitles)/sizeof(stringTitles[0]); i++){
		if(strcmp(column, stringTitles[i]) == 0)
			return 1;
	}
	for(i = 0; i < sizeof(numericTitles)/sizeof(numericTitles[0]); i++){
		if(strcmp(column, numericTitles[i]) == 0)
			return 2;
	}

	return -1;
}

// gets the data from a line at the given position
void * getData(char *line, int columnNumber)
{
	int i;
	char *data = (char *)malloc(sizeof(char)*strlen(line) + NULLTERMINATOR);
	data = strcpy(data, line);

	char *ptr = data;

	for(i = 0; i < columnNumber; i++)
	{
		if(ptr[0] == '"')
		{
			ptr++;
			ptr = strchr(ptr, '"');
		}
		ptr = strchr(ptr, ',');
		ptr++;

	}

	int dataLen = 0;

	if(ptr[0] == '"')
	{
		ptr++;
		dataLen = strcspn(ptr, "\"");
		dataLen++;
	} else
	{
		dataLen = strcspn(ptr, ",");
	}

	char *a = (char *)malloc(sizeof(char) * (dataLen + NULLTERMINATOR) );
	a = strncpy(a, ptr, dataLen + NULLTERMINATOR);

	a[dataLen] = '\0';

	free(data);

	return (void*)a;
}

// returns the number of the column name from the given header starting from 0
// returns -1 if column not found
int getColumnNumber(char *header, char *columnName){

	int i;
	int numOfColumns = (int)sizeof(stringTitles)/(int)sizeof(stringTitles[0]) + (int)sizeof(numericTitles)/(int)sizeof(numericTitles[0]);
	char *data;
	char *ptr = header;

	for(i = 0; i < numOfColumns; i++){
		
		data = (char *)getData(ptr, 0);

		if(strcmp(data, columnName) == 0){
			free(data);
			return i;
		} else {
			ptr = ptr+strlen(data)+1;
			free(data);
		}
	}

	//column not found
	return -1;
}

char *readLine(FILE *file)
{
        int linePos = 0;
        int dataPos = 0;
        int lineBufferSize = 100;
        int dataBufferSize = 50;
        char ch;

        char *line = (char *)malloc(sizeof(char) * lineBufferSize );
        char *data = (char *)malloc(sizeof(char) * dataBufferSize );
	
	strcpy(line, "\0");

        ch = getc(file);

        if(feof(file))
        {
                free(line);
                free(data);
                return NULL;
        } else
        {
                //line[linePos++] = ch;
                data[dataPos++] = ch;
        }
	if(ch == ',')
	{
		strcat(line, "NULL,");
		linePos += 6;
		data[--dataPos] = '\0';
	}

        do
        {
		ch = getc(file);
		//line[linePos++] = ch;
		data[dataPos++] = ch;

                if( (linePos+dataPos+6) > lineBufferSize)
                {
                	//printf("LinePos: %d\t DataPos: %d\t %d\n", linePos, dataPos, linePos+dataPos+6);
		        lineBufferSize *= 2;
                        line = (char *)realloc(line, (sizeof(char) * lineBufferSize) );
                }
                if( (dataPos) == dataBufferSize)
                {
                        dataBufferSize *= 2;
                        data = (char *)realloc(data, sizeof(char) * dataBufferSize );
                }

		//line[linePos] = '\0';
		data[dataPos] = '\0';

		if(ch == ',' || ch == '\r')
		{
			data[dataPos-1] = '\0';
			char *temp = trimString(data);

			if(temp == NULL)
			{
				//printf("%d -> %s\n", linePos, line);
				strcat(line, "NULL");
				linePos += 5;
			} else
			{
				char *trim = (char *)malloc(sizeof(char) * (strlen(temp) + NULLTERMINATOR) );
				strcpy(trim, temp);
				strcat(line, trim);
				linePos += strlen(temp);
				free(trim);
			}

			if(ch == ',')
			{
				strcat(line, ",");
				linePos += 1;
			}
			dataPos = 0;
			data[dataPos+1] = '\0';
		}

	} while(ch != '\n' && !feof(file));

	char *final = (char *)malloc(sizeof(char) * (strlen(line) + NULLTERMINATOR) );
	strcpy(final, line);

	free(data);
	free(line);

        return final;
}

int commaCheck(char *line)
{
	char *ptr = line;
	//char *doubleQuote = strchr(line, '"');
	int totalColumns = (int)sizeof(stringTitles)/(int)sizeof(stringTitles[0]) + (int)sizeof(numericTitles)/(int)sizeof(numericTitles[0]);
	int lineCommas = 0;

	while(ptr != NULL)
	{
		if(ptr[0] == '"')
		{
			ptr++;
			ptr = strchr(ptr, '"');
		}
		ptr = strchr(ptr, ',');
		lineCommas++;
		if(ptr == NULL)
			break;
		ptr++;
	}

	if(lineCommas == totalColumns)
		return 1;
	else
		return -1;
}

char *trimString(char *s)
{
        int i;
        char *leading = s;;
        int trailing = 0;
        int len = strlen(s);

        if(len == 0)
                return NULL;

        if(s[0] == ' ' && len == 1)
                return NULL;

        for(i = 0; i < len; i++)
                if(s[i] != ' ')
                        break;
                else
                        leading++;

        i = 0;
        for(i = len-1; i >= 0; i--)
        {
                if(s[i] != ' ')
                {
                        break;
                } else
                {
                        s[i] = '\0';
                        trailing++;
                }
        }

        if(trailing == len)
                return NULL;

        return leading;
}
