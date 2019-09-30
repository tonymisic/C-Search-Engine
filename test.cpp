/*
    CPS842 Info Retrival 
    Tony Misic 500759917
    Thomas Liu 500717670
*/
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iterator>
#include <chrono>
#include <string.h>

using namespace std;

// --- INPUT --- //
string dictonary_file = "output/dictonary.txt";
string postings_lists_file = "output/postings.txt";
string titles_file = "output/titles.txt";

vector<string> split(string text);
void printQueryTime(std::chrono::duration<double> run_time);
void print_average_run_time();
vector<string> delimit(string text);

vector<double> run_times;

int main(int argc, char *argv[]) {
    string input = "";
    ifstream posts_in_file(postings_lists_file);
    ifstream dictonary_in_file(dictonary_file);
    ifstream titles_in_file(titles_file);

    while (true) {
        cout << "Please enter a query: ";
        cin >> input;
        if (input == "ZZEND") {
            ifstream posts_in_file(postings_lists_file);
            ifstream dictonary_in_file(dictonary_file);
            ifstream titles_in_file(titles_file);
            posts_in_file.close();
            dictonary_in_file.close();
            titles_in_file.close();
            break;            
        } else {
            auto start = chrono::system_clock::now(); // start time
            ifstream posts_in_file(postings_lists_file);
            ifstream dictonary_in_file(dictonary_file);
            ifstream titles_in_file(titles_file);
            
            string line = "";
            string title_line = "";
            bool FOUND = false;
            int count = 0;
            string ID = "";
            vector<string> next_ten;

            while (std::getline(dictonary_in_file, line)) {
                vector<string> temp = split(line);
                count++;
                if (input == temp[0]) {
                    cout << "Overall Frequency: " << temp[1] << endl;
                    FOUND = true;
                    // for (int i = 0; i < 10; i++) {
                    //     std::getline(dictonary_in_file, line);
                    //     vector<string> temp2 = split(line);
                    //     next_ten.insert(next_ten.end(), temp2[0]);
                    // }
                    break;
                }
            }
            if (!FOUND) {
                cout << "Term not found! Try again." << endl;                
            } else {
                for (int i = 1; i <= count; i++) {
                    std::getline(posts_in_file, line);
                    if (i == count) {
                        vector<string> temp3 = split(line);
                        printf("Doc ID | Freq. | Positions Document | Document Title\n");
                        for (int j = 0; j < temp3.size(); j++) {
                            if (j % 3 == 0) {
                                printf("%6s |",temp3[j].c_str());
                                ID = temp3[j];
                            }
                            if (j % 3 == 1) {
                                printf("%6s |",temp3[j].c_str());
                            }
                            if (j % 3 == 2) {
                                printf("%19s | ",temp3[j].c_str());
                                int t = atoi(ID.c_str());
                                for (int k = 1; k <= t; k++) {
                                    std::getline(titles_in_file, title_line);
                                    if (k == t) {
                                        vector<string> temp = split(title_line);
                                        for (int m = 1; m < temp.size(); m++) {
                                            printf("%s ", temp[m].c_str());
                                        }
                                        printf("\n");
                                        break;
                                    }
                                }
                            }
                        }
                       
                    }
                }
            }
            
            auto end = chrono::system_clock::now(); // end time
            chrono::duration<double> elapsed_seconds = end-start;
            run_times.insert(run_times.end(), elapsed_seconds.count());
            printQueryTime(elapsed_seconds);
        }
    }
    print_average_run_time();
    return 0;
}

void printQueryTime(std::chrono::duration<double> run_time) {
    std::cout << "Computation Time: " << run_time.count() << "s" << std::endl;
}

void print_average_run_time() {
    double sum = 0;
    for (int i = 0; i < run_times.size(); i++) {
        sum += run_times[i];
    }
    cout << "Average Run Time: " << sum / run_times.size() << endl;
}

vector<string> split(string text) {
    istringstream iss(text);
    vector<string> results((istream_iterator<std::string>(iss)),istream_iterator<std::string>());
    return results;
}

vector<string> delimit(string text) {
    char * pch;
    vector<string> result;
    pch = strtok ((char*)text.c_str(),">");
    while (pch != NULL) {
        string tmp = pch;
        result.insert(result.end(), tmp);
        pch = strtok(NULL, ">");
    }
    return result;
}