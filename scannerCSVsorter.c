#include "scannerCSVsorter.h"

char *stringTitles[] = {"color", "director_name", "actor_2_name", "genres", "actor_1_name", "movie_title", "actor_3_name", "plot_keywords", "movie_imdb_link", "language", "country", "content_rating"};

char *numericTitles[] = {"num_critic_for_reviews", "duration", "director_facebook_likes", "actor_3_facebook_likes", "actor_1_facebook_likes", "gross", "num_voted_users", "cast_total_facebook_likes", "facenumber_in_poster", "num_user_for_reviews", "budget", "title_year", "actor_2_facebook_likes", "imdb_score", "aspect_ratio", "movie_facebook_likes"};




int main(int argc, char *argv[]){

	char *sortColumn;
        char *sortDirectory;
        char *outputDirectory;
        int hasSortColumn = 0;
        int hasSortDirectory = 0;
        int hasOutputDirectory = 0;
	char *flag;
	char *field;

        if(argc-1 > 6)
        {
                fprintf(stderr, "ERROR: Input fields is greater than 6");
                return -1;
        }

	if((argc-1) % 2 == 1)
        {
                fprintf(stderr, "ERROR: Flag missing field or field missing flag");
                return -1;
        }

	int i;
	for(i=1; i < argc; i+=2)
        {

		field = (char *)malloc(sizeof(char)*strlen(argv[i]));
                strcat(field, argv[i]);
		if(strcmp("-c", field) == 0)
                {
			flag = (char *)malloc(sizeof(char)*sizeof(argv[i+1]));
			strcpy(flag, argv[i+1]);
			sortColumn = (char *)malloc(sizeof(char)*sizeof(argv[i+1]));
			strcpy(sortColumn, flag);
	                hasSortColumn =	1;
                }else if(strcmp("-d", field) == 0)
	                {
			flag = (char *)malloc(sizeof(char)*sizeof(argv[i+1]));
			strcpy(flag, argv[i+1]);
			sortDirectory = (char *)malloc(sizeof(char)*sizeof(field));
			strcpy(sortDirectory, flag);
                        hasSortDirectory = 1;
                }else if(strcmp("-o", field) == 0)
                {
			flag = (char *)malloc(sizeof(char)*sizeof(argv[i+1]));
			strcpy(flag, argv[i+1]);
			outputDirectory = (char *)malloc(sizeof(char)*sizeof(field));
			strcpy(outputDirectory, flag);
                        hasOutputDirectory = 0;
                } else {
                        fprintf(stderr, "ERROR: INCORRECT FLAG \"&s\" \n", argv[i+1]);
                        return -1;
                }
        }

        if(hasSortColumn == 0)
        {
                fprintf(stderr, "FATAL ERROR: No sort column given");
                return -1;
        }
	if(checkColumn(sortColumn) < 0)
	{
		fprintf(stderr, "FATAL ERROR: Sort column incorrect");
		return -1;
	}
	 if(hasSortDirectory == 0)
        {
                sortDirectory = "./";
        }

        if(hasOutputDirectory == 0)
        {
                outputDirectory = "./";
        }

        fprintf(stdout, "Initial PID: %d\n", getpid());
        fprintf(stdout, "PIDS of all child processes: ");

        int numProcs = directoryHandler(sortDirectory, outputDirectory, sortColumn);

        fprintf(stdout, "\n");
        fprintf(stdout,	"Total number of processes: %d\n", numProcs);

	free(field);
	free(flag);
	free(sortColumn);
	free(sortDirectory);
	free(outputDirectory);
        return 0;
}
