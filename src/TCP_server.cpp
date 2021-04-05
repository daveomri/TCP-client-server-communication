// @author David Omrai

#include <iostream>     
#include <map>          //map
#include <string>       //string
#include <time.h>       //time_t, time()
#include <cmath>        // sqrt()
//-------------------------------
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h> // socket(), bind(), connect(), listen()
#include <unistd.h>     // close(), read(), write()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons(), htonl()
#include <strings.h>    // bzero()
#include <string.h>
#include <wait.h>       // waitpid()
//-------------------------------
#include <sys/types.h>
#include <netdb.h>
//------------------------------
using namespace std;

/**
 * This structure manage time
 * spend from its creation
 * it's used for time check
 */
struct TimeCheck{
    time_t seconds; //time in seconds
    int    life;    //life in seconds
    /**
     * Constructor, sets life param
     * logs current time to seconds param
     * @param timeOut life in seconds
     */
    TimeCheck(int timeOut):life(timeOut){seconds=time(NULL);}
    /**
     * Function returns time spend from creation of structure
     * @return time left
     */
    int lifeLeft(){
        int time_left = (life - (time(NULL)-seconds));
        if (life <= 0)
            return 0;
        return time_left;
    }
};

/**
 * Structure represents position of robot 
 * in space
 * it has two positions, x and y
 */
struct SpacePositon{
    int x = 0;//position x
    int y = 0;//position y
    /**
     * Constructor sets positions
     * @param x position x
     * @param y position y
     */
    SpacePositon(int x = 10, int y = 10) :x(x), y(y) {}
};
//-----------------------------------------------------------------------------------

/**
 * Class represents and mantain position 
 * of robot in space, it knows current position
 * and final position where to send robot
 * It also helps robot find message in final positions
 */
class RobotPosition{
    private:
        SpacePositon way;               //way of robot
        SpacePositon position;          //current position
        SpacePositon dest;              //destination position in maze
        int          robotLife = 100;   //max steps of robot
        bool         inDest = false;    //indicator of robot in maze
        bool         maze[25] = {0};    //visited positions in maze
    public:        
        /**
         * Constructor sets current position
         * and final position
         * @param x current position x
         * @param y current position y
         */
        RobotPosition(int x, int y);

        /**
         * Method sets way of robot
         * @param x position x
         * @param y positon y
         */
        void setWay(int x, int y);

        /**
         * Method sets position of robot
         * if robot is in maze it sets inMaze to 1
         * if new position is same like current, way wont be changed
         * @param x new position x
         * @param y new position y
         */
        void setPos(int x, int y);

        /**
         * Method converts 2d position to 1d
         * it maps x and y position to int
         * so it can be used as index in maze[]
         * @param x position x
         * @param y position y
         * @return index created from x and y positions
         */
        int toMaze(int x, int y);

        /**
         * Method returns which way is best to make in next mobe
         * @return 1:front, -1:down, 2:right: -2:left
         */
        int bestWay();

        /**
         * Method returns position of next position in maze
         * to go to
         * @return position of next move
         */
        SpacePositon nextInArea();

        /**
         * Method returns position of closest corner of maze
         * @return one position of corner [2,2],[2,-2],[-2,2],[-2,-2]
         */
        SpacePositon closestCorner();

        /**
         * Static method returns distance between two space positions
         * @param one first positions
         * @param two second positions
         * @return distance between this two positions
         */
        static double countDistance(SpacePositon & one, SpacePositon & two);

        /**
         * Static method checks if positions are in maze
         * @param x position x
         * @param y position y
         * @retuen true if positions are in maze, false otherwise
         */
        static bool isInArea(int x, int y);

        /**
         * Method compare given positions
         * with current positions of robot
         * @param x position x
         * @param y position y
         * @return true if given positions are same with current
         * false otherwise
         */
        bool at(int x, int y);

        /**
         * Method uses static inInArea method with current 
         * positions of robot
         * @return true if robot is in area, false otherwise
         */
        bool inArea();

        /**
         * Method checks if robot is in one of the corners
         * @return true if robot is in corner, false otherwise
         */
        bool isInCorner();

