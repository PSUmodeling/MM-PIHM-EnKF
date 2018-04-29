#!/bin/sh

first_cycle=$1

if [ $first_cycle == 1 ]; then
    param="-s -o $OUTPUT_DIR"
else
    param="-as -o $OUTPUT_DIR"
fi

if [ $RUN_MODE -eq 0 ]; then
    # Run MM-PIHM sequencially using current node
    for (( ind=1; ind<=$NUM_MEMBER; ind++ ))
    do
        simulation=$( printf '%s.%03d' $PROJECT $ind )
        export OMP_NUM_THREADS=20
        cd $PIHM_DIR
        echo "    Running ensemble member $ind"
        ./flux-pihm $param $simulation
    done
else
    # Create scripts directory
    mkdir -p scripts

    # Submit PBS jobs for ensemble simulations
    jobid=()

    for (( ind=1; ind<=$NUM_MEMBER; ind++ ))
    do
        jobname="ens."$ind
        simulation=$( printf '%s.%03d' $PROJECT $ind )

        # Create PBS script
        cat << EOF > "scripts/run_pihm.sh"
#!/bin/bash
#PBS -N $jobname
#PBS -l nodes=1:ppn=20
#PBS -l walltime=1:00:00
#PBS -A open
#PBS -j oe
#PBS -o $PIHM_DIR/job$ind.log
#PBS -d $PIHM_DIR

export OMP_NUM_THREADS=20
cd $PIHM_DIR
./flux-pihm $param $simulation
EOF

        # Submit job
        log="scripts/job"$ind".log"
        qsub scripts/run_pihm.sh >& $log

        # Get job id
        jobid[`expr $ind - 1`]=`cat $log |awk  -F. 'NR>1 {print $1}'`
    done

    # Wait for all simulations to end
    jobstat=1
    until [[ $jobstat == 0 ]]; do
        sleep 30s
        jobstat=0
        for (( ind=1; ind<=$NUM_MEMBER; ind++ ))
        do
            stat=$( qstat |grep ${jobid[`expr $ind - 1`]} |awk '{if($5=="R" || $5=="Q") print 1; else print 0;}' )
            jobstat=$((jobstat + stat))
        done
    done
fi
