TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ast.cpp \
    main.cpp \
    nodetype.cpp \
    funinterface.cpp \
    scope.cpp \
    output.cpp \
    copynode.cpp

HEADERS += \
    ast.h \
    nodetype.h \
    scope.h \
    funinterface.h \
    output.h \
    excep.h \
    marco.h
