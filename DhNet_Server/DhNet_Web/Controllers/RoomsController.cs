using System.ComponentModel.DataAnnotations;
using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("rooms")]
public class RoomsController(IAdminClient client) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<RoomDto>), 200)]
    public async Task<ActionResult<IEnumerable<RoomDto>>> Get(CancellationToken ct)
    {
        var rooms = await client.ListRoomsAsync(ct);
        return Ok(rooms);
    }

    public record BroadcastBody([Required] string Message);

    [HttpPost("{id:long}/broadcast")]
    public async Task<IActionResult> Broadcast([FromRoute] long id, [FromBody] BroadcastBody body, CancellationToken ct)
    {
        if (string.IsNullOrWhiteSpace(body.Message))
            return BadRequest(new { error = "Message is required" });

        await client.BroadcastAsync(id, body.Message, ct);
        return Ok(new { success = true });
    }
}
