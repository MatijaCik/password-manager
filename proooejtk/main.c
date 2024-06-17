#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<stdlib.h>
#include"header.h"
#include <ctype.h>

int main(void) {

    static enum Choice1 choice1;
    int num_users = 0;
    USER* users = NULL;

    char input1[100];

    if (_access(file_users, 0) == 0) {

        printf("File %s already exists.\n", file_users);
        users = load_users(file_users, &num_users);
    }
    else {

        create_users_file(file_users);
        printf("File %s created successfully.\n", file_users);
        users = malloc(sizeof(USER) * MAX_USERS); // Initialize users array
        if (users == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    }
    system("pause");


    while (1) {
        system("cls");
        printf("Unesite broj odabira izbornika:\n");
        printf("1. New user\n");
        printf("2. Login existing user\n");
        printf("3. Delete existing user\n");
        printf("4. Show all users\n");
        printf("5. Exit manager\n");

        if (fgets(input1, sizeof(input1), stdin)) {
            char* endptr;
            choice1 = strtol(input1, &endptr, 10);


            if (endptr != input1) {
                while (isspace((unsigned char)*endptr)) {
                    endptr++;
                }
                if (*endptr == '\0' || *endptr == '\n') {

                    while (*endptr != '\n' && *endptr != '\0') {
                        endptr++;
                    }
                    switch (choice1) {

                    case NEW_USER:  new_user(&num_users, users, file_users);

                        break;

                    case LOGIN_USER:  login_user(&num_users, users); break;

                    case DELETE_USER:  delete_user(&num_users, users); break;

                    case DISPLAY_USER: display_users(&num_users, users);  break;

                    case EXIT_MANAGER: exit_manager(&num_users, users); break;

                    default:
                        printf("Invalid choice. Please enter a number between 1 and 5.\n");
                        break;
                    }
                    system("pause");
                }
                else {
                   
                    printf("Invalid input. Please enter a valid integer.\n");
                    system("pause");
                }
            }
            //else {
            //    // Invalid input
            //    printf("Invalid input. Please enter a valid integer.\n");
            //   system("pause");
            //}
        }
        else {

            printf("Error reading input. Please try again.\n");
            system("pause");
        }
    }
    return 0;
}
