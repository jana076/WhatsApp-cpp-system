#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include "../../WhatsApp.h"

namespace {

int passed = 0;
int failed = 0;

bool disableTerminalPasswordInput() {
#ifdef _WIN32
    int pipeDescriptors[2];
    if (_pipe(pipeDescriptors, 256, _O_BINARY) != 0) {
        return false;
    }
    _close(pipeDescriptors[1]);
    const int result = _dup2(pipeDescriptors[0], _fileno(stdin));
    _close(pipeDescriptors[0]);
    return result == 0;
#else
    int pipeDescriptors[2];
    if (pipe(pipeDescriptors) != 0) {
        return false;
    }
    close(pipeDescriptors[1]);
    const int result = dup2(pipeDescriptors[0], STDIN_FILENO);
    close(pipeDescriptors[0]);
    return result == STDIN_FILENO;
#endif
}

template <typename Action>
std::string invoke(const std::string& input, Action action) {
    std::istringstream redirectedInput(input);
    std::ostringstream redirectedOutput;
    std::streambuf* oldInput = std::cin.rdbuf(redirectedInput.rdbuf());
    std::streambuf* oldOutput = std::cout.rdbuf(redirectedOutput.rdbuf());
    std::cin.clear();
    std::cout.clear();

    action();

    std::cin.rdbuf(oldInput);
    std::cout.rdbuf(oldOutput);
    std::cin.clear();
    std::cout.clear();
    return redirectedOutput.str();
}

void check(bool condition, const std::string& description) {
    if (condition) {
        ++passed;
        std::cout << "PASS: " << description << '\n';
    } else {
        ++failed;
        std::cout << "FAIL: " << description << '\n';
    }
}

void signUp(WhatsApp& app, const std::string& username,
            const std::string& password = "pass123",
            const std::string& phone = "555-0101") {
    invoke("\n" + username + "\n" + password + "\n" + phone + "\n",
           [&app]() { app.signUp(); });
}

void login(WhatsApp& app, const std::string& username,
           const std::string& password = "pass123") {
    invoke("\n" + username + "\n" + password + "\n",
           [&app]() { app.login(); });
}

void testAuthentication() {
    WhatsApp app;
    const std::string registered = invoke(
        "\nalice\npass123\n555-0101\n", [&app]() { app.signUp(); });
    const std::string duplicate = invoke(
        "\nalice\nsecond7\n555-0102\n", [&app]() { app.signUp(); });
    const std::string shortPassword = invoke(
        "\nbob\nshort\n555-0103\n", [&app]() { app.signUp(); });
    const std::string failedLogin = invoke(
        "\nalice\nwrongpw\n", [&app]() { app.login(); });
    const std::string successfulLogin = invoke(
        "\nalice\npass123\n", [&app]() { app.login(); });
    const std::string logout = invoke("", [&app]() { app.logout(); });

    check(registered.find("Account created successfully") != std::string::npos,
          "sign up with valid details");
    check(duplicate.find("already registered") != std::string::npos,
          "reject duplicate username");
    check(shortPassword.find("at least 6") != std::string::npos,
          "reject short password");
    check(failedLogin.find("Invalid username or password") != std::string::npos,
          "reject invalid login");
    check(successfulLogin.find("Login successful") != std::string::npos,
          "accept valid login");
    check(logout.find("Logout successful") != std::string::npos,
          "logout current user");
    check(!app.sendMessage(0, "not logged in"),
          "reject chat operation after logout");
}

void testChatsAndMessages() {
    WhatsApp app;
    signUp(app, "alice");
    signUp(app, "bob");
    signUp(app, "charlie");
    login(app, "alice");

    const std::string privateCreated = invoke(
        "\nbob\n", [&app]() { app.startPrivateChat(); });
    const std::string privateReused = invoke(
        "\nbob\n", [&app]() { app.startPrivateChat(); });
    const std::string selfChat = invoke(
        "\nalice\n", [&app]() { app.startPrivateChat(); });
    const std::string invalidGroup = invoke(
        "\nInvalid group\nunknown\n", [&app]() { app.createGroup(); });
    const std::string groupCreated = invoke(
        "\nStudy group\nbob, charlie\n", [&app]() { app.createGroup(); });

    check(privateCreated.find("created successfully") != std::string::npos,
          "create private chat with registered user");
    check(privateReused.find("Opening existing private chat") !=
              std::string::npos,
          "reuse existing private chat");
    check(selfChat.find("yourself") != std::string::npos,
          "reject private chat with self");
    check(invalidGroup.find("not a registered user") != std::string::npos,
          "reject unregistered group participant");
    check(groupCreated.find("Group created successfully") != std::string::npos,
          "create group with registered participants");

    check(app.sendMessage(0, "hello :)"), "send message to private chat");
    const std::vector<Message> sentMessages =
        app.searchChatMessages(0, "hello");
    check(sentMessages.size() == 1 &&
              sentMessages[0].getContent().find(":)") == std::string::npos,
          "search message and replace supported emoji code");
    check(app.replyToMessage(0, 0, "reply text"),
          "reply to existing message");

    const std::vector<Message> replies = app.searchChatMessages(0, "reply text");
    check(replies.size() == 1 && replies[0].getReplyTo() != nullptr &&
              replies[0].getReplyTo()->getContent() ==
                  sentMessages[0].getContent(),
          "reply retains a valid quoted message snapshot");
    check(!app.replyToMessage(0, 99, "invalid target"),
          "reject out-of-range reply index");
    check(!app.sendMessage(0, "   "), "reject empty message content");
    check(app.sendMessage(1, "group message"), "send message to group chat");
    check(app.searchChatMessages(1, "group message").size() == 1,
          "search messages in group chat");

    const std::string aliceChatList =
        invoke("", [&app]() { app.viewChats(); });
    check(aliceChatList.find("Chat between alice and bob") != std::string::npos &&
              aliceChatList.find("Study group") != std::string::npos &&
              aliceChatList.find("Admins (1): alice") != std::string::npos,
          "view participant chats and group creator admin");

    invoke("", [&app]() { app.logout(); });
    login(app, "charlie");
    const std::string charlieChatList =
        invoke("", [&app]() { app.viewChats(); });
    check(charlieChatList.find("Study group") != std::string::npos &&
              charlieChatList.find("Chat between alice and bob") ==
                  std::string::npos,
          "show only chats in which current user participates");
    check(!app.sendMessage(0, "unauthorized") &&
              app.searchChatMessages(0, "hello").empty(),
          "deny nonparticipant access to private chat");

    invoke("", [&app]() { app.logout(); });
    login(app, "bob");
    check(!app.deleteMessage(0, 0),
          "prevent participant from deleting another user's message");
    invoke("", [&app]() { app.logout(); });
    login(app, "alice");

    check(app.deleteMessage(0, 0), "allow sender to delete own message");
    const std::vector<Message> repliesAfterDelete =
        app.searchChatMessages(0, "reply text");
    check(repliesAfterDelete.size() == 1 &&
              repliesAfterDelete[0].getReplyTo() == nullptr,
          "deleting original clears reply snapshot safely");
}

} // namespace

int main() {
    if (!disableTerminalPasswordInput()) {
        std::cerr << "FAIL: could not redirect native stdin for password tests\n";
        return 2;
    }

    testAuthentication();
    testChatsAndMessages();

    std::cout << "\nPassed: " << passed << ", failed: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
