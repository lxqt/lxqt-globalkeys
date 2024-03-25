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

#include "client.h"
#include "client_p.h"
#include "action_p.h"
#include "org.lxqt.global_key_shortcuts.native.h"

#include <QDBusConnection>
#include <QRegularExpression>


namespace GlobalKeyShortcut
{

ClientImpl::ClientImpl(Client *interface, QObject *parent)
    : QObject(parent)
    , mInterface(interface)
    , mServiceWatcher(new QDBusServiceWatcher(QLatin1String("org.lxqt.global_key_shortcuts"), QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this))
    , mDaemonPresent(false)
{
    connect(mServiceWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &ClientImpl::daemonDisappeared);
    connect(mServiceWatcher, &QDBusServiceWatcher::serviceRegistered,   this, &ClientImpl::daemonAppeared);
    mProxy = new org::lxqt::global_key_shortcuts::native(QLatin1String("org.lxqt.global_key_shortcuts"), QStringLiteral("/native"), QDBusConnection::sessionBus(), this);
    mDaemonPresent = mProxy->isValid();

    connect(this, &ClientImpl::emitShortcutGrabbed,       mInterface, &Client::shortcutGrabbed);
    connect(this, &ClientImpl::emitGrabShortcutFailed,    mInterface, &Client::grabShortcutFailed);
    connect(this, &ClientImpl::emitGrabShortcutCancelled, mInterface, &Client::grabShortcutCancelled);
    connect(this, &ClientImpl::emitGrabShortcutTimedout,  mInterface, &Client::grabShortcutTimedout);
    connect(this, &ClientImpl::emitDaemonDisappeared,     mInterface, &Client::daemonDisappeared);
    connect(this, &ClientImpl::emitDaemonAppeared,        mInterface, &Client::daemonAppeared);
    connect(this, &ClientImpl::emitDaemonPresenceChanged, mInterface, &Client::daemonPresenceChanged);
}

ClientImpl::~ClientImpl()
{
    QMap<QString, Action*>::iterator M = mActions.end();
    for (QMap<QString, Action*>::iterator I = mActions.begin(); I != M; ++I)
    {
        QDBusConnection::sessionBus().unregisterObject(QLatin1String("/global_key_shortcuts") + I.key());

        delete I.value();
    }
    mActions.clear();
}

void ClientImpl::daemonDisappeared(const QString &)
{
    mDaemonPresent = false;
    emit emitDaemonDisappeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

void ClientImpl::daemonAppeared(const QString &)
{
    QMap<QString, Action*>::iterator last = mActions.end();
    for (QMap<QString, Action*>::iterator I = mActions.begin(); I != last; ++I)
    {
        ActionImpl *globalActionImpl = I.value()->impl;

        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(mProxy->addClientAction(globalActionImpl->shortcut(), QDBusObjectPath(globalActionImpl->path()), globalActionImpl->description()));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &ClientImpl::registrationFinished);
        mPendingRegistrationsActions[watcher] = globalActionImpl;
        mPendingRegistrationsWatchers[globalActionImpl] = watcher;
        globalActionImpl->setRegistrationPending(true);
    }
    mDaemonPresent = true;
    emit emitDaemonAppeared();
    emit emitDaemonPresenceChanged(mDaemonPresent);
}

bool ClientImpl::isDaemonPresent() const
{
    return mDaemonPresent;
}

void ClientImpl::registrationFinished(QDBusPendingCallWatcher *watcher)
{
    QMap<QDBusPendingCallWatcher*, ActionImpl*>::Iterator I = mPendingRegistrationsActions.find(watcher);
    if (I != mPendingRegistrationsActions.end())
    {
        ActionImpl *globalActionImpl = I.value();

        QDBusPendingReply<QString, qulonglong> reply = *watcher;
        globalActionImpl->setValid(!reply.isError() && reply.argumentAt<1>());

        if (globalActionImpl->isValid())
        {
            globalActionImpl->setShortcut(reply.argumentAt<0>());
        }

        mPendingRegistrationsWatchers.remove(globalActionImpl);
        mPendingRegistrationsActions.erase(I);
        watcher->deleteLater();

        globalActionImpl->setRegistrationPending(false);
    }
}

Action *ClientImpl::addClientAction(const QString &shortcut, const QString &path, const QString &description, QObject *parent)
{
    static const QRegularExpression regexp(QRegularExpression::anchoredPattern(QStringLiteral("(/[A-Za-z0-9_]+){2,}")));
    if (!regexp.match(path).hasMatch())
    {
        return nullptr;
    }

    if (mActions.contains(path))
    {
        return nullptr;
    }

    Action *globalAction = new Action(parent);

    ActionImpl *globalActionImpl = new ActionImpl(this, globalAction, path, description, globalAction);
    globalAction->impl = globalActionImpl;

    if (!QDBusConnection::sessionBus().registerObject(QLatin1String("/global_key_shortcuts") + path, globalActionImpl))
    {
        return nullptr;
    }

    if (mDaemonPresent)
    {
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(mProxy->addClientAction(shortcut, QDBusObjectPath(path), description));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &ClientImpl::registrationFinished);
        mPendingRegistrationsActions[watcher] = globalActionImpl;
        mPendingRegistrationsWatchers[globalActionImpl] = watcher;
        globalActionImpl->setRegistrationPending(true);
    }
    else
    {
        globalActionImpl->setValid(false);
    }

