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
#include <unordered_map>
#include <iterator>
#include <locale>
#include "porter2_stemmer.h"
using namespace std;

bool STOP_WORD_FLAG = true;
bool STEMMING_FLAG = true;

// --- INPUT --- //
string dictonary_file = "output/dictonary.txt";
string postings_lists_file = "output/postings.txt";
string titles_file = "output/titles.txt";

map<string, map<int, int>> document_frequency_table; // map<term, map<doc_id, term-freq>>
map<string, map<int, double>> document_weight_table; // map<term, map<doc_id, weight>>
map<string, double> idf_table; // map<term, idf-val>

// functions
vector<string> delimit(string text);
vector<string> split(string text);


int main(int argc, char *argv[]) {
    string input_dictonary = "";
    string input_postings = "";
    ifstream posts_in_file(postings_lists_file);
    ifstream dictonary_in_file(dictonary_file);
    ifstream titles_in_file(titles_file);

    while ( getline(dictonary_in_file, input_dictonary) && getline(posts_in_file, input_postings)) {
        string term = split(input_dictonary)[0];
        vector<string> postings_term = split(input_postings);
        pair<int, int> p;
        map<int, int> temp;   
        for (int i = 0; i < postings_term.size(); i++) {
            if (i % 3 == 0) {
                p.first = stoi(postings_term[i].c_str());
            }
            if (i % 3 == 1) {
                p.second = stoi(postings_term[i].c_str());
                temp.insert(p);
            }
        }
        pair<string, map<int,int>> p2 = {term, temp};
        document_frequency_table.insert(p2);
    }
    posts_in_file.close();
    dictonary_in_file.close();
    titles_in_file.close();

    for (auto i : document_frequency_table) {
        pair<string, double> p(i.first, log10(3204 / i.second.size()));
        idf_table.insert(p);
    }

    for (auto i : document_frequency_table) {
        map<int, double> temp; 
        for (auto j : i.second) {
            pair<int, double> p(j.first, ((1 + log10(j.second)) * idf_table[i.first]));
            temp.insert(p);
        }
        pair<string, map<int, double>> p(i.first, temp);
        document_weight_table.insert(p);
    }
    
    return 0;
}

vector<string> delimit(string text) {
    char * pch;
    vector<string> result;
    pch = strtok ((char*)text.c_str()," ");
    while (pch != NULL) {
        string tmp = pch;
        result.insert(result.end(), tmp);
        pch = strtok(NULL, " ");
    }
    return result;
}
vector<string> split(string text) {
    istringstream iss(text);
    vector<string> results((istream_iterator<std::string>(iss)),istream_iterator<std::string>());
    return results;
}
