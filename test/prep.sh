#!/bin/bash

file=$1
cat $file

classname=$(cat $file | grep "TEST_CLASS" | sed 's/TEST_CLASS(\(.*\))/\1/g' | awk '{$1=$1};1' | tr -d '\r\n')
if [ -n "$classname" ]; then
    echo "namespace { static void unittest(){"
    echo "UnitTest::$classname test;"

    methods=$(cat $file | grep "TEST_METHOD" | sed 's/TEST_METHOD(\(.*\))/\1/g')
    if [ -n "$methods" ]; then
        for method in $methods; do
            method=$(echo $method | awk '{$1=$1};1' | tr -d '\r\n')
            echo "std::cout << \"$method...\" << std::endl;"
            echo "test.$method();"
        done
    fi

    echo "}"
    echo " struct Init { Init() { AddUnitTest(\"$classname\", unittest); } }; Init init; }"
fi
