#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// =====================================================
// PROJECT 6 : ADDRESS BOOK SYSTEM
// =====================================================

// Structure for storing contact details
struct Contact
{
    string name;
    string phone;
    string email;
    string address;
    string group;
};

// =====================================================
// Convert string to lowercase
// =====================================================

string toLowerCase(string str)
{
    int i;

    for (i = 0; i < (int)str.length(); i++)
    {
        str[i] = (char)tolower((unsigned char)str[i]);
    }

    return str;
}

// =====================================================
// Check phone number
// =====================================================

bool validPhone(string phone)
{
    int i;

    if (phone.length() != 10)
    {
        return false;
    }

    for (i = 0; i < (int)phone.length(); i++)
    {
        if (!isdigit((unsigned char)phone[i]))
        {
            return false;
        }
    }

    return true;
}

// =====================================================
// Select Group
// =====================================================

string selectGroup()
{
    int choice;

    cout << "\n========== SELECT GROUP ==========\n";
    cout << "1. Family\n";
    cout << "2. Friends\n";
    cout << "3. College\n";
    cout << "4. Work\n";
    cout << "==================================\n";

    cout << "Enter group choice: ";
    cin >> choice;

    switch (choice)
    {
        case 1:
            return "Family";

        case 2:
            return "Friends";

        case 3:
            return "College";

        case 4:
            return "Work";

        default:
            cout << "\nInvalid group choice!\n";
            cout << "Group set to Other.\n";
            return "Other";
    }
}

// =====================================================
// Add Contact
// =====================================================

void addContact(vector<Contact> &contacts)
{
    Contact c;

    cout << "\n====================================\n";
    cout << "           ADD CONTACT\n";
    cout << "====================================\n";

    cout << "Enter Name    : ";
    getline(cin >> ws, c.name);

    cout << "Enter Phone   : ";
    getline(cin, c.phone);

    if (!validPhone(c.phone))
    {
        cout << "\nInvalid phone number!\n";
        cout << "Phone number must contain exactly 10 digits.\n";
        return;
    }

    cout << "Enter Email   : ";
    getline(cin, c.email);

    cout << "Enter Address : ";
    getline(cin, c.address);

    // Show group options
    c.group = selectGroup();

    contacts.push_back(c);

    cout << "\nContact added successfully!\n";
    cout << "Group : " << c.group << endl;
}

// =====================================================
// Display One Contact
// =====================================================

void displayContact(Contact c, int number)
{
    cout << "\n------------------------------------\n";
    cout << "Contact " << number << endl;
    cout << "------------------------------------\n";

    cout << "Name    : " << c.name << endl;
    cout << "Phone   : " << c.phone << endl;
    cout << "Email   : " << c.email << endl;
    cout << "Address : " << c.address << endl;
    cout << "Group   : " << c.group << endl;

    cout << "------------------------------------\n";
}

// =====================================================
// Display All Contacts
// =====================================================

void displayContacts(vector<Contact> &contacts)
{
    int i;

    cout << "\n====================================\n";
    cout << "          ALL CONTACTS\n";
    cout << "====================================\n";

    if (contacts.size() == 0)
    {
        cout << "No contacts available.\n";
        return;
    }

    for (i = 0; i < (int)contacts.size(); i++)
    {
        displayContact(contacts[i], i + 1);
    }
}

// =====================================================
// Search Contact
// =====================================================

void searchContact(vector<Contact> &contacts)
{
    string search;
    string name;
    string phone;
    string email;
    string group;

    bool found = false;
    int count = 0;
    int i;

    if (contacts.size() == 0)
    {
        cout << "\nNo contacts available.\n";
        return;
    }

    cout << "\n====================================\n";
    cout << "          SEARCH CONTACT\n";
    cout << "====================================\n";

    cout << "Enter name, phone, email or group: ";
    getline(cin >> ws, search);

    search = toLowerCase(search);

    for (i = 0; i < (int)contacts.size(); i++)
    {
        name = toLowerCase(contacts[i].name);
        phone = toLowerCase(contacts[i].phone);
        email = toLowerCase(contacts[i].email);
        group = toLowerCase(contacts[i].group);

        if (name.find(search) != string::npos ||
            phone.find(search) != string::npos ||
            email.find(search) != string::npos ||
            group.find(search) != string::npos)
        {
            count++;

            displayContact(contacts[i], count);

            found = true;
        }
    }

    if (!found)
    {
        cout << "\nContact not found.\n";
    }
}

