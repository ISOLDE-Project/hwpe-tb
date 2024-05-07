#!/usr/bin/env bash
# Copyleft

# Define environment variables
MINICONDA=~/miniconda3/etc/profile.d/conda.sh
MINICONDA_ENV=snitch
# To activate this environment, use
#
#     $ conda activate snitch
#
# To deactivate an active environment, use
#
#     $ conda deactivate

# Get the root directory of the Git repository
export ROOT_DIR=$(git rev-parse --show-toplevel)

export BENDER=~/eth/bin/bender
export PULP_RISCV_GCC_TOOLCHAIN=~/isolde-project/install/riscv



source $MINICONDA
conda activate $MINICONDA_ENV

export PATH=~/eth/bin:~/verible/bin:$ROOT_DIR/install/verilator/bin:$PATH
source ~/vivado.sh