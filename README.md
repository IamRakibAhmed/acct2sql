# acct2sql

## Overview

`acct2sql` is a utility that transforms incoming data with account balances into a set of SQL `INSERT` statements. The input data is provided in TLV (Tag-Length-Value) format and the output is a series of SQL statements to insert the data into a database table.

## Features

- Parses TLV formatted input data.
- Supports tags for account number, available balance, ledger balance, and update time.
- Generates SQL `INSERT` statements for the data.
- Optionally commits the inserts in batches.

## Requirements

- Language: C (C99 or higher)
- Platform: Linux/x86
- Build Tools: `gcc`, `make`
- Dependencies: `libc` only

## Input Format

The utility reads from the standard input stream. Each account record consists of multiple TLV fields, which can appear in any order. The supported tags are:

- `NA`: Account Number (`an..24`)
- `AB`: Available Balance (`x+n12`)
- `LB`: Ledger Balance (`x+n12`) - Optional
- `UT`: Update Time (`ans19`)

### Example Input

```
NA0810000001AB13C000000001000LB13C000000002000UT192022-05-01 09:00:00
NA09100000002AB13D000000005000LB13C000000000050UT192022-05-01 10:00:00
NA101000000003AB13C000000010000UT192022-05-01 11:00:00
```

## Output Format

The utility writes to the standard output stream. Each account record is transformed into an SQL `INSERT` statement for the `T_ACCOUNT` table. The table structure is as follows:

```
T_ACCOUNT (
    ACCOUNT_NO VARCHAR(24) NOT NULL,
    AVAIL_BAL NUMBER(12) NOT NULL,
    LEDGER_BAL NUMBER(12) NULL,
    UPD_DATE VARCHAR(8) NOT NULL,
    UPD_TIME VARCHAR(6) NOT NULL
)
```

### Example Output

```
INSERT INTO T_ACCOUNT (ACCOUNT_NO, AVAIL_BAL, LEDGER_BAL, UPD_DATE, UPD_TIME) VALUES ('10000001', 1000, 2000, '2022-05-01', '09:00:00');
INSERT INTO T_ACCOUNT (ACCOUNT_NO, AVAIL_BAL, LEDGER_BAL, UPD_DATE, UPD_TIME) VALUES ('100000002', -5000, 50, '2022-05-01', '10:00:00');
INSERT INTO T_ACCOUNT (ACCOUNT_NO, AVAIL_BAL, LEDGER_BAL, UPD_DATE, UPD_TIME) VALUES ('1000000003', 10000, NULL, '2022-05-01', '11:00:00');
COMMIT;
```

## Usage

### Command-line Usage

```sh
acct2sql [-c <commit limit>]
```

- `-c <commit limit>`: Optional argument to insert `COMMIT` statement after every `N` insert lines.

### Example Usage

```sh
cat input.txt | ./acct2sql -c 100 | sqlplus username/password@hostname:port/service_name
```

## Compilation

To compile the `acct2sql` utility, use the following commands:

```sh
gcc -o acct2sql acct2sql.c
```

## Example Script

Here is an example script to run the utility:

```sh
#!/bin/bash

# ORA_CONN - contains Oracle connection string

{
   echo -n 'NA0810000001'
   echo -n 'AB13C000000001000'
   echo -n 'LB13C000000002000'
   echo    'UT192022-05-01 09:00:00'

   echo -n 'NA09100000002'
   echo -n 'AB13D000000005000'
   echo -n 'LB13C000000000050'
   echo    'UT192022-05-01 10:00:00'

   echo -n 'NA101000000003'
   echo -n 'AB13C000000010000'
   echo    'UT192022-05-01 11:00:00'
} | ./acct2sql -c 100 | sqlplus $ORA_CONN
```

## Contact

For any issues or questions, please contact me.
