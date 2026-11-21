#ifndef WHATSAPP_H
#define WHATSAPP_H

#include <iostream>
#include <vector>
#include <string>
#include "User.h"
#include "Chat.h"
#include "PrivateChat.h"
#include "GroupChat.h"

using namespace std;

class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;

public:
    WhatsApp() {
        currentUserIndex = -1;
    }

    ~WhatsApp() {
        for (size_t i = 0; i < chats.size(); i++) {
            delete chats[i];
        }
        chats.clear();
    }

    int findUserIndex(string username) const {
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i].getUsername() == username) {
                return (int)i;
            }
        }
        return -1;
    }

    bool isLoggedIn() const {
        return currentUserIndex != -1;
    }

    string getCurrentUsername() const {
        if (!isLoggedIn()) {
            return "";
        }
        return users[currentUserIndex].getUsername();
    }

    void signUp() {
        string uname, pass, phone;
        cout << "Username: ";
        cin >> uname;

        if (findUserIndex(uname) != -1) {
            cout << "Username already exists." << endl;
            return;
        }

        cout << "Password (min 6 chars): ";
        cin >> pass;

        if (pass.length() < 6) {
            cout << "Password must be at least 6 characters." << endl;
            return;
        }

        cout << "Phone: ";
        cin >> phone;

        User u(uname, pass, phone);
        users.push_back(u);
        cout << "Account created." << endl;
    }

    void login() {
        if (isLoggedIn()) {
            cout << "Already logged in." << endl;
            return;
        }

        string uname, pass;
        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pass;

        int idx = findUserIndex(uname);
        if (idx == -1) {
            cout << "User not found." << endl;
            return;
        }

        if (users[idx].checkPassword(pass)) {
            currentUserIndex = idx;
            users[idx].setStatus("Online");
            cout << "Welcome " << uname << "!" << endl;
        }
        else {
            cout << "Incorrect password." << endl;
        }
    }

    void startPrivateChat() {
        if (!isLoggedIn()) {
            cout << "Please login first." << endl;
            return;
        }

        string target;
        cout << "Chat with: ";
        cin >> target;

        if (target == getCurrentUsername()) {
            cout << "Cannot chat with yourself." << endl;
            return;
        }

        if (findUserIndex(target) == -1) {
            cout << "User not found." << endl;
            return;
        }

        for (size_t i = 0; i < chats.size(); i++) {
            vector<string> p = chats[i]->getParticipants();
            if (p.size() == 2 &&
                ((p[0] == getCurrentUsername() && p[1] == target) ||
                 (p[0] == target && p[1] == getCurrentUsername()))) {
                cout << "Private chat already exists." << endl;
                return;
            }
        }

        PrivateChat* c = new PrivateChat(getCurrentUsername(), target);
        chats.push_back(c);
        cout << "Private chat created." << endl;
    }

    void createGroup() {
        if (!isLoggedIn()) {
            cout << "Please login first." << endl;
            return;
        }

        string name;
        int n;
        cout << "Group name: ";
        cin.ignore();
        getline(cin, name);

        if (name.empty()) {
            cout << "Group name cannot be empty." << endl;
            return;
        }

        cout << "Number of members to add: ";
        cin >> n;

        vector<string> members;
        members.push_back(getCurrentUsername());

        for (int i = 0; i < n; i++) {
            string m;
            cout << "Member " << i + 1 << ": ";
            cin >> m;

            if (findUserIndex(m) == -1) {
                cout << m << " not found." << endl;
            }
            else if (m == getCurrentUsername()) {
                cout << "You are already added as the creator." << endl;
            }
            else {
                members.push_back(m);
            }
        }

        if (members.size() < 2) {
            cout << "Group must have at least 2 valid participants." << endl;
            return;
        }

        GroupChat* g = new GroupChat(members, name, getCurrentUsername());
        chats.push_back(g);
        cout << "Group created." << endl;
    }

    void viewChats() const {
        if (!isLoggedIn()) {
            cout << "Please login first." << endl;
            return;
        }

        string me = getCurrentUsername();
        bool found = false;

        for (size_t i = 0; i < chats.size(); i++) {
            vector<string> p = chats[i]->getParticipants();
            for (size_t j = 0; j < p.size(); j++) {
                if (p[j] == me) {
                    chats[i]->displayChat();
                    cout << endl;
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            cout << "No chats yet." << endl;
        }
    }

    void logout() {
        if (!isLoggedIn()) {
            cout << "Not logged in." << endl;
            return;
        }

        users[currentUserIndex].setStatus("Offline");
        cout << "Logged out." << endl;
        currentUserIndex = -1;
    }
};

#endif
