#pragma warning disable CS1591 // 공개된 형식 또는 멤버에 대한 XML 주석이 없습니다.

namespace DhNet.Web.Services;

public record HealthDto(string Status);

public record RoomDto(long Id, string Name, int PlayerCount, int Capacity);

public interface IAdminClient
{
    Task<HealthDto> HealthCheckAsync(CancellationToken ct);
    Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct);
    Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct);
}
