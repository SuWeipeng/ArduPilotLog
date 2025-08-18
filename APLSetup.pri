QMAKE_POST_LINK += echo "Copying files"

#
# Copy the application resources to the associated place alongside the application
#

LinuxBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR
}

MacBuild {
    DESTDIR_COPY_RESOURCE_LIST = $$DESTDIR/$${TARGET}.app/Contents/MacOS
}

# Windows version of QMAKE_COPY_DIR of course doesn't work the same as Mac/Linux. It will only
# copy the contents of the source directory. It doesn't create the top level source directory
# in the target.
WindowsBuild {
    # Make sure to keep both side of this if using the same set of directories
    DESTDIR_COPY_RESOURCE_LIST = $$replace(DESTDIR,"/","\\")
    BASEDIR_COPY_RESOURCE_LIST = $$replace(BASEDIR,"/","\\")
#   QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY_DIR \"$$BASEDIR_COPY_RESOURCE_LIST\\resources\\flightgear\" \"$$DESTDIR_COPY_RESOURCE_LIST\\flightgear\"
} else {
    !MobileBuild {
        # Make sure to keep both sides of this if using the same set of directories
#       QMAKE_POST_LINK += && $$QMAKE_COPY_DIR $$BASEDIR/resources/flightgear $$DESTDIR_COPY_RESOURCE_LIST
    }
}

#
# Perform platform specific setup
#

iOSBuild | MacBuild {
    # Update version info in bundle
    QMAKE_POST_LINK += && /usr/libexec/PlistBuddy -c \"Set :CFBundleShortVersionString $${MAC_VERSION}\" $$DESTDIR/$${TARGET}.app/Contents/Info.plist
    QMAKE_POST_LINK += && /usr/libexec/PlistBuddy -c \"Set :CFBundleVersion $${MAC_BUILD}\" $$DESTDIR/$${TARGET}.app/Contents/Info.plist
}

MacBuild {
    # Copy non-standard frameworks into app package
    QMAKE_POST_LINK += && rsync -a --delete $$BASEDIR/libs/lib/Frameworks $$DESTDIR/$${TARGET}.app/Contents/
}

WindowsBuild {
    BASEDIR_WIN = $$replace(BASEDIR, "/", "\\")
    DESTDIR_WIN = $$replace(DESTDIR, "/", "\\")
    QT_BIN_DIR  = $$dirname(QMAKE_QMAKE)

    # Copy dependencies
    DebugBuild: DLL_QT_DEBUGCHAR = "d"
    ReleaseBuild: DLL_QT_DEBUGCHAR = ""

    for(COPY_FILE, COPY_FILE_LIST) {
        QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"$$COPY_FILE\" \"$$DESTDIR_WIN\"
    }

#    ReleaseBuild {
#        # Copy Visual Studio DLLs
#        # Note that this is only done for release because the debugging versions of these DLLs cannot be redistributed.
#        win32-msvc2010 {
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp100.dll\"  \"$$DESTDIR_WIN\"
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr100.dll\"  \"$$DESTDIR_WIN\"
#
#        } else:win32-msvc2012 {
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp110.dll\"  \"$$DESTDIR_WIN\"
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr110.dll\"  \"$$DESTDIR_WIN\"
#
#        } else:win32-msvc2013 {
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp120.dll\"  \"$$DESTDIR_WIN\"
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcr120.dll\"  \"$$DESTDIR_WIN\"
#
#        } else:win32-msvc2015 {
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\msvcp140.dll\"  \"$$DESTDIR_WIN\"
#            QMAKE_POST_LINK += $$escape_expand(\\n) $$QMAKE_COPY \"C:\\Windows\\System32\\vcruntime140.dll\"  \"$$DESTDIR_WIN\"
#
#        } else {
#            error("Visual studio version not supported, installation cannot be completed.")
#        }
#    }

    DEPLOY_TARGET = $$shell_quote($$shell_path($$DESTDIR_WIN\\$${TARGET}.exe))
    QMAKE_POST_LINK += $$escape_expand(\\n) $$QT_BIN_DIR\\windeployqt --no-compiler-runtime --qmldir=$${BASEDIR_WIN}\\src $${DEPLOY_TARGET}
}

LinuxBuild {
    QMAKE_POST_LINK += && mkdir -p $$DESTDIR/Qt/libs && mkdir -p $$DESTDIR/Qt/plugins

    # QT_INSTALL_LIBS
    QT_LIB_LIST = \
        libQt6Core.so.6 \
        libQt6DBus.so.6 \
        libQt6Gui.so.6 \
        libQt6Location.so.6 \
        libQt6Multimedia.so.6 \
        libQt6Network.so.6 \
        libQt6OpenGL.so.6 \
        libQt6Positioning.so.6 \
        libQt6PrintSupport.so.6 \
        libQt6Qml.so.6 \
        libQt6Quick.so.6 \
        libQt6QuickControls2.so.6 \
        libQt6QuickTemplates2.so.6 \
        libQt6QuickWidgets.so.6 \
        libQt6SerialPort.so.6 \
        libQt6Sql.so.6 \
        libQt6Svg.so.6 \
        libQt6Test.so.6 \
        libQt6Widgets.so.6 \
        libQt6XcbQpa.so.6 \
        libQt6Xml.so.6 \
        libQt6TextToSpeech.so.6


    for(QT_LIB, QT_LIB_LIST) {
        QMAKE_POST_LINK += && $$QMAKE_COPY --dereference $$[QT_INSTALL_LIBS]/$$QT_LIB $$DESTDIR/Qt/libs/
    }

    # QT_INSTALL_PLUGINS
    QT_PLUGIN_LIST = \
        bearer \
        geoservices \
        iconengines \
        imageformats \
        platforminputcontexts \
        platforms \
        position \
        sqldrivers

    !contains(DEFINES, __rasp_pi2__) {
        QT_PLUGIN_LIST += xcbglintegrations
    }

    for(QT_PLUGIN, QT_PLUGIN_LIST) {
        QMAKE_POST_LINK += && $$QMAKE_COPY --dereference --recursive $$[QT_INSTALL_PLUGINS]/$$QT_PLUGIN $$DESTDIR/Qt/plugins/
    }

    # QT_INSTALL_QML
    QMAKE_POST_LINK += && $$QMAKE_COPY --dereference --recursive $$[QT_INSTALL_QML] $$DESTDIR/Qt/

    # ArduPilotLog start script
    QMAKE_POST_LINK += && $$QMAKE_COPY $$BASEDIR/deploy/ardupilotlog-start.sh $$DESTDIR
    QMAKE_POST_LINK += && $$QMAKE_COPY $$BASEDIR/deploy/ardupilotlog.desktop $$DESTDIR
}
