#include"my402list.h"
#include"cs402.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

FILE* input = NULL; // global var storing input path

typedef struct transaction {
    char type;           // '+' for deposit, '-' for withdraw
    time_t time;         
    unsigned int amount;
    char* description;
} trans;


int inputTransaction (My402List* statement) {
    char line[1026];
    unsigned long int line_count = 0;

    while (fgets(line, sizeof(line), input) != NULL) {
        line_count ++;
        if (strcmp(line, "\n") != 0) {
            if (strlen(line) > 1024) {
                fprintf(stderr, "%s %lu %s", "line ", line_count, ": line should have length of no more than 1024.\n");
                exit(-1);
            }
            trans* trade = (trans*)malloc(sizeof(trans));
            // parse line
            int field_count = 0;
            char* start_ptr = line;
            char *tab_ptr = strchr(line, '\t');
            // stop parsing when end of line is hit
            while (tab_ptr != NULL) {
                *tab_ptr++ = '\0';

                if (field_count == 0) { // check '+' or '-'
                    char* current = strdup(start_ptr);
                    if (strlen(current) > 1 || (strcmp(current, "+") && strcmp(current, "-"))) {
                        fprintf(stderr,"%s %lu %s", "line ", line_count, ": Transaction type should only be + or - \n");
                        exit(-1);
                    } else {
                        if (strcmp(current, "+") == 0) {
                            trade->type = '+';
                        } else {
                            trade->type = '-';
                        }
                        free(current);
                    }
                } else if (field_count == 1) { // check timestamp
                    if (strlen(start_ptr) >= 11) {
                        fprintf(stderr, "%s %lu %s", "line ", line_count, ": Timestamp length should be less than 11");
                        exit(-1);
                    }
                    unsigned long timestamp = (unsigned long)strtoul(start_ptr, NULL, 0);
                    if (timestamp < 0 || timestamp > (unsigned long)time(NULL)) {
                        fprintf(stderr, "%s %lu %s", "line ", line_count, ": timestamp should be between 0 and current time \n");
                        exit(-1);
                    }
                    trade->time = (time_t)timestamp;
                } else if (field_count == 2) { // check money amount
                    char d_check;
                    int index = 0;
                    int decimal = 0;
                    int d_count = 0;
                    trade -> amount = 0;
                    while(start_ptr[index] != '\0') {
                        d_check = start_ptr[index];
                        if (d_check == '.' && decimal == 0) {
                            decimal ++;
                            if(trade->amount >= 10000000) {
                                fprintf(stderr, "%s %lu %s", "line ", line_count, ": amount too large \n");
                                exit(-1);
                            }
                        } else if (d_check == '.' && decimal == 1) {
                            fprintf(stderr, "%s %lu %s", "line ", line_count, ": Only 1 decimal allowed \n");
                            exit(-1);
                        } else {
                            if (!isdigit(d_check)) {
                                fprintf(stderr, "%s %lu %s", "line ", line_count, ": amount not numeric \n");
                                exit(-1);
                            }
                            if (decimal == 1) d_count ++;
                            if (d_count > 2) {
                                fprintf(stderr, "%s %lu %s", "line ", line_count, ": Too many number after decimal point \n");
                                exit(-1);
                            }
                            trade->amount = trade->amount * 10 + (unsigned int)atoi(&d_check);
                            if (decimal == 1 && trade->amount >= 1000000000) {
                                fprintf(stderr, "%s %lu %s", "line ", line_count, ": amount too large \n");
                                exit(-1);
                            }
                        }
                        index ++;            
                    }
                    if (decimal == 0) {
                        fprintf(stderr, "%s %lu %s", "line ", line_count, ": amount decimal missing \n");
                        exit(-1);
                    }
                    //free(current);
                } else { // exceeds field number print error
                    fprintf(stderr, "%s %lu %s", "line ", line_count, ": Transaction input can ONLY have 4 fields\n");
                    exit(-1);
                }
                // update pointer to next field and field count;
                field_count++;
                start_ptr = tab_ptr;
                tab_ptr = strchr(start_ptr, '\t');
            }
            // if field count is less than 4
            if (field_count < 3) {
                fprintf(stderr, "%s %lu %s", "line ", line_count, ": Transaction input can ONLY have 4 fields\n");  
                exit(-1);
            } else { // check description format
                /*
                Transcation description (cannot be empty after removing leading space)
                */
               while (*start_ptr == ' ') {
                   start_ptr ++;
               }
               if (strlen(start_ptr) == 0) {
                   fprintf(stderr, "%s %lu %s", "line ", line_count, ": Description Cannot be empty \n");
                   exit (-1);
               }
               trade->description = strdup(start_ptr);
            }
            My402ListAppend(statement, (void*) trade);
            //free(trade);
        } else {
            line_count --;
            break;
        }
    }
    if (line_count == 0) return FALSE;
    return TRUE;
}

