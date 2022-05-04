/*
    Author: Emily McCue
    Title: Programming Assignment 3
    Due Date: April 19, 2022
            ---
    Class: CS 457, Section 1001
    Professor: Prof. Zhao
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool tokenize(char userInput[200]);
void nowUsing(string useDBName);
void createTB(char* useLoopTokens, string dbName);
void dropTB(char* useLoopTokens, string useDBName);
bool notUsing(char* tokens);
void create(char* tokens);
bool databaseExists(const string &s);
bool tableExists(string totalPath);
void drop(char* tokens);
void alter(char* useLoopTokens, string dbName);
void select(char* useLoopTokens, string dbName);
bool printFileOuterJoin(string totalPath2, string line1, string line2, string tempWord, int dataPosition);
void printFile(string totalPath2, string line1, string line2, string tempWord, int dataPosition);
bool printSelect(string dataArray[200], int dataCounter, string dbName, string totalPath, string dataName, string operand, string data);
void update(char* useLoopTokens, string useDBName);
void insert(char* useLoopTokens, string dbName);
void deleteData(char* useLoopTokens, string dbName);
bool modifyTable(string dbName, string totalPath, string dataName, string operand, string data);

int main() {

    bool exit = false;
    char userInput[200];
    
    cout << "-- CS457 PA2" << endl << endl;

    while(exit == false) {
        cin.getline(userInput, 200);
        exit = tokenize(userInput);
    }

    return 0;
}

/**
    Tokenize the first word in userInput.

    This token decides whether to operate within a database or whether
    to continue operating outside of a database. Depending on the 
    token, it calls different functions nowUsing() or notUsing().

    @param userInput which is the user inputted command
    @return boolean value T/F which decides if we continue or exit
*/
bool tokenize(char userInput[200]) {
    
    char* tokens = strtok(userInput, " ");
    string token1 = tokens;

    if (token1 == "USE") {

        // get next token in userInput[]
        tokens = strtok(NULL, " ");
        char* temp = tokens;
        string useDBName = temp;

        // remove ';' at the end of database name
        size_t pos;
        while ((pos = useDBName.find(";")) != string::npos) {
            useDBName.replace(pos, 1, "");
        }

        bool exists = databaseExists(useDBName);
        if(exists) {
            cout << "-- Using database " << useDBName << "." << endl;

            // enter nowUsing loop (able to access greater range of commands)
            nowUsing(useDBName);
            return true;
        } else {
            cout << "-- !Failed to use database " << useDBName << " because it does not exist." << endl;
        }
        return false;

    } else {
        // continue in notUsing loop (limited range of commands)
        bool notUsingLoop = notUsing(tokens);
        return notUsingLoop;
    }

}

/**
    Using a database.

    User has entered USE "database name" and now operating within the use
    database loop which allows user to create and edit tables within
    entered database.

    @param useDBName which is the name of the database currently in use
*/
void nowUsing(string useDBName) {

    bool exitUseLoop = false;
    char userInputUseLoop[200];

    while (exitUseLoop == false) {

        cin.getline(userInputUseLoop, 200);
        char* useLoopTokens = strtok(userInputUseLoop, " ");
        char* charToken1 = useLoopTokens;
        string token1 = charToken1;

        // remove ';' from token1;
        size_t pos;
        while ((pos = token1.find(";")) != string::npos) {
            token1.replace(pos, 1, "");
        }

        // below is the range of commands allowed while in the nowUsing loop
        if (token1 == "USE") {

            useLoopTokens = strtok(NULL, " ");
            string useDBName = useLoopTokens;

            // remove ';' from database name
            while ((pos = useDBName.find(";")) != string::npos) {
                useDBName.replace(pos, 1, "");
            }

            bool exists = databaseExists(useDBName);

            if(exists) {
                cout << "-- Using database " << useDBName << endl;
                // continue using the nowUsing loop
                nowUsing(useDBName);
            } else {
                cout << "-- !Failed to use database " << useDBName << " because it does not exist." << endl;
            }

        } else if (token1 == "CREATE" || token1 == "create") {
            createTB(useLoopTokens, useDBName);
        } else if (token1 == "DROP") {
            dropTB(useLoopTokens, useDBName);
        } else if (token1 == "ALTER") {
            alter(useLoopTokens, useDBName);
        } else if (token1 == "SELECT" || token1 == "select") {
            select(useLoopTokens, useDBName);
        } else if (token1 == "insert") {
            insert(useLoopTokens, useDBName);
        } else if (token1 == "update") {
            update(useLoopTokens, useDBName);
        } else if (token1 == "delete") {
            deleteData(useLoopTokens, useDBName);
        } else if (token1 == ".EXIT" || token1 == ".exit") {
            cout << "-- All done." << endl;
            exitUseLoop = true;
        } else {
            cout << "Invalid Input." << endl;
        }

    }

}

/**
    Create tables within the database in use.

    Using the command tokens and the name of the database in use, creates a
    table with inputted table name if the table name is not already in use.

    @param useLoopTokens which is the user inputted command as tokens
    @param dbName which is the database name currently in use
*/
void createTB(char* useLoopTokens, string dbName) {

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    if (tbCheck == "TABLE" || tbCheck == "table") {
        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;
        
        string restOfTokens;
        char* tempVar;

        // remove ';' from table name
        size_t pos;
        while ((pos = tbName.find(";")) != string::npos) {
            tbName.replace(pos, 1, "");
        }

        // create the total path to the table in the database in use
        string totalPath = dbName + "/" + tbName + ".txt";

        // while there are still tokens in useLoopTokens,
        // get the rest of tokens and store them as a string to variable restOfTokens
        while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
            tempVar = useLoopTokens;
            //cout << "temp var: " << tempVar << endl;
            restOfTokens = restOfTokens + " " + tempVar;
        }

        if (!restOfTokens.empty()) {
            // format the restOfTokens string
            string original = ",";
            string replacement = " |";
            while ((pos = restOfTokens.find(original)) != string::npos) {
                restOfTokens.replace(pos, 1, replacement);
            }
            // remove ';' from the restOfTokens string
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }
            // remove the first two characters in rest of tokens (opening parentheses and space)
            restOfTokens.erase(0,2);
            // remove the last character in restOfTokens (closing parentheses)
            restOfTokens.erase(prev(restOfTokens.end()));

        }
        
        bool exists = tableExists(totalPath);
 
        // if table does not already exist
        if (!exists) {
            // create table with totalPath as the path, pass restOfTokens into it, and close the table
            ofstream newTB(totalPath.c_str());
            newTB << restOfTokens;
            newTB.close();
            cout << "-- Table " << tbName << " created." << endl;
        } else {
            cout << "-- !Failed to create table " << tbName << " because it already exists." << endl;
        }
    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Drop tables within the database in use.

    Using the command tokens and the name of the database in use, drops the
    table with inputted table name if the table exists.

    @param useLoopTokens which is the user inputted command as tokens
    @param dbName which is the database name currently in use
