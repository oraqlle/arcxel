$SEEDS_PATH = "seeds/seeds.csv"
$seeds_list = @()
$new_seeds = $true

if (Test-Path -Path "$SEEDS_PATH") {
    $seeds_list = Get-Content -Path "$SEEDS_PATH"
    $new_seeds = $false
} else {
    Add-Content -Path "$SEEDS_PATH" -Value "round,seed"
    $seeds_list += "round,seed"
}

foreach ($round in 1..10) {
    $seed = $null

    if ($new_seeds) {
        $seed = Get-Random -Minimum 0 -Maximum 2147483647
        Add-Content -Path "$SEEDS_PATH" -Value "$round,$seed"
        $seeds_list += "$round,$seed"
    } else {
        $seed = $seeds_list[$round].Split(",")[1]
    }

    Write-Host $seed
}
