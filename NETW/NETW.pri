# NETW.pri

#网络
INCLUDEPATH += $$PWD/TCPClient
include ($$PWD/TCPClient/TCPClient.pri)

INCLUDEPATH += $$PWD/TCPServer
include ($$PWD/TCPServer/TCPServer.pri)

INCLUDEPATH += $$PWD/HTTP
include ($$PWD/HTTP/HTTP.pri)

INCLUDEPATH += $$PWD/UDP
include ($$PWD/UDP/UDP.pri)

INCLUDEPATH += $$PWD/UDPMulticast
include ($$PWD/UDPMulticast/UDPMulticast.pri)

FORMS += \
    $$PWD/netw.ui

HEADERS += \
    $$PWD/netw.h

SOURCES += \
    $$PWD/netw.cpp
