$WALL_TIME = 20
$NUM_ROUNDS = 10
$PAUSE_BETWEEN_RUNS = 30

$OBJECT_COUNTS    = if ($env:OBJECT_COUNTS)    { $env:OBJECT_COUNTS }    else { "100 500 1000 2500 5000" }
$THREADING_METHOD = if ($env:THREADING_METHOD) { $env:THREADING_METHOD } else { "serial static task" }

$cpus = Get-CimInstance Win32_Processor
$cores    = ($cpus | Measure-Object -Property NumberOfCores -Sum).Sum
$threads   = ($cpus | Measure-Object -Property NumberOfLogicalProcessors -Sum).Sum
$smt_label = if ($threads -gt $cores) { 'on' } else { 'off' }

$batch_stamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"

foreach ($round in 1..$NUM_ROUNDS) {
	foreach ($tmodel in $THREADING_METHOD.Split(" ")) {
		foreach ($num_objects in $OBJECT_COUNTS.Split(" ")) {

			if ($tmodel -eq "serial") {
				$arcxel_prog = "$pwd\build-serial\Release\arcxel.exe"
			} elseif ($tmodel -eq "static") {
				$arcxel_prog = "$pwd\build-static\Release\arcxel.exe"
			} elseif ($tmodel -eq "task") {
				$arcxel_prog = "$pwd\build-task\Release\arcxel.exe"
			} else {
				Write-Host "Unknown threading method: $tmodel"
				exit 1
			}

			Write-Host ""
			Write-Host ""
			Write-Host ""
			Write-Host "================================================== TEST RUNTIME = $WALL_TIME SECONDS =================================================="
			Write-Host ""
			Write-Host "PWD:              $pwd"
			Write-Host "Object count:     $num_objects"
			Write-Host "Threading method: $tmodel"
			Write-Host "Hyperthreading:   $smt_label"
			Write-Host "Round:			  $round"
			Write-Host "Program:          $arcxel_prog"
			Write-Host ""
			Write-Host "==============================================================================================================================="
			Write-Host ""

			$TRACE_DIR = "results\$batch_stamp\traces\$tmodel\smt-$smt_label\$num_objects\round0$round"
			$LOG_DIR   = "results\$batch_stamp\logs\$tmodel\smt-$smt_label\$num_objects\round0$round"

			$proc = Start-Process -FilePath "$arcxel_prog" -ArgumentList "-n $num_objects -t $TRACE_DIR -l $LOG_DIR" -NoNewWindow -PassThru

			if (-not $proc.WaitForExit($WALL_TIME * 1000)) {

				$shell = New-Object -ComObject WScript.Shell

				if ($shell.AppActivate($proc.Id)) {
					$shell.SendKeys("{ESC}")
				}
			}

			Start-Sleep -Seconds $PAUSE_BETWEEN_RUNS
		}
	}
}
