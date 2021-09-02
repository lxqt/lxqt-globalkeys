/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#pragma once

#include <QThread>
#include <QMap>
#include <QSet>
#include <QString>
#include <QQueue>
#include <QMutex>
#include <QList>
#include <QPair>
#include <QDBusMessage>
#include <QDBusObjectPath>

#include "meta_types.h"
#include "log_target.h"

extern "C" {
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#undef Bool
}


class QTimer;
class DaemonAdaptor;
class NativeAdaptor;
class DBusProxy;
class BaseAction;
class QDBusServiceWatcher;
class QSettings;

template<class Key>
class QOrderedSet : public QMap<Key, Key>
{
public:
    typename QMap<Key, Key>::iterator insert(const Key &akey)
    {
        return QMap<Key, Key>::insert(akey, akey);
    }
};

class Core : public QThread, public LogTarget
{
    Q_OBJECT
public:
    Core(bool useSyslog, bool minLogLevelSet, int minLogLevel, const QStringList &configFiles, bool multipleActionsBehaviourSet, MultipleActionsBehaviour multipleActionsBehaviour, QObject *parent = nullptr);
    ~Core() override;

    bool ready() const { return mReady; }

    void log(int level, const char *format, ...) const override;

signals:
    void onShortcutGrabbed();

private:
    Core(const Core &);
    Core &operator = (const Core &);

private:
    using X11Shortcut           = QPair<KeyCode, unsigned int>;
    using ShortcutByX11         = QMap<X11Shortcut, QString>;
    using X11ByShortcut         = QMap<QString, X11Shortcut>;
    using Ids                   = QOrderedSet<qulonglong>;
    using IdsByShortcut         = QMap<QString, Ids>;
    using ClientPath            = QDBusObjectPath;
    using IdByClientPath        = QMap<ClientPath, qulonglong>;
    using ShortcutAndAction     = QPair<QString, BaseAction *>;
    using ShortcutAndActionById = QMap<qulonglong, ShortcutAndAction>;
    using SenderByClientPath    = QMap<ClientPath, QString>;
    using ClientPaths           = QSet<ClientPath>;
    using ClientPathsBySender   = QMap<QString, ClientPaths>;

private:
    Q_INVOKABLE void serviceDisappeared(const QString &sender);

