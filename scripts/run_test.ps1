param (
    [Parameter()]
    [string]$tag
 )

$WALL_TIME = 30
$NUM_ROUNDS = 6
$PAUSE_BETWEEN_RUNS = 12

$OBJECT_COUNTS    = if ($env:OBJECT_COUNTS)    { $env:OBJECT_COUNTS }    else { "100 500 1000 2500 5000" }
$WORK_VAR         = if ($env:WORK_VAR)         { $env:WORK_VAR }         else { "0.0 0.75" }
$THREADING_METHOD = if ($env:THREADING_METHOD) { $env:THREADING_METHOD } else { "serial static task" }
$TAG              = if ($tag)                  { "-$tag" }               else { "" }

$cpus = Get-CimInstance Win32_Processor
$cores    = ($cpus | Measure-Object -Property NumberOfCores -Sum).Sum
$threads   = ($cpus | Measure-Object -Property NumberOfLogicalProcessors -Sum).Sum
$smt_label = if ($threads -gt $cores) { 'on' } else { 'off' }

$total = ($OBJECT_COUNTS.Split(" ") | Measure-Object).Count * ($THREADING_METHOD.Split(" ") | Measure-Object).Count * ($WORK_VAR.Split(" ") | Measure-Object).Count * $NUM_ROUNDS
$num   = 0
$time_left = ($WALL_TIME + $PAUSE_BETWEEN_RUNS) * $total

foreach ($round in 1..$NUM_ROUNDS) {
	foreach ($tmodel in $THREADING_METHOD.Split(" ")) {
		foreach ($num_objects in $OBJECT_COUNTS.Split(" ")) {
			foreach ($work_var in $WORK_VAR.Split(" ")) {
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

				$num++
				$WINDOW_NAME = "Arcxel Window - Sim ($num / $total) | Time Left: $time_left s | Model: $tmodel | NumObjects: $num_objects | Workload Var: $work_var | Round: $round | SMT: $smt_label"

				Write-Host ""
				Write-Host ""
				Write-Host ""
				Write-Host "================================================== TEST RUNTIME = $WALL_TIME SECONDS =================================================="
				Write-Host ""
				Write-Host "PWD:              $pwd"
				Write-Host "Object count:     $num_objects"
				Write-Host "Threading method: $tmodel"
				Write-Host "Hyperthreading:   $smt_label"
				Write-Host "Round:            $round"
				Write-Host "Workload Var:     $work_var"
				Write-Host "Program:          $arcxel_prog"
				Write-Host "Window Name:      $WINDOW_NAME"
				Write-Host ""
				Write-Host "==============================================================================================================================="
				Write-Host ""

				$var_str     = if ($work_var -eq "0.0") { "uniform" } else { "varied" }
				$OUTDIR      = "results$TAG\$tmodel\smt-$smt_label\$num_objects\round0$round\var-$var_str"
				$Args        = "-n $num_objects -t $OUTDIR -l $OUTDIR --workload-variance $work_var --window-name `"$WINDOW_NAME`""
				Write-Host "$Args"

				$proc = Start-Process -NoNewWindow -PassThru -FilePath "$arcxel_prog" -ArgumentList $Args 

				if (-not $proc.WaitForExit($WALL_TIME * 1000)) {

					$shell = New-Object -ComObject WScript.Shell

					if ($shell.AppActivate($proc.Id)) {
						$shell.SendKeys("{ESC}")
					}
				}

				$time_left -= $WALL_TIME + $PAUSE_BETWEEN_RUNS
				Start-Sleep -Seconds $PAUSE_BETWEEN_RUNS
			}
		}
	}
}
