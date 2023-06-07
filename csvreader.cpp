#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <exception>

using namespace std;

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
        cout << "File was opened\n";
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

    for(int i = 1; i < values.size(); i++){
        for(int j = 0; j < values[i].size(); j++){
            while (values[i][j] != delimiter)
            {
                if(!isdigit(values[i][j])){
                    cout << "Wrong number in row: " << i+1 << '\n';
                    cout << "Wrong symbol is: " << values[i][j] << '\n';
                    exit(0); 
                }  
                j++;
            }
            break; 
        }
    }

    //------------------------------------------------
    // Checking columns names
    //------------------------------------------------

    for(int i = 1; i < values[0].size(); i++){
        if(values[0][i] != delimiter && !isdigit(values[0][i]) && !isalpha(values[0][i])){
            cout << "Wrond symbol in position: " << i << '\n';
            exit(0);
        }
        if(values[0][i] == delimiter && isdigit(values[0][i+1])){
            cout << "Wrong name in position: " << i+2 <<'\n';
            exit(0);
        }
        if(isdigit(values[0][i]) && isalpha(values[0][i+1])){
            cout << "Wrong name in position: " << i+2 <<'\n';
            exit(0);
        }
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
    // Test fillArray action
    //------------------------------------------------

    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            table[i][j] = "qwe";
        }
    }

    for(int i = 0; i < rowCount; i++){
        for(int j = 0; j < colCount; j++){
            cout << table[i][j] << '\t';
        }
        cout << '\n';
    }



}