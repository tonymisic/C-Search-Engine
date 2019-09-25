/*
    CPS842 Info Retrival 
    Tony Misic 500759917
    Thomas Liu 500717670
*/
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <map>
#include <iterator>
#include <locale>
using namespace std;
// globals
map<string, int> mapping; // Ordered Map (Sorted alphabetically by key)
map<string, int> stopwords;

// --- INPUT --- //
string document_collection_file = "data/cacm.all";
string stop_words_file = "data/stopwords.txt";

// --- OUTPUT --- //
string dictonary_file = "output/dictonary.txt";
string postings_lists_file = "output/postings.txt";

// functions
vector<string> split(string text);
string lower(string text);
bool stop_word_removal(string text);
void fill_stop_words();
string delimit(string text);

int main(int argc, char *argv[]) {
    fill_stop_words();
    ifstream in_file(document_collection_file);
    string line;
    cout << delimit("Tony-Misic.username") << endl;
    if (in_file.is_open()) {
        cout << "Getting data...\n";
        while ( getline(in_file, line) ) {
            if (line == ".T" || line == ".W") {
                getline(in_file, line);
                while (line != ".X" && line != ".B" && line != ".A" && line != ".N" && line != ".K" && line != ".I" && line != ".C") {
                    vector<string> words = split(line);
                    for (int i = 0; i < words.size(); i++) {
                        string temp = delimit(lower(words[i]));
                        if (!stop_word_removal(temp)) {
                            if (mapping[temp] > 0) {
                                mapping[temp] += 1;
                            } else {
                                mapping[temp] = 1;
                            }
                        }
                    }
                    getline(in_file, line);
                }
            }
        }
        ofstream out_dict_file(dictonary_file);
        for (auto& t : mapping) {
            out_dict_file << t.first << " "  << t.second << "\n";
        }
        in_file.close();
        out_dict_file.close();
        cout << "Data received.\n";
    }
    return 0;
}

string delimit(string text) {
    char * pch;
    string result = "";
    pch = strtok ((char*)text.c_str(),"!@#$%^&*()_-+={}[]\\|:;,.<>/?`~\'\"");
    while (pch != NULL) {
        string tmp = pch;
        result += tmp;
        pch = strtok(NULL, "!@#$%^&*()_-+={}[]\\|:;,.<>/?`~\'\"");
    }
    return result;
}

vector<string> split(string text) {
    istringstream iss(text);
    vector<string> results((istream_iterator<std::string>(iss)),istream_iterator<std::string>());
    return results;
}

string lower(string text) {
    string result = "";
    for (int i = 0; i < text.length(); i++){
        result += tolower(text[i]);
    }
    return result;
}

bool stop_word_removal(string text) {
    if (stopwords[text] == 1) {
        return true;
    } else {
        return false;
    }
}

void fill_stop_words() {
    ifstream in_file(stop_words_file);
    string line;
    if (in_file.is_open()) {
        while (getline(in_file, line)) {
            stopwords[line] = 1;
        }
    }
}
