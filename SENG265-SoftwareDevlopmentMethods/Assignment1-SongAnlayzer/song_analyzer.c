/*
 * @file song_analyzer.c
 *  @brief A pipes & filters program that uses conditionals, loops, and string processing tools in C to process song
 *  data and printing it in a different format.
 *  @author Felipe R.
 *  @author Hausi M.
 *  @author Angadh S.
 *  @author Juan G.
 *  @author Austin G.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief The maximum line length.
 *
 */

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define MAX_LINE_LEN 183 // was 132 by default; This results in a segmentation fault.
#define COMMAND_LINE_ARGS 3
#define ARG_ONE_STRNGL 11
#define ARG_TWO_STRNGL 7
#define MAX_FILE_LENGTH 800
#define MAX_TITLE_LENGTH 200
#define MAX_ARTIST_LENGTH 100
#define MAX_MODE_LENGTH 10
#define MAX_KEY_LENGTH 10
#define MIN_MODE_LENGTH 5

//Implements a structure for the song data type.
struct Song {
    char title[MAX_TITLE_LENGTH];
    char artist[MAX_ARTIST_LENGTH];
    int numberArtists;
    int releaseYear;
    int inPlaylists;
    unsigned long long streams;
    char key[MAX_KEY_LENGTH];
    char mode[MAX_MODE_LENGTH]; 
};

//FUNCTION PROTOTYPES & DOUMENTATION:

int validateCLA(int argc, char* argv[]);
    /**
     * Returns 1 if the arguments passed to the command line are valid.
     * Returns 0 otherwise.
     * Arguments: argc, the number of command line arguments.
     * argv[], the array containing the command line arguments.
    */
    
int getQuestionNumber(char* inputString);
    /**
     * Gets the question number from the command line argument. In main, we will pass this function argv[1];
     * We also check to see that the question number passed is valid here. If it isn't, we exit the program with failure.
    */
    
char* getDataFile(char* inputString);
    /**
     * Gets the data file from the command lien arguments, in terms of a string;
     * Takes he input string obtained from the command line arguments as an input.
     * Returns a pointer to that string.
    */

int readFile(char* fileName, struct Song songArray[]);
    /**
     * Reads the data from the input file into an array of songs.
     * Returns an integer representing the number of elements added to songArray.
    */

void printContentsOfSongArray(struct Song songArray[], int size);
    /**
     * This is a function for debugging, it simlply prints the contents of the song array you pass it.
     * You also need to pass this the number of songs in the array you are trying to print, which is easily obtained from read file.
     * Note that mode has a new line character attatched to it, so no new line specifier is needed in the print statement.
    */

/**
 * FOR ALL FILTER BY FUNCTIONS, WE HAVE
 * 
 * Size refers to the size of the array songArray (obtained from the readFile function).
 * Each returns the number of elements in the new array.
*/

int filterByArtist(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char artist[MAX_ARTIST_LENGTH]);
    /**
     * Filters the tracks in the passed array by artist, and puts them all in newArray.
     * If artists is one of the artists in the song, that song will be appended to newArray.
    */

int filterByMode(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char mode[MAX_MODE_LENGTH]);
    /**
     * Filters the tracks into the new array of the given mode.
     * For example, we can obtain all the songs written in the major mode from songArray and put them in newArray in the order that they appear.
    */

int filterByPlaylist(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int minimumPlaylists);
    /**
     * Adds songs from songArray which are in more spotify playlists than minimumPlaylists to newArray.
    */


int filterByKey(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char keys[MAX_KEY_LENGTH]);
    /**
     * Adds all the keys to newArray which match any in the string keys from the songArray. The key String may look something like:
     * 'ADCG'
     * Preconditions: nan is NOT one of the keys listed.
    */

int filterByNumArtists(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int maxArtists);
    /**
     * Filters by the number of artists on each song. Returns size of the newArray.
     * Only songs that have a number of artists less than or equal to maxArtists get added to the new array.
    */
    
int filterByReleaseYears(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int minYear, int maxYear);
    /**
     * Filters the array songArray by adding only songs between minYear and maxYear (INCLUSIVE) to the newArray.
    */

void writeToFile(struct Song songArray[MAX_FILE_LENGTH], int size);
    /**
     * This function writes the filtered songs to an output file called output.csv.
     * On each line, we write the artist, followed by the song title.
     * They are seperated by a comma with no spaces or anything.
     * Example: Spiritbox,Angel Eyes
     * Note that the first line of the file will contain this text: Artist(s),Song
    */
   

//FUNCTIONS

