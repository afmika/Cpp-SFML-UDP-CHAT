#include <TGUI/TGUI.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include "classes/afTools.h"
using namespace std;
typedef unsigned short ushort;

// globals
sf::UdpSocket * socket = new sf::UdpSocket;

tgui::TextBox::Ptr chatScreen;
tgui::TextBox::Ptr messageInput;
tgui::TextBox::Ptr serverAdrInput;
tgui::TextBox::Ptr nameInput;

// btn
tgui::Button::Ptr connectBtn;
tgui::Button::Ptr sendBtn;

pair<string, ushort> parseIpPort(string str) {
    vector<string> sp_str = doSplit(str, ":");

    string ip = sp_str[0];
    ushort port = strToInt( sp_str[1] );

    sp_str.clear();
    pair<string, ushort> p{ip, port};
    return p;
}

void msg_log(string type,  string message ) {
    chatScreen->setText( chatScreen->getText() + "\n[ "+ type + " ] "+ " >> " + message);
}

void pingSocketConnection() {
    string ipport = serverAdrInput->getText();
    pair<string, ushort> adr = parseIpPort( ipport );
    string test = "CHECK_CONNECTION";

    if (socket->send(test.c_str(), test.size(), adr.first, adr.second) != sf::Socket::Done) {
        msg_log("Error", ipport + " Failed to connect !");
    } else {
        // the port is ready but we still 
        // dont know if the server received our msg or not (UDP)
        socket->setBlocking(false);
        // msg_log("Success", ipport + " Connected !");
    }
}

void waitNewDatasNonBlocking() {
    sf::IpAddress sender;
    unsigned short port;

    sf::Packet packet;
    bool is_ok = socket->receive(packet, sender, port) == sf::Socket::Done;
    if (is_ok) {
        const char * datas = (const char *) packet.getData();
        string str( datas );
        // cout << "NonBlocking wait received stuffs" << endl;
        msg_log("USER", str);
    }
}

void sendMessage() {
    string ipport = serverAdrInput->getText();
    pair<string, ushort> adr = parseIpPort( ipport );

    string username = nameInput->getText();
    string message = messageInput->getText();
    message = username + "|" + message;
    // sf::Packet packet;
    // packet << message.c_str();
    if (socket->send( message.c_str(), message.size(), adr.first, adr.second  ) != sf::Socket::Done) {
        msg_log("Error", "message not sent");
    }
}

void initGlobals( tgui::Gui& gui) {
    // textbox input
    chatScreen = gui.get<tgui::TextBox>("ChatScreen");
    messageInput = gui.get<tgui::TextBox>("MessageInput");
    serverAdrInput = gui.get<tgui::TextBox>("ServerAdrInput");
    nameInput = gui.get<tgui::TextBox>("NameInput");

    // btn
    connectBtn = gui.get<tgui::Button>("ConnectBtn");
    sendBtn = gui.get<tgui::Button>("SendBtn");
    
    // events
    connectBtn->connect("pressed", pingSocketConnection);
    sendBtn->connect("pressed", sendMessage);
}

void connectPort(sf::Socket& socket, string ipport, tgui::TextBox::Ptr chatScreen) {
    pair<string, ushort> adr = parseIpPort( ipport );
    // cout << adr.first << adr.second << endl;
    chatScreen->setText( chatScreen->getText() + "\n" + adr.first + ":" + to_string(adr.second) );
}

int main() {
    sf::RenderWindow window(sf::VideoMode(690, 500), "afChat Client");
    window.setFramerateLimit(24);

    // thread - udp listener
    /*
    sf::Thread* thread = 0;
    thread = new sf::Thread(&waitNewDatasNonBlocking);
	thread->launch();
    */

    tgui::Gui gui(window);

    try {
        gui.loadWidgetsFromFile("resources/client.ui.af", true);
        initGlobals( gui );
    } catch (const tgui::Exception& e) {
        std::cerr << "TGUI Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // delete thread;
                window.close();
            }
            gui.handleEvent(event);
        }
        window.clear(sf::Color::White);

        waitNewDatasNonBlocking(); // non blocking socket go brrr

        gui.draw();
        window.display();
    }

    return 0;
}
