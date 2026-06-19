using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Filters;

#pragma warning disable CS1591 // Missing XML comment for publicly visible type or member

namespace DhNet.Web.Filters;

// GrpcAdminClient가 던지는 매핑된 예외(TimeoutException 등)를 HTTP 상태 코드로
// 변환하는 로직을 컨트롤러마다 반복하지 않도록 전역 필터로 추출했다.
public sealed class GrpcExceptionFilterAttribute(ILogger<GrpcExceptionFilterAttribute> logger) : ExceptionFilterAttribute
{
    public override void OnException(ExceptionContext context)
    {
        var (statusCode, logLevel) = context.Exception switch
        {
            TimeoutException => (StatusCodes.Status504GatewayTimeout, LogLevel.Warning),
            KeyNotFoundException => (StatusCodes.Status404NotFound, LogLevel.Warning),
            ArgumentException => (StatusCodes.Status400BadRequest, LogLevel.Warning),
            HttpRequestException => (StatusCodes.Status503ServiceUnavailable, LogLevel.Warning),
            _ => (StatusCodes.Status500InternalServerError, LogLevel.Error)
        };

        var controller = context.RouteData.Values["controller"];
        var action = context.RouteData.Values["action"];
        var tag = context.RouteData.Values.TryGetValue("id", out var id)
            ? $"[{controller}] {action} (id={id})"
            : $"[{controller}] {action}";

        logger.Log(logLevel, context.Exception, "{Tag} failed", tag);

        context.Result = new ObjectResult(new { error = context.Exception.Message })
        {
            StatusCode = statusCode
        };
        context.ExceptionHandled = true;
    }
}
