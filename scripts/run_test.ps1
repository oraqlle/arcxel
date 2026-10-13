param (
    [Parameter()]
    [string]$tag
 )

$WALL_TIME = 90
$NUM_ROUNDS = 20
$PAUSE_BETWEEN_RUNS = 10

$OBJECT_COUNTS    = if ($env:OBJECT_COUNTS)    { $env:OBJECT_COUNTS }    else { "500 1000 2000" }
$WORK_VAR         = if ($env:WORK_VAR)         { $env:WORK_VAR }         else { "0.0 0.75" }
$THREADING_METHOD = if ($env:THREADING_METHOD) { $env:THREADING_METHOD } else { "serial static task" }
$TAG              = if ($tag)                  { "-$tag" }               else { "" }

$cpus      = Get-CimInstance Win32_Processor
$cores     = ($cpus | Measure-Object -Property NumberOfCores -Sum).Sum
$threads   = ($cpus | Measure-Object -Property NumberOfLogicalProcessors -Sum).Sum
$smt_label = if ($threads -gt $cores) { 'on' } else { 'off' }

$total = ($OBJECT_COUNTS.Split(" ") | Measure-Object).Count * ($THREADING_METHOD.Split(" ") | Measure-Object).Count * ($WORK_VAR.Split(" ") | Measure-Object).Count * $NUM_ROUNDS
$num   = 0
$time_left = ($WALL_TIME + $PAUSE_BETWEEN_RUNS) * $total

$SEEDS_PATH = "$results$TAG\seeds.csv"
$seeds_list = @()
$new_seeds = $true

New-Item -Path "$results$TAG" -ItemType Directory

if (Test-Path -Path "$SEEDS_PATH") {
    $seeds_list = Get-Content -Path "$SEEDS_PATH"
    $new_seeds = $false
} else {
    Add-Content -Path "$SEEDS_PATH" -Value "round,seed"
    $seeds_list += "round,seed"
}

foreach ($round in 1..$NUM_ROUNDS) {
    $seed = $null

    if ($new_seeds) {
        $seed = Get-Random -Minimum 0 -Maximum 2147483647
        Add-Content -Path "$SEEDS_PATH" -Value "$round,$seed"
        $seeds_list += "$round,$seed"
    } else {
        $seed = $seeds_list[$round].Split(",")[1]
    }

	foreach ($tmodel in $THREADING_METHOD.Split(" ")) {
                foreach ($num_objects in $OBJECT_COUNTS.Split(" ")) {
                        foreach ($var in $WORK_VAR.Split(" ")) {
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
				$WINDOW_NAME = "Arcxel Window - Sim ($num / $total) | Time Left: $time_left s | Model: $tmodel | NumObjects: $num_objects | Workload Var: $var | Round: $round | SMT: $smt_label"

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
				Write-Host "Workload Var:     $var"
				Write-Host "Program:          $arcxel_prog"
				Write-Host "Window Name:      $WINDOW_NAME"
				Write-Host ""
				Write-Host "==============================================================================================================================="
				Write-Host ""

				$var_str     = if ($var -eq "0.0") { "uniform" } else { "varied" }
				$OUTDIR      = "results$TAG\$tmodel\smt-$smt_label\$num_objects\var-$var_str\round0$round"
				$Args        = "-n $num_objects -t $OUTDIR -l $OUTDIR -s $seed --workload-factor $var --window-name `"$WINDOW_NAME`""
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
