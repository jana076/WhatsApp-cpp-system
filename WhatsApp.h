#ifndef WHATSAPP_MODULE_H
#define WHATSAPP_MODULE_H

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "User.h"
#include "Message.h"
#include "Chat.h"
#include "PrivateChat.h"
#include "GroupChat.h"

namespace whatsapp_detail {

inline void discardMenuLineEnding() {
    const int next = std::cin.peek();
    if (next == '\r') {
        std::cin.get();
        if (std::cin.peek() == '\n') {
            std::cin.get();
        }
    } else if (next == '\n') {
        std::cin.get();
    }
}

inline std::string trim(const std::string& value) {
    std::string::size_type first = 0;
    while (first < value.size() &&
           std::isspace(static_cast<unsigned char>(value[first]))) {
        ++first;
    }

    std::string::size_type last = value.size();
    while (last > first &&
           std::isspace(static_cast<unsigned char>(value[last - 1]))) {
        --last;
    }
    return value.substr(first, last - first);
}

inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    if (!std::getline(std::cin, value)) {
        std::cin.clear();
        return std::string();
    }
    return trim(value);
}

inline std::string readPassword(const std::string& prompt) {
    std::cout << prompt;

#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        std::string password;
        int character = 0;
        while ((character = _getch()) != '\r' && character != '\n') {
            if (character == 8) {
                if (!password.empty()) {
                    password.erase(password.size() - 1);
                }
            } else if (character == 0 || character == 224) {
                (void)_getch();
            } else {
                password.push_back(static_cast<char>(character));
            }
        }
        std::cout << '\n';
        return password;
    }
#else
    if (isatty(STDIN_FILENO)) {
        struct termios originalSettings;
        if (tcgetattr(STDIN_FILENO, &originalSettings) == 0) {
            struct termios hiddenSettings = originalSettings;
            hiddenSettings.c_lflag &= static_cast<tcflag_t>(~ECHO);
            if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &hiddenSettings) == 0) {
                std::string password;
                int character = 0;
                while ((character = std::getchar()) != '\n' &&
                       character != '\r' && character != EOF) {
                    if (character == 127 || character == 8) {
                        if (!password.empty()) {
                            password.erase(password.size() - 1);
                        }
                    } else {
                        password.push_back(static_cast<char>(character));
                    }
                }
                tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalSettings);
                std::cout << '\n';
                return password;
            }
        }
        std::cout << "Unable to read a password securely on this terminal.\n";
        return std::string();
    }
#endif

    // Redirected input is used by the test driver and is never echoed by this code.
    std::string password;
    if (!std::getline(std::cin, password)) {
        std::cin.clear();
        return std::string();
    }
    return password;
}

inline std::vector<std::string> parseParticipants(const std::string& input,
                                                   bool& valid) {
    std::vector<std::string> participants;
    std::stringstream parser(input);
    std::string item;
    valid = true;

    while (std::getline(parser, item, ',')) {
        item = trim(item);
        if (item.empty() ||
            std::find(participants.begin(), participants.end(), item) !=
                participants.end()) {
            valid = false;
            return std::vector<std::string>();
        }
        participants.push_back(item);
    }

    if (input.empty() || input[input.size() - 1] == ',') {
        valid = false;
    }
    return participants;
}

inline bool containsParticipant(const Chat* chat,
                                const std::string& username) {
    if (chat == nullptr) {
        return false;
    }
    const std::vector<std::string> participants = chat->getParticipants();
    return std::find(participants.begin(), participants.end(), username) !=
           participants.end();
}

} // namespace whatsapp_detail

class WhatsApp {
private:
    std::vector<User> users;
    std::vector<Chat*> chats;
    int currentUserIndex;

