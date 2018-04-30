# MM-PIHM-EnKF
MM-PIHM EnKF is an ensemble Kalman filter data assimilation system developed for Multi-Modular Penn State Integrated Hydrologic Model ([MM-PIHM](https://github.com/PSUmodeling/MM-PIHM))

## Installing MM-PIHM-EnKF

To install MM-PIHM-EnKF, please specify your MM-PIHM directory in the Makefile, and then simply run `make` in your `MM-PIHM-EnKF` directory.
The Makefile will first perform a compatibility test to check if the MM-PIHM version installed is compatible.

## Input files

### enkf.config

`enkf.config` is the main control file for the MM-PIHM EnKF system.
It consists of three blocks.

The "File paths" blocks defines the paths to other input files.

* `PARAM_TBL`: file name of the list of model parameters.
* `VAR_TBL`: file name of the list of model variables to be estimated by EnKF.
* `OBS_TBL`: file name of the list of assimilated observations.

The "EnKF configuration block defines parameters for the data assimilation process.
* `NUM_MEMBER`:   number of ensemble members.
* `ASSIM_INTVL`:  assimilation interval in hours.
* `PERTURB_MODE`: initial parameter perturbation mode.

  When `PERTURB_MODE` is 3, existing `.calib` files will be used without perturbing model parameters.
  When `PERTURB_MODE` is between -1 and 1, perturbed mean will be equal to (center of parameter range + `PERTURB_MODE` * SIGMA),
  where SIGMA is the initial standard deviation of parameters.
* `INFL_WEIGHT`:  covariance relaxation weight.
* `CYCLE_START_TIME` and `CYCLE_END_TIME`: EnKF cycle start and end times in "yyyy-mm-dd HH:MM".
* `ASSIM_START_TIME`: time of first assimilation in "yyyy-mm-dd HH:MM"

The "MM-PIHM configuration" block defines parameters for MM-PIHM.
* `PIHM_DIR`: path to MM-PIHM directory.
* `PROJECT`: name of MM-PIHM project.
* `OUTPUT_DIR`: name of MM-PIHM output directory.

  `OUTPUT_DIR` parameter can be left blank, in which case PROJECT.(simulation start time) style directory name will be used.
  EnKF output files (e.g., estimated parameters, assimilated observations) will also be written in this directory.
* `RUN_MODE`: ensemble simulation mode.

  If `RUN_MODE` is 0, ensemble simulations will be performed on current node sequentially without submitting jobs.
  This is good for a small number of ensemble members, in which case PBS queue time could be longer than running those simulations sequentially.
  
  If `RUN_MODE` is 1, ensemble simulations will be submitted as PBS jobs.
  This is desirable if you have your own PBS queue so wait time is minimum.
  
### param.tbl

The parameter table (`param.tbl`) should contain all calibration coefficients that are required by MM-PIHM, including those that are not to be perturbed or estimated by EnKF.
It should contain the following columns:
* `PARAMETER`: name of model parameters, which should be consistent with the parameter names in MM-PIHM `.calib` file.
* `PERTURB` and `UPDATE`: flags to indicate whether the parameter is to be perturbed\updated by EnKF (1) or not (0).

  Note that a parameters can be set to be perturbed but not updated. It, however, cannot be set to be updated but not perturbed.
* `PTB_MIN` and `PTB_MAX`: range of initial perturbation of calibration coefficients.
* `MIN` and `MAX`: plausible range of calibration coefficients.

  EnKF estimates out of this range will be rejected.
* `LOG`: flag to indicate whether the parameter (calibration coefficients) should be converted to log space (1) or not.

### var.tbl

The variable table (`var.tbl`) should contain model variables to be optimized by MM-PIHM.
Unlike `param.tbl`, `var.tbl` should only contain the variables that need to be optimized.
It should contain the following columns:
* `VARIABLE`: name of model variables, which should be consistent with MM-PIHM output file names.
* `TYPE`: to indicate whether the variable is for triangular elements (ELEM) or river segments (RIVER).
* `MIN` and `MAX`: plausible range of model variables.

  Variables out of range will be assigned the boundary value.
  A -999 value will skip checking the corresponding boundary.

### obs.tbl

The observation table (`obs.tbl`) should contain observations to be assimilated using EnKF.
It should contain the following columns:
* `OBS_TYPE`: type of observation.

  This is used to determine which observation operator should be applied.
  Currently, observation operators for "discharge", "skin_temperature", and "COSMOS" have been included in MM-PIHM EnKF.
* `OBS_FILE`: name of observation files that contain observed values.
* `LOCATION_X` and `LOCATION_Y`: location of the measurements.
* `RADIUS`: effective radius of measurements.
* `DEPTH`: depth of measurements.

The observatino files should use the following format for each line of file:

yyyy-mm-dd HH:MM | observed value | observation uncertainty
---------------- | -------------- | -----------------------

## Components

### run_cycles.sh

This is the main script that controls the data assimilation cycles.
It will run the other scripts and executables in the `util` directory.

### util/check_version.sh

This script is invoked by Makefile to check whether the installed MM-PIHM version is compatible with the current version of MM-PIHM EnKF.

### util/run_ens.sh

This script is used to perform ensemble simulations of MM-PIHM.
The parameters are intended to be used on Penn State ICS-ACI systems.
You can change the number of OMP threads or PBS queue name if you use different systems.

### util/write_para.sh

This script is invoked to write `.para` input files for MM-PIHM simulations.

### util/perturb

This is an executable compiled from C source files to perform initial perturbation.

### util/assim

This is an executable compiled from C source files to perform EnKF analysis step.
