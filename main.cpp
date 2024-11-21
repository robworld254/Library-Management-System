#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <limits>

using namespace std;

// Function to clear the console screen
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


// Book class to represent a book in the library
class Book {
private:
    string bookID;
    string title;
    string author;
    string genre;
    bool isAvailable;

public:
    Book(string id, string t, string a, string g)
        : bookID(id), title(t), author(a), genre(g), isAvailable(true) {}

    string getBookID() const { return bookID; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getGenre() const { return genre; }
    bool getAvailability() const { return isAvailable; }
    void setAvailability(bool status) { isAvailable = status; }

    string toString() const {
        return bookID + "," + title + "," + author + "," + genre + "," + (isAvailable ? "Available" : "Borrowed");
    }
};

// Member class to represent a library member
class Member {
private:
    string memberID;
    string name;
    string address;
    string phoneNumber;

public:
    Member(string id, string n, string addr, string phone)
        : memberID(id), name(n), address(addr), phoneNumber(phone) {}

    string getMemberID() const { return memberID; }
    string getName() const { return name; }
    string getAddress() const { return address; }
    string getPhoneNumber() const { return phoneNumber; }

    string toString() const {
        return memberID + "," + name + "," + address + "," + phoneNumber;
    }
};

// Transaction class to represent a book borrowing transaction
class Transaction {
private:
    string transactionID;
    string memberID;
    string bookID;
    time_t borrowDate;
    time_t returnDate;
    time_t expectedReturnDate;

public:
    Transaction(string tID, string mID, string bID)
        : transactionID(tID), memberID(mID), bookID(bID) {
        borrowDate = time(nullptr);
        returnDate = 0; // 0 indicates the book hasn't been returned yet
        expectedReturnDate = borrowDate + (14 * 24 * 60 * 60); // 14 days from borrow date
    }

    string getTransactionID() const { return transactionID; }
    string getMemberID() const { return memberID; }
    string getBookID() const { return bookID; }
    time_t getBorrowDate() const { return borrowDate; }
    time_t getReturnDate() const { return returnDate; }
    time_t getExpectedReturnDate() const { return expectedReturnDate; }
    void setReturnDate(time_t date) { returnDate = date; }

    int calculateOverdueDays() const {
        time_t now = time(nullptr);
        if (returnDate == 0) {
            return max(0, static_cast<int>((now - expectedReturnDate) / (24 * 60 * 60)));
        }
        return max(0, static_cast<int>((returnDate - expectedReturnDate) / (24 * 60 * 60)));
    }

    double calculateOverdueFees() const {
        return calculateOverdueDays() * 100.0; // 100 KSH per day
    }

    string toString() const {
        stringstream ss;
        ss << transactionID << "," << memberID << "," << bookID << "," << borrowDate << "," << returnDate << "," << expectedReturnDate;
        return ss.str();
    }

    string getFormattedDate(time_t date) const {
        char buffer[26];
        struct tm* timeinfo;
        timeinfo = localtime(&date);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
        return string(buffer);
    }

    string getFormattedBorrowDate() const {
        return getFormattedDate(borrowDate);
    }

    string getFormattedExpectedReturnDate() const {
        return getFormattedDate(expectedReturnDate);
    }
};

// Library class to manage books, members, and transactions
class Library {
private:
    vector<Book> books;
    vector<Member> members;
    vector<Transaction> transactions;

    // Load books from file
    void loadBooks() {
        ifstream file("books.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, title, author, genre, status;
            getline(ss, id, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, genre, ',');
            getline(ss, status, ',');
            Book book(id, title, author, genre);
            book.setAvailability(status == "Available");
            books.push_back(book);
        }
    }

    // Load members from file
    void loadMembers() {
        ifstream file("members.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, address, phone;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, address, ',');
            getline(ss, phone, ',');
            members.push_back(Member(id, name, address, phone));
        }
    }

    // Load transactions from file
    void loadTransactions() {
        ifstream file("transactions.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string tID, mID, bID, borrowDate, returnDate;
            getline(ss, tID, ',');
            getline(ss, mID, ',');
            getline(ss, bID, ',');
            getline(ss, borrowDate, ',');
            getline(ss, returnDate, ',');
            Transaction t(tID, mID, bID);
            t.setReturnDate(stoi(returnDate));
            transactions.push_back(t);
        }
    }

