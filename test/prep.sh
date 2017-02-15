#!/bin/bash

file=$1
output=$2/$(basename $file)
cp -f $file $output

classname=$(cat $file | grep "TEST_CLASS" | sed 's/TEST_CLASS(\(.*\))/\1/g')
if [ -n "$classname" ]; then
    echo "namespace { void unittest(){" >> $output
    echo "UnitTest::$classname test;" >> $output

    methods=$(cat $file | grep "TEST_METHOD" | sed 's/TEST_METHOD(\(.*\))/\1/g')
    if [ -n "$methods" ]; then
        for method in $methods; do
            method=$(echo $method | tr -d '\n')
            #echo "std::cout << \"$method...\" << std::endl;" >> $output
            echo "test.$method();" >> $output
        done
    fi

    echo "}" >> $output
    echo " struct Init { Init() {UnitTests.push_back(unittest);} }; Init init; }" >> $output
fi