*/
void dropTB(char* useLoopTokens, string useDBName) {

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    if (tbCheck == "TABLE") {
        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        // remove ';' from table name
        size_t pos;
        while ((pos = tbName.find(";")) != string::npos) {
            tbName.replace(pos, 1, "");
        }

        // create total path to table within database in use
        string totalPath = useDBName + "/" + tbName + ".txt";
        
        bool exists = tableExists(totalPath);

        // if a table exists with that path, delete the table
        if (exists) {
            if (remove(totalPath.c_str()) == 0) {
                cout << "-- Table " << tbName << " deleted." << endl;
            } else {
                cout << "-- Error deleting file, try again." << endl;
            }
        } else {
            cout << "-- !Failed to delete table " << tbName << " because it does not exist." << endl;

        }
    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Limits functions available until using a database.

    This function is used before the user inputs the "USE" command and
    is then working within a database. This limits the capabilities
    of the user until enters the nowUsing() loop.

    @param tokens which is the user inputted command as tokens
    @return boolean value T/F which decides if we continue or exit
*/
bool notUsing(char* tokens) {

    // get next token in tokens
    char* token1 = tokens;
    string functionName = token1;

    // below is the range of commands allowed while in the notUsing loop
    if (functionName == "CREATE") {
        create(tokens);
    } else if (functionName == "DROP") {
        drop(tokens);
    } else if (functionName == ".EXIT" || functionName == ".exit") {
        cout << "-- All done." << endl;
        return true;
    } else {
        cout << "-- Invalid Input." << endl;
    }

    // continue in loop
    return false;

}

/**
    Create database.

    This function creates a database with the user inputted token as
    the name as long as the name is not already in use (the database
    already exists).

    @param tokens which is the user inputted command as tokens
*/
void create(char* tokens) {

    // establish first token as the createToken
    char* createToken = tokens;

    // get next token in tokens
    tokens = strtok(NULL, " ");
    char* token2 = tokens;
    string strToken2 = token2;

    // because in the notUsing loop, only option to create is database
    if (strToken2 == "DATABASE") {
        // get next token in tokens
        tokens = strtok(NULL, " ");
        char* charDBName = tokens;

        // remove the last character of charDBName -> ';'
        charDBName[strlen(charDBName)-1] = '\0';
        string dbName = charDBName;

        bool exists = databaseExists(dbName);

        if (!exists) {
            // make the database
            mkdir(charDBName,0777);
            cout << "-- Database " << dbName << " created." << endl;
        } else {
            cout << "-- !Failed to create database " << dbName << " because it already exists." << endl;
        }

    } else {
        cout << "-- Invalid input." << endl;

    }
    
}

/**
    Check if database exists already.

    The inputted database name is passed into this function and it returns true
    or false depending on if the database name correlates to a pre-existing
    database (if the database already exists).

    @param s which is the user inputted database name
    @return boolean value T/F which correlates to if the database exists or not
*/
bool databaseExists(const string &s) {
    struct stat buffer;
    return(stat(s.c_str(), &buffer) == 0);
}

/**
    Check if table exists already.

    The inputted table path is passed into this function and it returns true
    or false depending on if the table path correlates to a pre-existing
    table already in the database in use (if the table already exists in
    said database).

    @param totalPath which is the user inputted table name and database in use 
                    name formatted to a total path to the table
    @return boolean value T/F which decides if we continue or exit
*/
bool tableExists(string totalPath) {
    ifstream tbCheck;
    tbCheck.open(totalPath);
    if(tbCheck) {
        tbCheck.close();
        return true;
    } else {
        return false;
    }
}

/**
    Drop database.

    Using the command tokens, it finds the name of the database to be dropped
    and drops the database with the inputted database name if it exists.

    @param tokens which is the user inputted command.
*/
void drop(char* tokens) {

    // establish first token as drop
    char* token1 = tokens;

    // get next token in tokens
    tokens = strtok(NULL, " ");
    char* token2 = tokens;
    string strToken2 = token2;

    // because in the notUsing loop, only option to create is database
    if (strToken2 == "DATABASE") {
        tokens = strtok(NULL, " ");
        char* charDBName = tokens;

        // remove the last character of charDBName -> ';'
        charDBName[strlen(charDBName)-1] = '\0';
        string dbName = charDBName;

        bool exists = databaseExists(dbName);
        // if database with that name exists
        if (exists) {
            // delete database
            rmdir(charDBName);
            cout << "-- Database " << dbName << " deleted." << endl;
        } else {
            cout << "-- !Failed to delete " << dbName << " because it does not exist." << endl;
        }

    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Alter tables.

    This function alters tables withing the database in use if given
    the table name, and information on how it will be altered.
    If the table exists within the database in use, it then alters it
    by processing the next tokens and stores the new information in the
    table provided.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void alter(char* useLoopTokens, string dbName) {

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* tableToken = useLoopTokens;
    string tbCheck = tableToken;

    // only table able to be altered currently
    if (tbCheck == "TABLE") {
        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        // create the total path to the table in the database in use
        string totalPath = dbName + "/" + tbName + ".txt";

        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* howToAlter = useLoopTokens;
        string strHowToAlter = howToAlter;

        // add is the only functionality currently
        if (strHowToAlter == "ADD") {

            string restOfTokens;
            char* tempVar;

            // while there are still tokens in useLoopTokens,
            // get the rest of tokens and store them as a string to variable restOfTokens            
            while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
                tempVar = useLoopTokens;
                restOfTokens = restOfTokens + " " + tempVar;
            }

            // remove ';' from the restOfTokens string
            size_t pos;
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            if (!restOfTokens.empty()) {
                // format the restOfTokens string
                string original = ",";
                string replacement = " |";
                size_t pos;
                while ((pos = restOfTokens.find(original)) != string::npos) {
                    restOfTokens.replace(pos, 1, replacement);
                }
            }
            
            bool exists = tableExists(totalPath);
    
            if (exists) {
                // create table with totalPath as the path, pass restOfTokens into it, and close the table
                fstream modifyTB;
                modifyTB.open(totalPath.c_str(), fstream::app);
                modifyTB << " |" << restOfTokens;
                modifyTB.close();
                cout << "-- Table " << tbName << " modified." << endl;
            } else {
                cout << "-- !Failed to modify table " << tbName << " because it does not exist." << endl;
            }
        }

    } else {
        cout << "-- Invalid input." << endl;
    }
        
}

/**
    Select tables.

    This function selects tables within the database in use if given
    the table name, and information on how much to be selected.
    If the table exists within the database in use, it then selects the
    amount by processing the next tokens and displays the information to
    the screen.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void select(char* useLoopTokens, string dbName) {

    bool exitLoop = false;
    string tempNextLine;
    string finalLine;

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* selectToken = useLoopTokens;
    string selectCheck = selectToken;

    // if selecting everything to display to screen
    if (selectCheck == "*") {

        while (exitLoop == false) {
            // get each inputted line and pass into string stream
            getline(cin, tempNextLine);
            istringstream issNextLine(tempNextLine);
            // if the last word in stream doesn't end with ';', add to finalLine
            if (tempNextLine.find(";") == string::npos) {
                finalLine = finalLine + " " + tempNextLine;
            } else {
                // remove the ';', add the word to finalLine, exit the loop
                size_t pos;
                while ((pos = tempNextLine.find(";")) != string::npos) {
                    tempNextLine.replace(pos, 1, "");
                }
                finalLine = finalLine + " " + tempNextLine;
                exitLoop = true;
            }
        }
        int n = finalLine.length();
        // declaring character array
        char char_array[n+1];

        // copying the contents of the finalLine to char array
        strcpy(char_array, finalLine.c_str());

        char* newLineTokens = strtok(char_array, " ");
        char* fromToken = newLineTokens;
        string strFromToken = fromToken;

        newLineTokens = strtok(NULL, " ");
        char* tb1NameToken = newLineTokens;
        string tb1Name = tb1NameToken;

        // get next token in useLoopTokens
        newLineTokens = strtok(NULL, " ");
        char* tb1AbrevToken = newLineTokens;
        string tb1Abrev = tb1AbrevToken;

        // get next token in useLoopTokens
        newLineTokens = strtok(NULL, " ");
        char* nextToken = newLineTokens;
        string strNextToken = nextToken;

        // if there is no specific joins being used
        if (strNextToken != "inner" && strNextToken != "left") {

            // more accuratelt name the nextToken variable
            string tb2Name = strNextToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* tb2AbrevToken = newLineTokens;
            string tb2Abrev = tb2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* whereToken = newLineTokens;
            string strWhereToken = whereToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location1AbrevToken = newLineTokens;
            string location1Abrev = location1AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location1IDToken = newLineTokens;
            string location1ID = location1IDToken; 

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* operandToken = newLineTokens;
            string operand = operandToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location2AbrevToken = newLineTokens;
            string location2Abrev = location2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location2IDToken = newLineTokens;
            string location2ID = location2IDToken; 

            // remove ';' from the table name string
            size_t pos;
            while ((pos = location2ID.find(";")) != string::npos) {
                location2ID.replace(pos, 1, "");
            }
            // remove ',' from the tb1 abbreviation string
            while ((pos = tb1Abrev.find(",")) != string::npos) {
                tb1Abrev.replace(pos, 1, "");
            }

            // create total path to tables from table names provided
            string totalPath1 = dbName + "/" + tb1Name + ".txt";
            string totalPath2 = dbName + "/" + tb2Name + ".txt";

            // declare variables
            bool exists1, exists2;
            exists1 = tableExists(totalPath1);
            exists2 = tableExists(totalPath2);

            // if table exists in database in use
            if (exists1 && exists2) {
                string tempVar1;
                string tempVar2;
                string firstLine1;
                string firstLine2;

                // open both tables in use
                ifstream printFile1 (totalPath1.c_str());
                ifstream printFile2 (totalPath2.c_str());
                if (printFile1.is_open()) {
                    // get first line of first table
                    for (int i = 0; i < 1; i++) {
                        getline(printFile1, firstLine1);
                    }
                    // get first line of second table
                    for (int j = 0; j < 1; j++) {
                        getline(printFile2, firstLine2);
                    }

                    // if the location ID's provided in the input are in both header lines of the tables
                    if (firstLine1.find(location1ID) != string::npos && firstLine2.find(location2ID) != string::npos) {

                        // create 2 string streams and pass both firstlines into them
                        istringstream tb1Stream1(firstLine1);
                        istringstream tb2Stream1(firstLine2);

                        int dataNamePosition1;
                        int dataNamePosition2;
                        int dataNameCounter1 = 0;
                        int dataNameCounter2 = 0;

                        // while the first stream still has words to process, keep looping
                        while (tb1Stream1) {
                            string tempWord1;
                            tb1Stream1 >> tempWord1;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord1.find(location1ID) != string::npos) {
                                dataNamePosition1 = dataNameCounter1;
                            } else if (tempWord1 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter1++;
                            }
                        }
                        dataNameCounter1++;

                        // while the second stream still has words to process, keep looping
                        while (tb2Stream1) {
                            string tempWord2;
                            tb2Stream1 >> tempWord2;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord2.find(location2ID) != string::npos) {
                                dataNamePosition2 = dataNameCounter2;
                            } else if (tempWord2 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter2++;
                            }
                        }
                        dataNameCounter2++;

                        string line1;
                        string line2;
                        string tb1TempWord4;
                        string tb1TempWord5;
                        string tb2TempWord4;
                        string tb2TempWord5;

                        // format the new first line and display to the screen
                        cout << firstLine1 << " | " << firstLine2 << endl;

                        // iterate through the lines of the first table and evaluate
                        while(getline(printFile1, line1)) {
                            // reset counter each loop
                            int tb1LoopCounter = 0;
                            // add line to a string stream and evaluate
                            istringstream tb1Stream4(line1);
                            while(tb1Stream4) {
                                tb1Stream4 >> tb1TempWord4;
                                if (tb1LoopCounter == dataNamePosition1) {
                                    tb1TempWord5 = tb1TempWord4;
                                    // call printFile function to print and display the inner join to the screen
                                    printFile(totalPath2, line1, line2, tb1TempWord5, dataNamePosition1);

                                }
                                tb1LoopCounter++;

                            }
                        }
                    }
                }
                printFile1.close();
                printFile2.close();

            } else {
                cout << "-- !Failed to query table " << tb1Name << " and " << tb2Name << " because it does not exist." << endl;
            }
    
        } else if (strNextToken == "inner") {
            
            string innerToken = strNextToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* joinToken = newLineTokens;
            string strJoinToken = joinToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* tb2NameToken = newLineTokens;
            string tb2Name = tb2NameToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* tb2AbrevToken = newLineTokens;
            string tb2Abrev = tb2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* onToken = newLineTokens;
            string strOnToken = onToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location1AbrevToken = newLineTokens;
            string location1Abrev = location1AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location1IDToken = newLineTokens;
            string location1ID = location1IDToken; 

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* operandToken = newLineTokens;
            string operand = operandToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location2AbrevToken = newLineTokens;
            string location2Abrev = location2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location2IDToken = newLineTokens;
            string location2ID = location2IDToken; 

            // remove ';' from the table name string
            size_t pos3;
            while ((pos3 = location2ID.find(";")) != string::npos) {
                location2ID.replace(pos3, 1, "");
            }

            // create total path to table from tables in use
            string totalPath1 = dbName + "/" + tb1Name + ".txt";
            string totalPath2 = dbName + "/" + tb2Name + ".txt";

            bool exists1, exists2;
            exists1 = tableExists(totalPath1);
            exists2 = tableExists(totalPath2);

            // if table exists in database in use
            if (exists1 && exists2) {
                // open file with totalPath
                string tempVar1;
                string tempVar2;
                string firstLine1;
                string firstLine2;
                ifstream printFile1 (totalPath1.c_str());
                ifstream printFile2 (totalPath2.c_str());
                if (printFile1.is_open()) {
                    // get first line of first table
                    for (int i = 0; i < 1; i++) {
                        getline(printFile1, firstLine1);
                    }
                    // get first line of second table
                    for (int j = 0; j < 1; j++) {
                        getline(printFile2, firstLine2);
                    }

                    if (firstLine1.find(location1ID) != string::npos && firstLine2.find(location2ID) != string::npos) {

                        // create 2 string streams and pass both firstlines into them
                        istringstream tb1Stream1(firstLine1);
                        istringstream tb2Stream1(firstLine2);

                        int dataNamePosition1;
                        int dataNamePosition2;
                        int dataNameCounter1 = 0;
                        int dataNameCounter2 = 0;

                        // while the first stream still has words to process, keep looping
                        while (tb1Stream1) {
                            string tempWord1;
                            tb1Stream1 >> tempWord1;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord1.find(location1ID) != string::npos) {
                                dataNamePosition1 = dataNameCounter1;
                            } else if (tempWord1 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter1++;
                            }
                        }
                        dataNameCounter1++;

                        // while the second stream still has words to process, keep looping
                        while (tb2Stream1) {
                            string tempWord2;
                            tb2Stream1 >> tempWord2;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord2.find(location2ID) != string::npos) {
                                dataNamePosition2 = dataNameCounter2;
                            } else if (tempWord2 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter2++;
                            }
                        }
                        dataNameCounter2++;

                        string line1;
                        string line2;
                        string tb1TempWord4;
                        string tb1TempWord5;
                        string tb2TempWord4;
                        string tb2TempWord5;

                        // format the new first line and display to the screen
                        cout << firstLine1 << " | " << firstLine2 << endl;

                        // iterate through the lines of the first table and evaluate
                        while(getline(printFile1, line1)) {
                            // reset counter each loop
                            int tb1LoopCounter = 0;
                            // add line to a string stream and evaluate
                            istringstream tb1Stream4(line1);
                            while(tb1Stream4) {
                                tb1Stream4 >> tb1TempWord4;
                                if (tb1LoopCounter == dataNamePosition1) {
                                    tb1TempWord5 = tb1TempWord4;
                                    // call printFile function to print and display the inner join to the screen
                                    printFile(totalPath2, line1, line2, tb1TempWord5, dataNamePosition1);

                                }
                                tb1LoopCounter++;

                            }
                        }
                    }
                }
                printFile1.close();
                printFile2.close();

            }

        } else if (strNextToken == "left") {

            string leftToken = strNextToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* outerToken = newLineTokens;
            string strOuterToken = outerToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* joinToken = newLineTokens;
            string strJoinToken = joinToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* tb2NameToken = newLineTokens;
            string tb2Name = tb2NameToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* tb2AbrevToken = newLineTokens;
            string tb2Abrev = tb2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* onToken = newLineTokens;
            string strOnToken = onToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location1AbrevToken = newLineTokens;
            string location1Abrev = location1AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location1IDToken = newLineTokens;
            string location1ID = location1IDToken; 

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* operandToken = newLineTokens;
            string operand = operandToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, ".");
            char* location2AbrevToken = newLineTokens;
            string location2Abrev = location2AbrevToken;

            // get next token in useLoopTokens
            newLineTokens = strtok(NULL, " ");
            char* location2IDToken = newLineTokens;
            string location2ID = location2IDToken; 

            // remove ';' from the table name string
            size_t pos3;
            while ((pos3 = location2ID.find(";")) != string::npos) {
                location2ID.replace(pos3, 1, "");
            }

            // create total path to table from tables in use
            string totalPath1 = dbName + "/" + tb1Name + ".txt";
            string totalPath2 = dbName + "/" + tb2Name + ".txt";
        
            bool exists1, exists2;
            exists1 = tableExists(totalPath1);
            exists2 = tableExists(totalPath2);

            // if table exists in database in use
            if (exists1 && exists2) {
                // open file with totalPath
                string tempVar1;
                string tempVar2;
                string firstLine1;
                string firstLine2;
                ifstream printFile1 (totalPath1.c_str());
                ifstream printFile2 (totalPath2.c_str());
                if (printFile1.is_open()) {
                    // get first line of first table
                    for (int i = 0; i < 1; i++) {
                        getline(printFile1, firstLine1);
                    }
                    // get first line of second table
                    for (int j = 0; j < 1; j++) {
                        getline(printFile2, firstLine2);
                    }

                    if (firstLine1.find(location1ID) != string::npos && firstLine2.find(location2ID) != string::npos) {

                        // create 2 string streams and pass both firstlines into them
                        istringstream tb1Stream1(firstLine1);
                        istringstream tb2Stream1(firstLine2);

                        int dataNamePosition1;
                        int dataNamePosition2;
                        int dataNameCounter1 = 0;
                        int dataNameCounter2 = 0;

                        // while the first stream still has words to process, keep looping
                        while (tb1Stream1) {
                            string tempWord1;
                            tb1Stream1 >> tempWord1;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord1.find(location1ID) != string::npos) {
                                dataNamePosition1 = dataNameCounter1;
                            } else if (tempWord1 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter1++;
                            }
                        }
                        dataNameCounter1++;

                        // while the second stream still has words to process, keep looping
                        while (tb2Stream1) {
                            string tempWord2;
                            tb2Stream1 >> tempWord2;
                            // if the word being processed is equal to the data name being found, store position as variable
                            if (tempWord2.find(location2ID) != string::npos) {
                                dataNamePosition2 = dataNameCounter2;
                            } else if (tempWord2 == "|") {
                                // else, keep enumerating the counter and looping
                                dataNameCounter2++;
                            }
                        }
                        dataNameCounter2++;

                        string line1;
                        string line2;
                        string tb1TempWord4;
                        string tb1TempWord5;
                        string tb2TempWord4;
                        string tb2TempWord5;

                        // format the new first line and display to the screen
                        cout << firstLine1 << " | " << firstLine2 << endl;

                        // iterate through the lines of the first table and evaluate
                        while(getline(printFile1, line1)) {
                            // reset counter each loop
                            int tb1LoopCounter = 0;
                            // add line to a string stream and evaluate
                            istringstream tb1Stream4(line1);
                            while(tb1Stream4) {
                                tb1Stream4 >> tb1TempWord4;
                                if (tb1LoopCounter == dataNamePosition1) {
                                    tb1TempWord5 = tb1TempWord4;
                                    // call printFile function to print and display the inner join to the screen
                                    bool check = printFileOuterJoin(totalPath2, line1, line2, tb1TempWord5, dataNamePosition1);
                                    if (check == false) {
                                        cout << line1 << endl;
                                    }
                                }
                                tb1LoopCounter++;

                            }
                        }
                    }
                }
                printFile1.close();
                printFile2.close();
            }
        


        }
        
    } else {   // selecting specific values to display to screen

        int dataCounter = 0;
        string dataArray[200];
        size_t found;

        bool exitLoop = false;
        char nextLine[200];
        string tempNextLine;
        string finalLine;

        // reference the last character of string and check if equal to ','
        while (selectCheck.back() == ',') {
            // remove the ',' characters
            while ((found = selectCheck.find(",")) != string::npos) {
                selectCheck.replace(found, 1, "");
            }
            // store selectCheck as element in dataArray
            dataArray[dataCounter] = selectCheck;

            // get next token in useLoopTokens
            useLoopTokens = strtok(NULL, " ");
            selectToken = useLoopTokens;
            selectCheck = selectToken;

            dataCounter++;
        }
        // store selectCheck in dataArray
        dataArray[dataCounter] = selectCheck;
        dataCounter++;

        while (exitLoop == false) {
            // get each inputted line and pass into string stream
            getline(cin, tempNextLine);
            istringstream issNextLine(tempNextLine);
            // if the last word in stream doesn't end with ';', add to finalLine
            if (tempNextLine.find(";") == string::npos) {
                finalLine = finalLine + " " + tempNextLine;
            } else {
                // remove the ';', add the word to finalLine, exit the loop
                size_t pos;
                while ((pos = tempNextLine.find(";")) != string::npos) {
                    tempNextLine.replace(pos, 1, "");
                }
                finalLine = finalLine + " " + tempNextLine;
                exitLoop = true;
            }
        }
 
        int n = finalLine.length();
        // declaring character array
        char char_array[n+1];
    
        // copying the contents of the finalLine to char array
        strcpy(char_array, finalLine.c_str());

        // get next token in newLineTokens
        char* newLineTokens = strtok(char_array, " ");
        char* fromToken = newLineTokens;
        string strFromToken = fromToken;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* tbToken = newLineTokens;
        string tbName = tbToken;

        // create total path to table name with database name in use
        string totalPath = dbName + "/" + tbName + ".txt";

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* whereToken = newLineTokens;
        string strWhereToken = whereToken;

        // only command available currently
        if (strWhereToken == "where") {

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* dataNameToken = newLineTokens;
            string dataName = dataNameToken;

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* operandToken = newLineTokens;
            string operand = operandToken;

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* dataToken = newLineTokens;
            string data = dataToken;

            // remove ';' from data variable
            size_t pos;
            while ((pos = data.find(";")) != string::npos) {
                data.replace(pos, 1, "");
            }

            bool exists = tableExists(totalPath);

            // if table exists at path totalPath
            if (exists) {

                bool success;
                success = printSelect(dataArray, dataCounter, dbName, totalPath, dataName, operand, data);

                if (!success) {
                    cout << "-- Error querying file, try again." << endl;
                }
            } else {
                cout << "-- !Failed to query table " << tbName << " because it does not exist." << endl;

            }

        } else {
        cout << "-- Invalid input." << endl;
        }

    }

}

/**
    Prints the outer join of files that is passed to it.

    This function prints and formats the inner join of the two tables
    being used with the correct information passed to it. It does this
    by passing the lines into string streams and evaluating through
    while loops. It displays this formatted information to the screen.

    @param totalPath2 which is the total path to the second table.
    @param line1 which is the line being evaluated from the first table.
    @param line2 which is the line being evaluated from the first table.
    @param tempWord which is the data being checked to see if the columns match.
    @param dataPosition which is the column index of the correct data.

    @return boolean flag which indicates whether left join has occurred.
*/
bool printFileOuterJoin(string totalPath2, string line1, string line2, string tempWord, int dataPosition) {

    // initializing the string stream and temporary variables
    ifstream printFile2 (totalPath2.c_str());
    string tb2TempWord4;
    string tb2TempWord5;
    bool found = false;

    // get the first line of second table and store as string
    while(getline(printFile2, line2)) {
        // pass string into string stream
        istringstream tb2Stream4(line2);
        // while string stream has strings to parse
        while(tb2Stream4) {
            tb2Stream4 >> tb2TempWord4;
            // if the string is equal to the temp word in table 1
            if (tempWord == tb2TempWord4) {
                // format line and print it
                cout << line1 << " | " << line2 << endl;
                // change flag to found
                printFile2.close();
                return true;
            }
        }
    }

    printFile2.close();
    return false;
}

/**
    Prints and formats the files that is passed to it.

    This function prints and formats the inner join of the two tables
    being used with the correct information passed to it. It does this
    by passing the lines into string streams and evaluating through
    while loops. It displays this formatted information to the screen.

    @param totalPath2 which is the total path to the second table.
    @param line1 which is the line being evaluated from the first table.
    @param line2 which is the line being evaluated from the first table.
    @param tempWord which is the data being checked to see if the columns match.
    @param dataPosition which is the column index of the correct data.
*/
void printFile(string totalPath2, string line1, string line2, string tempWord, int dataPosition) {

    // initializing the string stream and temporary variables
    ifstream printFile2 (totalPath2.c_str());
    string tb2TempWord4;
    string tb2TempWord5;

    while(getline(printFile2, line2)) {
        istringstream tb2Stream4(line2);
        while(tb2Stream4) {
            tb2Stream4 >> tb2TempWord4;
            if (tempWord == tb2TempWord4) {
                cout << line1 << " | " << line2 << endl;
            }
        }
    }

    // close the files
    printFile2.close();
}


/**
    Select specific columns from tables.

    This function selects tables withing the database in use if given
    the table name, and information on how much to be selected. It processes
    the data given and decides what data to print, and what data to ignore.
    It then displays this to the screen.

    @param dataArray which is an array with the columns to be printed.
    @param dataCounter which is the amount of columns in the array.
    @param dbName which is the database in use name.
    @param totalPath which is the total path to the table.
    @param dataName which is the data name which is being evaluated.
    @param operand which is the operand of the evaluation.
    @param data which is the data being used to evaluate.

    @return boolean value T/F if it was successful or not.
*/
bool printSelect(string dataArray[200], int dataCounter, string dbName, string totalPath, string dataName, string operand, string data) {

    int dataNamePosition;
    int dataNameCounter = 0;
    int indexCounter = 0;
    string firstLine;
    string tempToken;
    string line;

    // open file at path totalPath
    ifstream tableInUse;
    tableInUse.open(totalPath);

    // get first line of table
    for (int i = 0; i < 1; i++) {
        getline(tableInUse, firstLine);
    }

    // check if the data names that want to be selected exist in table header
    if (firstLine.find(dataName) != string::npos) {

        // create string stream and pass the firstline into it
        istringstream firstStream(firstLine);

        // while the stream still has words to process, keep looping
        while (firstStream) {
            string tempWord1;
            firstStream >> tempWord1;
            // if the word being processed is equal to the data name being found, store position as variable
            if (tempWord1.find(dataName) != string::npos) {
                dataNamePosition = dataNameCounter;
            } else if (tempWord1 == "|") {
                // else, keep enumerating the counter and looping
                dataNameCounter++;
            }
        }
        // taking the bars '|' into consideration, double the counter
        dataNameCounter += dataNameCounter;

        // create a second string stream and pass the first line into it
        istringstream secondStream(firstLine);
        int barCount = 0;
        int index[200];

        // while there are still words to process in stream
        while (secondStream) {
            string tempWord2;
            secondStream >> tempWord2;

            // count how many bars (i.e. how many columns in header)
            if (tempWord2 == "|") {
                barCount++;
                continue;
            }

            // finding the index of columns selected
            for (int i = 0; i < dataCounter; i++) {
                if (tempWord2 == dataArray[i]) {
                    index[indexCounter] = barCount;
                    indexCounter++;
                }
            }
        }

        // create another string stream and pass the first line into it
        istringstream fourthStream(firstLine);
        string tempWord4;
        string newFirstLine;

        // while there are still words to process in stream
        while (fourthStream) {

            // if the temporary variable is equal to the data name being looked for
            if (tempWord4 == dataName) {

                // get the next word in the stream and process it
                fourthStream >> tempWord4;
                while (tempWord4 != "|") {
                    fourthStream >> tempWord4;
                }
                fourthStream >> tempWord4;
            }
            // format the newFirstLine
            newFirstLine = newFirstLine + " " + tempWord4;

            // get the next word in the stream
            fourthStream >> tempWord4;
        }

        // format the new first line by removing the first two characters (two empty spaces)
        newFirstLine.erase(0,2);
        cout << newFirstLine << endl;

        // process the rest of the lines in the table, line by line
        while (getline(tableInUse, line)) {
            
            // if the line does not contain the data being searched for
            if (line.find(data) == string::npos) {
                int columnCounter = 0;

                // create another string stream and pass the line into it
                istringstream thirdStream(line);

                // while there are words to process in the stream
                while (thirdStream) {
                    string tempWord3;
                    thirdStream >> tempWord3;

                    // format the temporary variable by removing " ' "
                    size_t pos3;
                    while ((pos3 = tempWord3.find("'")) != string::npos) {
                        tempWord3.replace(pos3, 1, "");
                    }

                    // used to keep track of which column is being selected
                    if (tempWord3 == "|") {
                        columnCounter++;
                        continue;
                    }

                    // print out the selected rows and columns given the data above
                    for (int j = 0; j < (indexCounter); j++) {
                        if (columnCounter == index[j]) {
                            cout << tempWord3 << " | ";
                        } else if ((columnCounter - 1) == index[j]){
                            cout << tempWord3;
                            break;
                        }
                    }
                }
                cout << endl;
            }
        }

        tableInUse.close();

    } else {
        cout << "-- Error modifying records." << endl;
        tableInUse.close();
        return false;
    }
    
    tableInUse.close();
    return true;

}

/**
    Update tables.

    This function updates tables within the database in use if the table
    exists within the database in use. This is successful if given the table 
    name, information on what parameter will be updated, and information on 
    what parameter will be checked in order for the previous parameter to 
    be updated.

    @param useLoopTokens which is the user inputted command.
    @param useDBName which is the database name in use.
*/
void update(char* useLoopTokens, string useDBName) {

    int dataNameToBeChangedPosition;
    int dataNameFindingPosition;
    int dataNameFindingCounter = 0;
    int dataNameToBeChangedCounter = 0;
    int dataNameFinding = 0;
    int counter = 0;
    string firstLine;
    string tempToken;
    string line;

    bool exitLoop = false;
    string tempNextLine;
    string finalLine;

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* charTBName = useLoopTokens;
    string tbName = charTBName;
    
    // create the total path to table given database in use
    string totalPath = useDBName + "/" + tbName + ".txt";
    
    while (exitLoop == false) {
        // get each inputted line and pass into string stream
        getline(cin, tempNextLine);
        istringstream issNextLine(tempNextLine);
        // if the last word in stream doesn't end with ';', add to finalLine
        if (tempNextLine.find(";") == string::npos) {
            finalLine = finalLine + " " + tempNextLine;
        } else {
            // remove the ';', add the word to finalLine, exit the loop
            size_t pos;
            while ((pos = tempNextLine.find(";")) != string::npos) {
                tempNextLine.replace(pos, 1, "");
            }
            finalLine = finalLine + " " + tempNextLine;
            exitLoop = true;
        }
    }
 
    int n = finalLine.length();
    // declaring character array
    char char_array[n+1];
 
    // copying the contents of the finalLine to char array
    strcpy(char_array, finalLine.c_str());

    bool exists = tableExists(totalPath);

    // if table already exists
    if (exists) {

        char* newLineTokens = strtok(char_array, " ");
        string setToken = newLineTokens;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charDataNameToBeChanged = newLineTokens;
        string dataNameToBeChanged = charDataNameToBeChanged;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charFirstOperand = newLineTokens;
        string firstOperand = charFirstOperand;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charDataToBeSet = newLineTokens;
        string dataToBeSet = charDataToBeSet;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charWhere = newLineTokens;
        string whereToken = charWhere;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charDataNameFinding = newLineTokens;
        string dataNameFinding = charDataNameFinding;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charSecondOperand = newLineTokens;
        string secondOperand = charSecondOperand;

        // get next token in newLineTokens
        newLineTokens = strtok(NULL, " ");
        char* charDataToBeChanged = newLineTokens;
        string dataToBeChanged = charDataToBeChanged;

        // format the dataToBeChanged by removing the ';' from the end
        size_t pos;
        while ((pos = dataToBeChanged.find(";")) != string::npos) {
            dataToBeChanged.replace(pos, 1, "");
        }

        // open file at path totalPath
        ifstream tableInUse;
        tableInUse.open(totalPath);

        // create and open a temporary file in the same database to write to
        string tempPath = useDBName + "/Writing.txt";
        ofstream tempFile;
        tempFile.open(tempPath);

        // get the first line of table in use
        for (int i = 0; i < 1; i++) {
            getline(tableInUse, firstLine);
        }

        // if the first line includes the data name that has the data that will be inserted 
        // and the data name that has the data that will be changed
        if (firstLine.find(dataNameToBeChanged) != string::npos && firstLine.find(dataNameFinding) != string::npos) {

            // create a string stream and pass the first line of the table in use to it
            istringstream firstStream(firstLine);

            // while there are words in the stream to be processed
            while (firstStream) {
                string tempWord1;
                firstStream >> tempWord1;

                // if temporary variable contains the data name that will be changed
                if (tempWord1.find(dataNameToBeChanged) != string::npos) {

                    // store the position
                    dataNameToBeChangedPosition = dataNameToBeChangedCounter;

                    // create a second string stream and pass the first line into it
                    istringstream secondStream(firstLine);

                    // while there are words in the stream to be processed
                    while (secondStream) {
                        string tempWord2;
                        secondStream >> tempWord2;

                        // if the temporary variable contains the data name to be set
                        if (tempWord2.find(dataNameFinding) != string::npos) {
                            // store the position
                            dataNameFindingPosition = dataNameFindingCounter;

                        } else if (tempWord2.find(dataNameToBeChanged) != string::npos) { // if the temporary variable contains the data name to be changed
                            // store the position
                            dataNameToBeChangedPosition = dataNameToBeChangedCounter;

                        } else if (tempWord2 == "|") { // if neither of the options above, enumerate the counters
                            dataNameFindingCounter++;
                            dataNameToBeChangedCounter++;
                        }

                    }

                }

            }

            // if the data name to be set is the same as the data name to be changed, their positions are equal
            if (dataNameFinding == dataNameToBeChanged) {
                dataNameToBeChangedPosition = dataNameFindingPosition;
            }

            // to account for the bars '|', double the positions
            dataNameFindingPosition += dataNameFindingPosition;
            dataNameToBeChangedPosition += dataNameToBeChangedPosition;

            // insert the first line of the table in use to the temporary writing file
            tempFile << firstLine;

            // while there are still lines to process in the table in use
            while (getline(tableInUse, line)) {
                tempFile << endl;

                // create string string and pass the line being processed into it
                istringstream thirdStream(line);
                bool keepGoing = true;

                // while the stream has words to process and keepGoing == true, loop
                while (thirdStream && keepGoing) {
                    string tempWord3;

                    // store the data at the dataNameFindingPosition to a temporary variable
                    for (int j = 0; j < (dataNameFindingPosition + 1); j++) {
                        thirdStream >> tempWord3;
                    }

                    // if the data in the temporary variable is equal to the data to be changed
                    if (tempWord3 == dataToBeChanged) {

                        // create another string stream and pass the line being processed into it
                        istringstream fourthStream(line);
                        string tempWord4;

                        // create another string stream and pass the line being processed into it
                        istringstream fifthStream(line);
                        string tempWord5;

                        // pass the data from before the index of the dataNameToBeChanged to the temporary file and format
                        for (int l = 0; l < (dataNameToBeChangedPosition); l++) {
                            fourthStream >> tempWord4;
                            tempFile << tempWord4 + " ";
                        }

                        // while there are still words to be processed in the stream
                        while (fifthStream) {
                            // parse to the position of the data that is being changed
                            for (int k = 0; k < (dataNameToBeChangedPosition + 1); k++) {
                                fifthStream >> tempWord5;
                            }
                            // insert the new data that will be set in the location of the data being changed to the temporary file
                            tempFile << dataToBeSet << " ";
                            
                            // enumerate the counter to keep track of number of updates 
                            counter++;

                            // get the data at the position after the position of the data to be changed
                            fifthStream >> tempWord5;

                            // while there are still words to be processed in the stream after the change has been made,
                            //  add them to the temporary file
                            while(fifthStream) {
                                tempFile << tempWord5 + " ";
                                fifthStream >> tempWord5;
                            }
                            break;
                        }
                        break;
                        
                    } else {
                        tempFile << line;
                    }
                    break;
                }
            }
        }
        // close both files, delete the original file, rename the path of the temporary file to the totalPath
        tableInUse.close();
        tempFile.close();
        remove(totalPath.c_str());
        rename(tempPath.c_str(), totalPath.c_str());
        if (counter == 1) {
            cout << "-- " << counter << " record modified" << endl;
        } else {
            cout << "-- " << counter << " records modified" << endl;
        }
    } else {
        cout << "-- !Failed to update table " << tbName << " because it does not exist." << endl;
    }


}

/**
    Insert high-level data into tables.

    This function inserts high-level data into tables within the database
    in use if the table exists within the database in use. Depending on
    the success of the insertion, either a success or failure message is
    displayed on the screen.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void insert(char* useLoopTokens, string dbName) {

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* charPointName = useLoopTokens;
    string pointToTB = charPointName;

    // this is the only command available currently
    if (pointToTB == "into") {

        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        // create the total path to table with database in use
        string totalPath = dbName + "/" + tbName + ".txt";

        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, "(");
        char* howToAlter = useLoopTokens;
        string strHowToAlter = howToAlter;

        // this is the only command available currently
        if (strHowToAlter == "values") {

            // get next token in useLoopTokens
            useLoopTokens = strtok(NULL, " ");
            char* charRestOfTokens = useLoopTokens;
            string restOfTokens = charRestOfTokens;

            char* tempVar;

            // while there are still tokens in useLoopTokens,
            // get the rest of tokens and store them as a string to variable restOfTokens
            while ((useLoopTokens = strtok(NULL, " ")) != NULL) {
                tempVar = useLoopTokens;
                restOfTokens = restOfTokens + " " + tempVar;
            }

            // remove the ';' from the restOfTokens string
            size_t pos;
            while ((pos = restOfTokens.find(";")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            // remove the parentheses from the restOfTokens string
            while ((pos = restOfTokens.find(")")) != string::npos) {
                restOfTokens.replace(pos, 1, "");
            }

            if (!restOfTokens.empty()) {
                // format the string for insertion
                string original = ",";
                string replacement = " |";
                while ((pos = restOfTokens.find(original)) != string::npos) {
                    restOfTokens.replace(pos, 1, replacement);
                }
            }
            
            bool exists = tableExists(totalPath);
    
            // if the table exists
            if (exists) {
                // open table at path totalPath and insert the formatted restOfTokens
                fstream modifyTB;
                modifyTB.open(totalPath.c_str(), fstream::app);
                modifyTB << "\n" << restOfTokens;
                modifyTB.close();
                cout << "-- 1 new record inserted." << endl;
            } else {
                cout << "-- !Failed to insert record into table " << tbName << " because it does not exist." << endl;
            }
        }

    } else {
        cout << "-- Invalid input." << endl;
    }
}

/**
    Delete high-level data in tables.

    This function deletes row(s) of high-level data in table within the 
    database in use if the table exists within the database in use. 
    Depending on the success of the deletion, either a success or failure 
    message is displayed on the screen.

    @param useLoopTokens which is the user inputted command.
    @param dbName which is the database name in use.
*/
void deleteData(char* useLoopTokens, string dbName) {

    bool exitLoop = false;
    char nextLine[200];
    string tempNextLine;
    string finalLine;

    // get next token in useLoopTokens
    useLoopTokens = strtok(NULL, " ");
    char* fromToken = useLoopTokens;
    string pointToTB = fromToken;

    // only command available currently
    if (pointToTB == "from") {

        // get next token in useLoopTokens
        useLoopTokens = strtok(NULL, " ");
        char* charTBName = useLoopTokens;
        string tbName = charTBName;

        // create the total path to the table from database in use
        string totalPath = dbName + "/" + tbName + ".txt";

        while (exitLoop == false) {
            // get each inputted line and pass into string stream
            getline(cin, tempNextLine);
            istringstream issNextLine(tempNextLine);
            // if the last word in stream doesn't end with ';', add to finalLine
            if (tempNextLine.find(";") == string::npos) {
                finalLine = finalLine + " " + tempNextLine;
            } else {
                // remove the ';', add the word to finalLine, exit the loop
                size_t pos;
                while ((pos = tempNextLine.find(";")) != string::npos) {
                    tempNextLine.replace(pos, 1, "");
                }
                finalLine = finalLine + " " + tempNextLine;
                exitLoop = true;
            }
        }
 
        int n = finalLine.length();
        // declaring character array
        char char_array[n+1];
    
        // copying the contents of the finalLine to char array
        strcpy(char_array, finalLine.c_str());

        // get next token in newLineTokens
        char* newLineTokens = strtok(char_array, " ");
        char* whereToken = newLineTokens;
        string pointToValue = whereToken;

        // only command available currently
        if (pointToValue == "where") {

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* dataNameToken = newLineTokens;
            string dataName = dataNameToken;

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* operatorToken = newLineTokens;
            string operand = operatorToken;

            // get next token in newLineTokens
            newLineTokens = strtok(NULL, " ");
            char* dataToken = newLineTokens;
            string data = dataToken;

            // find ';' in data and remove
            size_t pos;
            while ((pos = data.find(";")) != string::npos) {
                data.replace(pos, 1, "");
            }

            bool exists = tableExists(totalPath);

            // if the table exists
            if (exists) {

                bool success;
                success = modifyTable(dbName, totalPath, dataName, operand, data);

                if (!success) {
                    cout << "-- Error deleting data in file, try again." << endl;
                }
            } else {
                cout << "-- !Failed to delete data in table " << tbName << " because it does not exist." << endl;

            }

        }

    } else {
        cout << "-- Invalid input." << endl;
    }

}

/**
    Modify high-level data in tables.

    This function deletes row(s) of high-level data in table within the 
    database in use if the table exists within the database in use. It
    processes the data given and decides which rows to delete based on the
    Depending on the success of the modification, either a success or failure 
    message is displayed on the screen, along with a counter of how many
    records have been deleted.

    @param dbName which is the database in use name.
    @param totalPath which is the total path to the table.
    @param dataName which is the data name which is being evaluated.
    @param operand which is the operand of the evaluation.
    @param data which is the data being used to evaluate.

    @return boolean value T/F if it was successful or not.
*/
bool modifyTable(string dbName, string totalPath, string dataName, string operand, string data) {
    
    int dataNamePosition;
    int dataNameCounter = 0;
    int counter = 0;
    string firstLine;
    string tempToken;
    string line;

    // open table in use at path totalPath
    ifstream tableInUse;
    tableInUse.open(totalPath);

    // create and open temporary table to write to
    string tempPath = dbName + "/Writing.txt";
    ofstream tempFile;
    tempFile.open(tempPath);

    // get the first line of the table in use
    for (int i = 0; i < 1; i++) {
        getline(tableInUse, firstLine);
    }

    // convert the operand to a char and enter switch statement
    char charOperand = operand[0];
    switch(charOperand) {
        case '>':
            // if the first line contains the data name
            if (firstLine.find(dataName) != string::npos) {

                // create a string string and pass the first line into it
                istringstream firstStream(firstLine);

                // while there are words to process in string stream
                while (firstStream) {
                    string tempWord1;
                    firstStream >> tempWord1;

                    // loop thorugh while until the position of data name is found and stored
                    if (tempWord1.find(dataName) != string::npos) {
                        dataNamePosition = dataNameCounter;
                    } else if (tempWord1 == "|") {
                        dataNameCounter++;
                    }
                }

                // to account for the bars '|', double the dataNamePosition
                dataNamePosition += dataNamePosition;

                // pass the first liine of the table in use into the temporary table
                tempFile << firstLine;
                float numData = stof(data);

                // while there are still lines to process in table in use
                while (getline(tableInUse, line)) {

                    // create string stream and pass line to be processed
                    istringstream secondStream(line);
                    bool keepGoing = true;

                    // while there are words to process in stream and keepGoing == true
                    while (secondStream && keepGoing) {
                        string tempWord2;

                        // store data at position dataNamePosition to temporary variable
                        for (int i = 0; i < dataNamePosition; i++) {
                            secondStream >> tempWord2;
                        }

                        // if '.' is found (it is a float)
                        if (tempWord2.find('.') != string::npos ) {

                            float tempFloat = stof(tempWord2);

                            // if the temporary variable is greater than the original data
                            if (tempFloat > numData) {
                                // enumerate counter and exit loop
                                counter++;
                                keepGoing = false;
                            } else {
                                // add line to temporary table
                                tempFile << endl << line;
                            }

                        }

                    }
                }

                // close both files, delete the original file, rename the path of the temporary file to the totalPath
                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record deleted" << endl;
                } else {
                    cout << "-- " << counter << " records deleted" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
        case '<':
            // if the first line contains the data name
            if (firstLine.find(dataName) != string::npos) {

                // create a string string and pass the first line into it
                istringstream firstStream(firstLine);

                // while there are words to process in string stream
                while (firstStream) {
                    string tempWord1;
                    firstStream >> tempWord1;

                    // loop thorugh while until the position of data name is found and stored
                    if (tempWord1.find(dataName) != string::npos) {
                        dataNamePosition = dataNameCounter;
                    } else if (tempWord1 == "|") {
                        dataNameCounter++;
                    }
                }

                // to account for the bars '|', double the dataNamePosition
                dataNameCounter += dataNameCounter;

                // pass the first liine of the table in use into the temporary table
                tempFile << firstLine;
                float numData = stof(data);

                // while there are still lines to process in table in use
                while (getline(tableInUse, line)) {

                    // create string stream and pass line to be processed
                    istringstream secondStream(line);
                    bool keepGoing = true;

                    // while there are words to process in stream and keepGoing == true
                    while (secondStream && keepGoing) {
                        string tempWord2;

                        // store data at position dataNamePosition to temporary variable
                        for (int i = 0; i < dataNameCounter; i++) {
                            secondStream >> tempWord2;
                        }

                        // if '.' is found (it is a float)
                        if (tempWord2.find('.') != string::npos ) {

                            float tempFloat = stof(tempWord2);

                            // if the temporary variable is less than the original data
                            if (tempFloat < numData) {
                                // enumerate counter and exit loop
                                counter++;
                                keepGoing = false;
                            } else {
                                // add line to temporary table
                                tempFile << endl << line;
                            }
                        }
                    }
                }
                // close both files, delete the original file, rename the path of the temporary file to the totalPath
                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record deleted" << endl;
                } else {
                    cout << "-- " << counter << " records deleted" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
        case '=':
            // if the first line contains the data name
            if (firstLine.find(dataName) != string::npos) {

                // pass the first line to the temporary file
                tempFile << firstLine;

                // while there are lines to process in table in use
                while (getline(tableInUse, line)) {
                    // if the line containes the data to be found, enumerate
                    if (line.find(data) != string::npos) {
                        counter++;
                    } else {
                        // add line to temporary file
                        tempFile << endl << line;
                    }

                }
                // close both files, delete the original file, rename the path of the temporary file to the totalPath
                tableInUse.close();
                tempFile.close();
                remove(totalPath.c_str());
                rename(tempPath.c_str(), totalPath.c_str());
                if (counter == 1) {
                    cout << "-- " << counter << " record modified" << endl;
                } else {
                    cout << "-- " << counter << " records modified" << endl;
                }
            } else {
                cout << "-- Error modifying records." << endl;
                tableInUse.close();
                tempFile.close();
                remove(tempPath.c_str());
                return false;
            }
            break;
    }
    // close both files, delete the temporary file
    tableInUse.close();
    tempFile.close();
    remove(tempPath.c_str());
    return true;

}
