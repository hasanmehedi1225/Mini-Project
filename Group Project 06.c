// UNIVERSITY RESULT MANAGEMENT SYSTEM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_NAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 20
#define MAX_STUDENTS 1200
#define MAX_SUBJECTS 10

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
} Teacher;

typedef struct {
    int roll;
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char department[MAX_NAME_LENGTH];
    int semester;
    char section;
    float cgpa;
    int marks[MAX_SUBJECTS];
    int credits[MAX_SUBJECTS];
} Student;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int credits;
} Subject;

typedef struct {
    int roll;
    char name[MAX_NAME_LENGTH];
    float cgpa;
    int semester;
} MeritPosition;

void add_teacher();
void add_student();
int id_compare(Teacher *t, int id);
void update_marks();
void display_student(Student *s);
int get_roll(Student *s);
void view_all_students();
void filter_and_sort_students();
void search_student();
void delete_student();
void load_students(Student *students, int *count);
void save_students(Student *students, int count);
void update_cgpa(Student *s);
void initialize_students();
float calculateCGPA(float marks[], int num_subjects, int credits[]);
void display_marksheet(Student *student, Subject *subjects, int num_subjects);
void load_subjects(Subject *subjects, int *num_subjects, int semester);
void initialize_subjects();
float get_grade_point(int marks);
const char* get_grade(int marks);
void calculate_merit_positions(int semester);
Student students[MAX_STUDENTS];
int student_count = 0;

int main() {
    initialize_students();
    initialize_subjects();

    printf("\n\nWelcome to University Result Management System");
    int opt = 0, subopt = 0;

    while (opt != 5) {
        printf("\n\nMenu:\n1. Admin\n2. Teacher: Update Marks\n3. Student: Check Results\n4. View All Students\n5. Exit\n\nEnter option: ");
        scanf("%d", &opt);

        switch (opt) {
            case 1:
                printf("\n\nSub Menu:\n1. Add teacher\n2. Add student\n3. Delete student\n\nEnter option: ");
                scanf("%d", &subopt);
                switch (subopt) {
                    case 1:
                        add_teacher();
                        break;
                    case 2:
                        add_student();
                        break;
                    case 3:
                        delete_student();
                        break;
                    default:
                        printf("\n\nInvalid choice");
                        break;
                }
                break;

            case 2:
                update_marks();
                break;

            case 3: {
                printf("\n\nEnter student roll no: ");
                int roll;
                scanf("%d", &roll);

                char password[MAX_PASSWORD_LENGTH];
                printf("Enter password: ");
                scanf("%s", password);

                Student st;
                FILE *fs = fopen("student.dat", "rb");
                if (!fs) {
                    printf("\n\nUnable to open student.dat file");
                    break;
                }

                int found = 0;
                while (fread(&st, sizeof(Student), 1, fs)) {
                    if (roll == get_roll(&st) && strcmp(st.password, password) == 0) {
                        display_student(&st);

                        Subject subjects[MAX_SUBJECTS];
                        int num_subjects = 0;
                        load_subjects(subjects, &num_subjects, st.semester);
                        display_marksheet(&st, subjects, num_subjects);

                        found = 1;
                        break;
                    }
                }

                if (found == 0)
                    printf("\n\nInvalid roll number or password");

                fclose(fs);
                break;
            }


case 4:
                printf("\n\nSub Menu:\n1. View all students\n2. Filter and sort students\n3. Search for a student\n4. Calculate Merit Positions (Semester-wise)\n\nEnter option: ");
                scanf("%d", &subopt);
                switch (subopt) {
                    case 1:
                        view_all_students();
                        break;
                    case 2:
                        filter_and_sort_students();
                        break;
                    case 3:
                        search_student();
                        break;
                    case 4:
                        printf("\nEnter semester: ");
                        int sem;
                        scanf("%d", &sem);
                        calculate_merit_positions(sem);
                        break;
                    default:
                        printf("\n\nInvalid choice");
                        break;
                }
                break;

            case 5:
                return 0;

            default:
                printf("\n\nInvalid choice");
                break;
        }
    }

    return 0;
}

void add_teacher() {
    FILE *f = fopen("teacher.dat", "ab");
    if (!f) {
        printf("Error opening file for writing.\n");
        return;
    }

    Teacher t;
    printf("Enter teacher ID: ");
    scanf("%d", &t.id);
    printf("Enter teacher name: ");
    scanf("%s", t.name);

    fwrite(&t, sizeof(Teacher), 1, f);
    fclose(f);
}

