param(
    [string]$VcpkgRoot = "..\..\..\Tools"
)

$ErrorActionPreference = 'Stop'

# Resolve tool paths
$Protoc               = Join-Path $VcpkgRoot 'tools\protobuf\protoc.exe'
$GrpcCppPlugin        = Join-Path $VcpkgRoot 'tools\grpc\grpc_cpp_plugin.exe'
<#$GrpcCsharpPlugin     = Join-Path $VcpkgRoot 'tools\grpc\grpc_csharp_plugin.exe'#>

# Paths
$ScriptDir   = $PSScriptRoot
$ProtoDir    = Split-Path -Parent (Join-Path $ScriptDir '..\dhnet.proto')
$ProtoFile   = Join-Path $ProtoDir 'dhnet.proto'
$OutDirCpp   = Join-Path $ProtoDir 'generated'
<#$OutDirCSharp = Join-Path $ProtoDir 'generated-csharp'#>  # optional manual generation; MSBuild already generates into obj/

# Ensure tools exist
if (!(Test-Path $Protoc)) { throw "protoc not found at: $Protoc" }
if (!(Test-Path $GrpcCppPlugin)) { throw "grpc_cpp_plugin not found at: $GrpcCppPlugin" }
if (!(Test-Path $ProtoFile)) { throw "dhnet.proto not found at: $ProtoFile" }

# Create output directories
New-Item -ItemType Directory -Force -Path $OutDirCpp | Out-Null
<#New-Item -ItemType Directory -Force -Path $OutDirCSharp | Out-Null#>

Write-Host "[PROTO] Generating C++ sources to: $OutDirCpp"
& $Protoc --proto_path="$ProtoDir" --cpp_out="$OutDirCpp" "$ProtoFile"
& $Protoc --proto_path="$ProtoDir" --grpc_out="$OutDirCpp" --plugin=protoc-gen-grpc="$GrpcCppPlugin" "$ProtoFile"

<#Write-Host "[PROTO] (Optional) Generating C# sources to: $OutDirCSharp"
try {
    & $Protoc --proto_path="$ProtoDir" --csharp_out="$OutDirCSharp" "$ProtoFile"
    & $Protoc --proto_path="$ProtoDir" --grpc_out="$OutDirCSharp" --plugin=protoc-gen-grpc="$GrpcCsharpPlugin" "$ProtoFile"
    Write-Host "[PROTO] C# generation succeeded (note: DhNet_Ipc.csproj already generates these at build)."
}
catch {
    Write-Warning "C# generation failed or plugin missing; this step is optional because DhNet_Ipc.csproj uses Grpc.Tools. Details: $_"
}#>

Write-Host "[DONE] Protobuf/gRPC code generation complete."