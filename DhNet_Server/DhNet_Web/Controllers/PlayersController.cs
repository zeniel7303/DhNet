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
        var players = await client.ListPlayersAsync(ct);
        return Ok(players);
    }

    [HttpPost("{id}/kick")]
    public async Task<IActionResult> Kick([FromRoute] ulong id, CancellationToken ct)
    {
        await client.KickPlayerAsync(id, ct);
        return Ok(new { success = true });
    }
}
