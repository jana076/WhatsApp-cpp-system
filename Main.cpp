#include <iostream>
#include <vector>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#include "WhatsApp.h"

using namespace std;

void runDemo() {
    cout << "\n--- System Demo ---\n" << endl;

    User u1("jana", "123456", "01000000001");
    User u2("sara", "123456", "01000000002");
    User u3("omar", "123456", "01000000003");
    u1.setStatus("Online");

    cout << u1.getUsername() << " (" << u1.getStatus() << ") - " << u1.getLastSeen() << endl;
    cout << u2.getUsername() << " (" << u2.getStatus() << ") - " << u2.getLastSeen() << "\n" << endl;

    vector<Chat*> chats;

    PrivateChat* p = new PrivateChat(u1.getUsername(), u2.getUsername());
    Message m1(u1.getUsername(), "Hey Sara, ready? :)");
    m1.addEmoji(":)");
    p->addMessage(m1);

    Message m2(u2.getUsername(), "Yes almost done <3");
    m2.addEmoji("<3");
    m2.setReplyTo(&m1);
    p->addMessage(m2);

    chats.push_back(p);

    vector<string> members;
    members.push_back(u1.getUsername());
    members.push_back(u2.getUsername());

    GroupChat* g = new GroupChat(members, "Team 5", u1.getUsername());
    g->setDescription("OOP Project");
    g->addAdmin(u2.getUsername());
    g->sendJoinRequest(u3.getUsername());

    Message gm(u1.getUsername(), "Welcome everyone! :D");
    gm.addEmoji(":D");
    g->addMessage(gm);

    chats.push_back(g);

    for (size_t i = 0; i < chats.size(); i++) {
        cout << "\nChat " << i + 1 << ":" << endl;
        chats[i]->displayChat();
    }

    cout << "\n--- Testing Search & Edge Cases ---" << endl;
    vector<Message> found = p->searchMessages("ready");
    cout << "Search 'ready' count: " << found.size() << endl;

    cout << "\nNon-admin (omar) trying to remove sara:" << endl;
    g->removeParticipant(u3.getUsername(), u2.getUsername());

    cout << "\nAdmin (jana) removing sara:" << endl;
    g->removeParticipant(u1.getUsername(), u2.getUsername());
    g->displayChat();

    for (size_t i = 0; i < chats.size(); i++) {
        delete chats[i];
    }
    chats.clear();
}

void runApp() {
    WhatsApp app;
    int ch;

    while (true) {
        cout << "\n--- Menu ---" << endl;
        if (app.isLoggedIn()) {
            cout << "User: " << app.getCurrentUsername() << endl;
            cout << "1. Start Private Chat" << endl;
            cout << "2. Create Group" << endl;
            cout << "3. View Chats" << endl;
            cout << "4. Logout" << endl;
            cout << "0. Back" << endl;
            cout << "Choice: ";
            cin >> ch;

            if (ch == 1) {
                app.startPrivateChat();
            }
            else if (ch == 2) {
                app.createGroup();
            }
            else if (ch == 3) {
                app.viewChats();
            }
            else if (ch == 4) {
                app.logout();
            }
            else if (ch == 0) {
                break;
            }
            else {
                cout << "Wrong choice." << endl;
            }
        }
        else {
            cout << "1. Sign Up" << endl;
            cout << "2. Login" << endl;
            cout << "0. Back" << endl;
            cout << "Choice: ";
            cin >> ch;

            if (ch == 1) {
                app.signUp();
            }
            else if (ch == 2) {
                app.login();
            }
            else if (ch == 0) {
                break;
            }
            else {
                cout << "Wrong choice." << endl;
            }
        }
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif

    int mode;

    while (true) {
        cout << "\n1. WhatsApp App" << endl;
        cout << "2. Run Demo" << endl;
        cout << "0. Exit" << endl;
        cout << "Choice: ";
        cin >> mode;

        if (mode == 1) {
            runApp();
        }
        else if (mode == 2) {
            runDemo();
        }
        else if (mode == 0) {
            break;
        }
        else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
