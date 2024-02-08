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
    Core(const Core&) = delete;
    Core(const Core&&) = delete;
    Core& operator =(const Core&) = delete;
    Core& operator =(const Core&&) = delete;

private:
    using X11Shortcut           = std::pair<KeyCode, unsigned int>;
    using ShortcutByX11         = QMap<X11Shortcut, QString>;
    using X11ByShortcut         = QMap<QString, X11Shortcut>;
    using Ids                   = QOrderedSet<qulonglong>;
    using IdsByShortcut         = QMap<QString, Ids>;
    using ClientPath            = QDBusObjectPath;
    using IdByClientPath        = QMap<ClientPath, qulonglong>;
    using ShortcutAndAction     = std::pair<QString, BaseAction *>;
    using ShortcutAndActionById = QMap<qulonglong, ShortcutAndAction>;
    using SenderByClientPath    = QMap<ClientPath, QString>;
    using ClientPaths           = QSet<ClientPath>;
    using ClientPathsBySender   = QMap<QString, ClientPaths>;

private:
    void serviceDisappeared(const QString &sender);

    void addClientAction(std::pair<QString, qulonglong> &result, const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender);
    void addMethodAction(std::pair<QString, qulonglong> &result, const QString &shortcut, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    void addCommandAction(std::pair<QString, qulonglong> &result, const QString &shortcut, const QString &command, const QStringList &arguments, const QString &description);

    void modifyClientAction(qulonglong &result, const QDBusObjectPath &path, const QString &description, const QString &sender);
    void modifyActionDescription(bool &result, const qulonglong &id, const QString &description);
    void modifyMethodAction(bool &result, const qulonglong &id, const QString &service, const QDBusObjectPath &path, const QString &interface, const QString &method, const QString &description);
    void modifyCommandAction(bool &result, const qulonglong &id, const QString &command, const QStringList &arguments, const QString &description);

    void enableClientAction(bool &result, const QDBusObjectPath &path, bool enabled, const QString &sender);
    void isClientActionEnabled(bool &enabled, const QDBusObjectPath &path, const QString &sender);
    void enableAction(bool &result, qulonglong id, bool enabled);
    void isActionEnabled(bool &enabled, qulonglong id);

    void getClientActionSender(QString &sender, qulonglong id);


    void changeClientActionShortcut(std::pair<QString, qulonglong> &result, const QDBusObjectPath &path, const QString &shortcut, const QString &sender);
    void changeShortcut(QString &result, const qulonglong &id, const QString &shortcut);

    void swapActions(bool &result, const qulonglong &id1, const qulonglong &id2);

    void removeClientAction(bool &result, const QDBusObjectPath &path, const QString &sender);
    void removeAction(bool &result, const qulonglong &id);

    void deactivateClientAction(bool &result, const QDBusObjectPath &path, const QString &sender);

    void setMultipleActionsBehaviour(const MultipleActionsBehaviour &behaviour);
    void getMultipleActionsBehaviour(MultipleActionsBehaviour &result) const;

    void getAllActionIds(QList<qulonglong> &result) const;
    void getActionById(std::pair<bool, GeneralActionInfo> &result, const qulonglong &id) const;
    void getAllActions(QMap<qulonglong, GeneralActionInfo> &result) const;

    void getClientActionInfoById(std::pair<bool, ClientActionInfo> &result, const qulonglong &id) const;
    void getMethodActionInfoById(std::pair<bool, MethodActionInfo> &result, const qulonglong &id) const;
    void getCommandActionInfoById(std::pair<bool, CommandActionInfo> &result, const qulonglong &id) const;

    void grabShortcut(const uint &timeout, QString &shortcut, bool &failed, bool &cancelled, bool &timedout, const QDBusMessage &message);
    void cancelShortcutGrab();

    void shortcutGrabbed();
    void shortcutGrabTimedout();

private:
    bool enableActionNonGuarded(qulonglong id, bool enabled);
    std::pair<QString, qulonglong> addOrRegisterClientAction(const QString &shortcut, const QDBusObjectPath &path, const QString &description, const QString &sender);
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
    void runEventLoop(Window rootWindow);

    KeyCode remoteStringToKeycode(const QString &str);
    QString remoteKeycodeToString(KeyCode keyCode);
    bool remoteXGrabKey(const X11Shortcut &X11shortcut);
    bool remoteXUngrabKey(const X11Shortcut &X11shortcut);

    QString grabOrReuseKey(const X11Shortcut &X11shortcut, const QString &shortcut);
    QString checkShortcut(const QString &shortcut, X11Shortcut &X11shortcut);

    constexpr bool isEscape(KeySym keySym, unsigned int modifiers) const { return ((keySym == XK_Escape) && (!modifiers)); }
    constexpr bool isModifier(KeySym keySym) const {
        switch (keySym)
        {
        case XK_Shift_L:
        case XK_Shift_R:
        case XK_Control_L:
        case XK_Control_R:
        case XK_Meta_L:
        case XK_Meta_R:
        case XK_Alt_L:
        case XK_Alt_R:
        case XK_Super_L: // FIXME: Super_L is not a modifier (X bug?)
        case XK_Super_R: // FIXME: Super_R is not a modifier (X bug?)
        case XK_Hyper_L:
        case XK_Hyper_R:
        case XK_ISO_Level3_Shift:
        case XK_ISO_Level5_Shift:
        case XK_ISO_Group_Shift:
            return true;

        }
        return false;
    }
    constexpr bool isSuperKey(KeySym keySym, unsigned int modifiers) const {
        switch (keySym) {
        case 0:
            // only MetaMask modifier is set (no key symbol)
            return modifiers == MetaMask;

        case XK_Super_L:
        case XK_Super_R:
        case XK_Meta_L:
        case XK_Meta_R:
            // meta key symbol with no modifers OR modifer matches MetaMask
            return !modifiers || modifiers == MetaMask;
        }

        return false;
    }
    constexpr bool isAllowed(KeySym keySym, unsigned int modifiers) const {
        switch (keySym)
        {
        case XK_Scroll_Lock:
        case XK_Num_Lock:
        case XK_Caps_Lock:
        case XK_ISO_Lock:
        case XK_ISO_Level3_Lock:
        case XK_ISO_Level5_Lock:
        case XK_ISO_Group_Lock:
        case XK_ISO_Next_Group_Lock:
        case XK_ISO_Prev_Group_Lock:
        case XK_ISO_First_Group_Lock:
        case XK_ISO_Last_Group_Lock:
            if (!modifiers)
            {
                return mAllowGrabLocks;
            }
            break;

        case XK_Home:
        case XK_Left:
        case XK_Up:
        case XK_Right:
        case XK_Down:
        case XK_Page_Up:
        case XK_Page_Down:
        case XK_End:
        case XK_Delete:
        case XK_Insert:
        case XK_BackSpace:
        case XK_Tab:
        case XK_Return:
        case XK_space:
            if (!modifiers)
            {
                return mAllowGrabBaseSpecial;
            }
            break;

        case XK_Pause:
        case XK_Print:
        case XK_Linefeed:
        case XK_Clear:
        case XK_Multi_key:
        case XK_Codeinput:
        case XK_SingleCandidate:
        case XK_MultipleCandidate:
        case XK_PreviousCandidate:
        case XK_Begin:
        case XK_Select:
        case XK_Execute:
        case XK_Undo:
        case XK_Redo:
        case XK_Menu:
        case XK_Find:
        case XK_Cancel:
        case XK_Help:
        case XK_Sys_Req:
        case XK_Break:
            if (!modifiers)
            {
                return mAllowGrabMiscSpecial;
            }
            break;

        case XK_KP_Enter:
        case XK_KP_Home:
        case XK_KP_Left:
        case XK_KP_Up:
        case XK_KP_Right:
        case XK_KP_Down:
        case XK_KP_Page_Up:
        case XK_KP_Page_Down:
        case XK_KP_End:
        case XK_KP_Begin:
        case XK_KP_Insert:
        case XK_KP_Delete:
        case XK_KP_Multiply:
        case XK_KP_Add:
        case XK_KP_Subtract:
        case XK_KP_Decimal:
        case XK_KP_Divide:
        case XK_KP_0:
        case XK_KP_1:
        case XK_KP_2:
        case XK_KP_3:
        case XK_KP_4:
        case XK_KP_5:
        case XK_KP_6:
        case XK_KP_7:
        case XK_KP_8:
        case XK_KP_9:
            if (!modifiers)
            {
                return mAllowGrabBaseKeypad;
            }
            break;

        case XK_KP_Space:
        case XK_KP_Tab:
        case XK_KP_F1:
        case XK_KP_F2:
        case XK_KP_F3:
        case XK_KP_F4:
        case XK_KP_Equal:
        case XK_KP_Separator:
            if (!modifiers)
            {
                return mAllowGrabMiscKeypad;
            }
            break;

        case XK_grave:
        case XK_1:
        case XK_2:
        case XK_3:
        case XK_4:
        case XK_5:
        case XK_6:
        case XK_7:
        case XK_8:
        case XK_9:
        case XK_0:
        case XK_minus:
        case XK_equal:
        case XK_Q:
        case XK_W:
        case XK_E:
        case XK_R:
        case XK_T:
        case XK_Y:
        case XK_U:
        case XK_I:
        case XK_O:
        case XK_P:
        case XK_bracketleft:
        case XK_bracketright:
        case XK_backslash:
        case XK_A:
        case XK_S:
        case XK_D:
        case XK_F:
        case XK_G:
        case XK_H:
        case XK_J:
        case XK_K:
        case XK_L:
        case XK_semicolon:
        case XK_apostrophe:
        case XK_Z:
        case XK_X:
        case XK_C:
        case XK_V:
        case XK_B:
        case XK_N:
        case XK_M:
        case XK_comma:
        case XK_period:
        case XK_slash:
            if (!(modifiers & ~(ShiftMask | Level3Mask | Level5Mask)))
            {
                return mAllowGrabPrintable;
            }
            break;

        }
        return true;
    }

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
    mutable QMutex mDataMutex;

    QDBusServiceWatcher *mServiceWatcher;
    DaemonAdaptor *mDaemonAdaptor;
    NativeAdaptor *mNativeAdaptor;
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

    QTimer* mShortcutGrabTimeout;
    QDBusMessage mShortcutGrabRequest;
    bool mShortcutGrabRequested;
    bool mSuppressX11ErrorMessages;
};
