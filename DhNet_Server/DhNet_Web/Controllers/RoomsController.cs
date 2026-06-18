using System.ComponentModel.DataAnnotations;
using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("rooms")]
public class RoomsController(IAdminClient client, ILogger<RoomsController> logger) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<RoomDto>), 200)]
    public async Task<ActionResult<IEnumerable<RoomDto>>> Get(CancellationToken ct)
    {
        try
        {
            var rooms = await client.ListRoomsAsync(ct);
            return Ok(rooms);
        }
        catch (TimeoutException e) { logger.LogWarning(e, "[Rooms] Get timeout"); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Rooms] Get not found"); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Rooms] Get bad request"); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Rooms] Get backend unavailable"); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Rooms] Get unexpected error"); return StatusCode(500, new { error = e.Message }); }
    }

    public record BroadcastBody([Required] string Message);

    [HttpPost("{id:long}/broadcast")]
    public async Task<IActionResult> Broadcast([FromRoute] long id, [FromBody] BroadcastBody body, CancellationToken ct)
    {
        if (string.IsNullOrWhiteSpace(body.Message))
            return BadRequest(new { error = "Message is required" });

        try
        {
            await client.BroadcastAsync(id, body.Message, ct);
            return Ok(new { success = true });
        }
        catch (TimeoutException e) { logger.LogWarning(e, "[Rooms] Broadcast timeout (id={Id})", id); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Rooms] Broadcast not found (id={Id})", id); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Rooms] Broadcast bad request (id={Id})", id); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Rooms] Broadcast backend unavailable (id={Id})", id); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Rooms] Broadcast unexpected error (id={Id})", id); return StatusCode(500, new { error = e.Message }); }
    }
}
