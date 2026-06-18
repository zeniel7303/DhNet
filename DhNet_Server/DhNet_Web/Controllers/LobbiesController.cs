using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // Missing XML comment for publicly visible type or member

namespace DhNet.Web.Controllers;

[ApiController]
[Route("lobbies")]
public class LobbiesController(IAdminClient client, ILogger<LobbiesController> logger) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<LobbyDto>), 200)]
    public async Task<ActionResult<IEnumerable<LobbyDto>>> Get(CancellationToken ct)
    {
        try
        {
            var lobbies = await client.ListLobbiesAsync(ct);
            return Ok(lobbies);
        }
        catch (TimeoutException e) { logger.LogWarning(e, "[Lobbies] Get timeout"); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Lobbies] Get not found"); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Lobbies] Get bad request"); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Lobbies] Get backend unavailable"); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Lobbies] Get unexpected error"); return StatusCode(500, new { error = e.Message }); }
    }
}
