using System.Runtime.CompilerServices;
using DhNet.Ipc;
using Grpc.Core;
using Grpc.Net.Client;

#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Services;

public sealed class GrpcAdminClient : IAdminClient, IDisposable
{
    private readonly GrpcChannel _channel;
    private readonly AdminService.AdminServiceClient _client;
    private readonly ILogger<GrpcAdminClient> _logger;

    public GrpcAdminClient(string address, ILogger<GrpcAdminClient> logger)
    {
        _channel = GrpcChannel.ForAddress(address);
        _client = new AdminService.AdminServiceClient(_channel);
        _logger = logger;
    }

    public Task<HealthDto> HealthCheckAsync(CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.HealthCheckAsync(new HealthCheckRequest(), cancellationToken: ct);
        return new HealthDto(resp.Status);
    });

    public Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.ListRoomsAsync(new ListRoomsRequest(), cancellationToken: ct);
        IReadOnlyList<RoomDto> list = resp.Rooms.Select(r => new RoomDto(r.Id, r.Name, r.PlayerCount, r.Capacity)).ToList();
        return list;
    });

    public Task<IReadOnlyList<PlayerDto>> ListPlayersAsync(CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.ListPlayersAsync(new ListPlayersRequest(), cancellationToken: ct);
        IReadOnlyList<PlayerDto> list = resp.Players.Select(p => new PlayerDto(p.Id, p.Name, p.LobbyIndex, p.RoomIndex)).ToList();
        return list;
    });

    public Task<bool> KickPlayerAsync(ulong id, CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.KickPlayerAsync(new KickPlayerRequest { Id = id }, cancellationToken: ct);
        return !resp.Success
            ? throw new RpcException(new Status(StatusCode.Unknown, resp.Detail))
            : true;
    });

    public Task<IReadOnlyList<LobbyDto>> ListLobbiesAsync(CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.ListLobbiesAsync(new ListLobbiesRequest(), cancellationToken: ct);
        IReadOnlyList<LobbyDto> list = resp.Lobbies.Select(l => new LobbyDto(l.Id, l.PlayerCount, l.Capacity)).ToList();
        return list;
    });

    public Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct) => ExecuteAsync(async () =>
    {
        var resp = await _client.BroadcastAsync(new BroadcastRequest { RoomId = roomId, Message = message }, cancellationToken: ct);
        return !resp.Success
            ? throw new RpcException(new Status(StatusCode.Unknown, resp.Detail))
            : true;
    });

    // [CallerMemberName]이 호출 지점의 메서드 이름을 자동으로 채워주므로
    // 각 RPC 메서드에서 try/catch + nameof(자기자신) 반복을 제거할 수 있다.
    private async Task<T> ExecuteAsync<T>(Func<Task<T>> rpcCall, [CallerMemberName] string methodName = "")
    {
        try
        {
            return await rpcCall();
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, methodName);
        }
    }

    private Exception CreateHttpMappedException(RpcException ex, string methodName)
    {
        var code = ex.StatusCode;
        var msg = ex.Status.Detail;

        if (code == StatusCode.DeadlineExceeded || code == StatusCode.Unavailable)
            _logger.LogWarning(ex, "[GrpcAdminClient] {Method} failed: {Code} {Detail}", methodName, code, msg);
        else
            _logger.LogError(ex, "[GrpcAdminClient] {Method} failed: {Code} {Detail}", methodName, code, msg);

        return code switch
        {
            StatusCode.InvalidArgument => new ArgumentException(msg),
            StatusCode.NotFound => new KeyNotFoundException(msg),
            StatusCode.DeadlineExceeded => new TimeoutException(msg),
            StatusCode.Unavailable => new HttpRequestException(msg),
            _ => new ApplicationException($"gRPC error ({code}): {msg}")
        };
    }

    public void Dispose()
    {
        _channel.Dispose();
    }
}