    int findUserIndex(std::string username) const {
        for (std::size_t i = 0; i < users.size(); ++i) {
            if (users[i].getUsername() == username) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool isLoggedIn() const {
        return currentUserIndex >= 0 &&
               static_cast<std::size_t>(currentUserIndex) < users.size();
    }

    std::string getCurrentUsername() const {
        if (!isLoggedIn()) {
            return std::string();
        }
        return users[static_cast<std::size_t>(currentUserIndex)].getUsername();
    }

    Chat* findAccessibleChat(int chatIndex) const {
        if (!isLoggedIn() || chatIndex < 0 ||
            static_cast<std::size_t>(chatIndex) >= chats.size()) {
            return nullptr;
        }
        Chat* chat = chats[static_cast<std::size_t>(chatIndex)];
        if (!whatsapp_detail::containsParticipant(chat, getCurrentUsername())) {
            return nullptr;
        }
        return chat;
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    ~WhatsApp() {
        for (std::size_t i = 0; i < chats.size(); ++i) {
            delete chats[i];
        }
    }

    WhatsApp(const WhatsApp&) = delete;
    WhatsApp& operator=(const WhatsApp&) = delete;

    // Jana's menu calls these operations. This class does not own menu navigation.
    void signUp() {
        whatsapp_detail::discardMenuLineEnding();
        const std::string username = whatsapp_detail::readLine("Username: ");
        const std::string password = whatsapp_detail::readPassword("Password: ");
        const std::string phone = whatsapp_detail::readLine("Phone number: ");

        if (username.empty() || phone.empty()) {
            std::cout << "Username and phone number are required.\n";
            return;
        }
        if (findUserIndex(username) != -1) {
            std::cout << "That username is already registered.\n";
            return;
        }
        if (password.length() < 6) {
            std::cout << "Password must be at least 6 characters long.\n";
            return;
        }

        users.push_back(User(username, password, phone));
        std::cout << "Account created successfully.\n";
    }

    void login() {
        if (isLoggedIn()) {
            std::cout << "A user is already logged in. Please log out first.\n";
            return;
        }

        whatsapp_detail::discardMenuLineEnding();
        const std::string username = whatsapp_detail::readLine("Username: ");
        const std::string password = whatsapp_detail::readPassword("Password: ");
        const int userIndex = findUserIndex(username);
        if (userIndex == -1 ||
            !users[static_cast<std::size_t>(userIndex)].checkPassword(password)) {
            std::cout << "Invalid username or password.\n";
            return;
        }

        currentUserIndex = userIndex;
        users[static_cast<std::size_t>(currentUserIndex)].updateLastSeen();
        std::cout << "Login successful. Welcome, " << username << ".\n";
    }

    void startPrivateChat() {
        if (!isLoggedIn()) {
            std::cout << "Please log in before opening a chat.\n";
            return;
        }

        whatsapp_detail::discardMenuLineEnding();
        const std::string currentUsername = getCurrentUsername();
        const std::string otherUsername =
            whatsapp_detail::readLine("Chat with username: ");
        const int otherUserIndex = findUserIndex(otherUsername);

        if (otherUserIndex == -1) {
            std::cout << "No registered user has that username.\n";
            return;
        }
        if (otherUsername == currentUsername) {
            std::cout << "You cannot start a private chat with yourself.\n";
            return;
        }

        for (std::size_t i = 0; i < chats.size(); ++i) {
            if (chats[i] == nullptr ||
                dynamic_cast<PrivateChat*>(chats[i]) == nullptr) {
                continue;
            }
            const std::vector<std::string> participants =
                chats[i]->getParticipants();
            if (participants.size() == 2 &&
                whatsapp_detail::containsParticipant(chats[i], currentUsername) &&
                whatsapp_detail::containsParticipant(chats[i], otherUsername)) {
                std::cout << "Opening existing private chat.\n";
                chats[i]->displayChat();
                return;
            }
        }

        std::unique_ptr<Chat> chat(
            new PrivateChat(currentUsername, otherUsername));
        chats.push_back(chat.get());
        chat.release();
        std::cout << "Private chat created successfully.\n";
    }

    void createGroup() {
        if (!isLoggedIn()) {
            std::cout << "Please log in before creating a group.\n";
            return;
        }

        whatsapp_detail::discardMenuLineEnding();
        const std::string groupName =
            whatsapp_detail::readLine("Group name: ");
        const std::string participantInput = whatsapp_detail::readLine(
            "Other participant usernames, separated by commas: ");
        if (groupName.empty()) {
            std::cout << "A group name is required.\n";
            return;
        }

        bool validList = false;
        const std::vector<std::string> participants =
            whatsapp_detail::parseParticipants(participantInput, validList);
        if (!validList) {
            std::cout << "Enter each participant once, separated by commas.\n";
            return;
        }

        const std::string creator = getCurrentUsername();
        for (std::size_t i = 0; i < participants.size(); ++i) {
            if (participants[i] == creator) {
                std::cout << "The creator is added automatically; do not list yourself.\n";
                return;
            }
            if (findUserIndex(participants[i]) == -1) {
                std::cout << "Participant is not a registered user: "
                          << participants[i] << ".\n";
                return;
            }
        }

        // SRS minimum is two participants. The temporary GroupChat contract
        // adds the creator, so the entered list must contain another user.
        if (participants.empty()) {
            std::cout << "A group needs at least one other participant.\n";
            return;
        }

        std::unique_ptr<Chat> chat(
            new GroupChat(participants, groupName, creator));
        chats.push_back(chat.get());
        chat.release();
        std::cout << "Group created successfully.\n";
    }

    void viewChats() const {
        if (!isLoggedIn()) {
            std::cout << "Please log in to view chats.\n";
            return;
        }

        const std::string username = getCurrentUsername();
        bool foundChat = false;
        for (std::size_t i = 0; i < chats.size(); ++i) {
            if (whatsapp_detail::containsParticipant(chats[i], username)) {
                std::cout << "[" << i << "] ";
                chats[i]->displayChat();
                foundChat = true;
            }
        }
        if (!foundChat) {
            std::cout << "You do not belong to any chats yet.\n";
        }
    }

    void logout() {
        if (!isLoggedIn()) {
            std::cout << "No user is currently logged in.\n";
            return;
        }
        users[static_cast<std::size_t>(currentUserIndex)].updateLastSeen();
        currentUserIndex = -1;
        std::cout << "Logout successful.\n";
    }

    // These parameterized actions let Jana's menu handle input and navigation.
    bool sendMessage(int chatIndex, const std::string& content) {
        Chat* chat = findAccessibleChat(chatIndex);
        if (chat == nullptr) {
            std::cout << "Chat not found or access denied.\n";
            return false;
        }
        if (whatsapp_detail::trim(content).empty()) {
            std::cout << "Message content cannot be empty.\n";
            return false;
        }
        Message message(getCurrentUsername(), content);
        message.addEmoji(":)");
        message.addEmoji(":(");
        message.addEmoji(":D");
        message.addEmoji("<3");
        message.addEmoji(":thumbsup:");
        chat->addMessage(message);
        return true;
    }

    bool replyToMessage(int chatIndex, int messageIndex,
                        const std::string& content) {
        Chat* chat = findAccessibleChat(chatIndex);
        if (chat == nullptr) {
            std::cout << "Chat not found or access denied.\n";
            return false;
        }
        if (whatsapp_detail::trim(content).empty()) {
            std::cout << "Reply content cannot be empty.\n";
            return false;
        }
        const std::vector<Message> messages = chat->getMessages();
        if (messageIndex < 0 ||
            static_cast<std::size_t>(messageIndex) >= messages.size()) {
            std::cout << "Message index is out of range.\n";
            return false;
        }

        // Keep a stable copy while setting the reply. Message::setReplyTo()
        // takes its own snapshot, which the reply then owns for its lifetime.
        std::unique_ptr<Message> originalCopy(
            new Message(messages[static_cast<std::size_t>(messageIndex)]));
        Message reply(getCurrentUsername(), content);
        reply.setReplyTo(originalCopy.get());
        chat->addMessage(reply);
        return true;
    }

    bool deleteMessage(int chatIndex, int messageIndex) {
        Chat* chat = findAccessibleChat(chatIndex);
        if (chat == nullptr) {
            std::cout << "Chat not found or access denied.\n";
            return false;
        }
        const bool deleted = chat->deleteMessage(messageIndex, getCurrentUsername());
        if (!deleted) {
            std::cout << "Message could not be deleted; check its index and sender.\n";
        }
        return deleted;
    }

    std::vector<Message> searchChatMessages(int chatIndex,
                                            const std::string& keyword) const {
        Chat* chat = findAccessibleChat(chatIndex);
        if (chat == nullptr || keyword.empty()) {
            if (chat == nullptr) {
                std::cout << "Chat not found or access denied.\n";
            }
            return std::vector<Message>();
        }
        return chat->searchMessages(keyword);
    }
};

#endif
