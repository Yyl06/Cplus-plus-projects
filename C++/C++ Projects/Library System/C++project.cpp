#include <iostream>
#include <string>
#include "Book.hpp"
#include <fstream>
#include <sstream>
#include "Library.hpp"
#include "Users.hpp"
using std::string, std::cout, std::cin;

void Library::saveToFile() {
    std::ofstream OutFile("Books.txt");
    for (const auto &Book : Shelf) {
        OutFile << Book->getTitle() << "," << Book->getAuthor() << "," << Book->statusBook() << ","
                << (dynamic_cast<ReferenceBook*>(Book) ? "Reference" : "Normal") << "\n";
    }
    OutFile.close();
}

void Library::loadFromFile() {
    std::ifstream InFile("Books.txt");
    string title, author, status, type;
    while (std::getline(InFile, title, ',') &&
        std::getline(InFile, author, ',') &&
        std::getline(InFile, status, ',') &&
        std::getline(InFile, type)) {
        if (type == "Reference") {
            Shelf.push_back(new ReferenceBook(title, author));
        } else {
            Shelf.push_back(new Book(title, author));
        }
        if (status == "1") {
            Shelf.back()->borrowBook();
        }
    }
    InFile.close();
}

void Library::saveUsersToFile(){
    std::ofstream OutFile("Users.txt");
    for (const auto &user : Users) {
        OutFile << user.getUsername() << ", " << user.getRoles();
        for (const auto &book : user.borrowedBooks) {
            OutFile << ", " << book.first << " " << book.second; // title, dueDate
        }
        OutFile << "\n";
    }
    OutFile.close();
}

void Library::loadUsersFromFile() {
    std::ifstream InFile("Users.txt");
    string username, role, line;
    while (std::getline(InFile, line)) {
        std::istringstream iss(line);
        std::getline(iss, username, ',');
        std::getline(iss, role, ',');

        User newUser(username, "password", role);
        string bookTitle;
        time_t dueDate;
        while (iss >> bookTitle >> dueDate) { 
            newUser.borrowedBooks[bookTitle] = dueDate;
        }
        Users.push_back(newUser);
    }
    InFile.close();
}

#include <iostream>
#include "Library.hpp"

using namespace std;

#include <iostream>
#include "Library.hpp"

using namespace std;

int main() {
    Library library;
    User* loggedInUser = nullptr; // Stores the logged-in user
    int choice;
    string title, author, username, password, role;

    while (true) {
        cout << "\nLibrary System:\n";
        cout << "1. Register User\n2. Login\n3. Add Book\n4. Add Reference Book\n5. Remove Book\n6. Search Book\n7. Borrow Book\n8. Return Book\n9. Display Books\n10. Logout\n11. Exit\nChoice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Enter a number.\n";
            continue;
        }
        cin.ignore(); // Clear newline from buffer

        switch (choice) {
            case 1: { // Register User
                cout << "Enter username: ";
                getline(cin, username);

                cout << "Enter password: ";
                getline(cin, password);

                cout << "Enter role (User/Librarian): ";
                getline(cin, role);

                if (role != "User" && role != "Librarian") {
                    cout << "Invalid role! Must be 'User' or 'Librarian'.\n";
                    break;
                }

                library.registerUser(username, password, role);
                cout << "User registered successfully!\n";
                break;
            }
            case 2: { // Login
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);

                loggedInUser = library.authenticateUser(username, password);
                if (loggedInUser) {
                    cout << "Login successful! Welcome, " << loggedInUser->getUsername() << ".\n";
                } else {
                    cout << "Invalid username or password.\n";
                }
                break;
            }
            case 3: // Add Book
            case 4: // Add Reference Book
            case 5: // Remove Book
                if (!loggedInUser || loggedInUser->getRole() != "Librarian") {
                    cout << "Access denied! Only librarians can manage books.\n";
                    break;
                }
                cout << "Enter title: ";
                getline(cin, title);
                if (choice == 3 || choice == 4) {
                    cout << "Enter author: ";
                    getline(cin, author);
                    library.Add(title, author, choice == 4); // True for ReferenceBook
                } else {
                    library.RemoveBook(title);
                }
                break;
            case 6: // Search Book
                cout << "Enter keyword to search: ";
                getline(cin, title);
                library.searchByKeyWord(title);
                break;
            case 7: // Borrow Book
            case 8: // Return Book
                if (!loggedInUser) {
                    cout << "Please log in first.\n";
                    break;
                }
                cout << "Enter book title: ";
                getline(cin, title);
                if (choice == 7)
                    library.borrowBook(*loggedInUser, title);
                else
                    library.returnBook(*loggedInUser, title);
                break;
            case 9: // Display Books
                library.Display();
                break;
            case 10: // Logout
                if (loggedInUser) {
                    cout << "Logged out successfully.\n";
                    loggedInUser = nullptr;
                } else {
                    cout << "You're not logged in.\n";
                }
                break;
            case 11: // Exit
                cout << "Exiting system...\n";
                return 0;
            default:
                cout << "Invalid choice!\n";
        }
    }
}
