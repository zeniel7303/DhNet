using DhNet.Web.Services;
using Microsoft.AspNetCore.Mvc;

#pragma warning disable CS1591 // ������ ���� �Ǵ� ����� ���� XML �ּ��� �����ϴ�.

namespace DhNet.Web.Controllers;

[ApiController]
[Route("health")]
public class HealthController(IAdminClient client) : ControllerBase
{
    [HttpGet]
    [ProducesResponseType(typeof(HealthDto), 200)]
    public async Task<ActionResult<HealthDto>> Get(CancellationToken ct)
    {
        var result = await client.HealthCheckAsync(ct);
        return Ok(result);
    }
}
