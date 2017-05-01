#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <memory.h>
#include <sys/stat.h>
#include <wait.h>

#define COMPILE_ERROR -2
#define TIMEOUT_EXEPTION -3
#define NOT_ACCURATE_OUTPUT -4
#define WRONG_OUTPUT -5
#define NO_C_FILE -6
#define FORK_FAILED -7
#define CORE_DUMPED_EXEPTION -8
#define WAIT_ERROR
#define SIZE 512

void startChecking(char *usersDirPath, char *inputSource, char *outputSource, int resultDescriptor);

int main(int argc, char *argv[]) {

    //declare variables
    char buff[SIZE];
    char outputSource[SIZE];
    char inputSource[SIZE];
    char usersDirPath[SIZE];
    char initialProgPath[SIZE];
    int resultFile;
    char *paths;
    DIR *users;
    ssize_t readNum;
    //check if we got the args
    if (argc < 2) {
        perror("wrong arguments");
        exit(-1);
    }

    if ((getcwd(initialProgPath, SIZE) == NULL)) {
        //todo handle error
    }
    //open the config file to read
    int config = open(argv[1], O_RDONLY);
    if (config == NULL) {
        perror("failed open config file");
        exit(-1);
    }

    //set the reading to br from start
    off_t seek = lseek(config, 0, SEEK_SET);
    if (seek == -2) {
        //todo handle error
    }


    //create result file
    resultFile = open("result.csv", O_CREAT || O_APPEND || O_RDWR);
    if (resultFile < 0) {
        perror("failed open result file");
        exit(-1);
    }

    //read the all file
    readNum = read(config, buff, SIZE);
    if (readNum < 0) {
        //todo handle error
    }

    //cut every line of it
    //start with users directory
    paths = strtok(buff, "\n");
    strcpy(usersDirPath, paths);

    //initialize the input output files pathes
    paths = strtok(NULL, "\n");
    strcpy(inputSource, paths);
    paths = strtok(NULL, "\n");
    strcpy(outputSource, paths);

    //run check on every student
    startChecking(usersDirPath, inputSource, outputSource, resultFile);

    //go back to initial directory
    chdir(initialProgPath);

    close(config);
    return 0;
}

void startChecking(char *usersDirPath, char *inputSource, char *outputSource, int resultDescriptor) {

    struct dirent *pDirent;
    DIR *users;
    int inputDescriptor;
    int outputDescriptor;
    char *studentName;
    int depth;
    int grade;
    int temp;
    char tempPath[SIZE];
    //open users directory
    users = opendir(usersDirPath);
    if (users == NULL) {
        perror("failed open users directory");
        exit(-1);
    }

    //go to the students directory
    chdir(usersDirPath);

    //open input source
    inputDescriptor = open(inputSource, O_RDONLY);
    if (inputDescriptor < 0) {
        perror("failed open input source");
        exit(-1);
    }

    //open output source
    outputDescriptor = open(outputSource, O_RDONLY);
    if (inputDescriptor < 0) {
        perror("failed open output source");
        exit(-1);
    }


    //iterate over users
    //for every user run test
    while ((pDirent = readdir(users)) != NULL) {
        studentName = pDirent->d_name;

        //check for more then one directory
        strtok(tempPath, usersDirPath);
        strtok(tempPath, "/");
        strtok(tempPath, pDirent->d_name);
        temp = checkForManyFOlders(tempPath);
        //if there is more then one
        if (temp > 1) {

            //todo handle
        }

        //iterate and find file
        if (pDirent->d_type == DT_DIR) { //TODO will work or need stat?
            depth = 0;
            // find the exe compile an run is
            //then compare to the expected files
            checkExecutableAndRun(usersDirPath, pDirent->d_name, depth, inputDescriptor, outputDescriptor);
            lseek(inputDescriptor, 0, SEEK_SET);
            lseek(outputDescriptor, 0, SEEK_SET);
            //has an executable
            setGrade()
            //todo what to do?
        }

    }

}


//close dir


void checkExecutableAndRun(char *dir, char *studentDirName, int depth, int inputFile, int outputFile,
                           char *compareProgPath) {

    //declare vars
    char *temp = dir;
    struct dirent *pDirent;
    ++depth;
    DIR *studentDir;


    strcat(dir, "/");
    strcat(dir, studentDirName);
    studentDir = opendir(dir);
    if (studentDir == NULL) {

        //todo HANDLE
    }

    chdir(dir);
    //search for c file
    if (((pDirent = readdir(studentDir)) == NULL)) {
        depth = -1;
        setgrade()
        return;
    } else if (is_C_file(pDirent->d_name) == 1) {
        //compile and run

        compileFile(dir, pDirent->d_name, inputFile, outputFile, depth, , , studentDirName);
        return;
    }
    checkExecutableAndRun(dir, pDirent->d_name, depth, inputFile, outputFile, studentDirName);
}


