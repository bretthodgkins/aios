#-------------------------------------------------
#
# Project created by QtCreator 2015-03-16T09:17:44
#
#-------------------------------------------------

requires(qtHaveModule(concurrent))

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Aios
TEMPLATE = app

SOURCES += stwindow.cpp \
    main.cpp\
    mainwindow.cpp \
    action.cpp \
    agent.cpp \
    controller.cpp \
    cv.cpp \
    maccontroller.cpp \
    util.cpp

HEADERS  += stwindow.h \
    mainwindow.h \
    action.h \
    agent.h \
    controller.h \
    cv.h \
    maccontroller.h \
    nativecontroller.h \
    util.h


FORMS    += stwindow.ui \
    mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -stdlib=libc++

# Windows specific code
win32 {
    # This line can be used to copy resource files into the build directory
    #copyfiles.commands = @call copy <from> <to>
    #QMAKE_EXTRA_TARGETS += copyfiles
    #POST_TARGETDEPS += copyfiles
}

# MacOSX specific code
macx {
    # This is needed for mac mouse and keyboard input
    QMAKE_LFLAGS += -framework ApplicationServices

    # This is needed to compile QT applications on Mac OSX 10.10
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

    # This sets a custom Info.plist file, which is necessary to remove from dock and menubar
    QMAKE_INFO_PLIST = $${PWD}/CustomInfo.plist
    plist.target = ".$${DESTDIR}/$${TARGET}.app/Contents/Info.plist"
    plist.depends = $${PWD}/CustomInfo.plist
    plist.commands = $(DEL_FILE) \".$${DESTDIR}/$${TARGET}.app/Contents/Info.plist\" $$escape_expand(\n\t) \
                     $(COPY_FILE) $${PWD}/CustomInfo.plist \".$${DESTDIR}/$${TARGET}.app/Contents/Info.plist\"
    QMAKE_EXTRA_TARGETS += plist
    PRE_TARGETDEPS += $$plist.target

    # This copies resource files into the .app bundle
    copyfiles.commands = cp -r $${PWD}/resources/graphics/ .$${DESTDIR}/$${TARGET}.app/Contents/Resources/graphics/ |
    copyfiles.commands += cp -r $${PWD}/resources/img/ .$${DESTDIR}/$${TARGET}.app/Contents/Resources/img/ |
    copyfiles.commands += cp $${PWD}/resources/plans.txt .$${DESTDIR}/$${TARGET}.app/Contents/Resources/
    QMAKE_EXTRA_TARGETS += copyfiles
    POST_TARGETDEPS += copyfiles

}

LIBS += -L/opt/local/lib \
        -lopencv_core.2.4.11 \
        -lopencv_highgui.2.4.11 \
        -lopencv_imgproc.2.4.11
INCLUDEPATH += /opt/local/include \
               /usr/lib \
               /usr/local/lib

RESOURCES = resources.qrc

