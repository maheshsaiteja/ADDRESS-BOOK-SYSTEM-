#include "address_book.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

AddressBook book;

string urlDecode(const string& s) {
    string out;
    for (size_t i=0;i<s.size();++i) {
        if (s[i]=='+' ) out+=' ';
        else if (s[i]=='%' && i+2<s.size()) {
            string h=s.substr(i+1,2);
            char ch=(char)strtol(h.c_str(),nullptr,16);
            out+=ch; i+=2;
        } else out+=s[i];
    }
    return out;
}

string htmlEscape(const string& s) {
    string r;
    for(char c:s) {
        if(c=='&') r+="&amp;";
        else if(c=='<') r+="&lt;";
        else if(c=='>') r+="&gt;";
        else if(c=='"') r+="&quot;";
        else r+=c;
    }
    return r;
}

map<string,string> formData(const string& body) {
    map<string,string> m;
    stringstream ss(body), pair;
    string item;
    while(getline(ss,item,'&')) {
        size_t p=item.find('=');
        if(p!=string::npos) m[urlDecode(item.substr(0,p))]=urlDecode(item.substr(p+1));
    }
    return m;
}

string cookieUser(const string& headers) {
    size_t p=headers.find("Cookie:");
    if(p==string::npos) return "";
    size_t end=headers.find("\r\n",p);
    string line=headers.substr(p,end-p);
    string key="user=";
    p=line.find(key);
    if(p==string::npos) return "";
    p+=key.size();
    size_t e=line.find(';',p);
    return line.substr(p,e==string::npos?string::npos:e-p);
}

string groupName(string g) {
    if(g=="Family") return "👨‍👩‍👧";
    if(g=="College") return "🎓";
    if(g=="Friends") return "👥";
    if(g=="Work") return "💼";
    return "📁";
}

string page(const string& title, const string& body, const string& user="") {
    string authNav = user.empty() ? "" :
      "<div class='userbar'><span>Signed in as <b>"+htmlEscape(user)+"</b></span><a href='/profile'>Profile</a><a href='/logout'>Logout</a></div>";
    return R"HTML(<!doctype html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>)HTML"+title+R"HTML( | Address Book</title><style>
*{box-sizing:border-box}body{margin:0;font-family:Segoe UI,Arial;background:#f4f6fb;color:#172033}
a{text-decoration:none;color:inherit}.wrap{max-width:1180px;margin:auto;padding:28px}.top{display:flex;justify-content:space-between;align-items:center;margin-bottom:25px}.brand{font-weight:800;font-size:20px}.brand span{color:#5b5ce2}.userbar{display:flex;gap:15px;align-items:center;font-size:13px;color:#667085}.userbar a{padding:9px 12px;background:white;border:1px solid #e4e7ef;border-radius:9px}
.hero{background:linear-gradient(120deg,#171c3b,#4248a0);color:white;border-radius:22px;padding:32px;margin-bottom:20px}.hero h1{font-size:34px;margin:8px 0}.hero p{color:#d7daf0;line-height:1.6}
.btn{display:inline-block;border:0;border-radius:10px;padding:11px 15px;background:#5b5ce2;color:white;font-weight:700;cursor:pointer}.btn.gray{background:#edf0f6;color:#4d566d}.btn.red{background:#e95762}.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:15px}.card{background:white;border:1px solid #e5e8f0;border-radius:16px;padding:18px;box-shadow:0 10px 30px #26334b0b}.card h3{margin:0 0 6px}.muted{color:#7d879b;font-size:13px}.stats{display:grid;grid-template-columns:repeat(4,1fr);gap:14px;margin:18px 0}.stat{background:white;border:1px solid #e5e8f0;border-radius:15px;padding:20px}.stat b{display:block;font-size:25px;margin-top:6px}.avatar{width:46px;height:46px;border-radius:14px;background:#e8e9ff;color:#4c4db9;display:grid;place-items:center;font-weight:800}.ctop{display:flex;gap:12px;align-items:center}.actions{display:flex;gap:7px;margin-top:15px;border-top:1px solid #edf0f5;padding-top:12px}.search{width:100%;padding:13px;border:1px solid #e1e5ee;border-radius:10px;margin:12px 0 18px;outline:none}.form{max-width:650px}.form label{display:block;font-size:12px;font-weight:700;color:#586174;margin-top:12px}.form input,.form select,.form textarea{width:100%;padding:12px;border:1px solid #dfe3ec;border-radius:9px;margin-top:6px}.form button{margin-top:17px}.groups{display:grid;grid-template-columns:repeat(5,1fr);gap:14px}.group{padding:25px;background:white;border:1px solid #e5e8f0;border-radius:16px;text-align:center}.group .icon{font-size:28px}.notice{padding:12px 15px;border-radius:10px;background:#e9f8f0;color:#26754d;margin-bottom:15px}.error{background:#fff0f1;color:#a93540}.empty{text-align:center;padding:45px;background:white;border:1px dashed #d8dce7;border-radius:15px;color:#7d879b}.footer{text-align:center;color:#929aab;font-size:12px;padding:30px}.nav{display:flex;gap:8px;flex-wrap:wrap;margin-bottom:18px}.nav a{padding:9px 12px;background:white;border:1px solid #e4e7ef;border-radius:9px;font-size:13px}
@media(max-width:800px){.grid{grid-template-columns:1fr 1fr}.stats{grid-template-columns:1fr 1fr}.groups{grid-template-columns:1fr 1fr}.top{align-items:flex-start;gap:10px}.userbar{flex-wrap:wrap}}
@media(max-width:520px){.wrap{padding:16px}.grid,.stats,.groups{grid-template-columns:1fr}.hero h1{font-size:27px}}
</style></head><body><div class="wrap"><div class="top"><a class="brand" href="/dashboard">Address<span>Book</span> <small>• Project 06</small></a>)HTML"+authNav+R"HTML(</div>)HTML"+body+R"HTML(<div class="footer">C++ Address Book System • Structures • String Handling • Searching • File Handling</div></div></body></html>)HTML";
}

string contactCard(const Contact& c, const string& user) {
    string initial;
    for(char x:c.name) if(isalnum((unsigned char)x)) { initial += (char)toupper((unsigned char)x); if(initial.size()==2) break; }
    return "<div class='card'><div class='ctop'><div class='avatar'>"+initial+
      "</div><div><h3>"+htmlEscape(c.name)+"</h3><span class='muted'>"+htmlEscape(c.group)+"</span></div></div>"+
      "<p class='muted'>☎ "+htmlEscape(c.phone)+"</p><p class='muted'>✉ "+htmlEscape(c.email)+"</p><p class='muted'>⌖ "+htmlEscape(c.address)+"</p>"+
      "<div class='actions'><a class='btn gray' href='/edit?id="+to_string(c.id)+"'>Edit</a>"+
      "<a class='btn gray' href='/fav?id="+to_string(c.id)+"'>"+(c.favorite?"★ Unfavorite":"☆ Favorite")+"</a>"+
      "<a class='btn red' href='/delete?id="+to_string(c.id)+"' onclick=\"return confirm('Delete this contact?')\">Delete</a></div></div>";
}

string loginPage(const string& msg="", bool error=false) {
    string n=msg.empty()?"":"<div class='notice "+string(error?"error":"")+"'>"+htmlEscape(msg)+"</div>";
    string body="<div style='max-width:450px;margin:70px auto'>"
      "<div class='card' style='padding:30px'><div class='avatar'>AB</div><h1>Welcome back</h1>"
      "<p class='muted'>Sign in to your personal address book.</p>"+n+
      "<form class='form' method='POST' action='/login'><label>Account Name<input name='username' required></label>"
      "<label>Password<input name='password' type='password' required></label><button class='btn'>Sign In</button></form>"
      "<p class='muted' style='margin-top:20px'>New here? <a style='color:#5b5ce2;font-weight:700' href='/register'>Create an account</a></p></div></div>";
    return page("Sign In",body);
}

string registerPage(const string& msg="") {
    string body="<div style='max-width:500px;margin:45px auto'><div class='card' style='padding:30px'><div class='avatar'>+</div>"
      "<h1>Create your account</h1><p class='muted'>Your contacts are separated by account name.</p>";
    if(!msg.empty()) body+="<div class='notice error'>"+htmlEscape(msg)+"</div>";
    body+="<form class='form' method='POST' action='/register'><label>Account Name<input name='username' minlength='3' required></label>"
      "<label>Password<input name='password' type='password' minlength='4' required></label>"
      "<label>Confirm Password<input name='confirm' type='password' minlength='4' required></label>"
      "<button class='btn'>Create Account</button></form><p class='muted' style='margin-top:20px'><a href='/'>← Back to sign in</a></p></div></div>";
    return page("Create Account",body);
}

string dashboard(const string& user) {
    auto cs=book.getContacts(user); auto fav=book.getFavorites(user);
    string body="<div class='hero'><span>PROJECT 06 • C++</span><h1>Your Address Book</h1><p>Store, search, group and manage your contacts using a C++ based system.</p><a class='btn' href='/add'>＋ Add New Contact</a></div>"
      "<div class='nav'><a href='/dashboard'>Dashboard</a><a href='/contacts'>Contacts</a><a href='/groups'>Groups</a><a href='/favorites'>Favorites</a><a href='/about'>About Project</a></div>"
      "<div class='stats'><div class='stat'>Total Contacts<b>"+to_string(cs.size())+"</b></div><div class='stat'>Favorites<b>"+to_string(fav.size())+"</b></div><div class='stat'>Groups<b>5</b></div><div class='stat'>C++ Core<b>ON</b></div></div>"
      "<h2>Recent Contacts</h2><div class='grid'>";
    if(cs.empty()) body+="<div class='empty' style='grid-column:1/-1'>No contacts yet. Create your first contact.</div>";
    else {
        int start=max(0,(int)cs.size()-6);
        for(int i=(int)cs.size()-1;i>=start;--i) body+=contactCard(cs[i],user);
    }
    body+="</div>";
    return page("Dashboard",body,user);
}

string contactsPage(const string& user, string query="") {
    auto cs=book.getContacts(user); vector<Contact> result;
    for(auto c:cs) {
        string all=c.name+" "+c.phone+" "+c.email+" "+c.address+" "+c.group;
        string a=all,b=query;
        for(char& x:a)x=(char)tolower((unsigned char)x); for(char& x:b)x=(char)tolower((unsigned char)x);
        if(b.empty()||a.find(b)!=string::npos) result.push_back(c);
    }
    string body="<h1>All Contacts</h1><p class='muted'>Search and manage your saved contacts.</p>"
      "<form method='GET' action='/contacts'><input class='search' name='q' value='"+htmlEscape(query)+"' placeholder='Search name, phone, email, address or group...'></form>"
      "<a class='btn' href='/add'>＋ Add Contact</a><div class='grid' style='margin-top:18px'>";
    if(result.empty()) body+="<div class='empty' style='grid-column:1/-1'>No matching contacts found.</div>";
    else for(const auto& c:result) body+=contactCard(c,user);
    body+="</div>";
    return page("Contacts",body,user);
}

string groupsPage(const string& user) {
    vector<string> gs={"Family","College","Friends","Work","Other"};
    string body="<h1>Contact Groups</h1><p class='muted'>Click a group to open only the contacts in that group.</p><div class='groups' style='margin-top:20px'>";
    for(auto& g:gs) {
        auto cs=book.getGroup(user,g);
        body+="<a class='group' href='/group?name="+g+"'><div class='icon'>"+groupName(g)+"</div><h3>"+g+"</h3><p class='muted'>"+to_string(cs.size())+" contact"+(cs.size()==1?"":"s")+"</p></a>";
    }
    body+="</div>";
    return page("Groups",body,user);
}

string groupPage(const string& user, const string& group) {
    auto cs=book.getGroup(user,group);
    string body="<a class='muted' href='/groups'>← Back to Groups</a><h1 style='margin-bottom:4px'>"+groupName(group)+" "+htmlEscape(group)+" Contacts</h1><p class='muted'>Only contacts assigned to this group are displayed.</p><div class='grid' style='margin-top:20px'>";
    if(cs.empty()) body+="<div class='empty' style='grid-column:1/-1'>No contacts in "+htmlEscape(group)+" yet.</div>";
    else for(const auto& c:cs) body+=contactCard(c,user);
    body+="</div>";
    return page(group+" Contacts",body,user);
}

string contactFormPage(const string& user, const Contact* c=nullptr) {
    bool edit=c;
    string body="<h1>"+string(edit?"Edit Contact":"Add New Contact")+"</h1><p class='muted'>Contact data is stored by the C++ program.</p>"
      "<form class='form card' style='margin-top:18px;padding:25px' method='POST' action='"+string(edit?"/edit":"/add")+"'>";
    if(edit) body+="<input type='hidden' name='id' value='"+to_string(c->id)+"'>";
    body+="<label>Full Name<input name='name' required value='"+(edit?htmlEscape(c->name):"")+"'></label>"
      "<label>Phone Number<input name='phone' required value='"+(edit?htmlEscape(c->phone):"")+"'></label>"
      "<label>Email<input name='email' type='email' value='"+(edit?htmlEscape(c->email):"")+"'></label>"
      "<label>Group<select name='group'>";
    for(auto& g:vector<string>{"Family","College","Friends","Work","Other"})
        body+="<option "+string(edit&&c->group==g?"selected":"")+">"+g+"</option>";
    body+="</select></label><label>Address<textarea name='address' rows='4'>"+(edit?htmlEscape(c->address):"")+"</textarea></label>"
      "<button class='btn'>"+string(edit?"Update Contact":"Save Contact")+"</button> <a class='btn gray' href='/contacts'>Cancel</a></form>";
    return page(edit?"Edit Contact":"Add Contact",body,user);
}

string profilePage(const string& user, const string& msg="") {
    string body="<h1>My Profile</h1><p class='muted'>Account settings for <b>"+htmlEscape(user)+"</b>.</p>";
    if(!msg.empty()) body+="<div class='notice'>"+htmlEscape(msg)+"</div>";
    body+="<div class='grid' style='margin-top:20px'><div class='card'><div class='avatar'>"+htmlEscape(user.substr(0,1))+
      "</div><h2>"+htmlEscape(user)+"</h2><p class='muted'>Address Book Account</p><p>Contacts: "+to_string(book.getContacts(user).size())+"</p></div>"
      "<div class='card'><h2>Change Password</h2><form class='form' method='POST' action='/password'><label>Old Password<input name='old' type='password' required></label>"
      "<label>New Password<input name='new' type='password' minlength='4' required></label><label>Confirm New Password<input name='confirm' type='password' required></label>"
      "<button class='btn'>Change Password</button></form></div></div>";
    return page("Profile",body,user);
}

string aboutPage(const string& user) {
    string body="<h1>Project 06 — Address Book System</h1><p class='muted'>Project implementation, documentation and viva voce.</p>"
      "<div class='grid' style='margin-top:20px'>"
      "<div class='card'><h3>Objective</h3><p class='muted'>Create a simple C++ system to store and manage contact information efficiently.</p></div>"
      "<div class='card'><h3>Data Structure Design</h3><p class='muted'>The Contact and User structures store the required fields for each record.</p></div>"
      "<div class='card'><h3>Searching Logic</h3><p class='muted'>Search can match name, phone, email, address and group.</p></div>"
      "<div class='card'><h3>Module Explanation</h3><p class='muted'>Account, Contacts, Groups, Favorites and Profile modules.</p></div>"
      "<div class='card'><h3>Sample Outputs</h3><p class='muted'>The browser interface demonstrates the operations of the C++ application.</p></div>"
      "<div class='card'><h3>Enhancements</h3><p class='muted'>Future versions can add database storage, encryption and a production API.</p></div></div>";
    return page("About Project",body,user);
}

void sendResponse(SOCKET s, const string& body, int code=200, const string& extra="") {
    string status=code==200?"200 OK":code==302?"302 Found":"400 Bad Request";
    string header="HTTP/1.1 "+status+"\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length: "+to_string(body.size())+"\r\nConnection: close\r\n"+extra+"\r\n";
    send(s,header.c_str(),(int)header.size(),0);
    send(s,body.c_str(),(int)body.size(),0);
}

void redirect(SOCKET s,const string& location,const string& cookie="") {
    sendResponse(s,"",302,"Location: "+location+"\r\n"+cookie);
}

void handle(SOCKET client) {
    char buffer[65536]{};
    int received=recv(client,buffer,sizeof(buffer)-1,0);
    if(received<=0){closesocket(client);return;}
    string req(buffer,received);
    size_t first=req.find("\r\n");
    if(first==string::npos){closesocket(client);return;}
    string requestLine=req.substr(0,first);
    stringstream rl(requestLine);
    string method,target,http; rl>>method>>target>>http;
    string headers=req.substr(first+2);
    string body;
    size_t bp=req.find("\r\n\r\n");
    if(bp!=string::npos) body=req.substr(bp+4);
    string user=cookieUser(req);

    size_t qpos=target.find('?');
    string path=qpos==string::npos?target:target.substr(0,qpos);
    string query=qpos==string::npos?"":target.substr(qpos+1);
    auto q=formData(query);

    if(path=="/" && method=="GET"){sendResponse(client,loginPage());}
    else if(path=="/register" && method=="GET"){sendResponse(client,registerPage());}
    else if(path=="/register" && method=="POST"){
        auto f=formData(body); string msg;
        if(f["password"]!=f["confirm"]) sendResponse(client,registerPage("Passwords do not match."));
        else if(book.registerUser(f["username"],f["password"],msg)) redirect(client,"/dashboard","Set-Cookie: user="+f["username"]+"; Path=/\r\n");
        else sendResponse(client,registerPage(msg));
    }
    else if(path=="/login" && method=="POST"){
        auto f=formData(body);
        if(book.login(f["username"],f["password"])) redirect(client,"/dashboard","Set-Cookie: user="+f["username"]+"; Path=/\r\n");
        else sendResponse(client,loginPage("Invalid account name or password.",true));
    }
    else if(path=="/logout"){redirect(client,"/","Set-Cookie: user=; Max-Age=0; Path=/\r\n");}
    else if(user.empty()){redirect(client,"/");}
    else if(path=="/dashboard"){sendResponse(client,dashboard(user));}
    else if(path=="/contacts"){sendResponse(client,contactsPage(user,q["q"]));}
    else if(path=="/groups"){sendResponse(client,groupsPage(user));}
    else if(path=="/group"){sendResponse(client,groupPage(user,q["name"]));}
    else if(path=="/favorites"){
        auto cs=book.getFavorites(user); string body2="<h1>Favorite Contacts</h1><p class='muted'>Your starred contacts.</p><div class='grid' style='margin-top:20px'>";
        if(cs.empty()) body2+="<div class='empty' style='grid-column:1/-1'>No favorites yet.</div>";
        else for(auto& c:cs) body2+=contactCard(c,user);
        body2+="</div>"; sendResponse(client,page("Favorites",body2,user));
    }
    else if(path=="/add" && method=="GET"){sendResponse(client,contactFormPage(user));}
    else if(path=="/add" && method=="POST"){
        auto f=formData(body); Contact c; c.owner=user;c.name=f["name"];c.phone=f["phone"];c.email=f["email"];c.address=f["address"];c.group=f["group"];
        string msg; if(book.addContact(c,msg)) redirect(client,"/contacts"); else sendResponse(client,page("Error","<div class='card'><h2>"+htmlEscape(msg)+"</h2><a class='btn' href='/add'>Try Again</a></div>",user),400);
    }
    else if(path=="/edit" && method=="GET"){
        int id=atoi(q["id"].c_str()); auto c=book.findContact(id,user);
        if(c) sendResponse(client,contactFormPage(user,c)); else sendResponse(client,page("Not Found","<div class='empty'>Contact not found.</div>",user),400);
    }
    else if(path=="/edit" && method=="POST"){
        auto f=formData(body); Contact c; c.name=f["name"];c.phone=f["phone"];c.email=f["email"];c.address=f["address"];c.group=f["group"];
        string msg; if(book.updateContact(atoi(f["id"].c_str()),user,c,msg)) redirect(client,"/contacts"); else sendResponse(client,page("Error","<div class='card'><h2>"+htmlEscape(msg)+"</h2><a class='btn' href='/contacts'>Back</a></div>",user),400);
    }
    else if(path=="/delete"){
        string msg; book.deleteContact(atoi(q["id"].c_str()),user,msg); redirect(client,"/contacts");
    }
    else if(path=="/fav"){
        string msg; book.toggleFavorite(atoi(q["id"].c_str()),user,msg); redirect(client,"/contacts");
    }
    else if(path=="/profile" && method=="GET"){sendResponse(client,profilePage(user));}
    else if(path=="/password" && method=="POST"){
        auto f=formData(body);
        if(f["new"]!=f["confirm"]) sendResponse(client,profilePage(user,"New passwords do not match."));
        else {string msg;book.changePassword(user,f["old"],f["new"],msg);sendResponse(client,profilePage(user,msg));}
    }
    else if(path=="/about"){sendResponse(client,aboutPage(user));}
    else sendResponse(client,page("404","<div class='empty'><h2>404</h2><p>Page not found.</p><a class='btn' href='/dashboard'>Dashboard</a></div>",user),400);

    closesocket(client);
}

int main() {
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){cerr<<"Winsock startup failed.\n";return 1;}
    SOCKET server=socket(AF_INET,SOCK_STREAM,0);
    if(server==INVALID_SOCKET){cerr<<"Socket creation failed.\n";WSACleanup();return 1;}

    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    addr.sin_port=htons(8080);

    if(bind(server,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR){
        cerr<<"Port 8080 is busy or unavailable.\n";closesocket(server);WSACleanup();return 1;
    }
    listen(server,10);

    cout<<"============================================\n";
    cout<<"       ADDRESS BOOK SYSTEM - PROJECT 06\n";
    cout<<"============================================\n";
    cout<<"C++ local web server is running.\n";
    cout<<"Open: http://127.0.0.1:8080\n";
    cout<<"Press Ctrl+C to stop.\n\n";

    while(true){
        SOCKET client=accept(server,nullptr,nullptr);
        if(client!=INVALID_SOCKET) handle(client);
    }
}