        /**
         * Method returns which way is given position
         * from current position of robot
         * @param pos given position where to go
         * @return 1:front, -1:down, 2:right: -2:left
         */
        int whatWay(SpacePositon& pos);

};
//-------------methods-of-RobotPosition---------------------------
RobotPosition::RobotPosition(int x, int y){
    position.x = x;
    position.y = y;
    dest = closestCorner();
}

bool RobotPosition::at(int x, int y){
    return (position.x==x && position.y==y);
}

int RobotPosition::toMaze(int x, int y){
    return ((x+2)+(y+2)*5);
}

bool RobotPosition::isInCorner(){
    return inDest;
}

SpacePositon RobotPosition::closestCorner(){
    SpacePositon left_up     ( -2, 2 ); //left up
    SpacePositon right_up    ( 2, 2 );  //right up
    SpacePositon left_down   ( -2, -2); //left down
    SpacePositon right_down  ( 2, -2 ); //right down

    double lu = countDistance(left_up,   position);//distance from left up
    double ru = countDistance(right_up,  position);//distance from right up
    double ld = countDistance(left_down, position);//distance from left down
    double rd = countDistance(right_down,position);//distance from right down

    double minInt = lu;
    if (minInt > ru)
        minInt = ru;
    if (minInt > ld)
        minInt = ld;
    if (minInt > rd)
        minInt = rd;

    if (minInt == lu)
        return left_up;
    if (minInt == ru)
        return right_up;
    if (minInt == ld)
        return left_down;
    else
        return right_down;
}

//being in corners
//then relove way, 0 1 2 3 right, down, left, up
void RobotPosition::setWay(int x, int y){
   way.x = x-position.x;//direction x
   way.y = y-position.y;//direction y
}

void RobotPosition::setPos(int x, int y){
   if (!(x==position.x && y==position.y)){
       setWay(x, y);
   }

   robotLife-=1;//decrease robot lifespan

   position.x = x;//direction x
   position.y = y;//direction y
   if (dest.x == x && dest.y == y){
       inDest = true;
    }
   if (inDest == true && isInArea(x, y) == true){
       maze[toMaze(x, y)] = true;
   }
}

bool RobotPosition::isInArea(int x, int y){
    //turn given coordinates to positive
    if (x<0)
        x*=-1;
    if (y<0)
        y*=-1;
    //if position in in maze
    if (x <=2 && y <= 2)
        return true;
    return false;
}

bool RobotPosition::inArea(){
    return isInArea(position.x, position.y);
}

double RobotPosition::countDistance(SpacePositon & one, SpacePositon & two){
    return ( sqrt((one.x-two.x)*(one.x-two.x)+(one.y-two.y)*(one.y-two.y)));
}

int RobotPosition::whatWay(SpacePositon & pos){
    if (robotLife<=0)
        return 0;

    if(way.x==1 && way.y==0){//right
        setWay(pos.x, pos.y);
        if(pos.x==(position.x+1) &&pos.y==(position.y+0))
            return 1;//front
            
        if(pos.x==(position.x-1) &&pos.y==(position.y+0))
            return -1;//back

        if(pos.x==(position.x+0) &&pos.y==(position.y+1))
            return -2;//left

        if(pos.x==(position.x+0) &&pos.y==(position.y-1))
            return 2;//right
    }
    if(way.x==-1 && way.y==0){//left
        setWay(pos.x, pos.y);
        if(pos.x==(position.x+1) &&pos.y==(position.y+0))
            return -1;//back

        if(pos.x==(position.x-1) &&pos.y==(position.y+0))
            return 1;//front

        if(pos.x==(position.x+0) &&pos.y==(position.y+1))
            return 2;//right

        if(pos.x==(position.x+0) &&pos.y==(position.y-1))
            return -2;//left
    }
    if(way.x==0 && way.y==1){//up
        setWay(pos.x, pos.y);
        if(pos.x==(position.x+1) &&pos.y==(position.y+0))
            return 2;//right

        if(pos.x==(position.x-1) &&pos.y==(position.y+0))
            return -2;//left

        if(pos.x==(position.x+0) &&pos.y==(position.y+1))
            return 1;//front

        if(pos.x==(position.x+0) &&pos.y==(position.y-1))
            return -1;//back
    }
    if(way.x==0 && way.y==-1){//down
        setWay(pos.x, pos.y);
        if(pos.x==(position.x+1) &&pos.y==(position.y+0))
            return -2;//left

        if(pos.x==(position.x-1) &&pos.y==(position.y+0))
            return 2;//right

        if(pos.x==(position.x+0) &&pos.y==(position.y+1))
            return -1;//back

        if(pos.x==(position.x+0) &&pos.y==(position.y-1))
            return 1;//front
    }

    setWay(pos.x, pos.y);
    return 0;
}

