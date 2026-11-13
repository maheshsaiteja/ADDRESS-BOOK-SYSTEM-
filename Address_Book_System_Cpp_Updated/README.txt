ADDRESS BOOK SYSTEM - PROJECT 06
Updated C++ Web Application

FEATURES
1. Create account with name and password.
2. Sign in / sign out.
3. Each account has its own contacts.
4. Add, edit and delete contacts.
5. Search by name, phone, email, address or group.
6. Family / College / Friends / Work / Other groups.
7. Clicking a group opens only contacts in that group.
8. Favorites.
9. Profile page.
10. Change password.
11. File handling using users.txt and contacts.txt.
12. C++ structures, functions, vectors and string handling.
13. Input validation.
14. Responsive browser interface.

PROJECT STRUCTURE
Address_Book_System_Cpp_Updated/
  main.cpp          -> C++ local web server and website routes
  address_book.h    -> structures and AddressBook class declarations
  address_book.cpp  -> C++ data storage and operations
  README.txt

REQUIREMENTS
Windows + MinGW g++.

COMPILE
g++ main.cpp address_book.cpp -o AddressBook.exe -lws2_32

RUN
AddressBook.exe

Then open:
http://127.0.0.1:8080

DATA FILES
The program automatically creates:
users.txt
contacts.txt

IMPORTANT COLLEGE/VIVA POINT
The website is only the user interface. The core project logic is C++:
- User and Contact are structures.
- Contacts are stored in C++ vectors.
- File handling stores records.
- Add/search/edit/delete/favorite are C++ functions.
- Group filtering is performed by C++.
- The browser communicates with the C++ program through a local HTTP server.

SECURITY NOTE
This is an educational project. Passwords are stored as plain text in users.txt for simplicity. A real application should hash passwords and use a database/HTTPS.
