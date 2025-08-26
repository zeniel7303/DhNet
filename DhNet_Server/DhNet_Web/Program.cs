using System.Net;
using DhNet.Web.Services;
using Microsoft.AspNetCore.HttpLogging;

var builder = WebApplication.CreateBuilder(args);

// Bind only to localhost
builder.WebHost.UseKestrel(options =>
{
    options.Listen(IPAddress.Loopback, 8080);
});

// Add services to the container.
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

builder.Services.AddHttpLogging(o =>
{
    o.LoggingFields = HttpLoggingFields.RequestMethod |
                      HttpLoggingFields.RequestPath |
                      HttpLoggingFields.ResponseStatusCode;
});

builder.Services.AddSingleton<IAdminClient>(_ =>
    new GrpcAdminClient("http://127.0.0.1:7778"));

// Controllers
builder.Services.AddControllers();

var app = builder.Build();

app.UseHttpLogging();

app.UseSwagger();
app.UseSwaggerUI();

app.MapControllers();

app.Run();