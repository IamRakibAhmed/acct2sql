#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORD_SIZE 1024

typedef struct {
    char account_no[25];
    long long avail_bal;
    long long ledger_bal;
    int ledger_bal_present;
    char upd_date[11]; // "YYYY-MM-DD\0"
    char upd_time[9];  // "HH:MM:SS\0"
} AccountRecord;

void parse_tlv(char *line, AccountRecord *record) {
    char *p = line;
    while (*p) {
        char tag[3];
        strncpy(tag, p, 2);
        tag[2] = '\0';
        p += 2;

        char length_str[3];
        strncpy(length_str, p, 2);
        length_str[2] = '\0';
        int length = atoi(length_str);
        p += 2;

        char value[length + 1];
        strncpy(value, p, length);
        value[length] = '\0';
        p += length;

        if (strcmp(tag, "NA") == 0) {
            strncpy(record->account_no, value, sizeof(record->account_no) - 1);
            record->account_no[sizeof(record->account_no) - 1] = '\0';
        } else if (strcmp(tag, "AB") == 0) {
            record->avail_bal = atoll(value + 1); // Skip the 'C' or 'D' sign character
            if (value[0] == 'D') {
                record->avail_bal = -record->avail_bal;
            }
        } else if (strcmp(tag, "LB") == 0) {
            record->ledger_bal = atoll(value + 1); // Skip the 'C' or 'D' sign character
            if (value[0] == 'D') {
                record->ledger_bal = -record->ledger_bal;
            }
            record->ledger_bal_present = 1;
        } else if (strcmp(tag, "UT") == 0) {
            strncpy(record->upd_date, value, 10);
            record->upd_date[10] = '\0';
            strncpy(record->upd_time, value + 11, 8);
            record->upd_time[8] = '\0';
        }
    }
}

void print_sql_insert(AccountRecord *record) {
    printf("INSERT INTO T_ACCOUNT (ACCOUNT_NO, AVAIL_BAL, LEDGER_BAL, UPD_DATE, UPD_TIME) VALUES ('%s', %lld, ",
           record->account_no, record->avail_bal);
    if (record->ledger_bal_present) {
        printf("%lld", record->ledger_bal);
    } else {
        printf("NULL");
    }
    printf(", '%s', '%s');\n", record->upd_date, record->upd_time);
}

int main(int argc, char *argv[]) {
    int commit_limit = 0;
    if (argc == 3 && strcmp(argv[1], "-c") == 0) {
        commit_limit = atoi(argv[2]);
    }

    char line[MAX_RECORD_SIZE];
    AccountRecord record;
    int count = 0;

    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n') continue;  // Skip empty lines
        memset(&record, 0, sizeof(record));
        parse_tlv(line, &record);
        print_sql_insert(&record);

        if (commit_limit > 0 && ++count % commit_limit == 0) {
            printf("COMMIT;\n");
        }
    }

    if (commit_limit > 0 && count % commit_limit != 0) {
        printf("COMMIT;\n");
    }

    return 0;
}
