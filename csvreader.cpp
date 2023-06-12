#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>

std::string computeResult(std::string &arg1_result, std::string &arg2_result, char op);
std::string computeFunction(std::string &value, std::string **table, std::map<std::string, int> &rowName, std::map<std::string, int> &colName, int i, int j);
int getColCount(std::vector<std::string> &values, char delimiter);
bool isOperator(char op);
bool checkPathArguments(int argc, char* argv[], std::string &path, std::ifstream &fin);
void printTable(std::string ** table, int rowCount, int colCount, char delimiter);
void computeTable(std::string ** table, int rowCount, int colCount, std::map<std::string, int> &colNames, std::map<std::string, int> &rowNames);
bool checkTableFormat(std::vector<std::string> &values, int colCount, char delimiter);
void fillArray(std::vector<std::string> &values, char delimiter, std::string ** table);
bool checkZeroDivision(std::vector<std::string> &values);
bool checkZeroZeroElement(std::vector<std::string> &values, char delimiter);
bool createRowMap(std::vector<std::string> &values, char delimiter, std::map<std::string, int> &rowNames);
bool createColMap(std::vector<std::string> &values, char delimiter, std::map<std::string, int> &colNames);

void writeToValues(std::ifstream &fin, std::vector<std::string> &values);

int main(int argc, char* argv[]){
    
    std::string path = "";
    std::string ** table;
    std::map<std::string, int> colNames; 
    std::map<std::string, int> rowNames;
    std::vector<std::string> values;
    std::ifstream fin;
    char delimiter = ',';
    int colCount = 0;
    int rowCount = 0;

    if(!checkPathArguments(argc, argv, path, fin))                     //Checking the opening of a file and its arguments
        return 0;   
                          
    writeToValues(fin, values);                                         //Writing from file to values 
    
    rowCount = values.size();                                           // Getting row count for checking table format
    colCount = getColCount(values, delimiter);                          // Getting column count for checking table format

    if(!checkTableFormat(values, colCount,delimiter))                   // Checking table format
        return 0;
    
    if(!checkZeroZeroElement(values, delimiter))                        // Checking element [0][0]
        return 0;

    if(!createRowMap(values, delimiter, rowNames))                      // Checking and creating row numerations
        return 0; 

    if(!createColMap(values, delimiter, colNames))                      // Checking and creating columns names
        return 0;

    if(!checkZeroDivision(values))                                      // Checking division by zero
        return 0;                                          


    table = new std::string *[rowCount];                                // Creating array of values  
    for(int i = 0; i < rowCount; i++){
        table[i] = new std::string[colCount];
    }

    fillArray(values, delimiter, table);                                // Fill array of values
    computeTable(table, rowCount, colCount, colNames, rowNames);        // Search for cells for compute. If found operator without '=' symbol, write: "_WRONG_FORMAT_" 
    printTable(table, rowCount, colCount, delimiter);                   //Final array output
    fin.close();

    for(int i = 0; i < rowCount;i++)
        delete [] table[i];
    delete [] table;

    return 0;
}

void writeToValues(std::ifstream &fin, std::vector<std::string> &values){
    std::string temp = "";
    while(!fin.eof()){
        fin >> temp;
        values.push_back(temp);
        if(fin.eof()){
            break;
        }
    }
    values.shrink_to_fit();
}

bool checkPathArguments(int argc, char* argv[], std::string &path, std::ifstream &fin){
    
    if(argc != 2){                              
        if(argc > 2){
            std::cerr << "Too many arguments\r\n";                          // Too many arguments
        } 
        else{                              
            std::cerr << "Path to file is empty\r\n";                       // No path argument                                        
        }
        return false;                                                                    
    }
    
    path = argv[1];                                                         // Path argument was found "./csvreader file.csv" 
    fin.open(path);
    if(!fin.is_open()){
        std::cerr << "Wrong path to file\r\n";
        return false; 
    }
    else{
        std::cout << "File was opened\r\n";
        return true;
    }
}