int validateCLA(int argc, char* argv[]) {

    //Checks to see if the user has passed the correcxt number of command line arguments.
    if (argc != COMMAND_LINE_ARGS) {
        printf("Error: Invalid number of command line arguments. Please execute in the follwoing format\n");
        printf("./song_analyzer --question=NUM --data=DATA_FILE_NAME.csv\n");
        return 0;
    }

    //Checks the first argument, using a string compare method.
    if (strncmp(argv[1], "--question=", ARG_ONE_STRNGL) != 0) {
        printf("Error At Argument 1: Please execute using the following format:\n");
        printf("./song_analyzer --question=NUM --data=DATA_FILE_NAME.csv\n");
        return 0;
    }

    //Checks the second argument,
    if (strncmp(argv[2], "--data=", ARG_TWO_STRNGL) != 0) {
        printf("Error At Argument 2: Please execute using the following format:\n");
        printf("./song_analyzer --question=NUM --data=DATA_FILE_NAME.csv\n");
        return 0;
    }

    //Otherwise, the command line arguments are valid.
    else{
        return 1;
    }
}

int getQuestionNumber(char* inputString) {

    //CommandLineArg is a pointer to the start of the string (everything before the '=' ).
    char* commandLineArg = strtok(inputString, "=");

    //Calling strtok again with Null returns everything after eqauls from the previous call of strtok. This includes our quesiton number.
    char* num = strtok(NULL, "=");

    //This code converts the resulting string to an integer and returns the result.
    int result = atoi(num);

    if (result < 1 || result > 5) {
        printf("Error: Please enter a number from 1 - 5 for the question number in the below format:\n");
        printf("./song_analyzer --question=NUM --data=DATA_FILE_NAME.csv\n");
        exit(EXIT_FAILURE);
    }

    return result;

}

char* getDataFile(char* inputString) {

    //CommandLineArg is a pointer to the start of the string (aeverything before the '=').
    char* commandLineArg = strtok(inputString, "=");

    //Calling strtok again with Null returns everything after eqauls from the previous call of strtok. This includes our quesiton number.
    char* fileName = strtok(NULL, "=");

    //Returns the resulting fileName
    return fileName;

}

int readFile(char* fileName, struct Song songArray[]) {
    
    //Opens the file in read mode.
    FILE* fileHandle = fopen(fileName, "r");

    // Skip the first line of the file
    char header[MAX_LINE_LEN];
    fgets(header, MAX_LINE_LEN, fileHandle);
    
    // Read song data from the file
    int i = 0;
    char line[MAX_LINE_LEN];

    while (fgets(line, MAX_LINE_LEN, fileHandle) != NULL) {

        struct Song curSong;

        //Ttile
        char* token = strtok(line, ",");
        strncpy(curSong.title, token, MAX_TITLE_LENGTH);
        
        //Artist
        token = strtok(NULL, ",");
        strncpy(curSong.artist, token, MAX_ARTIST_LENGTH);
        
        //Number of Artists
        token = strtok(NULL, ",");
        curSong.numberArtists = atoi(token);
        
        //Release Year
        token = strtok(NULL, ",");
        curSong.releaseYear = atoi(token);
       
        //Playlists
        token = strtok(NULL, ",");
        curSong.inPlaylists = atoi(token);
       
        //streams
        token = strtok(NULL, ",");
        curSong.streams = strtoull(token, NULL, 10);
       
        //Key
        token = strtok(NULL, ",");
        strncpy(curSong.key, token, MAX_KEY_LENGTH);
      
        //Mode
        token = strtok(NULL, ",");
        strncpy(curSong.mode, token, MAX_MODE_LENGTH);
       
        // Add the song to the array.
        songArray[i] = curSong;
        i++;
    }

    // Close the file
    fclose(fileHandle);
    return i;
}

void printContentsOfSongArray(struct Song songArray[], int size) {
    
    for(int i = 0; i < size; i++) {
        printf("%s, %s, %d, %d, %d, %llu, %s, %s", songArray[i].title, songArray[i].artist, songArray[i].numberArtists, songArray[i].releaseYear, songArray[i].inPlaylists, songArray[i].streams, songArray[i].key, songArray[i].mode);
    }
}

//THE NEXT PART OF THIS PROGRAM WILL BE USED TO FILTER THE SONG ARRAY DATA STRUCTURE:

int filterByArtist(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char artist[MAX_ARTIST_LENGTH]) {

    //loops through songArray
    int elements = 0;

    for (int i = 0; i < size; i++) {
        //strstr is used to see if string 2 is found anywhere in string 1, assuming the call is something like strstr(str1, str2);
        if (strstr(songArray[i].artist, artist) != NULL) {
            newArray[elements] = songArray[i];
            elements ++;
        }
     }
    return elements;
}

int filterByMode(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char mode[MAX_MODE_LENGTH]) {

    int elements = 0;

    for (int i = 0; i < size; i++) {
        //strncmp is used to compare the first c characters matching between str1 and str2, assuming the call is:
        //strncmp(str 1, str2, c)
        //These beats strcmp because strncmp allows us to ignores newline characters, which are attatched to mode.
        //MIN_MODE_LENGTH is 5, which is the number of letters in major or minor.
        if (strncmp(mode, songArray[i].mode, MIN_MODE_LENGTH) == 0) {
            newArray[elements] = songArray[i];
            elements ++;
        }
    }
    return elements;
}

