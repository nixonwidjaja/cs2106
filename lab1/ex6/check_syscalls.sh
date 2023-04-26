#!/bin/bash

####################
# Lab 1 Exercise 6
# Name: Nicholas Jimmy Alden & Nixon Widjaja
# Student No: A0244833A & A0236430N
# Lab Group: B13 & B06
####################

echo "Printing system call report"

# Compile file
gcc -std=c99 pid_checker.c -o ex6

# Use strace to get report
strace -c ./ex6