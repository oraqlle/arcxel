$TEST_RUNTIME = 5

# Override to run only some
# e.g. $env:OBJECT_COUNTS = "500 1000"; .\scripts\run_test.ps1

$OBJECT_COUNTS    = if ($env:OBJECT_COUNTS)    { $env:OBJECT_COUNTS }    else { "10 100" }   # 500 1000 2500 5000 10000
$THREADING_METHOD = if ($env:THREADING_METHOD) { $env:THREADING_METHOD } else { "serial" }   # broad fine
$SMT_METHOD       = if ($env:SMT_METHOD)       { $env:SMT_METHOD }       else { "off" }      # on off

$ARCXEL = ".\build\arcxel.exe"

# Windows has no kill -INT. Sending Ctrl+C to the console is what arcxel sees as SIGINT.
Add-Type -Namespace Win32 -Name Console -MemberDefinition @'
[DllImport("kernel32.dll")] public static extern bool GenerateConsoleCtrlEvent(uint ctrlEvent, uint processGroupId);
[DllImport("kernel32.dll")] public static extern bool SetConsoleCtrlHandler(System.IntPtr handler, bool add);
'@


# loop through all tests unless specified
foreach ($n in $OBJECT_COUNTS.Split(" ")) {

    foreach ($t in $THREADING_METHOD.Split(" ")) {

        foreach ($h in $SMT_METHOD.Split(" ")) {
            Write-Host ""
            Write-Host ""
            Write-Host ""
            Write-Host "================================================== TEST RUNTIME = $TEST_RUNTIME SECONDS =================================================="
            Write-Host ""
            Write-Host "Object count:     $n"
            Write-Host "Threading method: $t"
            Write-Host "Hyperthreading:   $h"
            Write-Host ""
            Write-Host "==============================================================================================================================="
            Write-Host ""
            # Write-Host "Writing to:       .log / .csv"

            $proc = Start-Process $ARCXEL -ArgumentList "-n", $n -NoNewWindow -PassThru

            if (-not $proc.WaitForExit($TEST_RUNTIME * 1000)) {
                # ignore the Ctrl+C ourselves so only arcxel reacts to it
                [Win32.Console]::SetConsoleCtrlHandler([IntPtr]::Zero, $true) | Out-Null
                [Win32.Console]::GenerateConsoleCtrlEvent(0, 0) | Out-Null
                $proc.WaitForExit()
                [Win32.Console]::SetConsoleCtrlHandler([IntPtr]::Zero, $false) | Out-Null
            }

        }
    }
}
