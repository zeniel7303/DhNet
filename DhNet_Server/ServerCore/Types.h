#pragma once

using SenderRef = std::shared_ptr<class Sender>;
using SessionRef = std::shared_ptr<class Session>;
using IocpCoreRef = std::shared_ptr<class IocpCore>;
using IocpObjectRef = std::shared_ptr<class IocpObject>;
using ListenerRef = std::shared_ptr <class Listener> ;
using ServiceRef = std::shared_ptr<class Service>;
using ServerServiceRef = std::shared_ptr<class ServerService>;
using ClientServiceRef = std::shared_ptr<class ClientService>;