int RobotPosition::bestWay(){
    if (robotLife<=0)
        return 0;
    
    SpacePositon up   (position.x,position.y+1);//up
    SpacePositon down (position.x,position.y-1);//down
    SpacePositon left (position.x-1,position.y);//left
    SpacePositon right(position.x+1,position.y);//right

    if (!inDest){
        //up or down
        SpacePositon one(0, 0);
        if ( countDistance(up, dest) <= countDistance(down, dest) )
            one = up;
        else
            one = down;
        //left or right
        SpacePositon two(0, 0);
        if ( countDistance(left, dest) <= countDistance(right, dest) )
            two = left;
        else
            two = right;    
        //which way
        SpacePositon output;
        if ( countDistance(one, dest) <= countDistance(two, dest) )
            output = one;
        else
            output = two;
        
        return whatWay(output);
    }
    else{
        if (isInArea(position.x+way.x, position.y+way.y)&&maze[toMaze(position.x+way.x, position.y+way.y)]==false){
            SpacePositon front (position.x+way.x, position.y+way.y);
            return whatWay(front);
        }
        if(isInArea(up.x, up.y)         && maze[toMaze(up.x, up.y)]==false)
            return whatWay(up);
        if(isInArea(down.x, down.y)     && maze[toMaze(down.x, down.y)]==false)
            return whatWay(down);
        if(isInArea(left.x, left.y)     && maze[toMaze(left.x, left.y)]==false)
            return whatWay(left);
        if(isInArea(right.x, right.y)   && maze[toMaze(right.x, right.y)]==false)
            return whatWay(right);
    }
    return 0;
}
//------------------------------------------------------------------------------------------

/**
 * Class represents TCP server
 * contains all methods to make it functional
 */
class TCPServer{
    private:
        int BUFFER_SIZE         = 10240;    //size of buffer
        int TIMEOUT             = 1;        //reaction timeout
        int TIMEOUT_RECHARGING  = 5;        //recharging timeout
        int clNum               = 10;       //number of clients
        int key                 = 54621;    //server key
        int cKey                = 45328;    //client key

        int port                = 0;        //server port
        int l                   = 0;        //socket

        map<string, string> server_messages = {
            {"SERVER_CONFIRMATION", "\a\b"},                    //confirm code, 5dig max
            {"SERVER_MOVE",         "102 MOVE\a\b"},            //one step forward
            {"SERVER_TURN_LEFT",    "103 TURN LEFT\a\b"},       //turn left
            {"SERVER_TURN_RIGHT",   "104 TURN RIGHT\a\b"},      //turn right
            {"SERVER_PICK_UP",      "105 GET MESSAGE\a\b"},     //pick up the message
            {"SERVER_LOGOUT",       "106 LOGOUT\a\b"},          //end connection
            {"SERVER_OK",           "200 OK\a\b"},              //confirm something
            {"SERVER_LOGIN_FAILED", "300 LOGIN FAILED\a\b"},    //login fail
            {"SERVER_SYNTAX_ERROR", "301 SYNTAX ERROR\a\b"},    //incorrect syntax
            {"SERVER_LOGIC_ERROR",  "302 LOGIC ERROR\a\b"}      //bad situation
        };


    public:
        //messages handler
        /**
         * Method sends given message
         * @param msg message to send
         * @param c socket
         * @return 1 if everything is ok, 0 otherwise
         */
        int         sendMessage     (string msg, int c);

        /**
         * Method send position where to go next
         * @param c socket
         * @param pos position 1:front, -1:down, 2:right: -2:left
         */
        void        sendPossition   (int c, int pos);

