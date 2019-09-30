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

// globals
map<string, int> mapping; // Ordered Map (Sorted alphabetically by key)
map<string, int> stopwords;
bool STOP_WORD_FLAG = true;
bool STEMMING_FLAG = true;
map<string, pair<int, vector<pair<int, vector<int>>>>> postings_map;

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
vector<string> delimit(string text);

int main(int argc, char *argv[]) {
    fill_stop_words();
    ifstream in_file(document_collection_file);
    ofstream posts_out_file(postings_lists_file);
    int position_counter = 1;
    map<string, int> occurences;
    map<string, vector<int>> positions;
    string line;

    if (*argv[1] == '0') {
        STOP_WORD_FLAG = false;
    }
    if (*argv[2] == '0') {
        STEMMING_FLAG = false;
    } 

    if (in_file.is_open()) {
        std::cout << "Getting data...\n";
        int current_ID = 1;
        while ( std::getline(in_file, line) ) {
            vector<string> w = split(line);
            string i_l = ".I";
            if (w[0] == i_l) {
                current_ID = atoi(w[1].c_str());
            }
            if (line == ".T" || line == ".W") {
                if (line == ".T") {
                    position_counter = 0;
                    occurences.clear();
                    positions.clear();
                }
                std::getline(in_file, line);
                // loop for one doc with associated ID
                while (line != ".X" && line != ".B" && line != ".A" && line != ".N" && line != ".K" && line != ".I" && line != ".C") {
                    vector<string> words = split(line);
                    for (int i = 0; i < words.size(); i++) {
                        vector<string> v = delimit(lower(words[i]));
                        for (int j = 0; j < v.size(); j++) {
                            string temp = v[j];
                            if (STEMMING_FLAG) {
                                Porter2Stemmer::stem(v[j]);
                                temp = v[j];
                            } else {
                                temp = v[j];
                            }
                            position_counter++;
                            if (STOP_WORD_FLAG) {
                                if (!stop_word_removal(temp)) {
                                    if (mapping[temp] > 0) {
                                        mapping[temp] += 1;
                                    } else {
                                        mapping[temp] = 1;
                                    }
                                    if (occurences[temp] > 0) {
                                        occurences[temp] += 1;
                                        positions[temp].insert(positions[temp].end(), position_counter);
                                    } else {
                                        occurences[temp] = 1;
                                        positions[temp].insert(positions[temp].end(), position_counter);
                                    }
                                }
                            } else {
                                if (mapping[temp] > 0) {
                                    mapping[temp] += 1;
                                } else {
                                    mapping[temp] = 1;
                                }
                                if (occurences[temp] > 0) {
                                    occurences[temp] += 1;
                                    positions[temp].insert(positions[temp].end(), position_counter);
                                } else {
                                    occurences[temp] = 1;
                                    positions[temp].insert(positions[temp].end(), position_counter);
                                }
                            }
                        }
                        
                    }
                    std::getline(in_file, line);
                }
                for (auto& i : positions) {
                    pair <int, vector<int>> p;
                    p.first = current_ID; 
                    p.second = positions[i.first];
                    postings_map[i.first].first = occurences[i.first];
                    postings_map[i.first].second.insert(postings_map[i.first].second.end(), p);
                }
            }
        }
        ofstream out_dict_file(dictonary_file);
        for (auto& t : mapping) {
            out_dict_file << t.first << " "  << t.second << "\n";
        }
        //     term         currendID        Total-in-ID [list of locations]
        // map<string, pair<int, vector<pair<int, vector<int>>>>>
        for (auto& t : postings_map) {
            for (int i = 0; i < t.second.second.size(); i++) {
                posts_out_file << t.second.second[i].first << " ";
                posts_out_file << t.second.second[i].second.size() << " ";
                for (int j = 0; j < t.second.second[i].second.size(); j++) {
                    if (j != t.second.second[i].second.size() - 1) {
                        posts_out_file << t.second.second[i].second[j] << ",";
                    } else {
                        posts_out_file << t.second.second[i].second[j] << " +++ ";
                    }
                }
            }
            posts_out_file << endl;
        }
        posts_out_file.close();
        in_file.close();
        out_dict_file.close();
        std::cout << "Data received.\n";
    }
    return 0;
}

vector<string> delimit(string text) {
    char * pch;
    vector<string> result;
    pch = strtok ((char*)text.c_str(),"!@#$%^&*()_-+={}[]\\|:;,.<>/?`~\'\"");
    while (pch != NULL) {
        string tmp = pch;
        result.insert(result.end(), tmp);
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
        while (std::getline(in_file, line)) {
            stopwords[line] = 1;
        }
    }
}