    // Save books to file
    void saveBooks() {
        ofstream file("books.txt");
        for (const auto& book : books) {
            file << book.toString() << endl;
        }
    }

    // Save members to file
    void saveMembers() {
        ofstream file("members.txt");
        for (const auto& member : members) {
            file << member.toString() << endl;
        }
    }

    // Save transactions to file
    void saveTransactions() {
        ofstream file("transactions.txt");
        for (const auto& transaction : transactions) {
            file << transaction.toString() << endl;
        }
    }

public:
    Library() {
        loadBooks();
        loadMembers();
        loadTransactions();
    }

    ~Library() {
        saveBooks();
        saveMembers();
        saveTransactions();
    }

    // Add a new book to the library
    void addBook(const Book& book) {
        if (findBook(book.getBookID()) != nullptr) {
            throw runtime_error("Book with this ID already exists.");
        }
        books.push_back(book);
        saveBooks();
    }

    // Edit an existing book's details
    void editBook(const string& bookID, const Book& updatedBook) {
        auto it = find_if(books.begin(), books.end(),
                          [&bookID](const Book& b) { return b.getBookID() == bookID; });
        if (it != books.end()) {
            *it = updatedBook;
            saveBooks();
        } else {
            throw runtime_error("Book not found.");
        }
    }

    // Delete a book from the library
    void deleteBook(const string& bookID) {
        size_t initialSize = books.size();
        books.erase(remove_if(books.begin(), books.end(),
                              [&bookID](const Book& b) { return b.getBookID() == bookID; }),
                    books.end());
        if (books.size() == initialSize) {
            throw runtime_error("Book not found.");
        }
        saveBooks();
    }

    // Get all books in the library
    vector<Book> getAllBooks() const {
        return books;
    }

    // Find a book by its ID
    Book* findBook(const string& bookID) {
        auto it = find_if(books.begin(), books.end(),
                          [&bookID](const Book& b) { return b.getBookID() == bookID; });
        return it != books.end() ? &(*it) : nullptr;
    }

    // Add a new member to the library
    void addMember(const Member& member) {
        if (findMember(member.getMemberID()) != nullptr) {
            throw runtime_error("Member with this ID already exists.");
        }
        members.push_back(member);
        saveMembers();
    }

    // Edit an existing member's details
    void editMember(const string& memberID, const Member& updatedMember) {
        auto it = find_if(members.begin(), members.end(),
                          [&memberID](const Member& m) { return m.getMemberID() == memberID; });
        if (it != members.end()) {
            *it = updatedMember;
            saveMembers();
        } else {
            throw runtime_error("Member not found.");
        }
    }

    // Delete a member from the library
    void deleteMember(const string& memberID) {
        size_t initialSize = members.size();
        members.erase(remove_if(members.begin(), members.end(),
                                [&memberID](const Member& m) { return m.getMemberID() == memberID; }),
                      members.end());
        if (members.size() == initialSize) {
            throw runtime_error("Member not found.");
        }
        saveMembers();
    }

    // Get all members of the library
    vector<Member> getAllMembers() const {
        return members;
    }

    // Find a member by their ID
    Member* findMember(const string& memberID) {
        auto it = find_if(members.begin(), members.end(),
                          [&memberID](const Member& m) { return m.getMemberID() == memberID; });
        return it != members.end() ? &(*it) : nullptr;
    }

    // Borrow a book
    string borrowBook(const string& memberID, const string& bookID) {
        Book* book = findBook(bookID);
        Member* member = findMember(memberID);
        if (!book) {
            throw runtime_error("Book not found.");
        }
        if (!member) {
            throw runtime_error("Member not found.");
        }
        if (!book->getAvailability()) {
            throw runtime_error("Book is not available for borrowing.");
        }
        book->setAvailability(false);
        string tID = to_string(transactions.size() + 1);
        transactions.push_back(Transaction(tID, memberID, bookID));
        saveBooks();
        saveTransactions();
        return tID;  // Return the transaction ID
    }

    // Return a borrowed book
    void returnBook(const string& bookIdentifier) {
        Book* book = nullptr;
        // Try to find the book by ID first, then by title
        book = findBook(bookIdentifier);
        if (!book) {
            auto books = searchBooks(bookIdentifier);
            if (!books.empty()) {
                book = &books[0];
            }
        }
        if (!book) {
            throw runtime_error("Book not found.");
        }

        auto it = find_if(transactions.rbegin(), transactions.rend(),
                          [&](const Transaction& t) { return t.getBookID() == book->getBookID() && t.getReturnDate() == 0; });
        if (it == transactions.rend()) {
            throw runtime_error("No active borrowing found for this book.");
        }

        book->setAvailability(true);
        it->setReturnDate(time(nullptr));
        saveBooks();
        saveTransactions();
    }

