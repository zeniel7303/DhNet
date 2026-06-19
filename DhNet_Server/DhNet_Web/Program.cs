using System.Net;
using DhNet.Web.Filters;
using DhNet.Web.Services;
using Serilog;

var builder = WebApplication.CreateBuilder(args);

builder.Host.UseSerilog((context, config) =>
    config.ReadFrom.Configuration(context.Configuration));

// Bind only to localhost
builder.WebHost.UseKestrel(options =>
{
    options.Listen(IPAddress.Loopback, 8080);
});

// Add services to the container.
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddSingleton<IAdminClient>(sp =>
    new GrpcAdminClient("http://127.0.0.1:7778", sp.GetRequiredService<ILogger<GrpcAdminClient>>()));

// Controllers
builder.Services.AddControllers(o => o.Filters.Add<GrpcExceptionFilterAttribute>());

var app = builder.Build();

app.UseSerilogRequestLogging();

app.UseSwagger();
app.UseSwaggerUI();

app.MapControllers();

app.Run();