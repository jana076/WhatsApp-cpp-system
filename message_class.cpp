#include <iostream>
#include <string>
#include <ctime>
#include "message_class.h"

using namespace std;


int main(){
    string sender;
    string content;
    cout<<"enter the sender and content"<<endl;
    cin>>sender;
    cin.ignore();
    getline(cin, content);
    
    Message m1(sender, content);
    m1.display();

    m1.setStatus("read");
    m1.addEmoji(":)");
    m1.display();
    return 0;
}