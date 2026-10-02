#!/usr/bin/env bash

TEST_RUNTIME=5

# Override to run only some
# e.g. OBJECT_COUNTS="500 1000" ./scripts/run_test.sh

OBJECT_COUNTS=${OBJECT_COUNTS:-"10 100"}            # 500 1000 2500 5000 10000"}
THREADING_METHOD=${THREADING_METHOD:-"serial"}      # broad fine"}
SMT_METHOD=${SMT_METHOD:-"off"}                     # on off"}



TIMEOUT=$(command -v gtimeout || command -v timeout)

# loop through all tests unless specified
for n in $OBJECT_COUNTS; do

    for t in $THREADING_METHOD; do

        for h in $SMT_METHOD; do
            echo ""
            echo ""
            echo ""
            echo "================================================== TEST RUNTIME = $TEST_RUNTIME SECONDS =================================================="
            echo ""
            echo "Object count:     $n"
            echo "Threading method: $t"
            echo "Hyperthreading:   $h"
            echo ""
            echo "==============================================================================================================================="
            echo ""
            # echo "Writing to:       .log / .csv"

            "$TIMEOUT" -s INT $TEST_RUNTIME ./build/arcxel -n "$n" # -t "$t"

        done
    done
done
