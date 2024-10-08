#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct List {
    List() = default;  

    void push_back(string const& s) {
        size_++;

        if (! tail) {
            tail = new Node{};
            tail->data = s;
            head = tail;
            return ;
        }

        auto new_tail = new Node{};
        new_tail->data = s;
        tail->next = new_tail;
        tail = new_tail;
    }

    string const& operator[](size_t idx) {
        Node* node = head;

        while (idx > 0) {
            node = node->next;
            idx--;
        }

        return node->data;
    }

    size_t size() const { return size_; }

    struct Node {
        std::string data;
        Node* next = nullptr;
    };

    Node* head = nullptr;
    Node* tail = nullptr;
    size_t size_ = 0;
};

List SplitByDelim(string const& in) {
    List out;

    size_t start = 0;
    bool found_start = false;
    for (size_t i=0; i<in.size(); i++) {
        if (in[i] == ' ' or in[i] == '\t' or in[i] == '\r' or in[i] == '\n') {
            if (found_start) {
                out.push_back(in.substr(start, i-start));
                found_start = false;
            }
            continue;
        }

        /// non whitespace char
        if (! found_start) {
            found_start = true;
            start = i;
        }
    }

    return out;
}

void Print(vector<string> const& vec) {
    cout << "size = " << vec.size() << endl;
    for (auto const& s : vec) {
        cout << s << "  ";
    }
    cout << endl;
}

void Dump(std::string const& file_name) {
    std::ifstream ifs{file_name};
    std::string line;

    if (! std::getline(ifs, line)) {
        return ;
    }
    
    while (std::getline(ifs, line)) {
        auto splitted = SplitByDelim(line);
        
        double time = stod(splitted[0]);
        double range = stod(splitted[1]);
        string status = splitted.size() == 6
            ? splitted[2]
            : splitted[2] + splitted[3];
        double b = splitted.size() == 6
            ? stod(splitted[3])
            : stod(splitted[4]);

        if (time >= 2.5 and time <= 6.5) {
            cout << time << "  " << range << "  " << status << "  " << b << endl;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "provide file name" << std::endl;
        return 1;
    }

    Dump(argv[1]);

    return 0;
}
