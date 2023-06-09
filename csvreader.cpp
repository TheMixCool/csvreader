#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

std::string computeResult(std::string arg1_result, std::string arg2_result, char op);
std::string computeFunction(std::string value, std::string **table, std::map<std::string, int> rowName, std::map<std::string, int> colName, int i, int j);
bool isOperator(char op);
void printTable(std::string ** table, int rowCount, int colCount, char delimiter);

int main(int argc, char* argv[]){
    
    std::string path = "";

    // No path argument
    if(argc == 1){                              
        std::cout << "Path to file is empty\n";       
        exit(0);                                
    }

    // Path argument was found ---- "./csvreader file.csv"
    else if(argc == 2){                              
        path = argv[1];                        
    }
    else{
        std::cout << "Too many arguments\n";
        exit(0);
    } 

    std::ifstream fin;
    fin.open(path);
    if(!fin.is_open()){
        std::cout << "Wrong path to file\n";
        exit(0);
    }
    else{
        std::cout << "File was opened\n\n";
    }

    char delimiter = ',';
    int colCount = 1;
    int rowCount = 0;

    std::vector<std::string> values;
    values.reserve(100000);
    std::string temp;

    while(!fin.eof()){
        fin >> temp;
        values.push_back(temp);
        if(fin.eof()){
            break;
        }
    }
    values.shrink_to_fit();

    // Getting column count for checking table format 
    rowCount = values.size();
    for(int i = 0; i < values[0].size();i++){
        if(values[0][i] == delimiter){
            colCount++; 
        }
    }

    // Checking table format
    int tempCol = 1;
    for(int i = 0; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == delimiter){
                tempCol++;
            }
        }
        if(tempCol != colCount){
            std::cout << "File preread error\n";
            std::cout << "Wrong table format\n";
            exit(0);
        }
        tempCol = 1;
    }

    // Checking element [0][0] 
    if(values[0][0] != delimiter){
        std::cout << "File preread error\n";
        std::cout<< "Wrong table format\n";
        exit(0);
    }

    // Checking row numerations
    std::map<std::string, int> rowNames;
    std::string tempRowName = "";

    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(!isdigit(values[i][0])){
                    std::cout << "File preread error\n";
                    std::cout << "Row name is empty: " << i+1 << '\n';
                    exit(0); 
            }
            while (values[i][j] != delimiter)
            {
                if(!isdigit(values[i][j])){
                    std::cout << "File preread error\n";
                    std::cout << "Wrong name symbol in row: " << i+1 << '\n';
                    exit(0); 
                } 
                tempRowName +=values[i][j];
                j++;
            }
            if(rowNames.count(tempRowName) == 1){
                std::cout << "File preread error\n";
                std::cout << "Row number is already used: " << i+1 << '\n';
                exit(0);
            }
            rowNames.insert(std::pair<std::string, int>(tempRowName, i));
            tempRowName = "";
            break; 
        }
    }

    // Checking columns names
    std::map<std::string, int> colNames;
    std::string tempColName = "";
    int colNamesPosition = 1;

    for(int i = 1; i < values[0].size(); i++){
        while (values[0][i] != delimiter && values[0][i] != '\0'){
            if(!isalpha(values[0][i])){
                std::cout << "File preread error\n";
                std::cout << "Wrong name symbol in postion: " << i+1 << '\n';
                exit(0); 
            } 
            tempColName +=values[0][i];
            i++;
        }      
        if(colNames.count(tempColName) == 1){
            std::cout << "File preread error\n";
            std::cout << "Column Name is already used.\n";
            std::cout << "Column: " << colNamesPosition << '\n';
            exit(0);
        }
        if(tempColName == "" || values[0][values[0].size()-1] == delimiter){
                std::cout << "File preread error\n";
                std::cout << "Column name is empty: " << i+1 << '\n';
                exit(0); 
        }
        colNames.insert(std::pair<std::string, int>(tempColName, colNamesPosition));
        colNamesPosition++;
        tempColName = ""; 
    }
    
    // Checking division by zero 
    // (first check before creating array with values)
    // (second check will be with table values)
    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == '/' && values[i][j+1] == '0'){
                std::cout << "File preread error\n";
                std::cout << "Division by zero\nRow: " << i+1 << "\tposition: " << j+1 <<'\n';
                exit(0);
            }
        }
    }

    // Creating array of values  
    std::string ** table = new std::string *[rowCount];
    for(int i = 0; i < rowCount; i++){
        table[i] = new std::string[colCount];
    }

    // Fill array of values
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
    
    // Search for cells for compute
    // If found operator without '=' symbol, write: "_WRONG_FORMAT_"
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

    //Final array output
    printTable(table, rowCount, colCount, delimiter);
    
    fin.close();

    for(int i = 0; i < rowCount;i++)
        delete [] table[i];
    delete [] table;

    return 0;
}

void printTable(std::string ** table, int rowCount, int colCount, char delimiter){
    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            std::cout << table[i][j];
            if(j != colCount-1){
                std::cout << delimiter;
             }
        }
        std::cout << '\n';
    }
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

std::string computeResult(std::string arg1_result, std::string arg2_result, char op){
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

std::string computeFunction(std::string value, std::string **table, std::map<std::string, int> rowName, std::map<std::string, int> colName, int i, int j){
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

