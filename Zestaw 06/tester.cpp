
// made by Karosek

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

const short port = 2020;
const char* ip = "127.0.0.1";

#define TIMEOUT_TIME        10s

void startTest(string input, string expected);
void startSegmTest(vector<string> inputs, string expected);
void startTimeoutTest(string input, string expected);
int testCounter = 1;

// socket
int s;

int main(int argc, char * argv[]) {
    bool waitForForks = true;
    if(argc == 2) {
        if(!strcmp(argv[1], "-nowait") || !strcmp(argv[1], "--nowait")) waitForForks = false;
    }
    struct sockaddr_in serverInfo = {};
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);
   
    if(inet_pton(serverInfo.sin_family, ip, &serverInfo.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
   
    s = socket(serverInfo.sin_family, SOCK_STREAM, 0);
    if(s < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
   
    if(connect(s, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    cout << "[\u001b[33m#\u001b[0m] Connected succesfully! Starting now..." << endl;
    string input = "";
    vector<string> in;
    const string expectError = "ERROR\r\n";

    // test 1, 1024 len of datagram
    for(int i=0; i<1020; i+=2) input.append("1 ");
    input.append("11\r\n");
    startTest(input, "521\r\n");

    // test 2, random bullshit
    input = "";
    for(int i=0; i<1022; i++)input.append("X");
    input.append("\r\n");
    startTest(input, expectError);

    // test 3, empty datagram
    startTest("\r\n", expectError);
   
    // test 4, datagram divided into 3 parts
    in = {"10 20", " 10 20 ", "10\r\n"};
    startSegmTest(in, "70\r\n");

    // test 5, two tasks in 3 parts
    in = {"0 1 2 1", "0 5\r\n9 2 1", "50 99\r\n"};
    startSegmTest(in, "18\r\n260\r\n");

    // test 6, one big task in 4 parts
    in = {"100 100 10", "0 100 100 100 ", "100 1", "00", " 100\r\n"};
    startSegmTest(in, "900\r\n");

    // test 7, two good task, one bad
    in = {"50 60 20\r\n10", "0 123456 \r\n10 0", "1 22\r\n"};
    startSegmTest(in, "130\r\nERROR\r\n33\r\n");

    // test 8, three bad tasks
    in = {"1 1 1 \r\n 1 1", " 1 1 ", "\r\n \r\n"};
    startSegmTest(in, "ERROR\r\nERROR\r\nERROR\r\n");

    // test 9, one bad task and two good tasks
    in = {"121234567899873456789987 10 10\r\n9 9 9 9 9\r\n", "1 1 1 ", "1 1 1", " 1 ", "1\r\n"};
    startSegmTest(in, "ERROR\r\n45\r\n8\r\n");

    // test 10, \r\n divided but tasks are good
    in = {"2 2 2 2\r", "\n2 2 2\r\n"};
    startSegmTest(in, "8\r\n6\r\n");

    // test 11, \r\n divided and bad tasks
    in = {"95 2 1 2 \r", "\n 1 1 1 1\r", "\n"};
    startSegmTest(in, "ERROR\r\nERROR\r\n");

    // test 12, \r\n divided but with space between
    in = {"0 1 1 0\r", " \n5 5\r\n"};
    startSegmTest(in, expectError);

    // test 13, same as 12 but space after \r
    in = {"0 1 1 0\r ", "\n5 5\r\n"};
    startSegmTest(in, expectError);

    // test 14 same as 12 and 13 but space in both places
    in = {"0 1 1 0\r ", " \n5 5\r\n"};
    startSegmTest(in, expectError);

    // test 15, same as 14 but works
    in = {"0 1 1 0\r", "\n5 5\r\n"};
    startSegmTest(in, "2\r\n10\r\n");

    // test 16, only zeros through parts
    in = {"0 0 0 0 0", " 0 0 0 0 ", "0 0 0 0 ", "0\r\n"};
    startSegmTest(in, "0\r\n");

    // test 17, \r\n in separate message
    in = {"1 2 3 ", "\r\n"};
    startSegmTest(in, "ERROR\r\n");

    // test 18, similar to 17 but space comes with \r\n
    in = {"1 2 3", " \r\n"};
    startSegmTest(in, "ERROR\r\n");

    // test 19, same as 16 and 17 but works
    in = {"1 2 3", "\r\n"};
    startSegmTest(in, "6\r\n");

    shutdown(s, SHUT_RDWR);

    // timeout test
    cout << "[=================================================================]" << endl;
    cout << "[ \u001b[36mTimeout tests starts in \u001b[31m5\u001b[36m seconds, this part will take a while!\u001b[0m ]" << endl;
    cout << "[=================================================================]" << endl;
    sleep_for(5s);

    const int tests_count = 8;
    string data[tests_count][2] = {
        {"10 20 30", "60\r\n"},
        {"1", "1\r\n"},
        {"0", "0\r\n"},
        {"qwertyuiopasdfghjklzxcvbnm", "ERROR\r\n"},
        {" ", "ERROR\r\n"},
        {" 1 2 3", "ERROR\r\n"},
        {"1 2 3 ", "ERROR\r\n"},
        {" 1  2  3 ", "ERROR\r\n"}
    };

    for(int i = 0; i < tests_count; i++) {
        pid_t c_pid = fork();

        if (c_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if(c_pid > 0) {
            if(waitForForks) wait(nullptr);
        } else {
            cout << endl << "[\u001b[33m" << getpid() << "\u001b[0m] Process created!" << endl;
            startTimeoutTest(data[i][0], data[i][1]);
            cout << endl;
            exit(EXIT_SUCCESS);
        }
    }

    // wait for all forks to end
    pid_t wpid;
    int status;
    while ((wpid = wait(&status)) > 0);

    shutdown(s, SHUT_RDWR);
    return 0;
}

void startTest(string input, string expected) {
    char result[1024];
    memset(result, 0, sizeof(result));

    cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test starting..." << endl;

    if(send(s, input.c_str(), input.length(), 0) < 0) {
        perror("send error");
        exit(EXIT_FAILURE);
    }
    int bytes = recv(s, result, sizeof(result), 0);
    if(bytes <= 0) {
        perror("recv error/server disconnected");
        exit(EXIT_FAILURE);
    }

    if(!strcmp(result, expected.c_str())) {
        cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test: \u001b[32mSuccess!\033[0m" << endl;
    } else {
        long unsigned int i = 0;
        cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test: \u001b[31mFailure!\033[0m" << endl;

        cout << "# Bytes send:\t\t";
        for(i=0; i<input.length(); i++)  {
            cout << (int)input[i];
            if(i != (input.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes expected:\t";
        for(i=0; i<expected.length(); i++)  {
            cout << (int)expected[i];
            if(i != (expected.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes received:\t";
        for(i=0; i<strlen(result); i++)  {
            cout << (int)result[i];
            if(i != (strlen(result)-1)) cout << ", ";
        }
        cout << endl;
    }
    cout << endl;

    testCounter++;
}

void startSegmTest(vector<string> inputs, string expected) {
    string input = "";
    char result[1024];
    memset(result, 0, sizeof(result));

    cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test starting..." << endl;

    for(size_t i=0; i<inputs.size(); i++) {
        input.append(inputs[i]);
         if(send(s, inputs[i].c_str(), inputs[i].length(), 0) < 0) {
            perror("send error");
            exit(EXIT_FAILURE);
        }
        cout << "[\u001b[33m" << testCounter << "\u001b[0m] Part #" << i << " was send!" << endl;
        sleep_for(500ms); 
    }

    cout << "[\u001b[33m" << testCounter << "\u001b[0m] Receiving data..." << endl;
    int bytes = recv(s, result, sizeof(result), 0);
    if(bytes <= 0) {
        perror("recv error/server disconnected");
        exit(EXIT_FAILURE);
    }

    if(!strcmp(result, expected.c_str())) {
        cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test: \u001b[32mSuccess!\033[0m" << endl;
    } else {
        long unsigned int i = 0;
        cout << "[\u001b[33m" << testCounter << "\u001b[0m] Test: \u001b[31mFailure!\033[0m" << endl;

        cout << "# Bytes send:\t\t";
        for(i=0; i<input.length(); i++)  {
            cout << (int)input[i];
            if(i != (input.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes expected:\t";
        for(i=0; i<expected.length(); i++)  {
            cout << (int)expected[i];
            if(i != (expected.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes received:\t";
        for(i=0; i<strlen(result); i++)  {
            cout << (int)result[i];
            if(i != (strlen(result)-1)) cout << ", ";
        }
        cout << endl;
    }
    cout << endl;
    testCounter++;
}

void startTimeoutTest(string input, string expected) {
    bool timeoutOccured = false;
    struct sockaddr_in serverInfo = {};
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);
   
    if(inet_pton(serverInfo.sin_family, ip, &serverInfo.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(EXIT_FAILURE);
    }
   
    s = socket(serverInfo.sin_family, SOCK_STREAM, 0);
    if(s < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
   
    if(connect(s, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    cout << "> Connected succesfully! Test starting..." << endl;
    string crlf = "\r\n";
    char result[1024];
    memset(result, 0, sizeof(result));

    if(send(s, input.c_str(), input.length(), 0) < 0) {
        perror("send error");
        exit(EXIT_FAILURE);
    }
    cout << "Data send! Sleeping..." << endl;

    sleep_for(TIMEOUT_TIME);

    cout << "Checking connection..." << endl;
    if(send(s, crlf.c_str(), crlf.length(), 0) <= 0) {
        if(errno == ETIMEDOUT || errno == ECONNRESET) {
            errno = 0;
            cout << "Timeout occured! \u001b[32mSuccess!\u001b[0m" << endl;
            timeoutOccured = true;
        } else {
            perror("send error");
            exit(EXIT_FAILURE);
        }
    }

    if(timeoutOccured) {
        shutdown(s, SHUT_RDWR);
        return;
    }

    int bytes = recv(s, result, sizeof(result), 0);
    if(bytes <= 0) {
        if(errno == ETIMEDOUT || errno == ECONNRESET) {
            errno = 0;
            cout << "Timeout occured! \u001b[32mSuccess!\u001b[0m" << endl;
            timeoutOccured = true;
        } else {
            perror("recv error");
            exit(EXIT_FAILURE);
        }
    }

    if(timeoutOccured) {
        shutdown(s, SHUT_RDWR);
        return;
    }

    if(!strcmp(result, expected.c_str())) {
        cout << "No timeout detected but received data \u001b[32mis correct!\033[0m" << endl;
    } else {
        long unsigned int i = 0;
        cout << "No timeout detected and received data \u001b[31misn't correct!\033[0m" << endl;

        cout << "# Bytes send:\t\t";
        for(i=0; i<input.length(); i++)  {
            cout << (int)input[i];
            if(i != (input.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes expected:\t";
        for(i=0; i<expected.length(); i++)  {
            cout << (int)expected[i];
            if(i != (expected.length()-1)) cout << ", ";
        }
        cout << endl;

        cout << "# Bytes received:\t";
        for(i=0; i<strlen(result); i++)  {
            cout << (int)result[i];
            if(i != (strlen(result)-1)) cout << ", ";
        }
        cout << endl;
    }
    shutdown(s, SHUT_RDWR);
    sleep_for(500ms);
}