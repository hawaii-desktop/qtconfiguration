QT += qml quick configuration

LIBS += -L$$QT.accountsservice.libs

SOURCES += main.cpp configurationitem.cpp

load(qml_plugin)
