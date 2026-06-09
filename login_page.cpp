#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<memory>
#include<algorithm>
using namespace std;
//=================================================
//USER DOMAIN MODEL CLASS
class User{
    private:
    std::string m_username;
    std::string m_password;// plain text password for now

    public:
        User(std::string username, std::string password): m_username(std::move(username)),m_password(std::move(password)){}

        std::string getUsername() const { return m_username;}
        std::string getPassword() const {return m_password;}
};
// ==================================================
// Authentication Manager class (handles systems' core logic)
//=======================================================
class AuthManager {
    private:
    string m_dbFilename;
    
    // checks if a username already exists in the text file
    bool checkUsername(const string&  username) const {
        ifstream file(m_dbFilename);
        if(!file.is_open()) return false;

        // Read upto colon seperator
        string line;
        while (getline(file,line)){
            stringstream ss(line);
            string existingUser;
            if (getline(ss, existingUser, ':')){
                if (existingUser == username){
                    return true;
                }
            }
        }
        return false;
    }
    public:
    explicit AuthManager(string dbFilename) : m_dbFilename(move(dbFilename)) {}
        // read up to the colon seperator
        void registerUser(){
            string username,password;
            cout << "\n-- Register ---\n";
            cout << "Enter Username: ";
            cin >> username;
        
        // verify the username isn't already taken 
        if (checkUsername(username)){
            cout << "[ERROR] Username Already exists. Registration Denied\n";
            return;
        }
        cout << "Enter Password:";
        cin >> password;

        // save direcly as plain text format -> for now later will do hashing and salting -> username: password
        ofstream file(m_dbFilename, ios::app);
        if (!file.is_open()){
            cout << "[SYSTEM ERROR] Could not open data file for writing.\n";
            return;
        }
    file << username << ":" << password << "\n";

}
bool loginUser() const {
    string username, password;
    cout << "\n -- login --\n";
    cout << "Enter username";
    cin >> username;
    cout << "Enter password";
    cin >> password;

    ifstream file(m_dbFilename);
    if (!file.is_open()){
        cout << "[ERROR] No registered users found yet .\n";
        return false;
    }
    string line;
    while(getline(file,line)){
        stringstream ss(line);
        string storedUser, storedPassword;
        // split the line into username and password fields using the ":" delimiter
        if (getline(ss,storedUser,':')&& getline(ss,storedPassword)){
            if (storedUser == username && storedPassword == password){
                cout << "{Success Acess Granted. Welcome back]" << username << "!\n";
                return true;
            }
        }
    }
     cout << "[ERROR] Invalid username or password. \n";
    return false;
}
   
    };
int main(){
    auto authSystem = make_unique<AuthManager>("user_credentials.dat");
    string choice;

    while (true){
        cout << "\n=====================================================\n";
        cout << "        AUTHENTICATION APP (PLAIN)                       ";
        // later to be made with hashing and salting
        cout << "=======================================================\n";
        cout << "1. Register an account\n";
        cout << "2. Login to account\n";
        cout << "3. Exist system\n";
        cout << "choose option (1-3)\n";
        cin >> choice;

        if (choice == "1"){
            authSystem->registerUser();
        } else if (choice == "2"){
            authSystem->loginUser();
        }else if (choice == "3"){
            cout << "Exiting application context safely. Goodbye .\n";
        } else {
            cout << "[INVALID OPTION] Please type numeric choice values 1,2 or 3\n";
        }
        return 0;
    }
}