    Q_INVOKABLE void addClientAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender);
    Q_INVOKABLE void addMethodAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    Q_INVOKABLE void addCommandAction(QPair<QString, qulonglong> &result, const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description);

    Q_INVOKABLE void modifyClientAction(qulonglong &result, const QDBusObjectPath &path, const QString &description, const QString &sender);
    Q_INVOKABLE void modifyActionDescription(bool &result, const qulonglong &id, const QString &description);
    Q_INVOKABLE void modifyMethodAction(bool &result, const qulonglong &id, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    Q_INVOKABLE void modifyCommandAction(bool &result, const qulonglong &id, const QString &command, const QStringList &arguments, const QString &description);

    Q_INVOKABLE void enableClientAction(bool &result, const QDBusObjectPath &path, bool enabled, const QString &sender);
    Q_INVOKABLE void isClientActionEnabled(bool &enabled, const QDBusObjectPath &path, const QString &sender);
    Q_INVOKABLE void enableAction(bool &result, qulonglong id, bool enabled);
    Q_INVOKABLE void isActionEnabled(bool &enabled, qulonglong id);

    Q_INVOKABLE void getClientActionSender(QString &sender, qulonglong id);


    Q_INVOKABLE void changeClientActionShortcut(QPair<QString, qulonglong> &result, const QDBusObjectPath &path, const QString &shortcut, const QString &sender);
    Q_INVOKABLE void changeShortcut(QString &result, const qulonglong &id, const QString &shortcut);

    Q_INVOKABLE void swapActions(bool &result, const qulonglong &id1, const qulonglong &id2);

    Q_INVOKABLE void removeClientAction(bool &result, const QDBusObjectPath &path, const QString &sender);
    Q_INVOKABLE void removeAction(bool &result, const qulonglong &id);

    Q_INVOKABLE void deactivateClientAction(bool &result, const QDBusObjectPath &path, const QString &sender);

    Q_INVOKABLE void setMultipleActionsBehaviour(const MultipleActionsBehaviour &behaviour);
    Q_INVOKABLE void getMultipleActionsBehaviour(MultipleActionsBehaviour &result) const;

    Q_INVOKABLE void getAllActionIds(QList<qulonglong> &result) const;
    Q_INVOKABLE void getActionById(QPair<bool, GeneralActionInfo> &result, const qulonglong &id) const;
    Q_INVOKABLE void getAllActions(QMap<qulonglong, GeneralActionInfo> &result) const;

    Q_INVOKABLE void getClientActionInfoById(QPair<bool, ClientActionInfo> &result, const qulonglong &id) const;
    Q_INVOKABLE void getMethodActionInfoById(QPair<bool, MethodActionInfo> &result, const qulonglong &id) const;
    Q_INVOKABLE void getCommandActionInfoById(QPair<bool, CommandActionInfo> &result, const qulonglong &id) const;

    Q_INVOKABLE void grabShortcut(const uint &timeout, QString &shortcut, bool &failed, bool &cancelled, bool &timedout, const QDBusMessage &message);
    Q_INVOKABLE void cancelShortcutGrab();

    Q_INVOKABLE void shortcutGrabbed();
    Q_INVOKABLE void shortcutGrabTimedout();

private:
    bool enableActionNonGuarded(qulonglong id, bool enabled);
    QPair<QString, qulonglong> addOrRegisterClientAction(const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender);
    qulonglong registerClientAction(const QString &shortcut, const QDBusObjectPath &path, const QString &description);
    qulonglong registerMethodAction(const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    qulonglong registerCommandAction(const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description);

    GeneralActionInfo actionInfo(const ShortcutAndAction &shortcutAndAction) const;

    friend void unixSignalHandler(int signalNumber);
    void unixSignalHandler(int signalNumber);

    friend int x11ErrorHandler(Display *display, XErrorEvent *errorEvent);
    int x11ErrorHandler(Display *display, XErrorEvent *errorEvent);

    X11Shortcut ShortcutToX11(const QString &shortcut);
    QString X11ToShortcut(const X11Shortcut &X11shortcut);

    void wakeX11Thread();

    void run() override;

    KeyCode remoteStringToKeycode(const QString &str);
    QString remoteKeycodeToString(KeyCode keyCode);
    bool remoteXGrabKey(const X11Shortcut &X11shortcut);
    bool remoteXUngrabKey(const X11Shortcut &X11shortcut);

    QString grabOrReuseKey(const X11Shortcut &X11shortcut, const QString &shortcut);

    QString checkShortcut(const QString &shortcut, X11Shortcut &X11shortcut);

    bool isEscape(KeySym keySym, unsigned int modifiers);
    bool isModifier(KeySym keySym);
    bool isAllowed(KeySym keySym, unsigned int modifiers);

    void saveConfig();

    void lockX11Error();
    bool checkX11Error(int level = LOG_NOTICE, uint timeout = 10);

    bool waitForX11Error(int level, uint timeout);

private:
    bool mReady;
    bool mUseSyslog;

    int mMinLogLevel;

    int mX11ErrorPipe[2];
    int mX11RequestPipe[2];
    int mX11ResponsePipe[2];
    Display *mDisplay;
    Window mInterClientCommunicationWindow;
    bool mX11EventLoopActive;

    mutable QMutex mX11ErrorMutex;

    QDBusServiceWatcher *mServiceWatcher;
    DaemonAdaptor *mDaemonAdaptor;
    NativeAdaptor *mNativeAdaptor;

    mutable QMutex mDataMutex;

    qulonglong mLastId;

    bool mGrabbingShortcut;

    X11ByShortcut mX11ByShortcut;
    ShortcutByX11 mShortcutByX11;
    IdsByShortcut mIdsByShortcut;
    IdsByShortcut mDisabledIdsByShortcut;
    ShortcutAndActionById mShortcutAndActionById;
    IdByClientPath mIdByClientPath;
    SenderByClientPath mSenderByClientPath; // add: path->sender
    ClientPathsBySender mClientPathsBySender; // disappear: sender->[path]

    // (UNUSED) const unsigned int NumLockMask;
    // (UNUSED) const unsigned int ScrollLockMask;
    // (UNUSED) const unsigned int CapsLockMask;
    const unsigned int AltMask    = Mod1Mask;
    const unsigned int MetaMask   = Mod4Mask;
    const unsigned int Level3Mask = Mod5Mask; // note: mask swapped
    const unsigned int Level5Mask = Mod3Mask; // note: mask swapped

    MultipleActionsBehaviour mMultipleActionsBehaviour;

    bool mAllowGrabLocks;
    bool mAllowGrabBaseSpecial;
    bool mAllowGrabMiscSpecial;
    bool mAllowGrabBaseKeypad;
    bool mAllowGrabMiscKeypad;
    bool mAllowGrabPrintable;

    bool mSaveAllowed;

    QTimer *mShortcutGrabTimeout;
    QDBusMessage mShortcutGrabRequest;
    bool mShortcutGrabRequested;

    bool mSuppressX11ErrorMessages;
};
