#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

class User {
private:
    string username;
    string password;
    string phoneNumber;
    string status;
    string lastSeen;

public:
    User() {
        username = "";
        password = "";
        phoneNumber = "";
        status = "Available";
        updateLastSeen();
    }

    User(string uname, string pwd, string phone) {
        username = uname;
        password = pwd;
        phoneNumber = phone;
        status = "Available";
        updateLastSeen();
    }

    string getUsername() const {
        return username;
    }

    string getPhoneNumber() const {
        return phoneNumber;
    }

    string getStatus() const {
        return status;
    }

    string getLastSeen() const {
        return lastSeen;
    }

    void setStatus(string newStatus) {
        status = newStatus;
        updateLastSeen();
    }

    void setPhoneNumber(string phone) {
        phoneNumber = phone;
        updateLastSeen();
    }

    void updateLastSeen() {
        time_t now = time(0);
        char* dt = ctime(&now);
        string timeStr(dt);

        if (!timeStr.empty() && timeStr.back() == '\n') {
            timeStr.pop_back();
        }

        lastSeen = timeStr;
    }

    bool checkPassword(string pwd) const {
        return (pwd.length() >= 6 && password == pwd);
    }

    void changePassword(string newPwd) {
        if (newPwd.length() >= 6) {
            password = newPwd;
            updateLastSeen();
        }
        else {
            cout << "Password must be at least 6 characters long.\n";
        }
    }
};

#endif
