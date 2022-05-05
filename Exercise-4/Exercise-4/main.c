#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define ALO "***Allocation failed!***\n"

typedef struct StudentCourseGrade
{
    char courseName[35];
    int grade;
}StudentCourseGrade;

typedef struct Student
{
    char name[35];
    StudentCourseGrade* grades; //dynamic array of courses
    int numberOfCourses;
}Student;

void itoa(int num, char** str, int base); //itoa() made for allocated memory
char *read_file(char *filename); //make file pointer in allocated memory
//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
    //Part A
    int* coursesPerStudent = NULL;
    int numberOfStudents = 0;
    char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
    factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
    printStudentArray(students, coursesPerStudent, numberOfStudents);
    //studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
    
    //Part B
    Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
    writeToBinFile("students.bin", transformedStudents, numberOfStudents);
    Student* testReadStudents = readFromBinFile("students.bin");

    //add code to free all arrays of struct Student


    /*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
    // Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

    return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    if (!fileName) return;
    fopen(fileName, "rt");
    char* fileTemp = (char*)calloc(strlen(fileName)+1, sizeof(char));
    if (fileTemp == NULL) {
        printf(ALO); exit(1);
    }
    strcpy(fileTemp, fileName);
    while (fileTemp) {
        (*numberOfStudents)++;
        fileTemp = strchr(fileTemp, '\n');
        fileTemp++;
    }
    *coursesPerStudent = (int*)calloc(sizeof(int),*numberOfStudents);
    if (*coursesPerStudent == NULL) {
        printf(ALO); exit(1);
    }
    for (int i = 0; i < *numberOfStudents; i++) {
       (*coursesPerStudent)[i] = countPipes(fileName, *numberOfStudents);
    }
    free(fileTemp);
}

int countPipes(const char* lineBuffer, int maxCount)
{
    int shown = 0;
    
    if (!lineBuffer) {
        return -1;
    }
    if (maxCount <= 0) {
        return 0;
    }
    while (lineBuffer || maxCount > 0) {
        lineBuffer = strchr(lineBuffer, '|');
        if (*lineBuffer == '|') shown++;
        lineBuffer++;
        maxCount--;
    }
    return shown;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    char* fileTemp;
    FILE* file = fopen(fileName, "rt");
    if (file == NULL) {
        printf("Error reading file!\n");
        exit(1);
    }
    fileTemp = file;
    printf("%s", file);
    char* pieceTok, *temp_str;
    int i = 0, j = 0;

    char*** newStudentArray = malloc(sizeof(char**));
    if (newStudentArray == NULL) {
        printf(ALO); exit(1);
    }
/*
    for (int i = 0; i < *numberOfStudents; i++) {
        for (int j = 0; j < (*coursesPerStudent)[i] *2; j++) {
            newStudentArray[i][j] = malloc(sizeof(char));
            if (newStudentArray[i][j] == NULL) {
                printf(ALO); exit(1);
            }
        }
    }*/
    strcpy(fileTemp, fileName);
    pieceTok = strtok(fileTemp, "|");
    while (pieceTok != NULL) {
                newStudentArray[i] = (char**)malloc(sizeof(char*));
        newStudentArray[i][j] = (char*)calloc(strlen(pieceTok)+1, sizeof(char));
                strcpy(newStudentArray[i][j++],pieceTok);
                pieceTok = strtok(NULL, ",");
            newStudentArray[i][j] = (char*)calloc(strlen(pieceTok)+1, sizeof(char));
            strcpy(newStudentArray[i][j++],pieceTok);
        pieceTok = strtok(NULL, "|");
            temp_str = pieceTok;
        pieceTok = strtok("\n", NULL);
            if (pieceTok){
                newStudentArray[i][j] = (char*)calloc(strlen(temp_str)+1, sizeof(char));
                strcpy(newStudentArray[i][j],temp_str);
                j++;
                newStudentArray[i][j] = (char*)calloc(strlen(pieceTok)+1, sizeof(char));
                strcpy(newStudentArray[i][j],pieceTok);
                j++;
            }
        i++;
            temp_str = NULL;
        pieceTok = strtok(NULL, "|");
    }
    countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
    return newStudentArray;
}
/*   for (int i = 0; i < 3; i++) {
    for (int j = 0; i < 3; j++) {
        free(newStudentArray[i][j]);
 newStudentArray[i][j] = NULL;
    }
}
 free(newStudentArray);
 newStudentArray = NULL;
 // free allocation for 3D array */

//    Student* student = (Student*)malloc(sizeof(Student));
    //add code here
