TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    client.cpp \
    semaphore.cpp \
    shmemory.cpp

HEADERS += \
    semaphore.h \
    shmemory.h
