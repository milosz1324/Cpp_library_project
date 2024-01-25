#include <iostream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <unordered_map>

#include "RandomNameGenerator.hpp"
#include "RandomBookGenerator.hpp"

using namespace std;



const vector<string> baseTitles = {
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

const vector<string> baseAuthors = {
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


// funkcja print date z stackoverlowa do drukowania dat w przyjaznym formacie :)
void print_date() {
    std::time_t current_time = std::time(nullptr);
    std::tm* now = std::localtime(&current_time);

    std::cout << std::put_time(now, "%d-%m-%Y %H:%M:%S") << std::endl;
}


class LibraryItem {
    public:
        LibraryItem(const std::string new_title) : title(std::move(new_title)), available(true) {};

        const std::string& getTitle() const {
            return title;
        }

        bool isAvailable() const {
            return available;
        }

        void bookStatus(bool status) {
            available = status;
        }

        virtual double calculateFee(time_t deadlineDate) const {
            return 0.0;      //metoda wirtualna, domyslna kara,  do override :) 
        }

        virtual ~LibraryItem() = default; // wirtualny destruktor bo nie chcemy wyciekow pamieci

    private:
        string title;
        bool available;
};

class Book : public LibraryItem {
    public:
        Book(const std::string& new_title, const std::string& new_author) : LibraryItem(std::move(new_title)), author(new_author) {};

        const std::string& getAuthor() const {
            return author;
        }

    private:
        std::string author;

};

class DelayFee : public LibraryItem {
    
    public:
        DelayFee(std::string new_title, double new_fee) : LibraryItem(std::move(new_title)), fee(new_fee) {};
        
        // ustalenie deadlineDate zostalo przeniesione do classy LibraryInterface
        // tam za pomoca metody borrowBook ustalamy czas na jaki wypozczyamy ksiazke
        double calculateFee(time_t deadlineDate) const override {
            //funkcje z <ctime> 
            // time(nullptr) podaje czas z systemu, ale go nie przelicza
            time_t currentDate = time(nullptr);
            double timePeriod = difftime(currentDate, deadlineDate);
            double daysOfDelay = max(0.0, timePeriod / secondsinDay);
            // warunek sprawdzajac czy naliczac kare
            if (daysOfDelay > 0) {
                return daysOfDelay * fee;
            }       

            return 0.0; // brak opoznieina
        }

    private:
        double const secondsinDay = 24 * 60 * 60;
        double fee;
};

class Reader {
    public:
        Reader() : ReaderName(get_random_name()), FinalLateFee(0.0), ReaderID(get_next_id()) {};

        int getID() const {
            return ReaderID;
        }

        const string& getName() const {
            return ReaderName;
        }

        void borrowItem(LibraryItem& item, time_t deadlineDate) {
            cout << "The book: " << item.getTitle() << "has been borrowed by: " << ReaderName << " till: " << ctime(&deadlineDate) << endl;
            borrowedItems[&item] = deadlineDate;
            item.bookStatus(false);
        }

        void returnItem(LibraryItem& item) {

            // funkcja oblicza kare poprzez metode calcualteFee z klasy item
            // argumenetem funkcji jest wartosc z mapy czyli data zwrotu
            double LateFee = item.calculateFee(borrowedItems[&item]);
            FinalLateFee += LateFee;

            // erase usuwa nasz item z mapy oraz zmieniamy status ksiazki na dostepna
            borrowedItems.erase(&item);
            item.bookStatus(true);

            cout << "The reader: " << ReaderName << " has returned: " << item.getTitle() << "\t";
            cout << "Late fee: " << LateFee << endl;
        }

        double getFinalLateFee() const {
            return FinalLateFee;
        }

        // funkcja zwracajace poprzez referencje wypozyczony itemy (ksiazki)
        // uzycie referencji pozwala unikniecia kopiowania calego obiektu 
        // const pozwala na wywolanie listy itemow bez mozliwosci (niechcianej) zmiany mapy
        const unordered_map<LibraryItem*, time_t>& getBorrowedItems() const {
            return borrowedItems;
        }

    
    private:

        int ReaderID;
        string ReaderName;
        double FinalLateFee;
        //mapa borrowedItems, reprezentuje ona tablice klucz-wartosc gdzie mamy wypozyczony przedmiot oraz date zwrotu
        unordered_map<LibraryItem*, time_t> borrowedItems;

        static int get_next_id() {
            static int next_id = 1;
            return next_id++;
        }

};

class Library {
    public:
        
        void addItem(LibraryItem* item) {
            vectorOfItems.emplace_back(item);
        }

        LibraryItem* findItem(const string title) {
            for (auto& item : vectorOfItems) {
                if (item->getTitle() == title && item->isAvailable()) {
                    return item;
                }
            }
            return 0;   
        }

        void displayAvailableBooks() const {
            int bookNumber = 1;
            cout << "List of available books: " << endl;
            for (auto& item : vectorOfItems) {
                if (dynamic_cast<Book*>(item) && item->isAvailable()) {
                    cout << "\t" << bookNumber << ". " << item->getTitle() << " by " << dynamic_cast<Book*>(item)->getAuthor() << endl;
                    bookNumber++;
                }
            }
            cout << endl;
        }

        const vector<LibraryItem*>& getAllItems() const {
            return vectorOfItems;
        }

    private:
        vector<LibraryItem*> vectorOfItems;

};

class UserInterface {
    public:
        static void runAplication(Library& library, vector<Reader> readers) {
            
            while(true) {
                cout << "\nWitam w programie Biblioteka, oto lista komend:\n"
                        "1. Wyswietl dostepne ksiazki w ofercie\n"
                        "2. Wypozycz ksiazke\n"
                        "3. Zwroc ksiazke\n"
                        "4. Zamknij program\n"
                        "5. Wyswietl liste czytelnikow i ich ksiazek\n"
                        "Twoj wybor: ";
                int choice;
                cin >> choice;

                switch (choice) {
                    case 1:
                        library.displayAvailableBooks();
                        break;
                    case 2:
                        borrowBook(library, readers);
                        break;
                    case 3:
                        returnBook(library, readers);
                        break;
                    case 4:
                        return;
                    case 5:
                        displayBorrowedBooks(readers);
                        break;
                    default:
                        cout << "Nieprawidlowa operacja, sprobuj ponownie\n";
                        // Wyczyść bufor wejścia, aby uniknąć nieskończonej pętli z błędnym wejściem
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        break;  
                }
            }
    }

    private:
        static void displayBorrowedBooks(const vector<Reader>& readers) {
            cout << "\nList of readers and their books:\n";

            for (const Reader& reader : readers) {
                cout << "Reader's with ID " << reader.getID() << ":\n";
                const unordered_map<LibraryItem*, time_t>& borrowedItems = reader.getBorrowedItems();

                if (borrowedItems.empty()) {
                    cout << "The is no borrowed books\n";
                } else {
                    for (const auto& pair : borrowedItems) {
                        LibraryItem* item = pair.first;
                        time_t deadlineDate = pair.second;
                        cout << "  - " << item->getTitle() << " Return date: " << ctime(&deadlineDate) << "\n";
                    }
                    }
                cout << "\n";
            }
        }

        static void borrowBook(Library& library, vector<Reader>& readers) {
            int readerID;
            string bookTitle;
            int borrowTime;

            // czesc odpowiedzialna za wyszukiwanie czytelnika
            cout << "Write reader ID: ";
            cin >> readerID;

            //funkcja lambda do przeszukiwania listy czytelnikow
            auto it = find_if(readers.begin(), readers.end(),
                [readerID](const Reader& r) {
                    return r.getID() == readerID;
                }
            );

            if (it == readers.end()) {
                cout << "Reader not found" << endl;
                return;
            }

            // Sprawdzenie czy czytelnik już wypożyczył książkę
            if (!it->getBorrowedItems().empty()) {
                cout << "Reader with ID " << readerID << " has already borrowed a book. Please return it before borrowing another one" << endl;
                return;
            }

            // czesc odpowiedzalna za wyszukiwanie tytulu
            cout << "Write the number of the book you want to borrow: ";
            int bookNumber;
            cin >> bookNumber;
            
        // Sprawdzenie, czy numer książki jest w prawidłowym zakresie
            if (bookNumber >= 1 && bookNumber <= library.getAllItems().size()) {
                LibraryItem* item = library.getAllItems()[bookNumber - 1];
                if (item->isAvailable()) {
                    cout << "For how many days would you like to borrow this book? " << endl;
                    cin >> borrowTime;
                    time_t deadlineDate = time(nullptr) + borrowTime * 24 * 60 * 60;
                    it->borrowItem(*item, deadlineDate);
                    cout << "Reader with ID " << readerID << " has borrowed: " << item->getTitle() << " for " << borrowTime << " days" << endl;
                    } else {
                        cout << "This book is not available now, try later" << endl;
                    }
                } else {
                    cout << "Invalid book number" << endl;
                }
        }

        
        static void returnBook(Library& library, vector<Reader>& readers) {
            int readerID;

            // Część odpowiedzialna za wyszukiwanie czytelnika
            cout << "Write reader ID: ";
            cin >> readerID;

            auto it = find_if(readers.begin(), readers.end(),
                [readerID](const Reader& r) {
                    return r.getID() == readerID;
                }
            );

            if (it == readers.end()) {
                cout << "Reader not found." << endl;
                return;
            }

            // Zwróć pierwszą dostępną wypożyczoną książkę danego czytelnika
            const unordered_map<LibraryItem*, time_t>& borrowedItems = it->getBorrowedItems();

            if (borrowedItems.empty()) {
                cout << "No borrowed books to return for this reader." << endl;
                return;
            }

            // Automatyczne zwracanie pierwszej wypożyczonej książki
            auto iter = borrowedItems.begin();
            LibraryItem* item = iter->first;

            it->returnItem(*item);
            cout << "Reader with ID " << readerID << " has returned: " << item->getTitle() << endl;

        }
};


int main() {

    cout << "debug message\n" << endl; 

    // Przykładowa inicjalizacja czytelników
    std::vector<Reader> readers;
    for (int i = 0; i < 5; ++i) {
        readers.emplace_back();
    }

    // Wyświetl informacje o aktualnych czytelnikach
    std::cout << "Current Readers:\n";
    for (const auto& reader : readers) {
        std::cout << "Reader's ID: " << reader.getID() << " Reader's name: " << reader.getName() << "\n";
    }

    // DelayFee book1(get_random_name(), 2.0);

    // // ARGUMENT JAKO aktualny czsa - 3 dni wiec 3*2 zl per day czyli 6 zl fee ;)
    // cout << "Amount of fee for delay: " << book1.calculateFee(std::time(nullptr) - 3*24*60*60) << " $" << endl;
    // cout << "\n\n";

    // Reader czytelnik1;
    // czytelnik1.borrowItem(book1, std::time(nullptr) + 3*24*60*60);
    // czytelnik1.returnItem(book1);

    Library biblioteka;

    vector<pair<string, string>> books;
    for (size_t i = 0; i < baseTitles.size(); ++i) {
        books.push_back(make_pair(baseTitles[i], baseAuthors[i]));
    }

    // Dodanie książek do biblioteki z wykorzystaniem gotowej bazy
    for (size_t i = 0; i < baseTitles.size(); ++i) {
        biblioteka.addItem(new Book(baseTitles[i], baseAuthors[i])); // Zakładam, że wszystkie książki są dostępne
    }

    biblioteka.addItem(new DelayFee("Late Fee for Overdue Book", 1.50));  // Kary za zwrot książki po terminie

    UserInterface::runAplication(biblioteka, readers);

    // Zwolnienie pamięci
    for (auto& item : biblioteka.getAllItems()) {
        delete item;
    }

    return 0;
}