int filterByPlaylist(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int minimumPlaylists) {

    int elements = 0;

    for (int i = 0; i < size; i++) {
        if (songArray[i].inPlaylists > minimumPlaylists) {
            newArray[elements] = songArray[i];
            elements ++;
        }
    }
    return elements;
}

int filterByKey(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], char keys[MAX_KEY_LENGTH]) {
   
    int elements = 0;
    
    for (int i = 0; i < size; i++) {
        for (int j = 0; keys[j] != '\0'; j++) {
            if (keys[j] == songArray[i].key[0]) {
                newArray[elements] = songArray[i];
                elements ++;
            }
       }
    }
    return elements;
}

int filterByNumArtists(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int maxArtists) {

    int elements = 0;

    for (int i = 0; i < size; i++) {
        if (songArray[i].numberArtists <= maxArtists) {
            newArray[elements] = songArray[i];
            elements ++;
        }
    }
    return elements;

}

int filterByReleaseYears(struct Song songArray[MAX_FILE_LENGTH], int size, struct Song newArray[MAX_FILE_LENGTH], int minYear, int maxYear) {

    int elements = 0;

    for (int i = 0; i < size; i++) {
        if (songArray[i].releaseYear >= minYear && songArray[i].releaseYear <= maxYear) {
            newArray[elements] = songArray[i];
            elements ++;
        }
    }
    return elements;

}

//THE NEXT PART OF THE PROGRAM IS RESPONSIBLE FOR WRITING THE DATA TO THE OUTPUT FILE.

void writeToFile(struct Song songArray[MAX_FILE_LENGTH], int size) {

    //Opens the file output.csv in write mode, writes the title line to the top of the file.
    FILE* fileHandle = fopen("output.csv", "w");
    fputs("Artist(s),Song\n", fileHandle);

    //Loops through the passed array and adds each song.
    for(int i = 0; i < size; i++) {
        fputs(songArray[i].artist, fileHandle);
        fputs(",", fileHandle);
        fputs(songArray[i].title, fileHandle);
        fputs("\n", fileHandle);
    }

    //Closes the file.
    fclose(fileHandle);
}

/**
 * Function: main
 * --------------
 * @brief The main function and entry point of the program.
 *
 * @param argc The number of arguments passed to the program.
 * @param argv The list of arguments passed to the program.
 * @return int 0: No errors; 1: Errors produced.
 *
 */

int main(int argc, char *argv[]) {
    // TODO: your code.

    int isValid = validateCLA(argc, argv);

    //Exit if the command line arguments are not valid.
    if(isValid == 0) {
        exit(EXIT_FAILURE);
    }

    //Otherwise, the CLA args are valid
    else {
        int questionNumber = getQuestionNumber(argv[1]);
        char* fileName = getDataFile(argv[2]);
        struct Song songArray[MAX_FILE_LENGTH];
        int size = readFile(fileName, songArray);
        
        //Question 1:
        if (questionNumber == 1) {
            struct Song qOneArray[MAX_FILE_LENGTH];
            int filterSize = filterByNumArtists(songArray, size, qOneArray, 1);
            filterSize = filterByArtist(qOneArray, size, qOneArray, "Rae Spoon");
            writeToFile(qOneArray,filterSize);
        }

        //Question 2:
        if (questionNumber == 2) {
            struct Song qTwoArray[MAX_FILE_LENGTH];
            int filterSize = filterByNumArtists(songArray, size, qTwoArray, 1);
            filterSize = filterByArtist(qTwoArray, size, qTwoArray, "Tate McRae");
            writeToFile(qTwoArray,filterSize);
            //printContentsOfSongArray(qTwoArray, filterSize);
        }

        //Question 3:
        if (questionNumber == 3) {
            struct Song qThreeArray[MAX_FILE_LENGTH];
            int filterSize = filterByNumArtists(songArray, size, qThreeArray, 1);
            filterSize = filterByArtist(qThreeArray, size, qThreeArray, "The Weeknd");
            filterSize = filterByMode(qThreeArray, filterSize, qThreeArray, "Major");
            writeToFile(qThreeArray,filterSize);
            //printContentsOfSongArray(qThreeArray, filterSize);
        }
        
        //Question 4:
        if (questionNumber == 4) {
            struct Song qFourArray[MAX_FILE_LENGTH];
            int filterSize = filterByPlaylist(songArray, size, qFourArray, 5000);
            filterSize = filterByKey(qFourArray, filterSize, qFourArray, "AD");
            writeToFile(qFourArray, filterSize);
        }

        //Question 5:
        if (questionNumber == 5) {
            struct Song qFiveArray[MAX_FILE_LENGTH];
            int filterSize = filterByReleaseYears(songArray, size, qFiveArray, 2021, 2022);
            filterSize = filterByArtist(qFiveArray, size, qFiveArray, "Drake");
            writeToFile(qFiveArray, filterSize);
        }

        exit(EXIT_SUCCESS);

    }
}
    
