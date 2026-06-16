#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Services;

public record HealthDto(string Status);

public record RoomDto(long Id, string Name, int PlayerCount, int Capacity);

public record PlayerDto(ulong Id, string Name, int LobbyIndex, int RoomIndex);

public record LobbyDto(int Id, int PlayerCount, int Capacity);

public interface IAdminClient
{
    Task<HealthDto> HealthCheckAsync(CancellationToken ct);
    Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct);
    Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct);
    Task<IReadOnlyList<PlayerDto>> ListPlayersAsync(CancellationToken ct);
    Task<bool> KickPlayerAsync(ulong id, CancellationToken ct);
    Task<IReadOnlyList<LobbyDto>> ListLobbiesAsync(CancellationToken ct);
}
