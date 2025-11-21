#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <map>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
using namespace std;
#include "include/odf_types.hpp"



struct Session {
    string session_id;
    UserInfo user;
    uint64_t login_time;
    uint64_t last_activity;
    
    Session() : session_id(""), user(), login_time(0), last_activity(0) {}
    
    Session(const string& id, const UserInfo& u) 
        : session_id(id), user(u), login_time(time(nullptr)), last_activity(time(nullptr)) {}
};

class SessionManager {
private:
    map<string, Session> sessions;
    
    string generateSessionId() {
        stringstream ss;
        ss << "sess_" << hex << rand() << "_" << time(nullptr);
        return ss.str();
    }
    
public:
 
    string createSession(const UserInfo& user) {
        string sessionId = generateSessionId();
        sessions[sessionId] = Session(sessionId, user);
        return sessionId;
    }
 
    bool isValidSession(const string& session_id) {
        return sessions.find(session_id) != sessions.end();
    }
    

    UserInfo* getUserFromSession(const string& session_id) {
        auto it = sessions.find(session_id);
        if (it != sessions.end()) {
            return &it->second.user;
        }
        return nullptr;
    }
    

    bool isAdmin(const string& session_id) {
        auto it = sessions.find(session_id);
        if (it != sessions.end()) {
            return it->second.user.role == UserRole::ADMIN;
        }
        return false;
    }
    
    
    bool destroySession(const string& session_id) {
        return sessions.erase(session_id) > 0;
    }
    

    void updateActivity(const string& session_id) {
        auto it = sessions.find(session_id);
        if (it != sessions.end()) {
            it->second.last_activity = time(nullptr);
        }
    }
};

#endif
