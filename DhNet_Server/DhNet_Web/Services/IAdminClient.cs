using System.Threading;
using System.Threading.Tasks;

namespace DhNet.Web.Services;

public record HealthDto(string Status);

public record RoomDto(long Id, string Name, int PlayerCount, int Capacity);

public interface IAdminClient
{
    Task<HealthDto> HealthCheckAsync(CancellationToken ct);
    Task<IReadOnlyList<RoomDto>> ListRoomsAsync(CancellationToken ct);
    Task<bool> BroadcastAsync(long roomId, string message, CancellationToken ct);
}
