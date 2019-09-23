/*
    CPS842 Info Retrival 
    Tony Misic 500759917
    Thomas Liu 500717670
*/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <map>
using namespace std;

// globals
map<string, int> mapping; // Ordered Map (Sorted alphabetically by key)
map<string, string> fields = {
    {".I","ID"},
    {".T ","title"},
    {".W","abstract"},
    {".B","publication date"},
    {".A","list of authors"}
};

// --- INPUT --- //
string document_collection_file = "data/cacm.all";

// --- OUTPUT --- //
string dictonary_file = "output/dictonary.txt";
string postings_lists_file = "output/postings.txt";

// functions
vector<string> split(string text);

int main(int argc, char *argv[]) {
    ifstream in_file(document_collection_file);
    string line;
    if (in_file.is_open()) {
        cout << "Getting file...\n";
        while ( getline(in_file, line) ) {
            
        }
        in_file.close();
        cout << "Data received.\n";
    }
    return 0;
}

vector<string> split(string text) {
    istringstream iss(text);
    return vector<string> results(istream_iterator<string>{iss}, istream_iterator<string>());
}
 

