using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // Missing XML comment for publicly visible type or member

namespace DhNet.Web.Controllers;

[ApiController]
[Route("players")]
public class PlayersController(IAdminClient client, ILogger<PlayersController> logger) : ControllerBase
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
        catch (TimeoutException e) { logger.LogWarning(e, "[Players] Get timeout"); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Players] Get not found"); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Players] Get bad request"); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Players] Get backend unavailable"); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Players] Get unexpected error"); return StatusCode(500, new { error = e.Message }); }
    }

    [HttpPost("{id}/kick")]
    public async Task<IActionResult> Kick([FromRoute] ulong id, CancellationToken ct)
    {
        try
        {
            await client.KickPlayerAsync(id, ct);
            return Ok(new { success = true });
        }
        catch (TimeoutException e) { logger.LogWarning(e, "[Players] Kick timeout (id={Id})", id); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Players] Kick not found (id={Id})", id); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Players] Kick bad request (id={Id})", id); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Players] Kick backend unavailable (id={Id})", id); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Players] Kick unexpected error (id={Id})", id); return StatusCode(500, new { error = e.Message }); }
    }
}
