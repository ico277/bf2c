#!/bin/bash
for file in ./examples/*.bf; 
do
    ./bf2c $file -bo $file.out
done

read -r -p "Do you want to run all examples? [y/N]: " response
if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]
then
    for file in ./examples/*.out; 
    do
        echo "$file:"
        $file
        echo
    done
fi