void compileFile(char *dir, char *fileName, int inputFile, char *outputFile, int depth, char *compareProgPath,
                 char *initialPath, char *studentName) {

    char exeFilePath[SIZE];
    int status;
    int pid;
    char exeOutName[SIZE];

    //create exe path
    strtok(exeFilePath, dir);
    strtok(exeFilePath, "/");
    strtok(exeFilePath, fileName);


    pid = fork();
    //son process
    if (pid == 0) {

        execlp("gcc", "gcc", "-o", "student.out", fileName, NULL);

        //todo handle exec fail
    } else {

        //wait for sun
        pid = waitpid(pid, &status, 0);
        //check wait succeed
        if (pid == -1) {
            //todo handle wait failed
        };

        //check compile errors
        if (WIFEXITED (status)) {
            if (WEXITSTATUS(status) == 1) {
                //todo handle error compiling
            }
        } else if (WEXITSTATUS(status) == 0) {

            executeUserProg(dir, inputFile, initialPath, outputFile, depth);
        }
    }
}

int executeUserProg(char *dirName, int inputFileDescriptor,
                    char *compareProgPath, char *givenOutputFile, int depth) {

    //declare variables
    __pid_t pid;
    int status;
    int result;
    int success;
    int userOutputDescriptor;
    char userProgName[SIZE];
    char userOutputName[SIZE];

    //create output file
    userOutputDescriptor = open("studentOutput", O_RDWR | O_APPEND | O_EXCL, 0666);

    //create program out path
    strtok(userProgName, dirName);
    strtok(userProgName, "/");
    strtok(userProgName, "student.out");

    //create user output path
    strtok(userOutputName, dirName);
    strtok(userOutputName, "/");
    strtok(userOutputName, "studentOutput");

    //use dup for input
    success = dup2(inputFileDescriptor, 0);
    if (success == -1) {
        //todo handle dup fail
    }

    //ude dup for output
    success = dup2(userOutputDescriptor, 1);
    if (success == -1) {
        //todo handle dup fail
    }

    //create sun to run user program
    pid = fork();
    if (pid < 0) {
        //todo handle error
    }

    //we are son process
    if (pid == 0) {
        //run program
        execlp(dirName, userProgName, NULL);

        //todo check for timeout or exeptiom and set grade

        //we are father process
    } else {
        sleep(5);
        //wait for son
        if ((success = waitpid(pid, &status, WNOHANG)) == 0) {
            //todo handle wait error
            setgrade
        } else if (success == -1) {
            //todo handle error waiting
            setgrade
        }
        //run my compare program
        result = runCompare(userOutputName, givenOutputFile, compareProgPath, depth, studentName, resultFile);

        chdir(getenv("HOME"));
        chdir(dirName);
        unlink(userProgName);
        unlink(userOutputName);
        return result;

    }

    //todo delete a.out file and output

}

int runCompare(char *userOutput, char *outputFile, char *progDirPath, int depth, char *studentName, int resultFile) {
    int status;
    __pid_t pid;
    DIR *progDir;
    int compareResult;
    chdir(getenv("HOME"));

    progDir = opendir(progDirPath);
    if (progDir == NULL) {

        //todo handle
    }

    chdir(progDir);
    //fork a son
    if ((pid = fork()) < 0) {
        //todo handle error
    }

    if (pid == 0) {
        //run my ex11 on it
        execlp(progDirPath, "ex11", userOutput, outputFile, NULL);
        //check if fail
    } else {

        if (waitpid(pid, &status, 0) == -1) {
            // todo handle wait error
        }

        if (WIFEXITED(status)) {
            compareResult = WEXITSTATUS(status);
            setGrade(compareResult, depth, resultFile, studentName);
        } else {

        }


    }


}

int calcGrade(int grade, int depth) {

    int temp;
    switch (grade) {
        case 1:

            temp = (100) - (10 * depth);
            if (temp > 0) {
                return temp;
            }
            return 0;
        case 2:
            temp = (70) - (10 * depth);
            if (temp > 0) {
                return temp;
            }
            return 0;
        case 3:
            return 0;
        default:
            return 0;
    }
}

void setGrade(int grade, int depth, int resultFile, char *studentName) {
    char finalDetails[512];
    char gradeDescription[128];
    char gradeString[128];
    strtok(studentName, ",");
    //convert grade to string
    int temp;
    int writen;
    temp = calcGrade(grade, depth);

    writen = snprintf(gradeString, 128, "%d", temp);
    if (writen < 0) {
        //todo handle this
    }


    strtok(finalDetails, studentName);
    strtok(finalDetails, ",");
    strtok(finalDetails, gradeString);
    strtok(finalDetails, ",")
    strtok(finalDetails, gradeDescription);
    write(resultFile, studentName, strlen(finalDetails));
    check
    write fail

}

int is_C_file(char *pDirent) {
    size_t suffix = strlen(pDirent) - 1;
    if ((suffix > 2) && (pDirent[suffix] == 'c') && (pDirent[suffix - 1] == '.')) { //todo >2 or >=2
        return 1;
    }
    return 0;
}

int checkForManyFOlders(char *temp) {

    struct dirent *pDirnet;
    DIR *dir;
    int num = 0;
    dir = opendir(temp);
    if (dir == NULL) {

        //TODO HANDLE!
    }

    while ((pDirnet = readdir(dir)) != NULL) {
        num++;
    }
    return num;
}