    // Get all transactions
    vector<Transaction> getAllTransactions() const {
        return transactions;
    }

    // Search for books by ID or title
    vector<Book> searchBooks(const string& query) const {
        vector<Book> results;
        for (const auto& book : books) {
            if (book.getBookID() == query || book.getTitle().find(query) != string::npos) {
                results.push_back(book);
            }
        }
        return results;
    }

    // Search for members by ID or name
    vector<Member> searchMembers(const string& query) const {
        vector<Member> results;
        for (const auto& member : members) {
            if (member.getMemberID() == query || member.getName().find(query) != string::npos) {
                results.push_back(member);
            }
        }
        return results;
    }

    // Get all available books
    vector<Book> getAvailableBooks() const {
        vector<Book> availableBooks;
        copy_if(books.begin(), books.end(), back_inserter(availableBooks),
                [](const Book& b) { return b.getAvailability(); });
        return availableBooks;
    }

    // Get all borrowed books with their transaction details
    vector<pair<Book, Transaction>> getBorrowedBooksWithTransactions() const {
        vector<pair<Book, Transaction>> borrowedBooks;
        for (const auto& book : books) {
            if (!book.getAvailability()) {
                auto it = find_if(transactions.rbegin(), transactions.rend(),
                                  [&](const Transaction& t) { return t.getBookID() == book.getBookID() && t.getReturnDate() == 0; });
                if (it != transactions.rend()) {
                    borrowedBooks.push_back(make_pair(book, *it));
                }
            }
        }
        return borrowedBooks;
    }