void add_student() {
    if (student_count >= MAX_STUDENTS) {
        printf("Maximum student limit reached.\n");
        return;
    }

    Student s;
    printf("Enter student roll number: ");
    scanf("%d", &s.roll);
    printf("Enter student name: ");
    scanf("%s", s.name);
    printf("Enter password: ");
    scanf("%s", s.password);
    printf("Enter department: ");
    scanf("%s", s.department);

    printf("Enter semester: ");
    scanf("%d", &s.semester);

    printf("Enter section: ");
    scanf(" %c", &s.section);
    printf("Enter marks for subjects: \n");

    // Load subjects for the semester
    Subject subjects[MAX_SUBJECTS];
    int num_subjects = 0;
    load_subjects(subjects, &num_subjects, s.semester);

    // Input marks and credits
    for (int i = 0; i < num_subjects; i++) {
        printf("%s: ", subjects[i].name);
        scanf("%d", &s.marks[i]);
        s.credits[i] = subjects[i].credits;  // Assign credits from loaded subjects
    }

    // Calculate CGPA based on marks and credits
    update_cgpa(&s);

    students[student_count++] = s;
    save_students(students, student_count);
}

int id_compare(Teacher *t, int id) {
    return t->id == id;
}

void update_marks() {
    FILE *fs = fopen("student.dat", "rb+");
    if (!fs) {
        printf("\n\nUnable to open student.dat file");
        return;
    }

    int roll;
    printf("\n\nEnter student roll no: ");
    scanf("%d", &roll);

    Student st;
    int found = 0;
    while (fread(&st, sizeof(Student), 1, fs)) {
        if (roll == get_roll(&st)) {
            printf("\n\nUpdating marks for student: %s (Roll No: %d)\n", st.name, st.roll);
            printf("Enter marks for subjects: \n");
            for (int i = 0; i < MAX_SUBJECTS; i++) {
                scanf("%d", &st.marks[i]);
            }
            update_cgpa(&st);

            fseek(fs, -sizeof(Student), SEEK_CUR);
            fwrite(&st, sizeof(Student), 1, fs);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("\n\nStudent not found");

    fclose(fs);
}

void display_student(Student *s) {
    printf("\n\nStudent Details:\n");
    printf("Roll No: %d\n", s->roll);
    printf("Name: %s\n", s->name);
    printf("Department: %s\n", s->department);
    printf("Semester: %d\n", s->semester);
    printf("Section: %c\n", s->section);
    printf("CGPA: %.2f\n", s->cgpa);
}

int get_roll(Student *s) {
    return s->roll;
}

void view_all_students() {
    printf("\n\nAll Students:\n");

    for (int i = 0; i < student_count; i++) {
        display_student(&students[i]);
    }
}


void filter_and_sort_students() {
    // Example: Filter by department and sort by CGPA
    char department[MAX_NAME_LENGTH];
    printf("\n\nEnter department to filter by: ");
    scanf("%s", department);

    Student filtered_students[MAX_STUDENTS];
    int count = 0;

    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].department, department) == 0) {
            filtered_students[count++] = students[i];
        }
    }

    // Sort by CGPA in descending order
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (filtered_students[j].cgpa < filtered_students[j + 1].cgpa) {
                Student temp = filtered_students[j];
                filtered_students[j] = filtered_students[j + 1];
                filtered_students[j + 1] = temp;
            }
        }
    }

    printf("\n\nFiltered and Sorted Students:\n");
    for (int i = 0; i < count; i++) {
        display_student(&filtered_students[i]);
    }
}

void search_student() {
    int roll;
    printf("\n\nEnter student roll number: ");
    scanf("%d", &roll);

    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            display_student(&students[i]);
            return;
        }
    }

    printf("Student not found.\n");
}

void delete_student() {
    int roll;
    printf("\n\nEnter student roll number to delete: ");
    scanf("%d", &roll);

    int found = 0;
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            for (int j = i; j < student_count - 1; j++) {
                students[j] = students[j + 1];
            }
            student_count--;
            found = 1;
            save_students(students, student_count);
            printf("Student deleted.\n");
            break;
        }
    }

    if (!found) {
        printf("Student not found.\n");
    }
}

void load_students(Student *students, int *count) {
    FILE *fs = fopen("student.dat", "rb");
    if (!fs) {
        printf("Error opening student.dat file.\n");
        return;
    }

    int i = 0;
    while (fread(&students[i], sizeof(Student), 1, fs)) {
        i++;
    }
    *count = i;
    fclose(fs);
}

void save_students(Student *students, int count) {
    FILE *fs = fopen("student.dat", "wb");
    if (!fs) {
        printf("Error opening student.dat file for writing.\n");
        return;
    }

    fwrite(students, sizeof(Student), count, fs);
    fclose(fs);
}

void update_cgpa(Student *s) {
    int num_subjects = MAX_SUBJECTS;
    float marks[num_subjects];
    int credits[num_subjects];

    for (int i = 0; i < num_subjects; i++) {
        marks[i] = s->marks[i];
        credits[i] = s->credits[i];
    }

    s->cgpa = calculateCGPA(marks, num_subjects, credits);
}

