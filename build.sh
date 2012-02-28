#!/bin/sh

which icu-config > /dev/null 2> /dev/null
if [ $? -ne 0 ]
then
    echo 'Cannot find `icu-config` in $PATH,'
    echo 'please install it from http://site.icu-project.org/'
    exit 1
fi

node-waf configure || exit 1
node-waf build || exit 1
