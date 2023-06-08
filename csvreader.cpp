#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

string computeFunction(string value, string **table, map<string, int> rowName, map<string, int> colName);

int main(int argc, char* argv[]){
    
    string path = "";

    //------------------------------------------------
    // No path argument
    //------------------------------------------------

    if(argc == 1){                              
        cout << "Path to file is empty\n";       
        exit(0);                                
    }

    //------------------------------------------------
    // Path argument was found 
    // "./a file.csv"
    //------------------------------------------------
    
    else if(argc == 2){                              
        path = argv[1];                        
    }
    else{
        cout << "Too much arguments\n";
        exit(0);
    } 

    ifstream fin;
    fin.open(path);
    if(!fin.is_open()){
        cout << "Wrong path to file\n";
        exit(0);
    }
    else{
        cout << "File was opened\n\n";
    }

    char delimiter = ',';
    int colCount = 1;
    int rowCount = 0;

    vector<string> values;
    values.reserve(100000);
    string temp;

    while(!fin.eof()){
        fin >> temp;
        values.push_back(temp);
        if(fin.eof()){
            break;
        }
    }
    values.shrink_to_fit();

    //------------------------------------------------
    // Getting column count for checking table format 
    //------------------------------------------------

    rowCount = values.size();
    for(int i = 0; i < values[0].size();i++){
        if(values[0][i] == delimiter){
            colCount++; 
        }
    }

    //------------------------------------------------
    // Checking table format
    //------------------------------------------------

    int tempCol = 1;
    for(int i = 0; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == delimiter){
                tempCol++;
            }
        }
        if(tempCol != colCount){
            cout << "Wrong table format\n";
            exit(0);
        }
        tempCol = 1;
    }

    //------------------------------------------------
    // Checking element [0][0] 
    //------------------------------------------------

    if(values[0][0] != delimiter){
        cout<< "Wrong table format\n";
        exit(0);
    }

    //------------------------------------------------
    // Checking row numerations
    //------------------------------------------------
    map<string, int> rowNames;
    string tempRowName = "";

    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            while (values[i][j] != delimiter)
            {
                if(!isdigit(values[i][j])){
                    cout << "Wrong name symbol in row: " << i+1 << '\n';
                    exit(0); 
                } 
                tempRowName +=values[i][j];
                j++;
            }
            if(rowNames.count(tempRowName) == 1){
                cout << "Row number is already used: " << i+1 << '\n';
                exit(0);
            }
            rowNames.insert(pair<string, int>(tempRowName, i));
            tempRowName = "";
            break; 
        }
    }

    //------------------------------------------------
    // Checking columns names
    //------------------------------------------------

    map<string, int> colNames;
    string tempColName = "";
    int colNamesPosition = 1;

    for(int i = 1; i < values[0].size(); i++){
        while (values[0][i] != delimiter && values[0][i] != '\0'){
            if(!isalpha(values[0][i])){
                cout << "Wrong name symbol in postion: " << i+1 << '\n';
                exit(0); 
            } 
            tempColName +=values[0][i];
            i++;
        }      
        if(colNames.count(tempColName) == 1){
            cout << "Column Name is already used.\n";
            cout << "Column: " << colNamesPosition << '\n';
            exit(0);
        }
        colNames.insert(pair<string, int>(tempColName, colNamesPosition));
        colNamesPosition++;
        tempColName = ""; 
    }
    
    //------------------------------------------------
    // Checking division by zero 
    // (first check before creating array with values)
    // (second check will be with table values)
    //------------------------------------------------

    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            if(values[i][j] == '/' && values[i][j+1] == '0'){
                cout << "Division by zero\nRow: " << i+1 << "\tposition: " << j+1 <<'\n';
                exit(0);
            }
        }
    }

    //------------------------------------------------
    // Creating array of values
    //------------------------------------------------
    
    string ** table = new string *[rowCount];
    for(int i = 0; i < rowCount; i++){
        table[i] = new string[colCount];
    }

    //------------------------------------------------
    // Fill array of values
    //------------------------------------------------

    string valueToWrite = "";
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
    
    //------------------------------------------------
    // Search for cells for compute
    //------------------------------------------------

    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            if(table[i][j][0] == '='){
                table[i][j] = computeFunction(table[i][j], table, rowNames, colNames);
            }
        }
    }

    //------------------------------------------------
    //Final array output
    //------------------------------------------------

    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            cout << table[i][j];
            if(j != colCount-1){
                cout << delimiter;
             }
        }
        cout << '\n';
    }

    fin.close();
    return 0;
}

string computeFunction(string value, string **table, map<string, int> rowName, map<string, int> colName){
    string arg1_row = "";
    string arg1_col = "";
    string arg2_row = "";
    string arg2_col = "";
    char op = ' ';
    string arg1_result = "";
    string arg2_result = "";

    //------------------------------------------------
    //Start from 1 because value[0] == '='
    //------------------------------------------------

    for(int i = 1; i < value.length();i++){
        while(isalpha(value[i])){
            arg1_row += value[i];
            i++;
        }
        while(isdigit(value[i])){
            arg1_col += value[i];
            i++;
        }
        op = value[i++];
        if(op != '+' && op != '-' && op !='*' && op != '/'){
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

    arg1_result = table[rowName.find(arg1_col)->second][colName.find(arg1_row)->second];
    arg2_result = table[rowName.find(arg2_col)->second][colName.find(arg2_row)->second];

    if(arg2_result == "0" && op == '/'){
        return "_DIVISION_BY_ZERO_";
    }

    int arg1_result_int = 0;
    int arg2_result_int = 0;

    for(int i = arg1_result.length() - 1, temp = 1; i >= 0; i--, temp *= 10){
        arg1_result_int += int(arg1_result[i] - 48) * temp;
    }

    for(int i = arg2_result.length() - 1, temp = 1; i >= 0; i--, temp *= 10){
        arg2_result_int += int(arg2_result[i] - 48) * temp;
    }

    switch (op)
    {
    case '+':
        return to_string(arg1_result_int + arg2_result_int);
    
    case '-':
        return to_string(arg1_result_int - arg2_result_int);
    
    case '*':
        return to_string(arg1_result_int * arg2_result_int);

    case '/':
        return to_string(double(arg1_result_int / arg2_result_int));

    }
}