        /**
         * Method sends syntax error to client
         * @param c socket
         */
        void        syntaxError     (int c);
        
        /**
         * Method sends logic error to client
         * @param c socket
         */
        void        logicError      (int c);

        /**
         * Method sends login error to client
         * @param c socket
         */
        void        loginError      (int c);

        /**
         * Method sends turn left to client
         * @param c socket
         */
        void        turnLeft        (int c);

        /**
         * Method sends turn right to client
         * @param c socket
         */
        void        turnRight       (int c);

        /**
         * Method sends move front to client
         * @param c socket
         */
        void        moveFront       (int c);

        /**
         * Method sends pick up to client
         * @param c socket
         */
        void        pickUp          (int c);

        /**
         * Method sends logout to client
         * @param c socket
         */
        void        logOut          (int c);

        /**
         * Method sends ok to client
         * @param c socket
         */
        void        sendOk          (int c);

        /**
         * Method sends confirm with message to client
         * @param msg confirm code
         * @param c socket
         */
        void        confirm         (string msg, int c);

        //test message content-------------------------
        /**
         * Method checks if given string is ment to recharge robot
         * @param msg message to compare to valid recharging string
         * @return true if msg is valid, false otherwise
         */
        bool        isRecharging    (string msg);

        /**
         * Method compares given string if it is valid
         * with full power string
         * @param msg message to compare with valid full power string
         * @return true if string is valid, false otherwise
         */
        bool        isFullPower     (string msg);
        //---------------------------------------------

        /**
         * Method is used to wait for message from client
         * @param c socket
         * @return 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         waitToRecharge (int c);

        /**
         * Method takes control of loading data from buffer
         * @param msg here goes received message
         * @param c socket
         * @param len mex length of data to load
         * @param time_out timeout
         * @return 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         whileRecharge  (string & msg, int c, int len, int time_out);

        //---------------------------------------------
        /**
         * Method starts TCP server
         * @param argc number of parameters
         * @param argv array od strings
         * @return 1:good, -1:bad
         */
        int         start           (int argc, char ** argv);
        
        /**
         * Mehod is used to comunicate and operate movements of robot
         * @param c socket
         * @return 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         aliveConnection ( int c );

        /**
         * Method navigate robot to maze, one of its corners
         * @param c socket
         * @param roboPos position of robot, it saves data here
         * @return 2:logout, 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         navigateRobot   ( int c, RobotPosition& roboPos );

        /**
         * Method looks for message in maze
         * @param c socket
         * @param roboPos position of robot, it saves data here
         * @return 2:logout, 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         findMessage     ( int c, RobotPosition& roboPos );

        /**
         * Method checks if secret message in on current position of robot
         * @param c socket
         * @param result true if secret message is here, false otherwise
         */
        bool        isSecretHere    (int c, int &result, string& message );

        /**
         * Method bind socket to interface
         * @return 1 if everything is ok, false otherwise
         */
        int         bindSocket      ();

        /**
         * Method creates server socket for robot
         * @return socket
         */
        int         createSocket    ();

        /**
         * Method checks if given port is not equal to 0
         * @param p port
         * @return 0:good, -1:bad
         */
        int         createPort      ( int p );

        /**
         * Method is used to loads name of robot,
         * send it confirm code
         * check received confirm code from robot
         * @param c socket
         * @return 2:logout, 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         authentize      ( int c );

        /**
         * Method translate string representation of possition to
         * int and then it stores it in SpacePosition
         * @param mess string with positions
         * @param pos structure to store positions
         * @return 0:good, -1:bad
         */
        int         readPos         (string mess, SpacePositon& pos);

        /**
         * Method used to count hash code
         * @param name with name of robot
         * @return uint16_t hash code
         */
        uint16_t    countHash       (string name);

        /**
         * Method generates confirm code
         * @param name name of robot
         * @param k key
         * @return uint16_t confirm code
         */
        uint16_t    confirmCode     (string name, int k);

        /**
         * Method loads received messages from robot
         * @param mess here goes received message
         * @param len maximal length of message to load
         * @param c socket
         * @param time_out time in which we need to get message
         * @return 1:good, 0:bad syntax, -1:system bad, -2:waiting bad
         */
        int         getMessage      (string & mess, int len, int c, int time_out);

