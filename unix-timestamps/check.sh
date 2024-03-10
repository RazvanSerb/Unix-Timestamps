#!/bin/bash

# Special thanks to Cosmin, Mihnea & Teo (313CB) for repairing the checker
# at 2:40am on a Sunday night


function init {
    total_score=0
    pad=$(printf '%.1s' "."{1..70})
    padlength=71

    TESTS_OUT=tests-out
    TESTS=tests
    NUM_TESTS=10
    NUM_SCORE_50=0
    NUM_SCORE_25=0
    NUM_SCORE_150=0
    # Cleanup the previous run's temporary files
    rm -rf ${TESTS_OUT}

    # Run tests
}

function print_result {
    printf "%s" "$1"
    printf "%*.*s" 0 $((padlength - ${#1} - ${#2} )) "$pad"
    printf "%s\n" "$2"
}

function check_homework {
    echo "..............................TASK $1..............................."

    mkdir -p ${TESTS_OUT}/$1

    for (( i = 0; i < $3; ++i))
    do
        in_file="${TESTS}/input/$1/$i.in"
        ref_file="${TESTS}/ref_output/$1/$i.ref"
        out_file="${TESTS_OUT}/$1/$i.out"

        test_id=$i
        test_score=$2
        task_str=$1
        task_num=${task_str:4}

        # if task_num is between 1 and 6
        if [ $task_num -ge 1 -a $task_num -le 6 ]
        then
            make -f ./Makefile -s main
            ./main $task_num < ${in_file} > ${out_file}
        # else if task is 7 or 8
        elif [ $task_num -ge 7 -a $task_num -le 8 ]
        then
            make -f ./Makefile -s bitpacking
            ./bitpacking < ${in_file} > ${out_file}
        else
            make -f ./Makefile -s planner
            ./planner < ${in_file} > ${out_file}
        fi

        diff --ignore-all-space ${out_file} ${ref_file} > /dev/null
        if [ $? -eq 0 ]
        then
            print_result "Test ${test_id}" "${test_score}/${test_score}p passed"
            if [[ $task_num -eq 1 || $task_num -eq 2 || $task_num -eq 3 ]]
            then
                let "NUM_SCORE_50++"
            elif [[ $task_num -eq 4 || $task_num -eq 5 || $task_num -eq 6 ]]
            then
                let "NUM_SCORE_25++"
            elif [ $task_num -eq 9 ]
            then
                let "NUM_SCORE_150++"
            else
                let "total_score += $test_score"
            fi
        else
            print_result "Test ${test_id}" "0/${test_score}p failed"
        fi
    done
}

function check_valgrind {
    echo "..............................TASK 10..............................."

    mkdir -p ${TESTS_OUT}/task9

    # if NUM_SCORE_150 is less than 20, skip this test
    if [ $NUM_SCORE_150 -lt 20 ]
    then
        print_result "Skipping Valgrind check - you must first have maximum score for task 9"
        return
    fi

    for (( i = $2; i < $3; ++i))
    do
        in_file="${TESTS}/input/task9/$i.in"

        test_id=$((i - $2))
        test_score=$1

        make -f ./Makefile -s planner
        valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./planner < ${in_file} > /dev/null 2>&1

        if [ $? -eq 0 ]
        then
            let "NUM_SCORE_50++"
            print_result "Test ${test_id}" "${test_score}/${test_score}p passed"
        else
            print_result "Test ${test_id}" "0/${test_score}p failed"
        fi
    done
}

function check_readme {
    echo -e "\n..............................TASK README.............................."

    print_result "Test README" "$1/$1p passed"
    let "total_score += $1"
}

function check_style {
    echo -e "\n..............................CODING STYLE............................."
    is_style_ok=1
    echo -e "\nRun cpplint\n" > result
    cpplint --filter=-legal/copyright,-readability/casting,-build/include_subdir,-runtime/threadsafe_fn,-build/header_guard,-runtime/int \
        --linelength=120 $( find . -name \*.c | grep -v -e "main.c" ) &>> result

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    echo -e "\nRun clang-tidy\n" >> result
    clang-tidy -checks='-*,cppcoreguidelines*' --warnings-as-errors='-*,cppcoreguidelines*' $( find . -name \*.h -or -name \*.c | grep -v -e "main.c" -e "main.c" -e "bmp.c" ) -- &>> result

    if [ ! $? -eq 0 ]; then
        is_style_ok=0
    fi

    if [ $is_style_ok -eq 0 ]; then
        print_result "Test Coding Style" "-20/0p failed"
        let "total_score -= 20"
    else
        print_result "Test Coding Style" "0/0p passed"
    fi

    # Display run summary
    awk -f parse.awk result

    # Display detailed report
    echo -e "\n================================"
    echo -e "= Detailed coding style report ="
    echo -e "================================\n"
    cat result
    echo " "
    echo " "
}

function run_checker {
    init
	check_homework task1 0.5 10
	check_homework task2 0.5 10 
	check_homework task3 0.5 10
	check_homework task4 0.25 20
	check_homework task5 0.25 20
	check_homework task6 0.25 20
	check_homework task7 1 10
	check_homework task8 2 10
	check_homework task9 1.5 20

    check_valgrind 0.5 10 20 # 0.5 pct, 10 tests, skip first 10
    check_readme 5 # 5 pct
    check_style
    echo -n "$NUM_SCORE_25 $NUM_SCORE_50 $NUM_SCORE_150 $total_score" > temp_score
    awk -F " " '{
        total_score = $1 * 0.25 + $2 * 0.50 + $3 * 1.50 + $4
        printf("Final score.....................................................%.02f/100\n",total_score)
    }' temp_score
    rm temp_score
}

TIMEOUT_VAL=1

export -f run_checker init check_homework check_readme check_style print_result
#timeout ${TIMEOUT_VAL} run_checker
#timeout ${TIMEOUT_VAL} bash <<EOT
#run_checker
#EOT
run_checker

if [ ! $? -eq 0 ]
then
    echo "TIMEOUT. Tests exceeded maximum allowed time of $TIMEOUT_VAL"
fi

