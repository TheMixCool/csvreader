# Program information

 To compile write:

    g++ csvreader.cpp -o csvreader

 To run write:

    ./csvreader filename.csv


# Tests information

file.csv - file without erros

file1.csv - division by zero (preread error)

file2.csv - division by zero (value error)

file3.csv - was not found '=' symbol

file4.csv - was found '=' symbol without operator

file5.csv - self-linking cell

file6.csv - wrong table format ( additional ',' symbol)

file7.csv - wrong column name

file8.csv - wrong row name

file9.csv - wrong table format (existing symbol in position [0][0] )

file10.csv - empty row name

file11.csv - empty column name

file12.csv - big file with different errors

file13.csv - sum of negative values
