#include <array>
#include <random>
#include <string>
#include <sstream>

struct RandomGenerator{
    static std::mt19937 prng;
};

std::mt19937 RandomGenerator::prng = std::mt19937{std::random_device{}()};

constexpr std::array BaseTitles{
    "Introduction to C++",
    "Data Structures and Algorithms",
    "The C++ Programming Language",
    "Clean Code",
    "Design Patterns",
    "Algorithm Design Manual",
    "Effective Modern C++",
    "Refactoring",
    "Code Complete",
    "The Pragmatic Programmer",
    "Cracking the Coding Interview",
    "Head First Design Patterns",
    "Accelerated C++",
    "C++ Primer",
    "The Art of Computer Programming",
    "Domain-Driven Design",
    "Test-Driven Development",
    "Game Programming Patterns",
    "Programming Pearls",
    "Thinking in C++"
};

constexpr std::array Authors{
    "Bjarne Stroustrup",
    "Robert C. Martin",
    "Erich Gamma",
    "Jon Bentley",
    "Martin Fowler",
    "Steve McConnell",
    "Scott Meyers",
    "Kent Beck",
    "Steve McConnell",
    "Andrew Hunt",
    "Gayle Laakmann McDowell",
    "Eric Freeman",
    "Andrew Koenig",
    "Stanley B. Lippman",
    "Donald Knuth",
    "Eric Evans",
    "Kent Beck",
    "Robert Nystrom",
    "Jon Bentley",
    "Bruce Eckel"
};

std::string get_random_title(){
    std::uniform_int_distribution<std::size_t> fnd{0, BaseTitles.size() - 1};
    std::stringstream name;
    name << BaseTitles[fnd(RandomIntGenerator::prng)];
    return name.str();
}

std::string get_random_author(){
    std::uniform_int_distribution<std::size_t> lnd{0, Authors.size() - 1};
    std::stringstream name;
    name << Authors[lnd(RandomIntGenerator::prng)];
    return name.str();
}