bool createColMap(std::vector<std::string> &values, char delimiter, std::map<std::string, int> &colNames){
    std::string tempColName = "";
    int colNamesPosition = 1;

    for(int i = 1; i < values[0].size(); i++){
        while (values[0][i] != delimiter && values[0][i] != '\0'){
            if(!isalpha(values[0][i])){
                std::cerr << "File preread error\r\n";
                std::cerr << "Wrong name symbol in postion: " << i+1 << "\r\n";
                return false;  
            } 
            tempColName +=values[0][i];
            i++;
        }      
        if(colNames.count(tempColName) == 1){
            std::cerr << "File preread error\r\n";
            std::cerr << "Column Name is already used.\r\n";
            std::cerr << "Column: " << colNamesPosition << "\r\n";
            return false;  
        }
        if(tempColName == "" || values[0][values[0].size()-1] == delimiter){
                std::cerr << "File preread error\r\n";
                std::cerr << "Column name is empty: " << i+1 << "\r\n";
                return false; 
        }
        colNames.insert(std::pair<std::string, int>(tempColName, colNamesPosition));
        colNamesPosition++;
        tempColName = ""; 
    }
    return true;
}

bool createRowMap(std::vector<std::string> &values, char delimiter, std::map<std::string, int> &rowNames){
    std::string tempRowName = "";

    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(!isdigit(values[i][0])){
                    std::cerr << "File preread error\r\n";
                    std::cerr << "Row name is empty: " << i+1 << "\r\n";
                    return false;   
            }
            while (values[i][j] != delimiter)
            {
                if(!isdigit(values[i][j])){
                    std::cerr << "File preread error\r\n";
                    std::cerr << "Wrong name symbol in row: " << i+1 << "\r\n";
                    return false; 
                } 
                tempRowName +=values[i][j];
                j++;
            }
            if(rowNames.count(tempRowName) == 1){
                std::cerr << "File preread error\r\n";
                std::cerr<< "Row number is already used: " << i+1 << "\r\n";
                return false;  
            }
            rowNames.insert(std::pair<std::string, int>(tempRowName, i));
            tempRowName = "";
            break; 
        }
    }
    return true;
}

bool checkZeroZeroElement(std::vector<std::string> &values, char delimiter){
    if(values[0][0] != delimiter){
        std::cerr << "File preread error\r\n";
        std::cerr << "Wrong table format\r\n";
        return false;
    }
    return true;
}

bool checkZeroDivision(std::vector<std::string> &values){
    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == '/' && values[i][j+1] == '0'){
                std::cerr << "File preread error\r\n";
                std::cerr << "Division by zero\r\nRow: " << i+1 << "\tposition: " << j+1 <<"\r\n";
                return false; 
            }
        }
    }
    return true;
}

void fillArray(std::vector<std::string> &values, char delimiter, std::string ** table){
    std::string valueToWrite = "";
    int tableRowPosition = 0;
    int tableColPosition = 0;
    for(int i = 0; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(i == 0 && j == 0){
                table[tableColPosition][tableRowPosition] = valueToWrite;
                tableRowPosition++;
                continue;
            }
            while(values[i][j] != delimiter && values[i][j] != '\0'){
                valueToWrite += values[i][j];
                j++;
            }
            table[tableColPosition][tableRowPosition] = valueToWrite;
            valueToWrite = "";
            tableRowPosition++;
        }
        tableColPosition++;
        tableRowPosition = 0;
    }
}

void computeTable(std::string ** table, int rowCount, int colCount, std::map<std::string, int> &colNames, std::map<std::string, int> &rowNames){
    for(int i = 1; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            if(table[i][j][0] == '=' ){
                table[i][j] = computeFunction(table[i][j], table, rowNames, colNames, i , j);
                continue;
            }
            for(int m = 0; m < table[i][j].length();m++){
                if((isOperator(table[i][j][m]) && !isdigit(table[i][j][m+1])) || isalpha(table[i][j][m])){
                    table[i][j] = "_WRONG_FORMAT_";
                    break;
                }
            }
        }
    }
}

void printTable(std::string ** table, int rowCount, int colCount, char delimiter){
    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            std::cout << table[i][j];
            if(j != colCount-1){
                std::cout << delimiter;
             }
        }
        std::cout << "\r\n";
    }
}