    // Get members with overdue books and their fees
    vector<pair<Member, double>> getOverdueMembers() const {
        vector<pair<Member, double>> overdueMembers;
        for (const auto& transaction : transactions) {
            if (transaction.getReturnDate() == 0 && transaction.calculateOverdueDays() > 14) {
                Member* member = const_cast<Library*>(this)->findMember(transaction.getMemberID());
                if (member) {
                    double overdueFee = transaction.calculateOverdueFees();
                    overdueMembers.push_back(make_pair(*member, overdueFee));
                }
            }
        }
        return overdueMembers;
    }
};

// Function to display the main menu
void displayMenu() {
    cout << "\nLibrary Management System\n";
    cout << "1. Add Book\n";
    cout << "2. Edit Book\n";
    cout << "3. Delete Book\n";
    cout << "4. View All Books\n";
    cout << "5. Add Member\n";
    cout << "6. Edit Member\n";
    cout << "7. Delete Member\n";
    cout << "8. View All Members\n";
    cout << "9. Borrow Book\n";
    cout << "10. Return Book\n";
    cout << "11. Search Books\n";
    cout << "12. Search Members\n";
    cout << "13. View Available Books\n";
    cout << "14. View Borrowed Books\n";
    cout << "15. View Overdue Members\n";
    cout << "16. Clear Screen\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}


int main() {
    Library library;
    int choice;

    do {
        displayMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        try {
            switch (choice) {
                case 1: {
                    string id, title, author, genre;
                    cout << "Enter Book ID: ";
                    getline(cin, id);
                    cout << "Enter Title: ";
                    getline(cin, title);
                    cout << "Enter Author: ";
                    getline(cin, author);
                    cout << "Enter Genre: ";
                    getline(cin, genre);
                    library.addBook(Book(id, title, author, genre));
                    cout << "Book added successfully.\n";
                    break;
                }
                case 2: {
                    string id, title, author, genre;
                    cout << "Enter Book ID to edit: ";
                    getline(cin, id);
                    cout << "Enter new Title: ";
                    getline(cin, title);
                    cout << "Enter new Author: ";
                    getline(cin, author);
                    cout << "Enter new Genre: ";
                    getline(cin, genre);
                    library.editBook(id, Book(id, title, author, genre));
                    cout << "Book edited successfully.\n";
                    break;
                }
                case 3: {
                    string id;
                    cout << "Enter Book ID to delete: ";
                    getline(cin, id);
                    library.deleteBook(id);
                    cout << "Book deleted successfully.\n";
                    break;
                }
                case 4: {
                    auto books = library.getAllBooks();
                    for (const auto& book : books) {
                        cout << book.toString() << endl;
                    }
                    break;
                }
                case 5: {
                    string id, name, address, phone;
                    cout << "Enter Member ID: ";
                    getline(cin, id);
                    cout << "Enter Name: ";
                    getline(cin, name);
                    cout << "Enter Address: ";
                    getline(cin, address);
                    cout << "Enter Phone Number: ";
                    getline(cin, phone);
                    library.addMember(Member(id, name, address, phone));
                    cout << "Member added successfully.\n";
                    break;
                }
                case 6: {
                    string id, name, address, phone;
                    cout << "Enter Member ID to edit: ";
                    getline(cin, id);
                    cout << "Enter new Name: ";
                    getline(cin, name);
                    cout << "Enter new Address: ";
                    getline(cin, address);
                    cout << "Enter new Phone Number: ";
                    getline(cin, phone);
                    library.editMember(id, Member(id, name, address, phone));
                    cout << "Member edited successfully.\n";
                    break;
                }
                case 7: {
                    string id;
                    cout << "Enter Member ID to delete: ";
                    getline(cin, id);
                    library.deleteMember(id);
                    cout << "Member deleted successfully.\n";
                    break;
                }
                case 8: {
                    auto members = library.getAllMembers();
                    for (const auto& member : members) {
                        cout << member.toString() << endl;
                    }
                    break;
                }
                case 9: {
                    string memberID, bookID;
                    cout << "Enter Member ID: ";
                    getline(cin, memberID);
                    cout << "Enter Book ID: ";
                    getline(cin, bookID);
                    string transactionID = library.borrowBook(memberID, bookID);
                    cout << "Book borrowed successfully. Transaction ID: " << transactionID << endl;
                    break;
                }
                case 10: {
                    string bookIdentifier;
                    cout << "Enter Book ID or Title to return: ";
                    getline(cin, bookIdentifier);
                    library.returnBook(bookIdentifier);
                    cout << "Book returned successfully.\n";
                    break;
                }
                case 11: {
                    string query;
                    cout << "Enter search query for books (ID or Title): ";
                    getline(cin, query);
                    auto results = library.searchBooks(query);
                    for (const auto& book : results) {
                        cout << book.toString() << endl;
                    }
                    break;
                }
                case 12: {
                    string query;
                    cout << "Enter search query for members (ID or Name): ";
                    getline(cin, query);
                    auto results = library.searchMembers(query);
                    for (const auto& member : results) {
                        cout << member.toString() << endl;
                    }
                    break;
                }
                case 13: {
                    auto availableBooks = library.getAvailableBooks();
                    for (const auto& book : availableBooks) {
                        cout << book.toString() << endl;
                    }
                    break;
                }
                case 14: {
                    auto borrowedBooks = library.getBorrowedBooksWithTransactions();
                    if (borrowedBooks.empty()) {
                        cout << "No books are currently borrowed." << endl;
                    } else {
                        cout << "Borrowed Books:" << endl;
                        for (const auto& pair : borrowedBooks) {
                            const Book& book = pair.first;
                            const Transaction& transaction = pair.second;
                            cout << "Book: " << book.getTitle() << " (ID: " << book.getBookID() << ")" << endl;
                            cout << "  Borrowed by Member ID: " << transaction.getMemberID() << endl;
                            cout << "  Transaction ID: " << transaction.getTransactionID() << endl;
                            cout << "  Borrow Date: " << transaction.getFormattedBorrowDate() << endl;
                            cout << "  Expected Return Date: " << transaction.getFormattedExpectedReturnDate() << endl;
                            cout << "  Days Overdue: " << transaction.calculateOverdueDays() << endl;
                            cout << "  Overdue Fee: KSH " << fixed << setprecision(2) << transaction.calculateOverdueFees() << endl;
                            cout << endl;
                        }
                    }
                    break;
                }
                case 15: {
                    auto overdueMembers = library.getOverdueMembers();
                    for (const auto& pair : overdueMembers) {
                        cout << pair.first.toString() << " | Overdue Fee: $" << fixed << setprecision(2) << pair.second << endl;
                    }
                    break;
                }
                case 16: {
                    clearScreen();
                    break;
                }
                case 0:
                    cout << "Exiting...\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
         } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }

        if (choice != 0 && choice != 16) {
            cout << "Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (choice != 0);

    return 0;
}



