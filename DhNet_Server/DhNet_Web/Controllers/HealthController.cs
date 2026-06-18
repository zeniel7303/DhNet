using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("health")]
public class HealthController(IAdminClient client, ILogger<HealthController> logger) : ControllerBase
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
        catch (TimeoutException e) { logger.LogWarning(e, "[Health] timeout"); return StatusCode(504, new { error = e.Message }); }
        catch (KeyNotFoundException e) { logger.LogWarning(e, "[Health] not found"); return NotFound(new { error = e.Message }); }
        catch (ArgumentException e) { logger.LogWarning(e, "[Health] bad request"); return BadRequest(new { error = e.Message }); }
        catch (HttpRequestException e) { logger.LogWarning(e, "[Health] backend unavailable"); return StatusCode(503, new { error = e.Message }); }
        catch (Exception e) { logger.LogError(e, "[Health] unexpected error"); return StatusCode(500, new { error = e.Message }); }
    }
}
