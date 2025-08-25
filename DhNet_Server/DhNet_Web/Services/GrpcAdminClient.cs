using System;
using System.Threading;
using System.Threading.Tasks;
using DhNet.Ipc;
using Grpc.Core;
using Grpc.Net.Client;

namespace DhNet.Web.Services;

public sealed class GrpcAdminClient : IAdminClient, IDisposable
{
    private readonly GrpcChannel _channel;
    private readonly AdminService.AdminServiceClient _client;

    public GrpcAdminClient(string address)
    {
        _channel = GrpcChannel.ForAddress(address);
        _client = new AdminService.AdminServiceClient(_channel);
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
            throw CreateHttpMappedException(ex);
        }
    }

    public async Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct)
    {
        try
        {
            var resp = await _client.ListRoomsAsync(new ListRoomsRequest(), cancellationToken: ct);
            var list = new List<RoomDto>(resp.Rooms.Count);
            foreach (var r in resp.Rooms)
            {
                list.Add(new RoomDto(r.Id, r.Name, r.PlayerCount, r.Capacity));
            }
            return list;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex);
        }
    }

    public async Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct)
    {
        try
        {
            var resp = await _client.BroadcastAsync(new BroadcastRequest { RoomId = roomId, Message = message }, cancellationToken: ct);
            if (!resp.Success)
                throw new RpcException(new Status(StatusCode.Unknown, resp.Detail));
            return true;
        }
        catch (RpcException ex)
        {
            throw CreateHttpMappedException(ex);
        }
    }

    private static Exception CreateHttpMappedException(RpcException ex)
    {
        var code = ex.StatusCode;
        var msg = ex.Status.Detail;
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
