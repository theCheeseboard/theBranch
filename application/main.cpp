/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
//#include "mainwindow.h"

#include <QCommandLineParser>
#include <QDir>
#include <QJsonArray>
#include <QUrl>
#include <tapplication.h>
#include <tsettings.h>
#include <tstylemanager.h>

#include "libthebranch_global.h"
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);

    if (QDir(QStringLiteral("%1/share/thebranch/").arg(SYSTEM_PREFIX_DIRECTORY)).exists()) {
        a.setShareDir(QStringLiteral("%1/share/thebranch/").arg(SYSTEM_PREFIX_DIRECTORY));
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thebranch/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thebranch/"));
    }
    a.installTranslators();

    a.setApplicationVersion("1.0");
    a.setGenericName(QApplication::translate("main", "Git Client"));
    //    a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2022");
    a.setOrganizationName("theSuite");
    a.setApplicationUrl(tApplication::HelpContents, QUrl("https://help.vicr123.com/docs/thebranch/intro"));
    a.setApplicationUrl(tApplication::Sources, QUrl("http://github.com/vicr123/thebranch"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("http://github.com/vicr123/thebranch/issues"));
    a.setApplicationName(T_APPMETA_READABLE_NAME);
    a.setDesktopFileName(T_APPMETA_DESKTOP_ID);

    a.registerCrashTrap();

#if defined(Q_OS_WIN)
//    a.setWinApplicationClassId("{98fd3bc5-b39c-4c97-b483-4c95b90a7c39}");
#elif defined(Q_OS_MAC)
    a.setQuitOnLastWindowClosed(false);
#endif

    QCommandLineParser parser;
    QCommandLineOption helpOption = parser.addHelpOption();
    parser.parse(a.arguments());

    if (parser.isSet(helpOption)) {
        parser.showHelp();
        return 0;
    }

    theBranch::init();

    tSettings settings;

    QObject::connect(&settings, &tSettings::settingChanged, [=](QString key, QVariant value) {
        if (key == "theme/mode") {
            tStyleManager::setOverrideStyleForApplication(value.toString() == "light" ? tStyleManager::ContemporaryLight : tStyleManager::ContemporaryDark);
        }
    });
    tStyleManager::setOverrideStyleForApplication(settings.value("theme/mode").toString() == "light" ? tStyleManager::ContemporaryLight : tStyleManager::ContemporaryDark);

    MainWindow* w = new MainWindow();

    QStringList files;
    for (QString arg : parser.positionalArguments()) {
        if (QUrl::fromLocalFile(arg).isValid()) {
            files.append(QUrl::fromLocalFile(arg).toEncoded());
        } else {
            files.append(QUrl(arg).toEncoded());
        }
    }
    a.ensureSingleInstance({
        {"files", QJsonArray::fromStringList(files)}
    });

    for (QString repo : files) {
        w->openRepo(QUrl(repo).toLocalFile());
    }

    w->show();

    int retval = a.exec();

    theBranch::teardown();

    return retval;
}