        /**
         * Method is used by getMessage method, it loads one char at time from buffer
         * @param msgChar here goes loaded char from buffer
         * @param c socket
         * @return 1:good, 0:bad
         */
        int         getMessChar     (char & msgChar, int c);

        /**
         * Method used to await robot message
         * @param c socket
         * @param tout timeout
         * @return 1:good, 0:bad syntax, -1:bad waiting
         */
        int         waitForMess     (int c, int tout);
};

int TCPServer::sendMessage (string msg, int c){
    string message = msg;
    if(send(c, message.c_str(), message.length(), 0) < 0){
        perror("Nemohu odeslat data!");
        return 0;
    }
    return 1;
}

void TCPServer::sendPossition (int c, int pos){
    if (pos == 1)
        moveFront(c);
    else if (pos == -1)
        turnRight(c);
    else if (pos == -2)
        turnLeft(c);
    else if (pos == 2)
        turnRight(c);
}

void TCPServer::syntaxError (int c){
    sendMessage(server_messages.at("SERVER_SYNTAX_ERROR"), c);
}

void TCPServer::logicError (int c){
    sendMessage(server_messages.at("SERVER_LOGIC_ERROR"), c);
}

void TCPServer::loginError (int c){
    sendMessage(server_messages.at("SERVER_LOGIN_FAILED"), c);
}

void TCPServer::turnLeft (int c){
    sendMessage(server_messages.at("SERVER_TURN_LEFT"), c);
}

void TCPServer::turnRight (int c){
    sendMessage(server_messages.at("SERVER_TURN_RIGHT"), c);
}

void TCPServer::moveFront (int c){
    sendMessage(server_messages.at("SERVER_MOVE"), c);
}

void TCPServer::pickUp (int c){
    sendMessage(server_messages.at("SERVER_PICK_UP"), c);
}

void TCPServer::logOut (int c){
    sendMessage(server_messages.at("SERVER_LOGOUT"), c);
}

void TCPServer::sendOk (int c){
    sendMessage(server_messages.at("SERVER_OK"), c);
}

void TCPServer::confirm (string msg, int c){
    sendMessage((msg+server_messages.at("SERVER_CONFIRMATION")), c);
}

//----------------------------------------------------
bool TCPServer::isRecharging (string msg){
    if (msg=="RECHARGING\a\b")
        return true;
    return false;
}

bool TCPServer::isFullPower (string msg){
    if (msg=="FULL POWER\a\b")
        return true;
    return false;
}

//----------------------------------------------------
int TCPServer::waitToRecharge (int c){
    TimeCheck tmchk (TIMEOUT_RECHARGING);
    int waitOut = waitForMess(c, tmchk.lifeLeft());
    
    //test connection
    if (waitOut!=1){
        return -2;
    }

    string message = "";
    int messOut = getMessage(message, 12, c, tmchk.lifeLeft());

    if (messOut!=1){
        return messOut;
    }

    if (!isFullPower(message)){
        perror("unexpected message");
        logicError(c);
        return -3;
    }

    return 1;
}

int TCPServer::whileRecharge(string & msg, int c, int len, int time_out){
    int messOut;
    int bfLen = len;
    if (len<12)
        len = 12;

    do{
        messOut = getMessage(msg, len, c, time_out);

        if (messOut!=1){
            perror("Chyba pri cteni ze socketu.");
            return messOut;
        }

        if (isRecharging(msg)){
            int oo = waitToRecharge(c);
            if ( oo != 1)
                return oo;
        }           
    }while(isRecharging(msg));

    if (bfLen<(int)msg.length()){
        syntaxError(c);
        return 0;
    }
    
    return 1;
}

//----------------------------------------------------
int TCPServer::waitForMess(int c, int tout){
    time_t cur_seconds;
    cur_seconds = time(NULL);

    struct timeval timeout;
    timeout.tv_sec = tout;
    timeout.tv_usec = 0;
    fd_set sockets;

    FD_ZERO(&sockets);//clears sockets
    FD_SET(c, &sockets);//add socket to sockets

    int retval = select(c+1, &sockets, NULL, NULL, &timeout);//waiting

    cur_seconds = time(NULL) - cur_seconds;

    if(retval < 0){
        perror("Chyba v select()");
        return -1;
    }
    
    if(!FD_ISSET(c, &sockets)){
        // Zde je jasne, ze funkce select() skoncila cekani kvuli timeoutu.
        return 0;
    }
    return 1;
}


