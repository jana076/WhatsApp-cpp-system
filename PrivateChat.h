#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <iostream>
#include <vector>
#include <string>
#include "Chat.h"
#include "Message.h"
using namespace std;

class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(string u1, string u2)
        : Chat({ u1, u2 }, "Chat between " + u1 + " and " + u2) {
        user1 = u1;
        user2 = u2;
    }

    void displayChat() const override {
        cout << "Chat Name: " << chatName << endl;
        if (messages.size() == 0) {
            cout << "No messages yet\n";
        }
        else {
            for (size_t i = 0; i < messages.size(); i++) {
                cout << "[" << i << "] ";
                messages[i].display();
            }
        }
    }

    void showTypingIndicator(const string& username) const {
        cout << username << " is typing...\n";
    }
};

#endif
