
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"header.h"
#include<io.h>
#include <stdbool.h>


void replace_spaces_with_underscores(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            str[i] = '_';
        }
    }
}
//17
void add_password(const char* filename, USER* user) {
    if (user->num_passwords >= MAX_PASSWORDS) {
        printf("Password capacity limit reached for user '%s'.\n", user->name);
        return;
    }

    printf("Enter what the password is used for: ");
    scanf(" %49[^\n]", user->passwords[user->num_passwords].name_of_use);
    replace_spaces_with_underscores(user->passwords[user->num_passwords].name_of_use);
    while (getchar() != '\n');



    for (int i = 0; i < user->num_passwords; i++) {
        if (strcmp(user->passwords[user->num_passwords].name_of_use, user->passwords[i].name_of_use) == 0) {
            printf("Name of use '%s' already exists. Please choose another name or delete the previous one.\n", user->passwords[user->num_passwords].name_of_use);
            return;
        }
    }

    do {
        printf("Enter password: ");
        scanf(" %19[^\n]", user->passwords[user->num_passwords].password_stored);
        while (getchar() != '\n');

        if (strchr(user->passwords[user->num_passwords].password_stored, ' ') != NULL) {
            printf("Spaces are not allowed in passwords. Please try again: ");
        }
        else if (!validatePlaintext(user->passwords[user->num_passwords].password_stored)) {
            printf("Error: Password contains disallowed characters. \n");
        }
        else {
            break;
        }
    } while (1);




   
    encryptXOR(user->passwords[user->num_passwords].password_stored);

    user->num_passwords++;

    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "%s %s\n", user->passwords[user->num_passwords - 1].name_of_use, user->passwords[user->num_passwords - 1].password_stored);
    fclose(file);
}

PASS* update_password(const char* const filename, int* p_number_of_pass) {
    FILE* file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    //18
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    *p_number_of_pass = file_size / sizeof(PASS);

    PASS* array_of_passwords = malloc(*p_number_of_pass * sizeof(PASS));
    if (array_of_passwords == NULL) {
        perror("Memory allocation failed for passwords \n");
        fclose(file);
        return NULL;
    }

    fread(array_of_passwords, sizeof(PASS), *p_number_of_pass, file);
    printf("passwords have been updated. \n");
    fclose(file);
    return array_of_passwords;
}

void replace_underscores_with_spaces(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
}
void writeout_passwords(const char* filename, const USER* user) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    printf("Passwords for user '%s':\n", user->name);
    PASS password = { 0 };
    while (fscanf(file, "%s %s", password.name_of_use, password.password_stored) == 2) {

        decryptXOR(password.password_stored);

        replace_underscores_with_spaces(password.name_of_use);

        printf("Usage: %s, Password: %s\n", password.name_of_use, password.password_stored);
    }

    fclose(file);
}


void delete_password(const char* filename, USER* user) {
    char usage[MAX_NAME_LENGTH];

    printf("Enter the usage of the password you want to delete: ");
    scanf(" %49[^\n]", usage);


    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    //temp file w out deleted password
    FILE* temp_file = fopen("temp.txt", "w");
    if (temp_file == NULL) {
        perror("Error opening temporary file");
        fclose(file);
        return;
    }

    PASS password = { 0 };
    int found = 0;


    while (fscanf(file, "%s %s", password.name_of_use, password.password_stored) == 2) {

        replace_underscores_with_spaces(password.name_of_use);
        if (strcmp(password.name_of_use, usage) == 0) {
            printf("Password found:\n");

            decryptXOR(password.password_stored);

            printf("Usage: %s, Password: %s\n", password.name_of_use, password.password_stored);
            found = 1;

            char confirmation;
            printf("Are you sure you want to delete this password? (Y/N): ");
            scanf(" %c", &confirmation);
            if (confirmation == 'Y' || confirmation == 'y') {

                continue;
            }
            else if (confirmation == 'N' || confirmation == 'n') {

                fclose(file);
                fclose(temp_file);
                //19
                remove("temp.txt");
                printf("Deletion cancelled.\n");
                return;
            }
        }
        // temp file for passwords 
        fprintf(temp_file, "%s %s\n", password.name_of_use, password.password_stored);
    }

    fclose(file);
    fclose(temp_file);

    if (found) {

        if (remove(filename) != 0) {
            //20
            perror("Error removing original file");
            return;
        }

        // naming it back to original
        if (rename("temp.txt", filename) != 0) {
            perror("Error renaming file");
            return;
        }

        printf("Password deleted successfully.\n");
    }
    else {
        remove("temp.txt");
        printf("Password for '%s' was not found.\n", usage);
    }
}

void encryptXOR(char* password) {
    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        password[i] = password[i] ^ XOR_KEY;
    }
}

