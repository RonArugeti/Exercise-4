#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LINE 1023
#define ALO "***Allocation Failed***\n" // failed in allocating memory
#define EIO fprintf(stdout, "Error opening file\n") // screen printing failed in openning
#define EIC fprintf(stdout, "Error in closing file\n") //screen printing failed in closeing
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
void printStudentArray(char** const* students, const int* coursesPerStudent, int numberOfStudents);
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
    for (int i = 0, j = 0; i < numberOfStudents; i++) {
        printf("%s\n", students[i][j++]);
        for (; j <= (2 * coursesPerStudent[i]); j++) {
            printf("%s\n", students[i][j]);
        }
        printf("\n");
        j = 0;
    }
    
    factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
    printStudentArray(students, coursesPerStudent, numberOfStudents);
    //studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)
    
    //Part B
    Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
    
    for (int i = 0; i < numberOfStudents; i++) {
        for (int j = 0; j < coursesPerStudent[i]; j++) {
            if (!j) {
            printf("name: %s\n", (transformedStudents + i)->name);
                printf("number of courses: %d\n", (transformedStudents + i)->numberOfCourses);}
            printf("course number %d is name: %s\n", j+1, (transformedStudents + i)->grades[j].courseName);
            printf("grade of the course: %d\n", (transformedStudents + i)->grades[j].grade);
        }
        printf("\n");
    }

    writeToBinFile("students.bin", transformedStudents, numberOfStudents);
    Student* testReadStudents = readFromBinFile("students.bin");
     
    for (int i = 0; i < numberOfStudents; i++) {
        printf("%s\n %d\n\n", (testReadStudents + i)->name, (testReadStudents + i)->numberOfCourses);
        for (int j = 0; j < (testReadStudents + i)->numberOfCourses; j++) {
            printf("%s\n %d\n", (testReadStudents + i)->grades[j].courseName, (testReadStudents + i)->grades[j].grade);
        }
    }
    
    //code to free all arrays of struct Student
    for (int i = numberOfStudents; i >= 0; i--) {
     free((testReadStudents + i)->grades);
        (testReadStudents + i)->grades = NULL;
      }
    free(testReadStudents);
         testReadStudents = NULL;
  
    fflush(stdin);
    /*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
    // Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

    return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    FILE* file = fopen(fileName, "rt");
    if (file == NULL) { EIO; exit(1);}
    char* fileTemp = (char*)calloc(MAX_LINE, sizeof(char));
    if (fileTemp == NULL) { printf(ALO); exit(1);}
    (*numberOfStudents) = -1;
    while (!feof(file)) {
    fgets(fileTemp, MAX_LINE, file);
        (*numberOfStudents)++;
    }
    fseek(file, 0, SEEK_SET);
    (*coursesPerStudent) = (int*)calloc(sizeof(int), (*numberOfStudents));
    if (*coursesPerStudent == NULL || fileTemp == NULL) { printf(ALO); exit(1);}
    for (int i = 0; i < (*numberOfStudents); i++) {
        fgets(fileTemp, MAX_LINE, file);
        (*coursesPerStudent)[i] = countPipes(fileTemp, MAX_LINE);
    }
    if (fclose(file) == EOF) EIC;
    free(fileTemp);
    fileTemp = NULL;
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
        if (!lineBuffer) break;
        shown++;
        lineBuffer++;
        maxCount--;
    }
    return shown;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
    FILE* file = fopen(fileName,"rt");
    if (file == NULL) { EIO; exit(1);}
    
    char* fileTemp = (char*)calloc(sizeof(char) ,(MAX_LINE+1));
    if (fileTemp == NULL) { printf(ALO); exit(1);}
    
    char* pieceTok, *temp_str;
    int i = 0, j = 0;
    fseek(file, 0, SEEK_END);
    char*** newStudentArray = (char***)calloc(sizeof(char**), (int)ftell(file));
    if (newStudentArray == NULL) { printf(ALO); exit(1);}
    fseek(file, 0, SEEK_SET);
    //set the first line in string "fileTemp"
    fgets(fileTemp, MAX_LINE, file);
    while (file) {
        //if the file gets to the end of the file the loop ends
        if (feof(file)) break;
        fileTemp = strchr(fileTemp, '|');
        while (1) { fileTemp--; if(fileTemp[0] >= 'A' && fileTemp[0] <= 'Z') break; }
        pieceTok = strtok(fileTemp, "|");
    while (pieceTok != NULL) {
        if (!j) {
        newStudentArray[i] = (char**)calloc(sizeof(char*), MAX_LINE);
        newStudentArray[i][j] = (char*)calloc((int)strlen(pieceTok)+1, sizeof(char));
            newStudentArray[i][j][(int)strlen(pieceTok)] = '\0';
            strcpy(newStudentArray[i][j++],pieceTok);
        }
        pieceTok = strtok(NULL, ",");
        if (!pieceTok) break;
        newStudentArray[i][j] = (char*)calloc((int)strlen(pieceTok)+1, sizeof(char));
        newStudentArray[i][j][(int)strlen(pieceTok)] = '\0';
            strcpy(newStudentArray[i][j++],pieceTok);
        pieceTok = strtok(NULL, "|");
        if (!pieceTok) break;
            if (pieceTok){
                if ((temp_str = strchr(pieceTok, '\n'))) {
                    temp_str = pieceTok;
                    while (temp_str[0] >= '0' && temp_str[0] <= '9') {
                    temp_str++;
                }
                    temp_str[0] = '\0';
                }
                newStudentArray[i][j] = (char*)calloc((int)strlen(pieceTok)+1, sizeof(char));
                newStudentArray[i][j][(int)strlen(pieceTok)] = '\0';
                strcpy(newStudentArray[i][j++],pieceTok);
            }
        temp_str = NULL;
        }
        i++;
        j = 0;
        
        fileTemp = (char*)calloc(sizeof(char), MAX_LINE);
        if (fileTemp == NULL) {printf(ALO); exit(1);}
     //set each next line in the string "fileTemp"
        fgets(fileTemp, MAX_LINE, file);
    }
    free(fileTemp);
    fileTemp = NULL;
    if (fclose(file) == EOF) EIC;
    countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
    return newStudentArray;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
int grade_num = 0;
if (factor > 20 || factor < - 20) return;
for (int i = 0; i < numberOfStudents; i++) {
 for (int j = 1; j < (coursesPerStudent[i]); j += 2) {
    if (!strcmp(courseName, students[i][j])) {
        grade_num = atoi(students[i][j+1]) + factor;
    if ((grade_num > 0) && (grade_num < 100))
    itoa(grade_num, &(students[i][j+1]), 10);
    if (grade_num < 0) {
    students[i][j+1] = (char*)realloc(students[i][j+1], sizeof(char)*2);
        strcpy(students[i][j+1], "0");
    }
    if (grade_num >= 100) {
    students[i][j+1] = (char*)realloc(students[i][j+1], sizeof(char)*4);
        strcpy(students[i][j+1], "100");
      }
    }
}
    }
}

void printStudentArray(char** const* students, const int* coursesPerStudent, int numberOfStudents)
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
    FILE* newFile;
    newFile = fopen("studenList.txt", "wt+");
    if (newFile == NULL) { EIO; exit(1);}
    rewind(newFile);
    for (int i = 0; i < numberOfStudents; i++) {
        for (int j = 0; j < coursesPerStudent[i]; j++) {
            fputs(students[i][j],newFile);
            if (j % 2) {
            fseek(newFile, -1, SEEK_CUR);
            fputc(',', newFile);
        }
            fseek(newFile, -1, SEEK_CUR);
            fputc('|', newFile);
            
        }
        fseek(newFile, -1, SEEK_CUR);
        fputc('\n', newFile);
    }
    for (int i = numberOfStudents -1; i >= 0 ; i--) {
        for (int j = coursesPerStudent[i]; j >= 0; j--) {
            free(students[i][j]);
            students[i][j] = NULL;
        }
        free(students[i]);
             students[i] = NULL;
            }
    fflush(newFile);
    if (fclose(newFile) == EOF) EIC;
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
    char bar = '|', comma = ',', space_bar = '\n';
    FILE* StuBinFile = fopen(fileName, "wb");
    if (StuBinFile == NULL) { EIO; exit(1);}
    fwrite(&numberOfStudents, sizeof(int), 1, StuBinFile);
    for (int i = 0; i < numberOfStudents; i++) {
        for (int j = 0; j < ((students + i)->numberOfCourses); j++) {
            if (j == 0) {
    fwrite((students + i)->name, sizeof(char), 35, StuBinFile);
    fseek(StuBinFile, - sizeof(char), SEEK_CUR);
    fwrite(&bar, sizeof(char), 1, StuBinFile);
    fwrite(&((students + i)->numberOfCourses), sizeof(int), 1, StuBinFile);
    }
    fwrite((students + i)->grades[j].courseName, sizeof(char), 35, StuBinFile);
    fseek(StuBinFile, - sizeof(char), SEEK_CUR);
    fwrite(&comma, sizeof(char), 1, StuBinFile);
    fwrite(&((students + i)->grades[j].grade), sizeof(int), 1, StuBinFile);
        }
    fwrite(&space_bar, sizeof(char), 1, StuBinFile);
    }
    fflush(StuBinFile);
    if (fclose(StuBinFile) == EOF) EIC;
}
Student* readFromBinFile(const char* fileName)
{
    FILE* BinFile = fopen(fileName, "rt");
    if (BinFile == NULL) { EIO; exit(1);}
    int sizeOfStudents = 0, i = 0;
    fscanf(BinFile, "%d", &sizeOfStudents);
    Student* newStudent = (Student*)calloc(sizeof(Student), sizeOfStudents);
    fseek(BinFile, sizeof(int), SEEK_SET);
    while (feof(BinFile) != EOF) {
        fscanf(BinFile, "%d", &((newStudent + i)->numberOfCourses));
        fscanf(BinFile, "%[^|]", (newStudent + i)->name);
        (newStudent + i)->grades = (StudentCourseGrade*)calloc(sizeof(StudentCourseGrade), (newStudent + i)->numberOfCourses);
        fseek(BinFile, sizeof(char), SEEK_CUR);
        for (int j = 0; j < 2 *(newStudent + i)->numberOfCourses; j++) {
            fscanf(BinFile, "%[^ A-Z]", (newStudent + i)->grades[j].courseName);
            fscanf(BinFile, "%[^ ,]", (newStudent + i)->grades[j].courseName);
            fseek(BinFile, sizeof(char), SEEK_CUR);
            fscanf(BinFile, "%d", &((newStudent + i)->grades[j].grade));
            fseek(BinFile, sizeof(char), SEEK_CUR);
        }
        i++;
    }
    i = 0;
    fflush(BinFile);
    if (fclose(BinFile) == EOF) EIC;
    return newStudent;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
    int d = 0;
    Student* std = (Student*)calloc(sizeof(Student), numberOfStudents);
    for (int i = 0; i < numberOfStudents; i++) {
(std + i)->grades = (StudentCourseGrade*)calloc(sizeof(StudentCourseGrade), coursesPerStudent[i]);
        if ((std + i) == NULL) { printf(ALO); exit(1);}
        for (int j = 0; j < (2* coursesPerStudent[i]); j++) {
            if (j == 0) strcpy((std + i)->name, students[i][j++]);
            strcpy((std + i)->grades[d].courseName, students[i][j++]);
            (std + i)->grades[d++].grade = atoi(students[i][j]);
        }
        d = 0;
        (std + i)->numberOfCourses = coursesPerStudent[i];
    }
    return std;
}
    // Implementation of itoa()
void itoa(int num, char** str, int base)
    {
        int i = 0, temp_num = 0, rem = 0, mul = 0;
        int isNegative = 0;
    
      temp_num = num;
    
        // Handle 0 explicitly, otherwise empty string is printed for 0
        if (num == 0)
        {
           *str = realloc(*str, sizeof(char)*2);
            str[0][i++] = '0';
            str[0][i] = '\0';
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
        mul++;
        temp_num = temp_num/base;
    }
    
    *str = (char*)realloc(*(str), sizeof(char)*(mul+1));
    
        // If number is negative, append '-'
        if (isNegative){
            *str = realloc(*(str), sizeof(char)*(mul+2));
            str[0][i] = '-';
            mul++;
        }
    i = (int)strlen(*str);
        // Process individual digits
        while (num != 0)
        {
            rem = num % base;
            str[0][--mul] = (rem > 9)? (rem-10) + 'a' : rem + '0';
            num = num/base;
        }
    str[0][++i] = '\0';
    return ;
}

