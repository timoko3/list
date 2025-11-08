#!/bin/bash

TEST_PROG="./timeComp_perf.out"   # путь к вашей программе
CPU_CORE=6                        # ядро для запуска (укажите подходящее)
RUNS=30
LOGFILE="perf_results_v2.csv"

# Check for root
if [[ $EUID -ne 0 ]]; then
  echo "Please run as root: sudo ./runPerformanceTest.sh"
  exit 1
fi

# echo "== Preparing environment =="
# DRIVER=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_driver 2>/dev/null)
# echo "CPU freq driver: $DRIVER"

echo "→ Setting performance governor"
for c in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  if [[ -f "$c" ]]; then
    echo performance > "$c"
  fi
done

echo "→ Disabling boost (if applicable)"
if [[ -f /sys/devices/system/cpu/cpufreq/boost ]]; then
  echo 0 > /sys/devices/system/cpu/cpufreq/boost
fi

echo "→ Stopping irqbalance (if present)"
systemctl stop irqbalance 2>/dev/null || true

echo "== Starting test loop =="
echo "start" > $LOGFILE

for ((i=1; i<=RUNS; i++)); do
  echo "→ Run #$i"
  printf "$i " >> $LOGFILE
  taskset -c ${CPU_CORE} nice -n -20 "$TEST_PROG"
done

echo "Test complete. Results in $LOGFILE"

echo "== Restoring environment =="
if [[ -f /sys/devices/system/cpu/cpufreq/boost ]]; then
  echo 1 > /sys/devices/system/cpu/cpufreq/boost
fi

echo "→ Restoring governor to powersave"
for c in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do
  if [[ -f "$c" ]]; then
    echo powersave > "$c"
  fi
done

systemctl start irqbalance 2>/dev/null || true

echo "All done."