int TCPServer::getMessChar (char & msgChar, int c){
    char buffer[1]; 

    int bytesRead = recv(c, buffer, 1, 0);
    if(bytesRead <= 0)
        return 0;
    msgChar = buffer[0];
    return 1;
}


 int TCPServer::getMessage (string & mess, int len, int c, int time_out){
    string output = "";
    char ch;
    int ba = 0;
    int bb = 0;
    //TimeCheck tmChk(time_out);//todo
    for (int i = 0; i < len; i++){
        //--------------------------------------------------
        //if (tmChk.lifeLeft()==0)
        //   return -1;//timeout
        //--------------------------------------------------

        int waitOut = waitForMess(c, time_out);
        if (waitOut!=1){//login error
            return -2;
        }
        //--------------------------------------------------

        if ( !getMessChar(ch, c) )
            return -1;//reading fail
        
        if (ch == '\a')
            ba = 1;
        else if (ba==1 && ch == '\b')
            bb = 1;
        else
            ba = bb = 0;

        output+=ch;

        if (ba == 1 && bb == 1)
            break;
    }

    if (ba != 1 || bb != 1){
        syntaxError(c);
        return 0;//bad format
    }

    mess = output;
    return 1;
 }

uint16_t TCPServer::countHash ( string name ){
    uint16_t hash = 0;

    for(int i = 0; name[i]!='\a'||name[i+1]!='\b'; i++){
        hash += name[i];
        //if (i>=12)
        //    break;
    }
    
    hash*=1000;
    hash = hash % 65536;
    return hash;
}

uint16_t TCPServer::confirmCode ( string name, int k ){
    return (countHash(name) + k)%65536; 
}

int TCPServer::authentize ( int c ){
    //read message --------------------------------------
    string message = "";
    int messOut = whileRecharge(message, c, 12, TIMEOUT);
    if (messOut!=1){
        return messOut;
    }

    //test message---------------------------------------
    if (message.length() < 3){
        printf("Invalid message.: Length\n");
        return -2;
    }

    string robot_name = message;
    uint16_t hash = confirmCode(robot_name, key);
    message = to_string(hash);

    //send hash------------------------------------------
    confirm(message, c);

    //read message --------------------------------------
    string second_message = "";

    messOut = whileRecharge(second_message, c, 7, TIMEOUT);

    if (messOut!=1){
        return messOut;
    }
    
    //test message---------------------------------------
    if (second_message.length() < 2){
        printf("Invalid message.: Length\n");
        return -2;
    }

    second_message = second_message.substr(0, second_message.length()-2);
    //count hash-----------------------------------------
    hash = confirmCode(robot_name, cKey);

    //test hash with client message----------------------
    for (size_t i = 0; i < second_message.length(); i++){
        if (second_message[i] < '0' || second_message[i] > '9'){
            syntaxError(c);
            return -2;
        }
    }

    if (hash != atoi(second_message.c_str())){//not same
        loginError(c);
        perror("SERVER_LOGIN_FAILED");
        return -6;
    }

    //everything went ok---------------------------------
    sendOk(c);

    return 1;
}

int TCPServer::readPos (string mess, SpacePositon& pos){
    if ((mess.length() > 12) ||
        (mess.length() < 5)||
        (mess.substr(0, 3) != "OK ") ||
        (mess.substr(mess.length()-2, 2) != "\a\b") ){
            return -1;
    }
    mess = mess.substr(3, mess.length()-5);
    
    int space = 0;
    for (size_t i = 0; i < mess.length(); i++){
        if (mess[i] ==  ' ')
            space+=1;
        else if (mess[i] == '-'){
            if (i==(mess.length()-1))
                return -1;
            if (mess[i+1] < '0' || mess[i+1] > '9')
                return -1;
        }
        else if (mess[i] < '0' || mess[i] > '9'){
            return -1;
        } 
    }
    
    if (space != 1)
        return -1;

    int i = 0;
    for (i = 0; i < (int)mess.length(); i++){
        if (mess[i]==' ')
            break;
    }
    if (i == (int)mess.length()){
        return -1;
    }
    int fNum = atoi(mess.substr(0, i).c_str());
    int sNum = atoi(mess.substr(i+1, mess.length()-i-1).c_str());
    pos.x = fNum;
    pos.y = sNum;
    return 0;
}

