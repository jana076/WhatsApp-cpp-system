#ifndef MESSAGE_CLASS_H
#define MESSAGE_CLASS_H

#include <iostream>
#include <string>
#include <ctime>

using namespace std;


// ========================
//      MESSAGE CLASS
// ========================
class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;
    Message* replyTo;
    
public:
    Message() {
        sender = " ";
        content = " ";
        timestamp = " ";
        status = " ";
        replyTo = nullptr;
    }
    
    Message(string sndr, string cntnt) {
        sender = sndr;
        content = cntnt;
        updateTimestamp();
        status = "sent";
        replyTo = nullptr;
    }
    
    string getContent() const {
        return content ;
    }
    
    string getSender() const {
        return sender ;
    }
    
    string getTimestamp() const {
        return timestamp ;
    }
    
    string getStatus() const {
        return status ;
    }
    
    Message* getReplyTo() const {
        return replyTo ;
    }
    
    void setStatus(string newStatus) {
        status = newStatus;
    }
    
    void setReplyTo(Message* msg) {
        replyTo = msg;
    }
    
    void updateTimestamp() {
        time_t now = time(0);
        tm* localT = localtime(&now);
        char buffer[100];
        strftime(buffer, 100, "%H:%M", localT);
        timestamp = buffer;
    }
    
    void display() const {
        cout<< sender << " sent: "<< content<<endl;
        cout<<timestamp<<endl;
        cout<< status<<endl;

        if(replyTo != nullptr){
            cout<< "replying to: "<< replyTo->getContent() <<endl;
        }
    }
    
    void addEmoji(string emojiCode) {
        string emoji;
        
        if(emojiCode == ":)" ){
            emoji = "😊";
        }
        else if(emojiCode == ":(" ){
            emoji = "😢";
        }
        else if(emojiCode == ":D" ){
            emoji = "😃";
        }
        else if(emojiCode == "<3" ){
            emoji = "❤️";
        }
        else if(emojiCode == "thumbsup" ){
            emoji = "👍";
        }
        else{
            return;
        }

        size_t position = content.find(emojiCode);
        
        while(position != string::npos){
            content.replace(position, emojiCode.length(), emoji);
            position = content.find(emojiCode, position + emoji.length());
        }
    }
};

#endif

