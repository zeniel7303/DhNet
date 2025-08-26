using System.ComponentModel.DataAnnotations;
using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // 공개된 형식 또는 멤버에 대한 XML 주석이 없습니다.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("rooms")]
public class RoomsController(IAdminClient client) : ControllerBase
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
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
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
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
    }
}