    mActions[path] = globalAction;


    return globalAction;
}

void removeAction(Action *action);

QString ClientImpl::changeClientActionShortcut(const QString &path, const QString &shortcut)
{
    if (!mActions.contains(path))
    {
        return QString();
    }

    QDBusPendingReply<QString> reply = mProxy->changeClientActionShortcut(QDBusObjectPath(path), shortcut);
    reply.waitForFinished();
    if (reply.isError())
    {
        return QString();
    }

    return reply.argumentAt<0>();
}

bool ClientImpl::modifyClientAction(const QString &path, const QString &description)
{
    if (!mActions.contains(path))
    {
        return false;
    }

    QDBusPendingReply<bool> reply = mProxy->modifyClientAction(QDBusObjectPath(path), description);
    reply.waitForFinished();
    if (reply.isError())
    {
        return false;
    }

    return reply.argumentAt<0>();
}

bool ClientImpl::removeClientAction(const QString &path)
{
    if (!mActions.contains(path))
    {
        return false;
    }

    QDBusPendingReply<bool> reply = mProxy->removeClientAction(QDBusObjectPath(path));
    reply.waitForFinished();
    if (reply.isError())
    {
        return false;
    }

    QDBusConnection::sessionBus().unregisterObject(QLatin1String("/global_key_shortcuts") + path);

    mActions[path]->disconnect();
    mActions.remove(path);

    return reply.argumentAt<0>();
}

void ClientImpl::removeAction(ActionImpl *actionImpl)
{
    if (actionImpl->isRegistrationPending())
    {
        QMap<ActionImpl*, QDBusPendingCallWatcher*>::Iterator I = mPendingRegistrationsWatchers.find(actionImpl);
        if (I != mPendingRegistrationsWatchers.end())
        {
            QDBusPendingCallWatcher *watcher = I.value();

            watcher->disconnect();

            mPendingRegistrationsActions.remove(watcher);
            mPendingRegistrationsWatchers.erase(I);
            watcher->deleteLater();
        }
    }

    QString path = actionImpl->path();
    if (!mActions.contains(path))
    {
        return;
    }

    QDBusPendingReply<bool> reply = mProxy->deactivateClientAction(QDBusObjectPath(path));
    reply.waitForFinished();

    QDBusConnection::sessionBus().unregisterObject(QLatin1String("/global_key_shortcuts") + path);

    mActions[path]->disconnect();
    mActions.remove(path);
}

void ClientImpl::grabShortcut(uint timeout)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(mProxy->grabShortcut(timeout), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &ClientImpl::grabShortcutFinished);
}

void ClientImpl::cancelShortcutGrab()
{
    mProxy->cancelShortcutGrab();
}

void ClientImpl::grabShortcutFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, bool, bool, bool> reply = *call;
    if (reply.isError())
    {
        emit emitGrabShortcutFailed();
    }
    else
    {
        if (reply.argumentAt<1>())
        {
            emit emitGrabShortcutFailed();
        }
        else
        {
            if (reply.argumentAt<2>())
            {
                emit emitGrabShortcutCancelled();
            }
            else
            {
                if (reply.argumentAt<3>())
                {
                    emit emitGrabShortcutTimedout();
                }
                else
                {
                    emit emitShortcutGrabbed(reply.argumentAt<0>());
                }
            }
        }
    }

    call->deleteLater();
}


static std::unique_ptr<Client> globalActionNativeClient;

Client *Client::instance()
{
    if (!globalActionNativeClient)
    {
        globalActionNativeClient.reset(new Client());
    }

    return globalActionNativeClient.get();
}

Client::Client()
    : QObject(nullptr)
    , impl(new ClientImpl(this, this))
{
}

Client::~Client()
{
    globalActionNativeClient.release();
}

Action *Client::addAction(const QString &shortcut, const QString &path, const QString &description, QObject *parent) { return impl->addClientAction(shortcut, path, description, parent); }
bool Client::removeAction(const QString &path) { return impl->removeClientAction(path); }
void Client::grabShortcut(uint timeout) { impl->grabShortcut(timeout); }
void Client::cancelShortcutGrab() { impl->cancelShortcutGrab(); }
bool Client::isDaemonPresent() const { return impl->isDaemonPresent(); }

}
