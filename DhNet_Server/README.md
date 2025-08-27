# DhNet Server / Web / IPC ������Ʈ �ȳ���

�߿�: �������丮�� Ŭ���� ����, ���� ���� setup.bat�� �����ؾ��մϴ�. vcpkg�� �غ���� ������ gRPC�� protobuf ������ ������ C++ ���� ���� �ʽ��ϴ�.

## ����
�� ����Ҵ� C++ ��Ʈ��ŷ ������ .NET 8 ������ҷ� �̷���� �ַ���Դϴ�.

���� ���:
- C++: ServerCore, DhNet_Server, DhNet_Protocol
- .NET 8: DhNet_Ipc (���̺귯��/����), DhNet_Web (ASP.NET Core)

�ַ�� ����: DhNet_Server.sln

## ���� ���� (���)
- ServerCore: ����, ����, ���� �� �ٽ� ���� ������ (C++)
- DhNet_Server: ���� ���ø����̼� (C++)
- DhNet_Protocol: �������� (��Ŷ ����) (C++)
- DhNet_Ipc: .NET 8 IPC/��ƿ ���� (C#)
- DhNet_Web: .NET 8 Web/API/���� ������(swagger) (C#)

���� ���⹰ : Binary\Debug\DhNet_Server.exe

## ���� ����

1) �۾� ȯ��
- Windows 10 �̻�, x64
- Visual Studio 2022, Rider 2025.2
- .NET SDK 8.0
- Git

2) ó�� �� ����: setup.bat ���� (�ݵ�� ����)
- �������丮 ��Ʈ���� setup.bat�� ����Ŭ���ϰų� �ֿܼ��� �����մϴ�.
- �� ��ũ��Ʈ�� ������ �����ϵ��� ����Ǿ� �ֽ��ϴ�:
  - vcpkg ��ġ �Ǵ� ��ġ Ȯ��
  - bootstrap-vcpkg.bat ����
  - vcpkg install grpc:x64-windows ���� (grpc, protobuf ��ġ)

3) �ַ�� ���� �� ����
- DhNet_Server.sln�� Visual Studio 2022 �Ǵ� JetBrains Rider�� ���ϴ�.
- ����(Configuration): Debug x64 (�⺻)
- DhNet_Ipc�� generated ���� ���� Ȯ��. ���ٸ� DhNet_Ipc�� tools�� generate_protos.ps1 ���� (dhnet.grpc.pb, dhnet.pb 2���� .cc, .h �����Ǿ��ִ��� Ȯ��)
- DhNet_Server ������Ʈ ���� �� ���� �ȳ�:
  1. DhUtil (c++)
  2. ServerCore (C++)
  3. DhNet_Server (C++)
  
4) ����
- IPC/����: DhNet_Ipc�� �ʿ� �� ������ �����մϴ�. (generate_protos.ps1 ����)
- C++ ����: DhNet_Server�� �����Ͽ� ���� ���μ����� �����մϴ�.
- ��: DhNet_Web�� �����Ͽ� ����/�׽�Ʈ�� �� �������̽��� �����մϴ�.