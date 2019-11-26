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
bool PRINTING = false;

// --- INPUT --- //
string dictonary_file = "output/dictonary.txt";
string postings_lists_file = "output/postings.txt";
string titles_file = "output/titles.txt";
string authors_file = "output/authors.txt";
string stop_words_file = "data/stopwords.txt";
string page_rank = "output/pagerank.txt";

// variables
map<string, map<int, int>> document_frequency_table; // map<term, map<doc_id, term-freq>>
map<string, map<int, double>> document_weight_vectors; // map<term, map<doc_id, weight>>
map<int, map<string, double>> document_sorted_vectors; // map<doc_id, map<term, weight>>
map<string, double> idf_table; // map<term, idf-val>
int K = 10; // Used for Top-K Retrival
map<string, int> stopwords;
double THRESHOLD = 1; // Used for Index Elimination
// PageRank vars
map<int, vector<int>> links; // Document -> All documents that point to Document
map<int, vector<int>> points; // Document -> All documents Document points to
double damping_factor = 0.85; // probability of random link
double w1 = 0.5;
double w2 = 0.5;
map<int, float> PageRanks; // Document -> Value

vector<vector<float>> PR_array(3204, vector<float>(3204));
vector<float> temp_array(3204, 0);
vector<float> final_array(3204, 0);
// functions
vector<string> delimit(string text);
vector<string> split(string text);
double cosine_similarity(vector<double> v1, vector<double> v2, double dot_product);
double magnitude(vector<double> v);
double dot_product(vector<double> v1, vector<double> v2);
vector<double> weight_convert(map<string, double> m);
map<string, double> query_create_weights(string query);
void fill_stop_words();
string lower(string text);
map<double, int, greater <double>> search(string input);
void setup(int argc, char *argv[]);
void create_page_ranks();

void create_page_ranks() {
    ifstream pagerank_in_file(page_rank);
    string line = "";
    while ( getline(pagerank_in_file, line) ) {
        vector<string> items = split(line);
        if (items[0] != items[1]) {
            if (links.find(stoi(items[1])) != links.end()) {
                links[stoi(items[1])].insert(links[stoi(items[1])].end(), stoi(items[0]));
            } else {
                links[stoi(items[1])] = {stoi(items[0])};
            }
            if (points.find(stoi(items[0])) != points.end()) {
                points[stoi(items[0])].insert(points[stoi(items[0])].end(), stoi(items[1]));
            } else {
                points[stoi(items[0])] = {stoi(items[1])};
            }
        }
    }
    pagerank_in_file.close();
    for (int i = 1; i <= links.size(); i++) {
        links[i].erase(unique(links[i].begin(), links[i].end()), links[i].end());
        points[i].erase(unique(points[i].begin(), points[i].end()), points[i].end());
        PageRanks[i] = (float) 1 / 3204;
    }
    for (int i = 0; i < 100; i++) {
        for (int i = 1; i <= PageRanks.size(); i++) {
            vector<int> B = links[i];
            float sum = 0;
            for (auto a : B) {
                sum += (float) PageRanks[a] / points[a].size();
            }
            PageRanks[i] = sum;
        }
    }
    
    /*
    // matrix implementation
    for (int i = 0; i < PR_array.size(); i++) {
        vector<int> temp = points[i + 1];
        bool flag = false;
        for (int j = 0; j < PR_array[i].size(); j++) {
            for (auto a : temp) {
                if (a == j) {
                    PR_array[i][j] = 1;
                    flag = true;
                }
            }
        }
        if (!flag) {
            for (int j = 0; j < PR_array[i].size(); j++) {
                PR_array[i][j] = (float) 1 / 3204;
            }
            flag = false;
        }
    }
    vector<float> sums;
    for (int i = 0; i < PR_array.size(); i++) {
        float sum = 0;
        for (int j = 0; j < 3204; j++) {
            sum += PR_array[i][j];
        }
        sums.insert(sums.end(),sum);
    }
    for (int i = 0; i < PR_array.size(); i++) {
        for (int j = 0; j < 3204; j++) {
            PR_array[i][j] = PR_array[i][j] / sums[i];
        }
    }
    for (int i = 0; i < PR_array.size(); i++) {
        for (int j = 0; j < PR_array[i].size(); j++) {
            PR_array[i][j] *= (float) 0.85;
            PR_array[i][j] += (float) 0.15 / 3204;
        }
    }
    temp_array[0] = 1;
    for (int z = 0; z < 50; z++){
        vector<float> temp(3204, 0);
        final_array = temp;
        for (int i = 0; i < PR_array.size(); i++) {
            for (int j = 0; j < PR_array[i].size(); j++) {
                final_array[i] += (PR_array[i][j] * temp_array[j]);
            }
        }
        temp_array = final_array;
    }
    */
}

