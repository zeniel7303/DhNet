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

    public async Task<HealthDto> HealthCheckAsync(CancellationToken ct)
    {
        try
        {
            var resp = await _client.HealthCheckAsync(new HealthCheckRequest(), cancellationToken: ct);
            return new HealthDto(resp.Status);
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(HealthCheckAsync));
        }
    }

    public async Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct)
    {
        try
        {
            var resp = await _client.ListRoomsAsync(new ListRoomsRequest(), cancellationToken: ct);
            var list = new List<RoomDto>(resp.Rooms.Count);
            list.AddRange(resp.Rooms.Select(r => new RoomDto(r.Id, r.Name, r.PlayerCount, r.Capacity)));
            return list;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(ListRoomsAsync));
        }
    }

    public async Task<IReadOnlyList<PlayerDto>> ListPlayersAsync(CancellationToken ct)
    {
        try
        {
            var resp = await _client.ListPlayersAsync(new ListPlayersRequest(), cancellationToken: ct);
            var list = new List<PlayerDto>(resp.Players.Count);
            list.AddRange(resp.Players.Select(p => new PlayerDto(p.Id, p.Name, p.LobbyIndex, p.RoomIndex)));
            return list;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(ListPlayersAsync));
        }
    }

    public async Task<bool> KickPlayerAsync(ulong id, CancellationToken ct)
    {
        try
        {
            var resp = await _client.KickPlayerAsync(new KickPlayerRequest { Id = id }, cancellationToken: ct);
            return !resp.Success
                ? throw new RpcException(new Status(StatusCode.Unknown, resp.Detail))
                : true;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(KickPlayerAsync));
        }
    }

    public async Task<IReadOnlyList<LobbyDto>> ListLobbiesAsync(CancellationToken ct)
    {
        try
        {
            var resp = await _client.ListLobbiesAsync(new ListLobbiesRequest(), cancellationToken: ct);
            var list = new List<LobbyDto>(resp.Lobbies.Count);
            list.AddRange(resp.Lobbies.Select(l => new LobbyDto(l.Id, l.PlayerCount, l.Capacity)));
            return list;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(ListLobbiesAsync));
        }
    }

    public async Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct)
    {
        try
        {
            var resp = await _client.BroadcastAsync(new BroadcastRequest { RoomId = roomId, Message = message }, cancellationToken: ct);
            return !resp.Success 
                ? throw new RpcException(new Status(StatusCode.Unknown, resp.Detail)) 
                : true;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex, nameof(BroadcastAsync));
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
