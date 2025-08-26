using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // 공개된 형식 또는 멤버에 대한 XML 주석이 없습니다.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("health")]
public class HealthController(IAdminClient client) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(HealthDto), 200)]
    public async Task<ActionResult<HealthDto>> Get(CancellationToken ct)
    {
        try
        {
            var result = await client.HealthCheckAsync(ct);
            return Ok(result);
        }
        catch (TimeoutException e) { return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { return StatusCode(500, new { error = e.Message }); }
    }
}