void initialize_students() {
    load_students(students, &student_count);
}

float calculateCGPA(float marks[], int num_subjects, int credits[]) {
    float totalGradePoints = 0;
    int totalCredits = 0;

    for (int i = 0; i < num_subjects; i++) {
        float gradePoint = get_grade_point(marks[i]);
        totalGradePoints += gradePoint * credits[i];
        totalCredits += credits[i];
    }

    if (totalCredits == 0) {
        return 0;
    }

    return totalGradePoints / totalCredits;
}

void load_subjects(Subject *subjects, int *num_subjects, int semester) {
    char filename[20];
    sprintf(filename, "semester%d.dat", semester);

    FILE *fs = fopen(filename, "rb");
    if (!fs) {
        printf("Error opening %s file.\n", filename);
        return;
    }

    int i = 0;
    while (fread(&subjects[i], sizeof(Subject), 1, fs)) {
        i++;
    }
    *num_subjects = i;
    fclose(fs);
}

void initialize_subjects() {
    Subject semester1[] = {{"GED", 3}, {"English 1", 0}, {"English 2", 3}, {"Programming", 2}};
    FILE *fs = fopen("semester1.dat", "wb");
    if (!fs) {
        printf("Error opening semester1.dat file for writing.\n");
        return;
    }
    fwrite(semester1, sizeof(Subject), 4, fs);
    fclose(fs);


Subject semester2[] = {{"C program", 3}, {"C program lab", 1}, {"Math 1", 3}, {"Economic", 3}, {"Discreate math", 3}};
    fs = fopen("semester2.dat", "wb");
    if (!fs) {
        printf("Error opening semester2.dat file for writing.\n");
        return;
    }
    fwrite(semester2, sizeof(Subject), 5, fs);
    fclose(fs);
}
float get_grade_point(int marks) {
    if (marks >= 80) return 4.00;
    if (marks >= 75) return 3.50;
    if (marks >= 70) return 3.00;
    if (marks >= 65) return 2.75;
    if (marks >= 60) return 2.50;
    if (marks >= 55) return 2.00;
    if (marks >= 50) return 1.75;
    if (marks >= 45) return 1.50;
    if (marks >= 40) return 1.00;
    return 0.0;
}

const char* get_grade(int marks) {
    if (marks >= 80) return "A+";
    if (marks >= 75) return "A";
    if (marks >= 70) return "A-";
    if (marks >= 65) return "B+";
    if (marks >= 60) return "B";
    if (marks >= 55) return "B-";
    if (marks >= 50) return "C";
    if (marks >= 45) return "D";
    if (marks >= 40) return "E";
    return "F";
}

void display_marksheet(Student *student, Subject *subjects, int num_subjects) {
    printf("\nMarksheet for %s (Roll No: %d):\n", student->name, student->roll);
    printf("--------------------------------------------------------\n");
    printf("%-20s%-10s%-10s%-10s%-15s\n", "Subjects", "Credits", "Marks", "Grades", "Grade Points");
    printf("--------------------------------------------------------\n");

    int total_credits = 0;
    float total_grade_points = 0;

    for (int i = 0; i < num_subjects; i++) {
        int marks = student->marks[i];
        int credits = subjects[i].credits;
        const char* grade = get_grade(marks);  // Use const char* to match return type
        float grade_point = get_grade_point(marks);

        total_credits += credits;
        total_grade_points += grade_point * credits;

        printf("%-20s%-10d%-10d%-10s%-15.2f\n", subjects[i].name, credits, marks, grade, grade_point);
    }

    printf("--------------------------------------------------------\n");
    printf("%-20s%-10d\n", "Total credits:", total_credits);
    printf("%-20s%-10.2f\n", "Total grade points:", total_grade_points);

    // Calculate CGPA
    if (total_credits > 0) {
        student->cgpa = total_grade_points / total_credits;
    } else {
        student->cgpa = 0.0;
    }

    printf("CGPA = total grade points / total credits = %.2f\n", student->cgpa);
}




void calculate_merit_positions(int semester) {
    Student filtered_students[MAX_STUDENTS];
    int count = 0;

    for (int i = 0; i < student_count; i++) {
        if (students[i].semester == semester) {
            filtered_students[count++] = students[i];
        }
    }

    // Sort by CGPA in descending order
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (filtered_students[j].cgpa < filtered_students[j + 1].cgpa) {
                Student temp = filtered_students[j];
                filtered_students[j] = filtered_students[j + 1];
                filtered_students[j + 1] = temp;
            }
        }
    }

    printf("\n\nMerit Positions for Semester %d:\n", semester);
    printf("--------------------------------------------------\n");
    printf("Position\tRoll No\tName\tCGPA\n");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        printf("%d\t\t%d\t%s\t%.2f\n", i + 1, filtered_students[i].roll, filtered_students[i].name, filtered_students[i].cgpa);
    }
}