// Bubble sort used in listtest
void BubbleForward(My402List *statement, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
    /* (*pp_elem1) must be closer to First() than (*pp_elem2) */
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(statement, elem1);
    My402ListElem *elem2next=My402ListNext(statement, elem2);

    My402ListUnlink(statement, elem1);
    My402ListUnlink(statement, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(statement, obj2);
        *pp_elem1 = My402ListFirst(statement);
    } else {
        (void)My402ListInsertAfter(statement, obj2, elem1prev);
        *pp_elem1 = My402ListNext(statement, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(statement, obj1);
        *pp_elem2 = My402ListLast(statement);
    } else {
        (void)My402ListInsertBefore(statement, obj1, elem2next);
        *pp_elem2 = My402ListPrev(statement, elem2next);
    }
}

void sortTransaction (My402List* statement, int trans_num) {
    My402ListElem * current = NULL;
    int i = 0;

    if (My402ListLength(statement) != trans_num) {
        fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", trans_num);
        exit(1);
    }
    for (i = 0; i < trans_num; i++) {
        int j = 0, something_swapped = FALSE;
        My402ListElem *next_elem=NULL;

        for (current = My402ListFirst(statement), j = 0; j < trans_num-i-1; current = next_elem, j++) {
            time_t cur_val = ((trans*)(current->obj))->time;

            next_elem = My402ListNext(statement, current);
            time_t next_val = ((trans*)(next_elem->obj))->time;

            if (cur_val > next_val) {
                BubbleForward(statement, &current, &next_elem);
                something_swapped = TRUE;
            } else if (cur_val == next_val) {
                fprintf(stderr, "Duplicate timestamp values exist \n");
                exit(-1);
            }
        }
        if (!something_swapped) break;
    }
}

void printTransaction (My402List* statement) {
	printf("+-----------------+--------------------------+----------------+----------------+\n");
	printf("|       Date      | Description              |         Amount |        Balance |\n");
	printf("+-----------------+--------------------------+----------------+----------------+\n");
  //printf("| WWW MMM DD YYYY | xxxxxxxxxxxxxxxxxxxxxxxx | (9,000,000.00) |  9,000,000.00  |\n");
    My402ListElem* current = NULL;
    long long int total = 0;
    for (current = My402ListFirst(statement); current != NULL; current = My402ListNext(statement, current)) {
        // Truncate time to WWW MMM DD YYYYY, and print
        time_t timestamp = ((trans*)current->obj)->time;
        char time_string[26];
        strncpy(time_string, ctime(&timestamp), sizeof(time_string));
        char truncate_time[16];
        int i;
        for (i = 0; i < 15; i ++) {
            int access = i;
            if (i > 10) access = i + 9;
            truncate_time[i] = time_string[access];
        }
        truncate_time[15] = '\0';
        printf("| %s | ", truncate_time);
        // truncate and print description
        char text[25];
        strncpy(text, ((trans*)current->obj)->description, 24);
        int index = 0;
        while (text[index] != '\0' && text[index] != '\n' && index < 25) {
            index ++;
        }
        // add space if text is not long enough
        for (i = index; i < 24; i ++) {
            text[i] = ' ';
        }
        text[24] = '\0';
        printf("%s | ", text);
        // Print amount, add () if amount is -
        char sign = ((trans*)current->obj)->type;
        long int amount = ((trans*)current->obj)->amount;
        char cash[15];
        // 9,999,999.99
        if (sign == '+') {
            cash[0] = ' '; cash[13] = ' '; 
        } else {
            cash[0] = '('; cash[13] = ')'; 
        }
        for (index = 12; index > 0; index --) {
            if (index == 10) {
                cash[index] = '.';
                continue;
            }
            if (index == 6||index == 2) {
                cash[index] = ',';
                continue;
            }
            cash[index] = (char)(((int)'0') + (amount % 10));
            amount /= 10;
            if (amount == 0) {
                index --;
                break;
            }
        }
        if (index == 10) {
            cash[index--] = '.';
            cash[index--] = '0';
        }
        while (index > 0) {
            cash[index --] = ' ';
        }
        cash[14] = '\0';
        printf("%s | ", cash);
        // print balence, print ?,???,???.?? if balence >= 10,000,000
        // print (?,???,???.??) if balence <= -10,000,000
        amount = ((trans*)current->obj)->amount;
        if (sign == '+') {
            total += amount;
        } else {
            total -= amount;
        }
        long long int bal = total;
        if (bal >= 1000000000) {
            printf(" \?,\?\?\?,\?\?\?.\?\? |\n");
        } else if (bal <= -1000000000) {
            printf("(\?,\?\?\?,\?\?\?.\?\?)|\n");
        } else {
            char balence[15];
            if (bal >= 0) {
                balence[0] = ' '; balence[13] = ' '; 
            } else {
                balence[0] = '('; balence[13] = ')'; 
                bal *= -1;
            }
            for (index = 12; index > 0; index --) {
                if (index == 10) {
                    balence[index] = '.';
                    continue;
                }
                if (index == 6||index == 2) {
                    balence[index] = ',';
                    continue;
                }
                balence[index] = (char)(((int)'0') + (bal % 10));
                bal /= 10;
                if (bal == 0) {
                    index --;
                    break;
                }
            }
            if (index == 10) {
                balence[index--] = '.';
                balence[index--] = '0';
            }
            while (index > 0) {
                balence[index --] = ' ';
            }
            balence[14] = '\0';
            printf("%s |\n", balence);
        }
    }
    printf("+-----------------+--------------------------+----------------+----------------+\n");
}

int main (int argc, char** argv) {
    struct stat file_status;
    // invalid command line
    if ((argc != 2 && argc != 3) || strcmp(argv[1], "sort") != 0) {
        perror("Incorrect command line syntax. Usage: warmup1 sort [tfile]");
        exit(-1);
    } else if (argc == 2) { // read from std in
        input = stdin;
    } else if (argc == 3) { // read from given tfile
        input = fopen(argv[2], "r");
        if (stat(argv[2], &file_status) == 0 && (file_status.st_mode & S_IFDIR)) {
            fprintf(stderr, "Input file path is a directory.\n");
            return -1;
        }
    }
    
    My402List statement;
    My402ListInit(&statement);
    if (input == NULL) {
        perror("File Cannot be Opened ");
        exit(-1);
    } else {
        if (inputTransaction(&statement) == FALSE) {
            fprintf(stderr, "File Cannot be read \n");
            exit(-1);
        }
        if (input != stdin) {
            fclose(input);
        }
        sortTransaction(&statement, *&statement.num_members);
        printTransaction(&statement);
        My402ListUnlinkAll(&statement);
    }
    return 0;
}
