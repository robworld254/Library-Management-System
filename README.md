# Library Management System

## Overview
This Library Management System is a C++ console application designed for librarians to efficiently manage books, members, and borrowing transactions. It offers a range of functionalities including book and member management, borrowing and returning books, searching capabilities, and report generation.

## Features
1. Book Management
   - Add new books
   - Edit existing books
   - Delete books
   - View all books
   - Search for books by ID or title

2. Member Management
   - Add new members
   - Edit existing members
   - Delete members
   - View all members
   - Search for members by ID or name

3. Transaction Management
   - Borrow books
   - Return books
   - View borrowed books with detailed transaction information
   - Calculate and display overdue fees
   - Show expected return dates for borrowed books

4. Reporting
   - View available books
   - View borrowed books with transaction details
   - View overdue members and their fees

5. Data Persistence
   - All data (books, members, transactions) is saved to and loaded from text files

## New Features
- Overdue fee calculation: 100 KSH per day for each day a book is overdue
- Display of expected return date for borrowed books
- Improved transaction details, including borrow date, expected return date, days overdue, and overdue fees

## File Structure
- `main.cpp`: Main source code file
- `books.txt`: Stores book data
- `members.txt`: Stores member data
- `transactions.txt`: Stores transaction data
- `README.md`: This file, containing project documentation

## Compilation and Execution
To compile the program, use a C++ compiler that supports C++11 or later. For example, using g++:

