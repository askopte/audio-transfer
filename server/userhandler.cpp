#include "userhandler.h"
#include "size.h"
#include "server/servermain.h"

UserHandler::UserHandler(QObject *parent) :
    QObject(parent)
{
    //bascicly: bytesRead store all the "offline" users data read (after they disconnected)
    this->bytesRead = 0;
}
bool UserHandler::append(User *user) {

    this->users.append(user);
    connect(user,SIGNAL(sockClose(User*)),this,SLOT(sockClose(User*)));
    connect(user,SIGNAL(debug(QString)),this,SIGNAL(debug(QString)));
    connect(user,SIGNAL(kicked()),this,SLOT(kicked()));
    return true;
}
void UserHandler::say(const QString message) {
    emit(debug("UserHandler: " + message));
}
void UserHandler::sockClose(User *user) {
    const int pos = users.indexOf(user);
    if (pos < 0) return;
    //Adding the total bytes read of this user to the main counter (offline counter)
    bytesRead += user->getBytesCount();

    say("deleting user: " + user->objectName());
    delete(users.at(pos));
    users.removeAt(pos);
    showUsersOnline();
}
void UserHandler::showUsersOnline() {
    say("current online users:");
    int count = 0;
    QList<User*>::Iterator i;
    for (i = users.begin() ; i != users.end() ; i++) {
        User* x = (User*) *i;
        say(QString::number(count++) + ": " + x->objectName() + QString(" -> readed: ") + Size::getWsize(x->getBytesCount()));
    }
    say("total readed data: " + Size::getWsize(getBytesRead()));
    if (!count) say("no user(s) online.");
    else say("end of list");
}
void UserHandler::killAll(const QString reason) {
    QList<User*>::Iterator i;
    for (i = users.begin() ; i != users.end() ; i++) {
        User* x = (User*) *i;
        x->kill(reason);
    }
}
bool UserHandler::contains(const QObject *socket) {
    QList<User*>::Iterator i;
    for (i = users.begin() ; i != users.end() ; i++) {
        User* x = (User*) *i;
        if (x->getSocketPointer() == socket) return true;
    }
    return false;
}
int UserHandler::indexOf(const QObject *socket) {
    int pos = -1;
    QList<User*>::Iterator i;
    for (i = users.begin() ; i != users.end() ; i++) {
        pos++;
        User* x = (User*) *i;
        if (x->getSocketPointer() == socket) return pos;
    }
    return -1;
}
User* UserHandler::at(const int pos) {
    if (users.count() < pos) return NULL;
    return users.at(pos);
}
quint64 UserHandler::getBytesRead() {
    return bytesRead + getBytesReadForConnected();
}
void UserHandler::kicked() {
    User* user = (User*) sender();
    sockClose(user);
}
Readini* UserHandler::getIni() {
    return qobject_cast<ServerMain*>(this->parent())->getIni();
}
ServerSecurity* UserHandler::callSecurity() {
    return qobject_cast<ServerMain*>(this->parent())->security;
}
quint64 UserHandler::getBytesReadForConnected() {
    quint64 size = 0;
    QList<User*>::iterator i;
    for (i = users.begin() ; i != users.end() ; i++) size += (*i)->getBytesCount();
    return size;
}
