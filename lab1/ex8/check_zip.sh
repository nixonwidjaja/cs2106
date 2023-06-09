#!/bin/bash

function check_files {
    for i do
        if [ ! -f $i ]; then
            return 1
        fi
    done
}

function check_compile {
    x=1
    cd $1
    #pwd
        make &> /dev/null
        x=$?
        #echo "x:"$x
    cd ../
    [[ $x -ne 0 ]]
}

function run_tests {
    tests=("sample" "small_test" "big_test")
    cd $1
    make &> /dev/null
    for test in "${tests[@]}"; do
        if [[ $1 == "ex2" ]]
        then
            ./ex2 < $test.in | diff -Z $test.out - &> /dev/null
            res=$?
        else
            ./ex3 $test.in | diff -Z $test.out - &> /dev/null
            res=$?
        fi

        if [[ res -ne 0 ]]; then
            echo "Test $test.in failed"
            break
        fi
    done
    cd ../
}

function transfer_files {
    files=(
        "Makefile"
        "node.h"
        "sample.in"
        "sample.out"
        "small_test.in"
        "small_test.out"
        "big_test.in"
        "big_test.out"
    )

    if [[ $2 == "ex2" ]]
    then
        files+=("ex2.c")
    else
        files+=("functions.h" "functions.c" "function_pointers.h")
    fi

    for file in "${files[@]}"; do
        cp $1/$2/$file $2
    done
}

echo "Checking zip file...."

if [ "$#" -ne 1 ]; then 
    echo "You should provide your zip file as a parameter and nothing else!"
    exit 1
fi

if ! [[ $1 =~ ^E[0-9]{7}.zip$ ]]; then
    #check if its two partners submitting
    if [[ $1 =~ ^E[0-9]{7}_E[0-9]{7}.zip$ ]]
    then
        echo "Submitting as a pair"
    else
        echo "zip file is wrongly named: it should be <NUSNET ID>.zip or <NUSNET ID>_<NUSNET ID>.zip (pairs)"
        exit 1
    fi
fi

echo "Unzipping file: $1"
if [ ! -f $1 ]; then
    echo "File $1 does not exist."
    exit 1
fi

#NITYA
STUDENT_FN=${1%.*}
tmp_folder="test_grading_123456"
rm -rf $tmp_folder
mkdir $tmp_folder

cp $1 $tmp_folder


cd $tmp_folder
unzip $1> /dev/null


if ! cp -R ${STUDENT_FN}/* .
then
    echo "wrong internal folder structure. please zip top level folder instead of all the ex folders"
    exit 1
fi


echo "Transferring necessary skeleton files"
sk="skeleton"
SK_FILENAME="lab1_2022_2023.tar.gz"
mkdir $sk
cd $sk
wget --no-check-certificate -O $SK_FILENAME 'https://docs.google.com/uc?export=download&id=1xDXCgVG2NlNMoPeKzITNy4RoPgteHxmC' 2> /dev/null
tar -xzf $SK_FILENAME 2> /dev/null

SK_FN=${SK_FILENAME%.*.*}

cp -R ${SK_FN}/* .
cd ../



exercises=(ex2 ex3 ex5 ex6 ex7)
ex2=("ex2/node.c")
ex3=("ex3/node.c" "ex3/function_pointers.c" "ex3/ex3.c")
ex5=("ex5/check_system.sh")
ex6=("ex6/check_syscalls.sh")
ex7=("ex7/file_renaming.sh")

for ex in "${exercises[@]}"; do

    declare -n files=$ex
    if [ ! -d $ex ]
    then
        echo "$ex: Failed - $ex folder missing"
        continue
    fi

    if ! check_files "${files[@]}"
    then
        echo "$ex: Failed - files missing"
        continue
    fi

   if [[ $ex == @('ex2'|'ex3') ]]
   then
        transfer_files $sk $ex
		
         if check_compile $ex
         then
             echo "$ex: Failed - does not compile"
             continue
         fi

         test_status="$(run_tests $ex)"
         #echo "test:" $test_status
         if ! [ -z "$test_status" ]; then
             echo "$ex: Failed - $test_status"
             continue
         fi
     fi

     echo "$ex: Success"
done

cd ../

rm -rf $tmp_folder