// =====================================================
// Delete Contact
// =====================================================

void deleteContact(vector<Contact> &contacts)
{
    string phone;
    int i;

    if (contacts.size() == 0)
    {
        cout << "\nNo contacts available.\n";
        return;
    }

    cout << "\n====================================\n";
    cout << "          DELETE CONTACT\n";
    cout << "====================================\n";

    cout << "Enter phone number: ";
    getline(cin >> ws, phone);

    for (i = 0; i < (int)contacts.size(); i++)
    {
        if (contacts[i].phone == phone)
        {
            cout << "\nContact found: "
                 << contacts[i].name << endl;

            contacts.erase(contacts.begin() + i);

            cout << "Contact deleted successfully!\n";

            return;
        }
    }

    cout << "\nContact not found.\n";
}

// =====================================================
// Display Contacts by Group
// =====================================================

void displayGroup(vector<Contact> &contacts)
{
    string selectedGroup;
    string currentGroup;

    bool found = false;
    int count = 0;
    int i;

    if (contacts.size() == 0)
    {
        cout << "\nNo contacts available.\n";
        return;
    }

    cout << "\n====================================\n";
    cout << "        VIEW GROUP CONTACTS\n";
    cout << "====================================\n";

    // Show group options
    selectedGroup = selectGroup();

    for (i = 0; i < (int)contacts.size(); i++)
    {
        currentGroup = toLowerCase(contacts[i].group);

        if (currentGroup == toLowerCase(selectedGroup))
        {
            count++;

            displayContact(contacts[i], count);

            found = true;
        }
    }

    if (!found)
    {
        cout << "\nNo contacts found in "
             << selectedGroup << " group.\n";
    }
}

// =====================================================
// Save Contacts
// =====================================================

void saveContacts(vector<Contact> &contacts)
{
    ofstream file;
    int i;

    file.open("contacts.txt");

    if (!file)
    {
        cout << "\nError opening file!\n";
        return;
    }

    for (i = 0; i < (int)contacts.size(); i++)
    {
        file << contacts[i].name << "|";
        file << contacts[i].phone << "|";
        file << contacts[i].email << "|";
        file << contacts[i].address << "|";
        file << contacts[i].group << endl;
    }

    file.close();

    cout << "\nContacts saved successfully!\n";
}

// =====================================================
// Load Contacts
// =====================================================

void loadContacts(vector<Contact> &contacts)
{
    ifstream file;
    Contact c;

    file.open("contacts.txt");

    if (!file)
    {
        return;
    }

    while (getline(file, c.name, '|'))
    {
        if (!getline(file, c.phone, '|'))
            break;

        if (!getline(file, c.email, '|'))
            break;

        if (!getline(file, c.address, '|'))
            break;

        if (!getline(file, c.group))
            break;

        contacts.push_back(c);
    }

    file.close();
}

// =====================================================
// Main Menu
// =====================================================

void showMenu()
{
    cout << "\n";
    cout << "============================================\n";
    cout << "            ADDRESS BOOK SYSTEM\n";
    cout << "============================================\n";
    cout << "1. Add Contact\n";
    cout << "2. Display All Contacts\n";
    cout << "3. Search Contact\n";
    cout << "4. Delete Contact\n";
    cout << "5. Display Group Contacts\n";
    cout << "6. Save Contacts\n";
    cout << "7. Exit\n";
    cout << "============================================\n";
    cout << "Enter your choice: ";
}

// =====================================================
// Main Function
// =====================================================

int main()
{
    vector<Contact> contacts;

    int choice;

    // Load saved contacts
    loadContacts(contacts);

    do
    {
        showMenu();

        cin >> choice;

        switch (choice)
        {
            case 1:
                addContact(contacts);
                break;

            case 2:
                displayContacts(contacts);
                break;

            case 3:
                searchContact(contacts);
                break;

            case 4:
                deleteContact(contacts);
                break;

            case 5:
                displayGroup(contacts);
                break;

            case 6:
                saveContacts(contacts);
                break;

            case 7:
                saveContacts(contacts);

                cout << "\n====================================\n";
                cout << " Thank you for using Address Book!\n";
                cout << "====================================\n";

                break;

            default:
                cout << "\nInvalid choice!\n";
                cout << "Please enter a number from 1 to 7.\n";
        }

    } while (choice != 7);

     return 0;
}

