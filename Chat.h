#ifndef CHAT_H
#define CHAT_H

#include <iostream>
#include <vector>
#include <string>
#include "Message.h"
using namespace std;

class Chat {
protected:
    vector<string> participants;
    vector<Message> messages;
    string chatName;

public:
    Chat() {
        chatName = "";
    }

    Chat(vector<string> users, string name) {
        participants = users;
        chatName = name;
    }

    virtual ~Chat() {}

    vector<string> getParticipants() const {
        return participants;
    }

    vector<Message> getMessages() const {
        return messages;
    }

    string getChatName() const {
        return chatName;
    }

    void setChatName(const string& name) {
        chatName = name;
    }

    void addMessage(const Message& msg) {
        messages.push_back(msg);
    }

    bool deleteMessage(int index, const string& username) {
        if (index < 0 || index >= (int)messages.size()) {
            return false;
        }
        if (messages[index].getSender() != username) {
            return false;
        }

        for (size_t i = 0; i < messages.size(); i++) {
            if (messages[i].getReplyTo() != nullptr &&
                messages[i].getReplyTo()->getContent() == messages[index].getContent() &&
                messages[i].getReplyTo()->getSender() == messages[index].getSender() &&
                messages[i].getReplyTo()->getTimestamp() == messages[index].getTimestamp()) {
                messages[i].setReplyTo(nullptr);
            }
        }

        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const {
        cout << "=== " << (chatName.empty() ? "Chat" : chatName) << " ===" << endl;
        if (messages.empty()) {
            cout << "No messages yet." << endl;
            return;
        }
        for (size_t i = 0; i < messages.size(); i++) {
            cout << "[" << i << "] ";
            messages[i].display();
        }
    }

    vector<Message> searchMessages(string keyword) const {
        vector<Message> results;
        if (keyword.empty()) {
            return results;
        }
        for (size_t i = 0; i < messages.size(); i++) {
            if (messages[i].getContent().find(keyword) != string::npos) {
                results.push_back(messages[i]);
            }
        }
        return results;
    }
};

#endif
