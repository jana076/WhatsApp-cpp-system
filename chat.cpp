#include <iostream>
#include "Chat.h"
using namespace std;

struct SimpleChat : Chat {
    SimpleChat(vector<string> users, string name) : Chat(users, name) {}
    void displayChat() const override {
        cout << "=== " << chatName << " ===" << endl;
        for (Message* m : messages) {
            m->display();
        }
    }
};

int main() {
    SimpleChat chat({"Ali", "Sara"}, "Study Group");

    cout << "Search empty chat:" << endl;
    chat.searchMessages("hello");

    Message* m1 = new Message("Ali", "hello everyone");
    Message* m2 = new Message("Sara", "hi Ali");
    chat.addMessage(m1);
    chat.addMessage(m2);
    chat.addMessage(new Message("Omar", "spam"));

    cout << endl << "Messages in chat: " << chat.getMessageCount() << endl;
    chat.displayChat();

    cout << endl << "Search 'hello':" << endl;
    chat.searchMessages("hello");

    cout << endl << "Search 'pizza':" << endl;
    chat.searchMessages("pizza");

    cout << endl << "Sara tries to delete Ali's message:" << endl;
    chat.deleteMessage(m1, "Sara");

    cout << endl << "Ali deletes their own message:" << endl;
    chat.deleteMessage(m1, "Ali");

    cout << "Messages in chat: " << chat.getMessageCount() << endl;
    chat.displayChat();

    return 0;
}
