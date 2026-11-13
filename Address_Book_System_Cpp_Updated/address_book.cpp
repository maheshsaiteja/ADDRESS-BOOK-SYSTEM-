#include "address_book.h"
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

static vector<string> split(const string& line, char delimiter) {
    vector<string> parts;
    string item;
    stringstream ss(line);
    while (getline(ss, item, delimiter)) parts.push_back(item);
    return parts;
}

static string clean(const string& s) {
    string r = s;
    for (char& c : r) if (c == '|') c = '/';
    return r;
}

AddressBook::AddressBook() {
    loadUsers();
    loadContacts();
}

void AddressBook::loadUsers() {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        auto p = split(line, '|');
        if (p.size() >= 2) users.push_back({p[0], p[1]});
    }
}

void AddressBook::loadContacts() {
    ifstream file("contacts.txt");
    string line;
    while (getline(file, line)) {
        auto p = split(line, '|');
        if (p.size() >= 8) {
            try {
                Contact c;
                c.id = stoi(p[0]);
                c.owner = p[1];
                c.name = p[2];
                c.phone = p[3];
                c.email = p[4];
                c.address = p[5];
                c.group = p[6];
                c.favorite = (p[7] == "1");
                contacts.push_back(c);
                nextId = max(nextId, c.id + 1);
            } catch (...) {}
        }
    }
}

void AddressBook::saveUsers() const {
    ofstream file("users.txt");
    for (const auto& u : users) file << clean(u.username) << "|" << clean(u.password) << "\n";
}

void AddressBook::saveContacts() const {
    ofstream file("contacts.txt");
    for (const auto& c : contacts) {
        file << c.id << "|" << clean(c.owner) << "|" << clean(c.name) << "|"
             << clean(c.phone) << "|" << clean(c.email) << "|" << clean(c.address)
             << "|" << clean(c.group) << "|" << (c.favorite ? "1" : "0") << "\n";
    }
}

bool AddressBook::registerUser(const string& username, const string& password, string& message) {
    if (username.size() < 3) { message = "Name must contain at least 3 characters."; return false; }
    if (password.size() < 4) { message = "Password must contain at least 4 characters."; return false; }

    for (const auto& u : users) {
        if (u.username == username) {
            message = "That account name already exists.";
            return false;
        }
    }
    users.push_back({username, password});
    saveUsers();
    message = "Account created successfully.";
    return true;
}

bool AddressBook::login(const string& username, const string& password) const {
    for (const auto& u : users)
        if (u.username == username && u.password == password) return true;
    return false;
}

bool AddressBook::changePassword(const string& username, const string& oldPassword,
                                 const string& newPassword, string& message) {
    if (newPassword.size() < 4) { message = "New password must contain at least 4 characters."; return false; }
    for (auto& u : users) {
        if (u.username == username) {
            if (u.password != oldPassword) { message = "Old password is incorrect."; return false; }
            u.password = newPassword;
            saveUsers();
            message = "Password changed successfully.";
            return true;
        }
    }
    message = "User not found.";
    return false;
}

bool AddressBook::addContact(Contact contact, string& message) {
    if (contact.owner.empty() || contact.name.empty() || contact.phone.empty()) {
        message = "Name and phone number are required.";
        return false;
    }
    if (contact.phone.size() < 7 || contact.phone.size() > 15) {
        message = "Phone number must contain 7 to 15 characters.";
        return false;
    }
    contact.id = nextId++;
    contact.owner = clean(contact.owner);
    contact.name = clean(contact.name);
    contact.phone = clean(contact.phone);
    contact.email = clean(contact.email);
    contact.address = clean(contact.address);
    contact.group = clean(contact.group);
    contacts.push_back(contact);
    saveContacts();
    message = "Contact added successfully.";
    return true;
}

bool AddressBook::updateContact(int id, const string& owner, Contact updated, string& message) {
    for (auto& c : contacts) {
        if (c.id == id && c.owner == owner) {
            if (updated.name.empty() || updated.phone.empty()) {
                message = "Name and phone number are required.";
                return false;
            }
            c.name = clean(updated.name);
            c.phone = clean(updated.phone);
            c.email = clean(updated.email);
            c.address = clean(updated.address);
            c.group = clean(updated.group);
            saveContacts();
            message = "Contact updated successfully.";
            return true;
        }
    }
    message = "Contact not found.";
    return false;
}

bool AddressBook::deleteContact(int id, const string& owner, string& message) {
    auto oldSize = contacts.size();
    contacts.erase(remove_if(contacts.begin(), contacts.end(),
        [&](const Contact& c){ return c.id == id && c.owner == owner; }), contacts.end());

    if (contacts.size() == oldSize) { message = "Contact not found."; return false; }
    saveContacts();
    message = "Contact deleted successfully.";
    return true;
}

bool AddressBook::toggleFavorite(int id, const string& owner, string& message) {
    for (auto& c : contacts) {
        if (c.id == id && c.owner == owner) {
            c.favorite = !c.favorite;
            saveContacts();
            message = c.favorite ? "Added to favorites." : "Removed from favorites.";
            return true;
        }
    }
    message = "Contact not found.";
    return false;
}

vector<Contact> AddressBook::getContacts(const string& owner) const {
    vector<Contact> result;
    for (const auto& c : contacts) if (c.owner == owner) result.push_back(c);
    return result;
}

vector<Contact> AddressBook::getGroup(const string& owner, const string& group) const {
    vector<Contact> result;
    for (const auto& c : contacts)
        if (c.owner == owner && c.group == group) result.push_back(c);
    return result;
}

vector<Contact> AddressBook::getFavorites(const string& owner) const {
    vector<Contact> result;
    for (const auto& c : contacts) if (c.owner == owner && c.favorite) result.push_back(c);
    return result;
}

const Contact* AddressBook::findContact(int id, const string& owner) const {
    for (const auto& c : contacts)
        if (c.id == id && c.owner == owner) return &c;
    return nullptr;
}
