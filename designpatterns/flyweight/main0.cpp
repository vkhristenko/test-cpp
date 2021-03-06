#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/bimap.hpp>
#include <boost/flyweight.hpp>

using namespace std;
using namespace boost;

using key = uint32_t;


struct User {
    User(std::string const& first_name, std::string const& last_name)
        : first_name{add(first_name)}, last_name{add(last_name)} 
    {}

    std::string const& get_first_name() const {
        return names.left.find(first_name)->second;
    }

    std::string const& get_last_name() const {
        return names.left.find(last_name)->second;
    }

    friend ostream& operator<<(ostream& os, User const& user ) {
        os << "first_name: " << user.get_first_name() << " last name: "
           << user.get_last_name() << "(" << user.last_name << "), " << "("
           << user.first_name << ")";
        return os;
    }
protected:
    key first_name, last_name;
    static bimap<key, std::string> names;
    static key seed;

    static key add(std::string const& s) {
        auto it = names.right.find(s);
        if (it == names.right.end()) {
            key id = ++seed;
            names.insert({seed, s});
            return id;
        } 

        return it->second;
    }
};

key User::seed{0};
bimap<key, std::string> User::names{};

struct User2 {
    flyweight<std::string> first_name, last_name;

    User2(std::string const& first_name, 
          std::string const& last_name) 
        : first_name(first_name), last_name(last_name) {
        
    }
};

int main() {
    User user1{"john", "doe"};
    User user2{"john", "smith"};

    std::cout << user1 << std::endl << user2 << std::endl;
    
    User2 first {"john", "smith"};
    User2 second {"jane", "smith"};

    std::cout << first.first_name << std::endl;
    std::cout << std::boolalpha;
    std::cout << (&first.first_name.get() == &second.first_name.get()) << std::endl;
    std::cout << (&first.last_name.get() == &second.last_name.get()) << std::endl;

    return 0;
}
