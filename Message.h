#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;
    Message* replyTo;

    static Message* copyMessage(const Message* msg) {
        if (msg == nullptr) {
            return nullptr;
        }
        Message* copy = new Message();
        copy->sender = msg->sender;
        copy->content = msg->content;
        copy->timestamp = msg->timestamp;
        copy->status = msg->status;
        copy->replyTo = nullptr;
        return copy;
    }

public:
    Message() {
        sender = "";
        content = "";
        timestamp = "";
        status = "";
        replyTo = nullptr;
    }

    Message(string sndr, string cntnt) {
        sender = sndr;
        content = cntnt;
        updateTimestamp();
        status = "sent";
        replyTo = nullptr;
    }

    Message(const Message& other) {
        sender = other.sender;
        content = other.content;
        timestamp = other.timestamp;
        status = other.status;
        replyTo = copyMessage(other.replyTo);
    }

    Message& operator= (const Message& other) {
        if (this != &other) {
            sender = other.sender;
            content = other.content;
            timestamp = other.timestamp;
            status = other.status;

            Message* newReplyTo = copyMessage(other.replyTo);
            delete replyTo;
            replyTo = newReplyTo;
        }
        return *this;
    }

    ~Message() {
        delete replyTo;
        replyTo = nullptr;
    }

    string getContent() const {
        return content;
    }

    string getSender() const {
        return sender;
    }

    string getTimestamp() const {
        return timestamp;
    }

    string getStatus() const {
        return status;
    }

    Message* getReplyTo() const {
        return replyTo;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void setReplyTo(Message* msg) {
        if (msg == replyTo) {
            return;
        }
        Message* newReplyTo = copyMessage(msg);
        delete replyTo;
        replyTo = newReplyTo;
    }

    void updateTimestamp() {
        time_t now = time(0);
        tm localT;
#ifdef _WIN32
        localtime_s(&localT, &now);
#else
        localtime_r(&now, &localT);
#endif
        char buffer[100];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localT);
        timestamp = buffer;
    }

    void display() const {
        cout << sender << " sent: " << content << endl;
        cout << timestamp << endl;
        cout << status << endl;

        if (replyTo != nullptr) {
            cout << "replying to: " << replyTo->getContent() << endl;
        }
    }

    void addEmoji(string emojiCode) {
        string emoji;

        if (emojiCode == ":)") {
            emoji = "😊";
        }
        else if (emojiCode == ":(") {
            emoji = "😢";
        }
        else if (emojiCode == ":D") {
            emoji = "😃";
        }
        else if (emojiCode == "<3") {
            emoji = "❤️";
        }
        else if (emojiCode == ":thumbsup:") {
            emoji = "👍";
        }
        else {
            return;
        }

        size_t position = content.find(emojiCode);

        while (position != string::npos) {
            content.replace(position, emojiCode.length(), emoji);
            position = content.find(emojiCode, position + emoji.length());
        }
    }
};

#endif