bool checkTableFormat(std::vector<std::string> &values, int colCount, char delimiter){
    int tempCol = 1;
    for(int i = 0; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == delimiter){
                tempCol++;
            }
        }
        if(tempCol != colCount){
            std::cerr << "File preread error\r\n";
            std::cerr << "Wrong table format\r\n";
            return false;
        }
        tempCol = 1;
    }
    return true;
}

int getColCount(std::vector<std::string> &values, char delimiter){
    int colCount = 0;
    for(int i = 0; i < values[0].size();i++){
        if(values[0][i] == delimiter){
            colCount++; 
        }
    }
    return colCount + 1;
}

bool isOperator(char op){
    if( op == '+' || op == '-' || 
        op == '*' || op == '/' ){
            return true;
    }
    else{
        return false;
    }
}

std::string computeResult(std::string &arg1_result, std::string &arg2_result, char op){
    int arg1_result_int = 0;
    bool arg1_isPositive = arg1_result[0] != '-';
    int arg2_result_int = 0;
    bool arg2_isPositive = arg2_result[0] != '-';
    
    if(!arg1_isPositive){
        arg1_result.erase(0,1);
    }
    if(!arg2_isPositive){
        arg2_result.erase(0,1);
    }
    
    for(int i = arg1_result.length() - 1, temp = 1; i >= 0; i--, temp *= 10){
        arg1_result_int += int(arg1_result[i] - 48) * temp;
    }

    if(!arg1_isPositive){
        arg1_result_int *= -1;
    }

    for(int i = arg2_result.length() - 1, temp = 1; i >= 0; i--, temp *= 10){
        arg2_result_int += int(arg2_result[i] - 48) * temp;
    }

    if(!arg2_isPositive){
        arg2_result_int *= -1;
    }

    switch (op){
    case '+':
        return std::to_string(arg1_result_int + arg2_result_int);
    case '-':
        return std::to_string(arg1_result_int - arg2_result_int);
    case '*':
        return std::to_string(arg1_result_int * arg2_result_int);
    case '/':
        return std::to_string(arg1_result_int / arg2_result_int);
    }
}

std::string computeFunction(std::string &value, std::string **table, std::map<std::string, int> &rowName, std::map<std::string, int> &colName, int i, int j){
    std::string arg1_row = "";
    std::string arg1_col = "";
    std::string arg2_row = "";
    std::string arg2_col = "";
    std::string arg1_result = "";
    std::string arg2_result = "";
    char op = ' ';

    for(int i = 0; i < value.length();i++){
        if(value[i] == '='){
            i++;
        }
        while(isalpha(value[i])){
            arg1_row += value[i];
            i++;
        }
        while(isdigit(value[i])){
            arg1_col += value[i];
            i++;
        }
        op = value[i++];
        if(!isOperator(op)){
            return "_WRONG_OP_";
        }
        while(isalpha(value[i])){
            arg2_row += value[i];
            i++;
        }
        while(isdigit(value[i])){
            arg2_col += value[i];
            i++;
        }
        if(i == value.length()){
            break;
        }
        else{
            return "_WRONG_FORMAT_";
        }
    }

    //If found non-existent cell
    if(rowName.count(arg1_col) == 0 || colName.count(arg1_row) == 0 || rowName.count(arg2_col) == 0 || colName.count(arg2_row) == 0){
        return "_CELLS_NOT_EXISTS_";
    }

    //If self linking
    if((i == rowName.find(arg1_col)->second && j == colName.find(arg1_row)->second) || (i == rowName.find(arg2_col)->second && j == colName.find(arg2_row)->second)){
        return "_SELF_LINKING_";
    }

    //Get value of arguments by rowName and colName
    arg1_result = table[rowName.find(arg1_col)->second][colName.find(arg1_row)->second];
    arg2_result = table[rowName.find(arg2_col)->second][colName.find(arg2_row)->second];
    if(arg2_result == "0" && op == '/'){
        return "_DIVISION_BY_ZERO_";
    }

    return computeResult(arg1_result, arg2_result, op);
}