$TEST_RUNTIME = 10
$PAUSE_BETWEEN_TESTS = 5

# Override to run only some
# e.g. $env:OBJECT_COUNTS = "500 1000"; .\scripts\run_test.ps1

$OBJECT_COUNTS    = if ($env:OBJECT_COUNTS)    { $env:OBJECT_COUNTS }    else { "100 500" } # 1000 2500 5000 10000" }
$THREADING_METHOD = if ($env:THREADING_METHOD) { $env:THREADING_METHOD } else { "serial static task"}
$SMT_LABEL        = if ($env:SMT_LABEL)        { $env:SMT_LABEL }        else { "off" }      # on off


# Windows has no kill -INT. Sending Ctrl+C to the console is what arcxel sees as SIGINT.
Add-Type -Namespace Win32 -Name Console -MemberDefinition @'
[DllImport("kernel32.dll")] public static extern bool GenerateConsoleCtrlEvent(uint ctrlEvent, uint processGroupId);
[DllImport("kernel32.dll")] public static extern bool SetConsoleCtrlHandler(System.IntPtr handler, bool add);
'@


# loop through all tests unless specified
foreach ($n in $OBJECT_COUNTS.Split(" ")) {


    foreach ($t in $THREADING_METHOD.Split(" ")) {

        if ($t -eq "serial") {
            $ARCXEL = "$pwd\build-serial\Release\arcxel.exe"
        } elseif ($t -eq "static") {
            $ARCXEL = "$pwd\build-static\Release\arcxel.exe"
        } elseif ($t -eq "task") {
            $ARCXEL = "$pwd\build-task\Release\arcxel.exe"
        } else {
            Write-Host "Unknown threading method: $t"
            exit 1
        }

        Write-Host ""
        Write-Host ""
        Write-Host ""
        Write-Host "================================================== TEST RUNTIME = $TEST_RUNTIME SECONDS =================================================="
        Write-Host ""
        Write-Host "PWD:              $pwd"
        Write-Host "Object count:     $n"
        Write-Host "Threading method: $t"
        Write-Host "Hyperthreading:   $SMT_LABEL"
        Write-Host "Program:          $ARCXEL"
        Write-Host ""
        Write-Host "==============================================================================================================================="
        Write-Host ""
        # Write-Host "Writing to:       .log / .csv"

        $RUN_SUBDIR = "round1\$t\smt-$SMT_LABEL\n$n"
        $TRACE_DIR  = "traces\$RUN_SUBDIR"
        $LOG_DIR    = "logs\$RUN_SUBDIR"

        $proc = Start-Process -FilePath "$ARCXEL" -ArgumentList "-n", $n, "-t", $TRACE_DIR, "-l", $LOG_DIR -NoNewWindow -PassThru

        if (-not $proc.WaitForExit($TEST_RUNTIME * 1000)) {
            # ignore the Ctrl+C ourselves so only arcxel reacts to it
            [Win32.Console]::SetConsoleCtrlHandler([IntPtr]::Zero, $true) | Out-Null
            [Win32.Console]::GenerateConsoleCtrlEvent(0, 0) | Out-Null
            $proc.WaitForExit()
            [Win32.Console]::SetConsoleCtrlHandler([IntPtr]::Zero, $false) | Out-Null
        }
        
        Start-Sleep -Seconds $PAUSE_BETWEEN_TESTS
    }
}
