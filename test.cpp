/*
    CPS842 Info Retrival 
    Tony Misic 500759917
    Thomas Liu 500717670
*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
    string input = "";
    while (true) {
        cout << "Please enter a query: ";
        cin >> input;
        if (input == "ZZEND") {
            break;
        } else { // Code for testing
            cout << "Query was: " + input + "\n";
            // TO DO
            
        }
    }
    return 0;
}