//    return '\0';
//}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
int grade_num;
if (factor > 20 || factor < -20) return;
for (int i = 0; i < numberOfStudents; i++) {
 for (int j = 1; j < (coursesPerStudent[i])*2; j++) {
    if (!strcmp(courseName, students[i][j])) {
    grade_num = atoi(students[i][j]) + factor;
    if ((grade_num >= 0) && (grade_num <= 100))
    itoa(grade_num, &(students[i][j]), 10);
    if (grade_num < 0) {
    students[i][j] = (char*)realloc(students[i][j], sizeof(char)*2);
    students[i][j] = "0";
    }
    if (grade_num > 100) {
    students[i][j] = (char*)realloc(students[i][j], sizeof(char)*4);
    students[i][j] = "100";
        }
    }
}
    }
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
    for (int i = 0; i < numberOfStudents; i++)
    {
        printf("name: %s\n*********\n", students[i][0]);
        for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
        {
            printf("course: %s\n", students[i][j]);
            printf("grade: %s\n", students[i][j + 1]);
        }
        printf("\n");
    }
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
    //add code here
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
    //add code here
}

Student* readFromBinFile(const char* fileName)
{
    Student* std= (Student*)malloc(sizeof(Student));
    return std;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
    Student* std= (Student*)malloc(sizeof(Student));
    return std;
}
    // Implementation of itoa()
void itoa(int num, char** str, int base)
    {
        int i = 0, temp_num = 0,rem = 0;
        int isNegative = 0;
    
    temp_num = num;
    
        /* Handle 0 explicitly, otherwise empty string is printed for 0 */
        if (num == 0)
        {
           *str = realloc(*str, sizeof(char)*2);
            *(str[i++]) = '0';
            *(str[i]) = '\0';
            return ;
        }
     
        // In standard itoa(), negative numbers are handled only with
        // base 10. Otherwise numbers are considered unsigned.
        if (num < 0 && base == 10)
        {
            isNegative = 1;
            num = -num;
        }
    
    while (temp_num != 0) {
        rem++;
        temp_num = temp_num/base;
    }
    
    *(str) = realloc(*(str), sizeof(char)*(rem+1));
    
        // If number is negative, append '-'
        if (isNegative){
            *(str) = realloc(*(str), sizeof(char)*(rem+2));
            *(str[i]) = '-';
            i++;
        }
    rem = 0;

        // Process individual digits
        while (num != 0)
        {
            rem = num % base;
            *(str[i++]) = (rem > 9)? (rem-10) + 'a' : rem + '0';
            num = num/base;
        }
        *(str[i]) = '\0'; // Append string terminator
    
    return ;
}

// Reads and stores the whole contents of the file with filename into a
// dynamically allocated char array on the heap, returns a pointer to this char
// array (or NULL if there was an error reading the file contents)/
char *read_file(char *filename)
{
  // file pointer variable used to access the file
  FILE *file;
  
  // attempt to open the file in read mode
  file = fopen(filename, "rt");
  
  // if the file fails to open, return NULL as an error return value
  if (file == NULL) return NULL;
  
  // move the file pointer to the end of the file
  fseek(file, 0, SEEK_END);

  // fseek(file) will return the current value of the position indicator,
  // which will give us the number of characters in the file
  int length = (int)ftell(file);

  // move file pointer back to start of file so we can read each character
  fseek(file, 0, SEEK_SET);
  
  // dynamically allocate a char array to store the file contents, we add 1 to
  // length for the null terminator we will need to add to terminate the string
  char *string = malloc(sizeof(char) * (length+1));
  
  // c will store each char we read from the string
  char c;

  // i will be an index into the char array string as we read each char
  int i = 0;

  // keep reading each char from the file until we reach the end of the file
  while ( (c = fgetc(file)) != EOF)
  {
    // store char into the char array string
    string[i++] = c;
  }

  // put a null terminator as the final char in the char array to properly
  // terminate the string
  string[i] = '\0';
  
  // close the file as we are now done with it
  fclose(file);
  
  // return a pointer to the dynamically allocated string on the heap
  return string;
}
/*
char* itoa(int num, char* buffer, int base)
{
int current = 0;
if (num == 0) {
buffer[current++] = '0';
buffer[current] = '\0';
return buffer;
}
int num_digits = 0;
if (num < 0) {
if (base == 10) {
num_digits ++;
buffer[current] = '-';
current ++;
num *= -1;
}
else
return NULL;
}
num_digits += (int)floor(log(num) / log(base)) + 1;
while (current < num_digits)
{
int base_val = (int) pow(base, num_digits-1-current);
int num_val = num / base_val;
 char value = num_val + '0';
buffer[current] = value;
current ++;
num -= base_val * num_val;
}
buffer[current] = '\0';
return buffer;*/


