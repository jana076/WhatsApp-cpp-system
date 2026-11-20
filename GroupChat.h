#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Chat.h"

using namespace std;

class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;
    vector<string> joinRequests;

public:
    GroupChat(vector<string> users, string name, string creator)
        : Chat(users, name), description("") {
        if (!creator.empty()) {
            if (!isParticipant(creator)) {
                participants.push_back(creator);
            }
            admins.push_back(creator);
        }
    }

    void addAdmin(string newAdmin) {
        if (!isParticipant(newAdmin)) {
            cout << "Error: " << newAdmin << " is not a participant in this group.\n";
            return;
        }
        if (isAdmin(newAdmin)) {
            cout << newAdmin << " is already an admin.\n";
            return;
        }
        admins.push_back(newAdmin);
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        if (!isAdmin(admin)) {
            cout << "Access denied: Only admins can remove participants.\n";
            return false;
        }
        if (!isParticipant(userToRemove)) {
            cout << "Error: User is not in this group.\n";
            return false;
        }
        if (isAdmin(userToRemove) && admins.size() == 1) {
            cout << "Error: Cannot remove the only remaining admin.\n";
            return false;
        }

        participants.erase(
            remove(participants.begin(), participants.end(), userToRemove),
            participants.end()
        );

        admins.erase(
            remove(admins.begin(), admins.end(), userToRemove),
            admins.end()
        );

        return true;
    }

    bool isAdmin(string username) const {
        for (size_t i = 0; i < admins.size(); i++) {
            if (admins[i] == username) {
                return true;
            }
        }
        return false;
    }

    bool isParticipant(string username) const {
        for (size_t i = 0; i < participants.size(); i++) {
            if (participants[i] == username) {
                return true;
            }
        }
        return false;
    }

    void setDescription(string desc) {
        description = desc;
    }

    void displayChat() const override {
        cout << "=== Group: " << (chatName.empty() ? "Unnamed Group" : chatName) << " ===\n";
        if (!description.empty()) {
            cout << "Description: " << description << "\n";
        }

        cout << "Participants (" << participants.size() << "): ";
        for (size_t i = 0; i < participants.size(); i++) {
            cout << participants[i] << (i + 1 < participants.size() ? ", " : "");
        }
        cout << "\n";

        cout << "Admins (" << admins.size() << "): ";
        for (size_t i = 0; i < admins.size(); i++) {
            cout << admins[i] << (i + 1 < admins.size() ? ", " : "");
        }
        cout << "\n----------------------------------------\n";

        if (messages.empty()) {
            cout << "No messages yet.\n";
            return;
        }

        for (size_t i = 0; i < messages.size(); i++) {
            cout << "[" << i << "] ";
            messages[i].display();
        }
    }

    void sendJoinRequest(const string& username) {
        if (isParticipant(username)) {
            cout << username << " is already a member of this group.\n";
            return;
        }
        for (size_t i = 0; i < joinRequests.size(); i++) {
            if (joinRequests[i] == username) {
                cout << username << " already sent a join request.\n";
                return;
            }
        }
        joinRequests.push_back(username);
        cout << "Join request sent by " << username << ".\n";
    }
};

#endif
