using System.ComponentModel.DataAnnotations;
using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

namespace DhNet.Web.Controllers;

[ApiController]
[Route("rooms")]
public class RoomsController : ControllerBase
{
    private readonly IAdminClient _client;

    public RoomsController(IAdminClient client) => _client = client;

    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<RoomDto>), 200)]
    public async Task<ActionResult<IEnumerable<RoomDto>>> Get(CancellationToken ct)
    {
        try
        {
            var rooms = await _client.ListRoomsAsync(ct);
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
            await _client.BroadcastAsync(id, body.Message, ct);
            return Ok(new { success = true });
        }
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
    }
}
