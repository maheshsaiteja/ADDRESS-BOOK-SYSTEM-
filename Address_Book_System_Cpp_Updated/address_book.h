#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H

#include <string>
#include <vector>

struct User {
    std::string username;
    std::string password;
};

struct Contact {
    int id{};
    std::string owner;
    std::string name;
    std::string phone;
    std::string email;
    std::string address;
    std::string group;
    bool favorite{false};
};

class AddressBook {
private:
    std::vector<User> users;
    std::vector<Contact> contacts;
    int nextId = 1;

    void loadUsers();
    void loadContacts();
    void saveUsers() const;
    void saveContacts() const;

public:
    AddressBook();

    bool registerUser(const std::string& username, const std::string& password, std::string& message);
    bool login(const std::string& username, const std::string& password) const;
    bool changePassword(const std::string& username, const std::string& oldPassword,
                        const std::string& newPassword, std::string& message);

    bool addContact(Contact contact, std::string& message);
    bool updateContact(int id, const std::string& owner, Contact updated, std::string& message);
    bool deleteContact(int id, const std::string& owner, std::string& message);
    bool toggleFavorite(int id, const std::string& owner, std::string& message);

    std::vector<Contact> getContacts(const std::string& owner) const;
    std::vector<Contact> getGroup(const std::string& owner, const std::string& group) const;
    std::vector<Contact> getFavorites(const std::string& owner) const;
    const Contact* findContact(int id, const std::string& owner) const;
};

#endif