void setup(int argc, char *argv[]) {
    string input_dictonary = "";
    string input_postings = "";
    ifstream posts_in_file(postings_lists_file);
    ifstream dictonary_in_file(dictonary_file);
    fill_stop_words();
    if (*argv[1] == '0') {
        STOP_WORD_FLAG = false;
    }
    if (*argv[2] == '0') {
        STEMMING_FLAG = false;
    }
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

    for (auto i : document_frequency_table) {
        pair<string, double> p(i.first, log10(3204 / i.second.size()));
        idf_table.insert(p);
    }

    for (auto i : document_frequency_table) {
        map<int, double> temp;
        for (auto j : i.second) {
            pair<int, double> p(j.first, ((1 + log10(j.second)) * idf_table[i.first]));
            temp.insert(p);
            pair<string, double> p1(i.first, p.second);
            document_sorted_vectors[j.first].insert(p1);
        }
        pair<string, map<int, double>> p(i.first, temp);
        document_weight_vectors.insert(p);
    }
}

map<double, int, greater <double>> search(string input) {
    map<string, double> query = query_create_weights(input);
    map<double, int, greater <double>> relevant_documents;
    for (int i = 0; i < 3204; i++) {
        map<string, double> shortened_query;
        map<string, double> shortened_doc;
        map<string, double> doc = document_sorted_vectors[i];
        for (auto a : query) {
            if (doc.find(a.first) != doc.end() && idf_table[a.first] > THRESHOLD) {
                pair<string, double> p(a.first, doc[a.first]);
                shortened_doc.insert(p);
                pair<string, double> p1(a.first, a.second);
                shortened_query.insert(p1);
            }
        }
        if (shortened_doc.size() > 0) {
            double sim = cosine_similarity(weight_convert(doc), weight_convert(query), 
            dot_product(weight_convert(shortened_doc), weight_convert(shortened_query)));
            double final_score = w1 * sim + w2 * PageRanks[i + 1];
            pair<double, int> p(final_score, i);
            relevant_documents.insert(p);
        }
    }
    int count = 1;
    map<double, int, greater <double>> result;
    for (auto a : relevant_documents) {
        if (count <= K) {
            result[a.first] = a.second;
            string author = "";
            string title = "";
            ifstream titles_in_file(titles_file);
            string line;
            for (int k = 1; k <= a.second; k++) {
                std::getline(titles_in_file, line);
                if (k == a.second) {
                    vector<string> temp = split(line);
                    for (int m = 1; m < temp.size(); m++) {
                        title += temp[m] + " ";
                    }
                    break;
                }
            }
            titles_in_file.close();
            ifstream authors_in_file(authors_file);
            line = "";
            for (int k = 1; k <= a.second; k++) {
                std::getline(authors_in_file, line);
                if (k == a.second) {
                    vector<string> temp = split(line);
                    for (int m = 1; m < temp.size(); m++) {
                        author += temp[m] + " ";
                    }
                    break;
                }
            }
            authors_in_file.close();
            if (PRINTING) {
                cout << "Rank: " << count << " docID: " << a.second << " Score: " << a.first << " PageRank: " << (float) PageRanks[a.second] << endl;
                cout << "Title: " << title << endl;
                cout << "Author(s): " << author << endl;
            }
            count++;
        } else {
            break;
        }
    }
    return result;
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

bool stop_word_removal(string text) {
    if (stopwords[text] == 1) {
        return true;
    } else {
        return false;
    }
}

map<string, double> query_create_weights(string query) {
    map<string, double> results;
    map<string, int> query_tf;
    vector<string> query_temp = split(query);
    for (auto b : query_temp) {
        vector<string> query_split = delimit(lower(b));
        for (auto a : query_split) {
            if (STEMMING_FLAG) {
                Porter2Stemmer::stem(a);
            }
            if (STOP_WORD_FLAG) {
                if (!stop_word_removal(a)) {
                    if (query_tf.find(a) != query_tf.end()) {
                        query_tf[a] += 1;
                    } else {
                        query_tf[a] = 1;
                    }
                }
            } else {
                if (query_tf.find(a) != query_tf.end()) {
                    query_tf[a] += 1;
                } else {
                    query_tf[a] = 1;
                }
            }
        }
    }
    for (auto a : query_tf) {
        results[a.first] = (1 + log10(a.second)) * idf_table[a.first];
    }
    return results;
}

vector<double> weight_convert(map<string, double> m) {
    vector <double> result;
    for (auto a : m) {
        result.insert(result.end(), a.second);
    }
    return result;
}

double dot_product(vector<double> v1, vector<double> v2) {
    if (v1.size() != v2.size()) {
        cout << "Vectors are not the same size, v1: " << v1.size() << " v2: " << v2.size() << endl;
        return 0;
    } else {
        double result = 0;
        for (int i = 0; i < v1.size(); i++) {
            result += v1[i] * v2[i];
        }
        return result;
    }
}

double magnitude(vector<double> v) {
    double result;
    for (int i = 0; i < v.size(); i++) {
        result += pow(v[i], 2);
    }
    return sqrt(result);
}

double cosine_similarity(vector<double> v1, vector<double> v2, double dot_product) {
    return dot_product / (magnitude(v1) * magnitude(v2));
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