int TCPServer::bindSocket(){
    struct sockaddr_in adresa;
	bzero(&adresa, sizeof(adresa));
	adresa.sin_family = AF_INET;
	adresa.sin_port = htons(port);
	adresa.sin_addr.s_addr = htonl(INADDR_ANY);

	// Prirazeni socketu k rozhranim
	if(bind(l, (struct sockaddr *) &adresa, sizeof(adresa)) < 0){
		return 0;
	}
    return 1;
}

int TCPServer::createSocket(){
    // Vytvoreni koncoveho bodu spojeni
	l = socket(AF_INET, SOCK_STREAM, 0);

	if(l < 0)
		return -1;
	return 0;
}

int TCPServer::createPort(int p){
    port = p;

	if(port == 0)
		return -1;
    
	return 0;
}

 bool TCPServer::isSecretHere (int c, int &result, string& message){
    //pick up the message
    pickUp(c);
    //read message---------------------------------------------------
    int messOut = whileRecharge(message, c, 100, TIMEOUT);
    if (messOut!=1){
        result = -1;
        return false;
    }
    //read input-----------------------------------------------------
    if (message.length() == 2){
        result = 0;
        return false;
    }
    return true;
 }

int TCPServer::navigateRobot( int c, RobotPosition & roboPos ){
    string message = "";
    //move front
    moveFront(c);

    //first position----------------------------------
    SpacePositon ps1(0, 0);
    int messOut = whileRecharge(message, c, 12, TIMEOUT);

    if (messOut!=1){
        //syntaxError(c);
        return messOut;
    }

    messOut = readPos(message, ps1);
    
    if (messOut!=0){
        syntaxError(c);
        return messOut;
    }

    bool prVal =  roboPos.at(ps1.x, ps1.y);

    roboPos.setPos(ps1.x, ps1.y);

    //test for secret message--------------------------
    if ( prVal == false && roboPos.inArea() == true && isSecretHere(c, messOut, message) == true){
        logOut(c);
        return 2;
    }
    if (messOut!=0){
        return messOut;
    }
    
    //second position----------------------------------
    SpacePositon ps2(0, 0);
    do{
        //move front
        moveFront(c);

        messOut = whileRecharge(message, c, 12, TIMEOUT);
        if (messOut!=1){
            return messOut;
        }

        messOut = readPos(message, ps2);
       
        if (messOut!=0){
            return messOut;
        }
    }while(ps1.x==ps2.x && ps1.y==ps2.y);

    prVal =  roboPos.at(ps2.x, ps2.y);

    roboPos.setPos(ps2.x, ps2.y);

    //test for secret message--------------------------
    if ( prVal == false && roboPos.inArea() == true && isSecretHere(c, messOut, message) == true){
        logOut(c);
        return 2;
    }
    if (messOut!=0){
        return messOut;
    }
    //-------------------------------------------------
    if (roboPos.isInCorner())
        return 1;

    int way = 0;
    while(true){
        //lead the way
        way = roboPos.bestWay();
        if (way == 0){
            return -1;
        }
        sendPossition(c, way);

        //read next message
        messOut = whileRecharge(message, c, 12, TIMEOUT);
        if (messOut!=1){
            return messOut;
        }

        //translate position and set
        messOut = readPos(message, ps2);
        if (messOut!=0){
            return messOut;
        }

        prVal =  roboPos.at(ps2.x, ps2.y);

        roboPos.setPos(ps2.x, ps2.y);


        //test for secret message--------------------------
        if ( prVal == false && roboPos.inArea() == true && isSecretHere(c, messOut, message) == true){
            logOut(c);
            return 2;
        }
        if (messOut!=0){
            return messOut;
        }
        //-------------------------------------------------

        if (roboPos.isInCorner())
            return 1;
    }
    return -1;
}

