using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // Missing XML comment for publicly visible type or member

namespace DhNet.Web.Controllers;

[ApiController]
[Route("lobbies")]
public class LobbiesController(IAdminClient client) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(IEnumerable<LobbyDto>), 200)]
    public async Task<ActionResult<IEnumerable<LobbyDto>>> Get(CancellationToken ct)
    {
        var lobbies = await client.ListLobbiesAsync(ct);
        return Ok(lobbies);
    }
}
