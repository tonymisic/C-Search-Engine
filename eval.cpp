/*
    CPS842 Info Retrival 
    Tony Misic 500759917
    Thomas Liu 500717670
*/
#include "search.cpp"
#include <chrono>

// --- INPUT --- //
string rel_file = "data/qrels.text";
string query_file = "data/query.text";

// variables
map<int, vector<int>> retrieved_map; // query_number -> [retrived documents]
map<int, vector<int>> relevant_map; // query_number -> [relevant documents]
vector<string> test_queries;

// functions
void printQueryTime(std::chrono::duration<double> run_time);
void fill_retrived(int query, map<double, int, greater <double>> result);
vector<string> delimit(string text);
void fill_relevant();
void fill_queries();
double AP(vector<int> relevant, vector<int> retrieved);
double r_precision(vector<int> relevant, vector<int> retrieved);
vector<int> intersection(vector<int> v1, vector<int> v2);

int main(int argc, char *argv[]) {
    create_page_ranks();
    setup(argc, argv);
    fill_relevant();
    fill_queries();
    cout << "Test 1 --- W1: " << w1 << " W2: " << w2 << endl;
    auto start = chrono::system_clock::now(); // start time
    for (int i = 0; i < test_queries.size(); i++) {
        fill_retrived(i + 1, search(test_queries[i]));
    }
    auto end = chrono::system_clock::now(); // end time
    printQueryTime(end-start);
    double sum_ap = 0;
    double sum_R = 0;
    for (int i = 1; i <= test_queries.size(); i++) {
        sum_ap += AP(relevant_map[i], retrieved_map[i]);
        sum_R += r_precision(relevant_map[i], retrieved_map[i]);
    }
    cout << "MAP: " << (double) sum_ap / test_queries.size() << endl;
    cout << "Average r-Precision: " << (double) sum_R / test_queries.size() << endl;

    w1 = 0.7; w2 = 0.3;
    cout << "Test 2 --- W1: " << w1 << " W2: " << w2 << endl;
    start = chrono::system_clock::now(); // start time
    for (int i = 0; i < test_queries.size(); i++) {
        fill_retrived(i + 1, search(test_queries[i]));
    }
    end = chrono::system_clock::now(); // end time
    printQueryTime(end-start);
    sum_ap = 0;
    sum_R = 0;
    for (int i = 1; i <= test_queries.size(); i++) {
        sum_ap += AP(relevant_map[i], retrieved_map[i]);
        sum_R += r_precision(relevant_map[i], retrieved_map[i]);
    }
    cout << "MAP: " << (double) sum_ap / test_queries.size() << endl;
    cout << "Average r-Precision: " << (double) sum_R / test_queries.size() << endl;

    w1 = 0.2; w2 = 0.8;
    cout << "Test 3 --- W1: " << w1 << " W2: " << w2 << endl;
    start = chrono::system_clock::now(); // start time
    for (int i = 0; i < test_queries.size(); i++) {
        fill_retrived(i + 1, search(test_queries[i]));
    }
    end = chrono::system_clock::now(); // end time
    printQueryTime(end-start);
    sum_ap = 0;
    sum_R = 0;
    for (int i = 1; i <= test_queries.size(); i++) {
        sum_ap += AP(relevant_map[i], retrieved_map[i]);
        sum_R += r_precision(relevant_map[i], retrieved_map[i]);
    }
    cout << "MAP: " << (double) sum_ap / test_queries.size() << endl;
    cout << "Average r-Precision: " << (double) sum_R / test_queries.size() << endl;

    return 0;
}

double AP(vector<int> relevant, vector<int> retrieved) {
    int A = relevant.size();
    int ret_count = 0; int rel_count = 0;
    double temp = 0;
    for (int i = 0; i < retrieved.size(); i++) {
        ret_count++;
        for (int j = 0; j < relevant.size(); j++) {
            if (retrieved[i] == relevant[j]) {
                rel_count++;
                temp += (double) rel_count / ret_count;
            }
        }
    }
    if (A != 0) {
        return (double) temp / A;
    } else {
        return 0;
    }
}

double r_precision(vector<int> relevant, vector<int> retrieved) {
    int R = retrieved.size();
    int r = intersection(relevant, retrieved).size();
    return (double) r / R;
}

vector<int> intersection(vector<int> v1, vector<int> v2) {
    if (v1.empty() || v2.empty()) {
        return {};
    }
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());
    vector<int> result;
    set_intersection(v1.begin(),v1.end(), v2.begin(),v2.end(), back_inserter(result));
    return result;
}

void printQueryTime(std::chrono::duration<double> run_time) {
    std::cout << "Computation Time: " << run_time.count() << "s" << std::endl;
}

void fill_relevant() {
    ifstream rel_in_file(rel_file); string line;
    while (getline(rel_in_file, line)) {
        vector<string> input = split(line);
        int current_query = atoi(input[0].c_str());
        int document = atoi(input[1].c_str());
        if (relevant_map.find(current_query) == relevant_map.end()) {
            relevant_map[current_query] = {document};
        } else {
            relevant_map[current_query].insert(relevant_map[current_query].end(), document);
        }
    }
    rel_in_file.close();
    for (int i = 1; i <= test_queries.size(); i++) {
        if (relevant_map.find(i) == relevant_map.end()) {
            relevant_map[i] = {};
        }
    }
    
}

void fill_queries() {
    ifstream query_in_file(query_file); string line;
    while (getline(query_in_file, line)) {
        if (line == ".W") {
            string current_query = "";
            getline(query_in_file, line);
            while (line != ".A" && line != ".N" && line != ".I") {
                current_query += line;
                getline(query_in_file, line);
            }
            test_queries.insert(test_queries.end(), current_query);
        }
        
    }
    query_in_file.close();
}

void fill_retrived(int query, map<double, int, greater <double>> result) {
    for (auto a : result) {
        if (retrieved_map[query].size() == 0) {
            retrieved_map[query] = {a.second};
        } else {
            retrieved_map[query].insert(retrieved_map[query].end(), a.second);
        }
    }
    for (int i = 1; i <= test_queries.size(); i++) {
        if (retrieved_map[i].size() == 0) {
            retrieved_map[i] = {};
        }
    }
}


