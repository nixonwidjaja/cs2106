#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Nicholas Jimmy Alden & Nixon Widjaja
# Student No: A0244833A & A0236430N
# Lab Group: B13 & B06
####################

# Fill the below up
hostname=`hostname`
machine_hardware="$(uname) $(uname -i)"
max_process_id=`cat /proc/sys/kernel/pid_max`
user_process_count=$(ps -aux | grep "$(whoami)" | wc -l)
user_with_most_processes="$(ps -eo user|sort|uniq -c|sort -n|tail -1|awk '{print $2}')"
mem_free_percentage="$(free | grep "^Mem" | awk '{print $4/$2 * 100}')"

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max Process ID: $max_process_id"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"
