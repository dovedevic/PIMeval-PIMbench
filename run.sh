#!/bin/bash

# Define the base path for executable files
BASE_PATH="/mnt/c/Users/milip/Documents/Mili/Research/MDL/PIM/Codebase/PIMeval-PIMbench/PIMbench"

# Define the configuration base path (centralized configuration files)
CONFIG_PATH="/mnt/c/Users/milip/Documents/Mili/Research/MDL/PIM/Codebase/PIMeval-PIMbench/configs/iiswc"

# Array of workloads
workloads=("knn" "brightness" "axpy")

# Array of architectures
architectures=("LUT" "Fulcrum" "Bank")

# Array of ranks
ranks=(1 4 8 16 32)

# Maximum number of parallel jobs
MAX_JOBS=4

# Function to wait until the number of background jobs is less than MAX_JOBS
wait_for_jobs() {
    while [ "$(jobs -rp | wc -l)" -ge "$MAX_JOBS" ]; do
        sleep 1
    done
}

# Delete all existing files in the output directories
echo "Deleting all existing files in the output directories..."
for workload in "${workloads[@]}"; do
    for architecture in "${architectures[@]}"; do
        rm -rf "$BASE_PATH/$workload/outputs/$architecture/*"
    done
done

# Create output directories
for workload in "${workloads[@]}"; do
    for architecture in "${architectures[@]}"; do
        mkdir -p "$BASE_PATH/$workload/outputs/$architecture"
    done
done

echo "Running various workloads with different configurations for each architecture and rank..."

# Loop over each workload, architecture, and rank
for workload in "${workloads[@]}"; do
    for architecture in "${architectures[@]}"; do
        for rank in "${ranks[@]}"; do
            # Define the configuration file path
            CONFIG_FILE="$CONFIG_PATH/PIMeval_${architecture}_Rank${rank}.cfg"

            # Check if configuration file exists
            if [ ! -f "$CONFIG_FILE" ]; then
                echo "Configuration file $CONFIG_FILE not found. Skipping $workload for $architecture Rank $rank."
                continue
            fi

            # Define output file path based on workload, architecture, and rank
            OUTPUT_FILE="$BASE_PATH/$workload/outputs/$architecture/${workload}_${architecture}_Rank${rank}.txt"

            # Execute the command based on the workload
            case $workload in
                "knn")
                    CMD="$BASE_PATH/$workload/PIM/knn.out -n 6710886 -c $CONFIG_FILE"
                    ;;
                "brightness")
                    CMD="$BASE_PATH/$workload/PIM/brightness.out -c $CONFIG_FILE -b 30 -v t"
                    ;;
                "axpy")
                    CMD="$BASE_PATH/$workload/PIM/axpy.out -l 6710886 -c $CONFIG_FILE"
                    ;;
                *)
                    echo "Unknown workload: $workload"
                    exit 1
                    ;;
            esac

            # Wait if maximum number of jobs are running
            wait_for_jobs

            # Execute the command and redirect output and errors to the same file
            echo "Executing $workload for $architecture with Rank $rank..."
            echo "Running command: $CMD"
            ($CMD > "$OUTPUT_FILE" 2>&1) &
        done
    done
done

# Wait for all background jobs to complete
wait

echo "All operations completed."