void decryptXOR(char* password) {
    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        password[i] = password[i] ^ XOR_KEY;
    }
}
//22 23 24
void search_password(const char* filename) {
    char search_name[MAX_NAME_LENGTH];
    printf("Enter the name to search: ");
    scanf(" %49[^\n]", search_name);

    int found = 0;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    PASS password = { 0 };
    while (fscanf(file, "%s %s", password.name_of_use, password.password_stored) == 2) {

        decryptXOR(password.password_stored);

        replace_underscores_with_spaces(password.name_of_use);

        if (strcmp(password.name_of_use, search_name) == 0) {
            found = 1;
            printf("Password found: %s\n", password.password_stored);
            break;
        }
    }

    fclose(file);

    if (!found) {
        printf("No password found for '%s'.\n", search_name);
    }
}

bool is_allowed(char c) {
   //allowed characters because encryption , some characters cant get encrypted by xor
    const char* allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 !#$%&/()=?*' ";

   
    return strchr(allowedChars, c) != NULL;
}

bool validatePlaintext(const char* plaintext) {

    for (int i = 0; plaintext[i] != '\0'; i++) {

        if (!is_allowed(plaintext[i])) {
            return false;
        }
    }

    return true;
}


int compare_names(const void* a, const void* b) {
    const PASS* password_a = (const PASS*)a;
    const PASS* password_b = (const PASS*)b;
    return strcmp(password_a->name_of_use, password_b->name_of_use);
}


void abc_print(const char* filename, USER* user) {
    user->num_passwords = count_lines_in_file(filename);
    if (user->num_passwords <= 0) {
        printf("No passwords to process or error occurred.\n");
        return;
    }

    printf("User: %s, Number of passwords: %d\n", user->name, user->num_passwords);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    printf("Passwords for user '%s':\n", user->name);

    PASS* passwords = malloc(user->num_passwords * sizeof(PASS));
    if (passwords == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return;
    }

    int count = 0;
    while (count < user->num_passwords) {
        int res = fscanf(file, "%99s %99s", passwords[count].name_of_use, passwords[count].password_stored);
        if (res == 2) {
          //  printf("Read pair: %s %s\n", passwords[count].name_of_use, passwords[count].password_stored); // using it for debug , to see any mistakes
            decryptXOR(passwords[count].password_stored);
            replace_underscores_with_spaces(passwords[count].name_of_use);
           // printf("Processed pair: %s %s\n", passwords[count].name_of_use, passwords[count].password_stored); // using it for debug , to see any mistakes
            count++;
        }
        else if (res == EOF) {
            if (feof(file)) {
                printf("End of file reached after reading %d pairs.\n", count);
            }
            else {
                perror("Error reading file");
            }
            break;
        }
        else {
            printf("Unexpected format or error at line %d. fscanf returned %d\n", count + 1, res);
            break;
        }
    }

    if (ferror(file)) {
        perror("Error reading file");
        free(passwords);
        fclose(file);
        return;
    }

   // sorts passwords
    qsort(passwords, count, sizeof(PASS), compare_names);

  //printing them sorted
    for (int i = 0; i < count; i++) {
        printf("Usage: %s, Password: %s\n", passwords[i].name_of_use, passwords[i].password_stored);
    }

    free(passwords);
    fclose(file);

}

int count_lines_in_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for counting lines");
        return -1;
    }

    int lines = 0;
    char ch;
    while (!feof(file)) {
        ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    fclose(file);
    return lines;
}

void change_password(const char* filename, USER* user) {
    char usage[MAX_NAME_LENGTH];

    printf("Enter the usage of the password you want to change: ");
    scanf(" %49[^\n]", usage);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    FILE* temp_file = fopen("temp.txt", "w");
    if (temp_file == NULL) {
        perror("Error opening temporary file");
        fclose(file);
        return;
    }

    PASS password = { 0 };
    int found = 0;

    while (fscanf(file, "%s %s", password.name_of_use, password.password_stored) == 2) {
        replace_underscores_with_spaces(password.name_of_use);
        decryptXOR(password.password_stored);

        if (strcmp(password.name_of_use, usage) == 0) {
            printf("Password found:\n");
            printf("Usage: %s, Password: %s\n", password.name_of_use, password.password_stored);
            found = 1;

            char new_password[MAX_PASS_LENGTH];
            printf("Enter new password: ");
            do {
                scanf(" %19[^\n]", new_password);
                while (getchar() != '\n');

                if (strchr(new_password, ' ') != NULL) {
                    printf("Spaces are not allowed in passwords! Please try again.\n");
                } else if (!validatePlaintext(new_password)) {
                    printf("Error: Password contains disallowed characters.\n");
                } else {
                    break;
                }
            } while (1);

            strcpy(password.password_stored, new_password);
        }

        encryptXOR(password.password_stored);
        replace_spaces_with_underscores(password.name_of_use);
        fprintf(temp_file, "%s %s\n", password.name_of_use, password.password_stored);
    }

    fclose(file);
    fclose(temp_file);

    if (found) {
        if (remove(filename) != 0) {
            perror("Error removing original file");
            return;
        }

        if (rename("temp.txt", filename) != 0) {
            perror("Error renaming file");
            return;
        }

        printf("Password changed successfully.\n");
    } else {
        remove("temp.txt");
        printf("Password for '%s' was not found.\n", usage);
    }
}