#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Nicholas Jimmy Alden & Nixon Widjaja
# Student No: A0244833A & A0236430N
# Lab Group: B13 & B06
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix
# Check the validity of the prefix #
echo "Enter prefix (only alphabets):"
read prefix
while [[ "$prefix" =~ [^a-zA-Z] || -z "$prefix" ]]
do
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]:"
    read prefix
done

# Enter numbers and create files #
echo "Number of files to create:"
read num
arr=()
echo "Enter $num numbers:"
for ((i = 0; i < num; )); do
    read name
    if [[ "$name" =~ [^0-9] ]]
        then 
            echo "INVALID"
            echo "Please enter a valid number [0-9]:"
        else
            `touch $prefix"_"$name.txt`
            arr+=("$name")
            let i++
    fi
done
echo ""
echo "Files Created"
ls *.txt
echo ""

# Fill in the code to request user for the new prefix
echo "Enter NEW prefix (only alphabets):"
read new
while [[ "$prefix" =~ [^a-zA-Z] || -z "$prefix" ]]
do
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]:"
    read new
done

# Renaming to new prefix #
for i in "${arr[@]}"; do
    `mv $prefix"_"$i.txt $new"_"$i.txt`
done
echo ""
echo "Files Renamed"
ls *.txt
echo ""