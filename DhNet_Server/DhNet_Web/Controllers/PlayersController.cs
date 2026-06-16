using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // Missing XML comment for publicly visible type or member

namespace DhNet.Web.Controllers;

[ApiController]
[Route("players")]
public class PlayersController(IAdminClient client) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<PlayerDto>), 200)]
    public async Task<ActionResult<IEnumerable<PlayerDto>>> Get(CancellationToken ct)
    {
        try
        {
            var players = await client.ListPlayersAsync(ct);
            return Ok(players);
        }
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
    }

    [HttpPost("{id}/kick")]
    public async Task<IActionResult> Kick([FromRoute] ulong id, CancellationToken ct)
    {
        try
        {
            await client.KickPlayerAsync(id, ct);
            return Ok(new { success = true });
        }
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
    }
}
