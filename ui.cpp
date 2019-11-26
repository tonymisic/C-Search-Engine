#include "search.cpp"

int main(int argc, char *argv[]) {
    create_page_ranks();
    setup(argc, argv);
    while (true) {
        string line;
        cout << "Please enter a query: ";
        getline(cin, line);
        if (line == "ZZEND") {
            break;            
        } else {
            PRINTING = true;
            search(line);
        }
    }
}