int TCPServer::findMessage( int c, RobotPosition & roboPos ){
    int way = 0;
    string message = "";
    int messOut = 0;
    //int waitOut = 0;
    SpacePositon ps2(0,0);
    while(true){
        //lead the way
        way = roboPos.bestWay();
        if (way == 0){
            return -1;
        }
        sendPossition(c, way);

        //read next message
        messOut = whileRecharge(message, c, 12, TIMEOUT);
        if (messOut!=1){
            //syntaxError(c);
            return messOut;
        }

        //translate position and set
        messOut = readPos(message, ps2);
        
        if (messOut!=0){
            return messOut;
        }
        bool prVal =  roboPos.at(ps2.x, ps2.y);
        roboPos.setPos(ps2.x, ps2.y);

        //test for secret message--------------------------
        if ( prVal == false && roboPos.inArea() == true && isSecretHere(c, messOut, message) == true){
            logOut(c);
            return 2;
        }
        if (messOut!=0){
            return messOut;
        }
        //-------------------------------------------------
    }
    return -1;
}

int TCPServer::aliveConnection( int c ){
    close(l);//copy of main thread has own reference, we dont neet it

    RobotPosition roboPos(0, 0);

    //authentizate
    int autRet = authentize( c );
    if (autRet != 1){
        close(c);
        return autRet;
    }

    autRet = navigateRobot( c, roboPos );
    if (autRet != 1){
        close(c);
        return autRet;
    }

    autRet = findMessage( c, roboPos );
    if (autRet != 1){
        close(c);
        return autRet;
    }
    //--------------------------------------------------------------------		
    close(c);
    return 0;
}



int TCPServer::start(int argc, char ** argv){

    if(argc < 2){
		cerr << "Usage: server port" << endl;
		return -1;
	}

	// Vytvoreni koncoveho bodu spojeni
	if (createSocket()!=0){
        perror("Nemohu vytvorit socket!");
		return -1;
    }

    //Creating of port
    if (createPort(atoi(argv[1]))!=0){
        cerr << "Usage: server port" << endl;
		close(l);
		return -1;
    }

    //Binding socket to localhost
    struct sockaddr_in adresa;
	bzero(&adresa, sizeof(adresa));
	adresa.sin_family = AF_INET;
	adresa.sin_port = htons(port);
	adresa.sin_addr.s_addr = htonl(INADDR_ANY);

	// Prirazeni socketu k rozhranim
	if(bind(l, (struct sockaddr *) &adresa, sizeof(adresa)) < 0){
        perror("Problem s bind()!");
		close(l);
		return -1;
	}

	//Set socket as pasive
	if(listen(l, 10) < 0){
		perror("Problem s listen()!");
		close(l);
		return -1;
	}

	struct sockaddr_in vzdalena_adresa;
	socklen_t velikost;

	while(true){
		//Extraction of first connection request on the queue
		int c = accept(l, (struct sockaddr *) &vzdalena_adresa, &velikost);

		if(c < 0){
			perror("Problem s accept()!");
			close(l);
			return -1;
		}

		pid_t pid = fork(); // new instance, proces

		if(pid == 0){
           int ress = aliveConnection(c);
           if (ress != 2){
               close(c);
               return ress;
           }

           return 0;
		}
		
		// Aby nam nezustavaly v systemu zombie procesy po kazdem obslouzeneme klientovi,
		// je nutne otestovat, zda se uz nejaky podproces ukoncil. 
		// Prvni argument rika, cekej na jakykoliv proces potomka, treti argument zajisti,
		// ze je funkce neblokujici (standardne blokujici je, coz ted opravdu nechceme).
		int status = 0;
   		waitpid(0, &status, WNOHANG);
		
		close(c); // Nove vytvoreny socket je nutne zavrit v hlavnim procesu, protoze by na nem v systemu 
			  // zustala reference a jeho zavreni v novem procesu by nemelo zadny efekt.
	}

	close(l);
	return 0;
}

int main(int argc, char ** argv){
    TCPServer server;
    if (server.start(argc, argv)!= 0)
        return 1;
    return 0;
}
