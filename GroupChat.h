#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <iostream>
#include <vector>
#include <string>
#include "Chat.h"

using namespace std;

class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;
    vector<string> joinRequests;

public:
    GroupChat(vector<string> users, string name, string creator)
        : Chat(users, name) {
        description = "";
        if (creator != "") {
            if (!isParticipant(creator)) {
                participants.push_back(creator);
            }
            admins.push_back(creator);
        }
    }

    bool isAdmin(string username) const {
        for (int i = 0; i < admins.size(); i++) {
            if (admins[i] == username) {
                return true;
            }
        }
        return false;
    }

    bool isParticipant(string username) const {
        for (int i = 0; i < participants.size(); i++) {
            if (participants[i] == username) {
                return true;
            }
        }
        return false;
    }

    void addAdmin(string newAdmin) {
        if (!isParticipant(newAdmin)) {
            cout << newAdmin << " is not in this group." << endl;
            return;
        }
        if (isAdmin(newAdmin)) {
            cout << newAdmin << " is already an admin." << endl;
            return;
        }
        admins.push_back(newAdmin);
    }

    bool removeParticipant(string admin, string userToRemove) {
        if (!isAdmin(admin)) {
            cout << "Only admins can remove members." << endl;
            return false;
        }
        if (!isParticipant(userToRemove)) {
            cout << "User not found in group." << endl;
            return false;
        }
        if (isAdmin(userToRemove) && admins.size() == 1) {
            cout << "Cannot remove the last admin." << endl;
            return false;
        }

        for (int i = 0; i < participants.size(); i++) {
            if (participants[i] == userToRemove) {
                participants.erase(participants.begin() + i);
                break;
            }
        }

        for (int i = 0; i < admins.size(); i++) {
            if (admins[i] == userToRemove) {
                admins.erase(admins.begin() + i);
                break;
            }
        }

        return true;
    }

    void setDescription(string desc) {
        description = desc;
    }

    void displayChat() const override {
        if (chatName == "") {
            cout << "=== Group: Unnamed ===" << endl;
        }
        else {
            cout << "=== Group: " << chatName << " ===" << endl;
        }

        if (description != "") {
            cout << "Description: " << description << endl;
        }

        cout << "Participants (" << participants.size() << "): ";
        for (int i = 0; i < participants.size(); i++) {
            cout << participants[i];
            if (i < participants.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;

        cout << "Admins (" << admins.size() << "): ";
        for (int i = 0; i < admins.size(); i++) {
            cout << admins[i];
            if (i < admins.size() - 1) {
                cout << ", ";
            }
        }
        cout<<endl;

        if (messages.size() == 0) {
            cout << "No messages yet." << endl;
            return;
        }

        for (int i = 0; i < messages.size(); i++) {
            cout << "[" << i << "] ";
            messages[i].display();
        }
    }

    void sendJoinRequest(string username) {
        if (isParticipant(username)) {
            cout << username << " is already in the group." << endl;
            return;
        }
        for (int i = 0; i < joinRequests.size(); i++) {
            if (joinRequests[i] == username) {
                cout << username << " already requested to join." << endl;
                return;
            }
        }
        joinRequests.push_back(username);
        cout << "Join request sent by " << username << "." << endl;
    }
};

#endif
