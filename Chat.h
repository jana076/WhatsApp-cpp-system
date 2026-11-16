#ifndef CHAT_H
#define CHAT_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include "message_class.h"
using namespace std;

class Chat {

    protected:
        vector<Message*> messages;
        vector<string> participants;
        string chatName;

        bool isParticipant(const string& user) const {
            return find(participants.begin(), participants.end(), user) != participants.end();
        }

        bool ownsMessage(const Message* message) const {
            return find(messages.begin(), messages.end(), message) != messages.end();
        }

    public:
        Chat() {}

        Chat(const vector<string>& users, const string& name = "") {
            for (size_t i = 0; i < users.size(); i++) {
                addParticipant(users[i]);
            }
            chatName = name;
        }

        virtual ~Chat() {
            for (size_t i = 0; i < messages.size(); i++) {
                delete messages[i];
            }
            messages.clear();
        }

        Chat(const Chat&) = delete;
        Chat& operator=(const Chat&) = delete;

        bool addParticipant(const string& user) {
            if (user.empty() || isParticipant(user)) {
                return false;
            }
            participants.push_back(user);
            return true;
        }

        bool removeParticipant(const string& user) {
            vector<string>::iterator it = find(participants.begin(), participants.end(), user);
            if (it == participants.end()) {
                return false;
            }
            participants.erase(it);
            return true;
        }

        vector<string> getParticipants() const {
            return participants;
        }

        bool hasParticipant(const string& user) const {
            return isParticipant(user);
        }

        string getChatName() const {
            return chatName;
        }

        void setChatName(const string& name) {
            chatName = name;
        }

        bool hasChatName() const {
            return !chatName.empty();
        }

        vector<Message*> getMessages() const {
            return messages;
        }

        size_t getMessageCount() const {
            return messages.size();
        }

        bool isEmpty() const {
            return messages.empty();
        }

        bool addMessage(Message* message) {
            if (message == nullptr) {
                return false;
            }
            if (ownsMessage(message)) {
                return false;
            }
            if (!isParticipant(message->getSender())) {
                cout << "Error: " << message->getSender()
                     << " is not a participant in this chat." << endl;
                delete message;
                return false;
            }
            messages.push_back(message);
            return true;
        }

        bool deleteMessage(Message* message, const string& requester) {
            if (message == nullptr) {
                return false;
            }

            vector<Message*>::iterator it = find(messages.begin(), messages.end(), message);
            if (it == messages.end()) {
                cout << "Error: message not found in this chat." << endl;
                return false;
            }

            if (message->getSender() != requester) {
                cout << "Error: " << requester
                     << " cannot delete a message sent by " << message->getSender() << "." << endl;
                return false;
            }

            for (size_t i = 0; i < messages.size(); i++) {
                if (messages[i]->getReplyTo() == message) {
                    messages[i]->setReplyTo(nullptr);
                }
            }

            messages.erase(it);
            delete message;
            return true;
        }

        bool deleteMessage(size_t index, const string& requester) {
            if (index >= messages.size()) {
                cout << "Error: no message at index " << index << "." << endl;
                return false;
            }
            return deleteMessage(messages[index], requester);
        }

        vector<Message*> searchMessages(const string& keyword) const {
            vector<Message*> results;

            if (messages.empty()) {
                cout << "Chat is empty, nothing to search." << endl;
                return results;
            }
            if (keyword.empty()) {
                cout << "Search keyword is empty." << endl;
                return results;
            }

            for (size_t i = 0; i < messages.size(); i++) {
                if (messages[i]->getContent().find(keyword) != string::npos) {
                    results.push_back(messages[i]);
                }
            }

            if (results.empty()) {
                cout << "No messages found containing \"" << keyword << "\"." << endl;
            } else {
                cout << results.size() << " message(s) found containing \"" << keyword << "\":" << endl;
                for (size_t i = 0; i < results.size(); i++) {
                    results[i]->display();
                }
            }
            return results;
        }

        virtual void displayChat() const = 0;
};

#endif
